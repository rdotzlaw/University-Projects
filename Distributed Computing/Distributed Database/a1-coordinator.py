import socket
import select
import sys
import time
import json
import random
from json import JSONDecodeError

TIMEOUT = 1  # time in seconds until timeout

if len(sys.argv) < 2:
    print("Required arguments: [coordinator port] [worker_address]:[worker_port]")
    sys.exit(-1)

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# bind to local host at given port
s.bind(('localhost', int(sys.argv[1])))

# listen for connection from client
s.listen()

workers = []
clients = []
cli_dict = {}
actions = []  # a queue of active actions (ie, coordinator is waiting for votes or smth
client = -1  # global scope for later <- idk if this is still being used, lol
msgs = []


# world's worst timeout
# efficiency? never heard of it,
# i put nested for loops inside my while loops
def ww_timeout():
    # get current time in seconds (from beginning of epoch)
    t = time.time()

    # search through actions
    for a in actions:
        if t - a["time"] > TIMEOUT:
            # if the action is still here, someone didn't respond fast enough
            responds = a["responds"]
            failures = a["total"] - responds
            # failures have timed-out, disconnect them from system
            for f in failures:
                if f in workers:
                    workers.remove(f)
                    f.close()
            # if a is a commit message, i have no idea how to revert it


# takes a list, makes sure ALL values in the list are equal to all others
def verify(l):
    for x in range(0, len(l)):
        for j in range(x + 1, len(l)):
            if (l[x] is None and l[j] is None) or (l[x] != l[j]):
                return False
    return True


def parsedata(data):
    return data.decode().split("#$#")


# msg is the message that was sent
# wait is the value for 'wait-for' key
# searches actions to prevent overlap/inconsistencies
# returns true if found, false otherwise
def search(msg, wait):
    for a in actions:
        if a["wait-for"] == wait:
            # found the message type
            if (wait == "VERIFY" and a["msg"] == msg) or (a["msg"]["key"] == msg["key"]):
                # found change to chosen key
                return a
    return False


# Sends a message to all client sockets, excluding the source of the message and the coordinator
# sock: socket that sent message
# msg: already encoded message to flood
def flood(sck, msg):
    print(" * * Flooding workers with message")
    # loop through all workers
    for worker_socket in workers:
        # if the client client_socket isn't the server or the client the message came from
        if worker_socket != sck and worker_socket != s:
            worker_socket.sendall(msg)


# takes a completed action (specifically a COMMIT type)
# and uses it to deal w/ the appropriate backlog of get/get-db requests that it was blocking
# Basically deals w/ all get requests that were blocked by a set, and deals w/ get-db requests if there are no current sets in progress
def backlog(act):
    for m in msgs:
        if m[0]["type"] == "GET-DB" and len(actions) == 0:
            # just deal w/ it
            proto_decode(json.dumps(m[0]).encode(), m[1])
        elif m[0]["type"] == "GET":
            if act["msg"]["key"] == m[1]["key"]:
                proto_decode(json.dumps(m[0]).encode(), m[1])
        elif m[0]["type"] == "VERIFY" and len(actions) == 0:
            proto_decode(json.dumps(m[0]).encode(), m[1])
        msgs.remove(m)


""" For actions tracker
    { 
        wait-for: VOTE, 
        msg: { [ this would be the full message ] }, 
        responds: [set of sockets that responded], 
        total: [set of sockets that should respond] , 
        time: [time sent]
    } # if voting, already doing a set
    
    { 
        wait-for: COMMIT, 
        msg: { [ this would be the full message ] }, 
        responds: [set of sockets that responded], 
        total: [set of sockets that respond] 
        time: [time sent]
    }

"""


# takes encoded data (JSON), decodes it and decides what to do with it
def proto_decode(d, msg_sock):
    print(" * Dealing with message...")
    jdata = json.loads(d)

    print(" * * Message type: ", jdata["type"])
    if jdata["type"] == "SET":
        # set a key
        # flood all workers with vote requests
        # ignore all requests to set the same k/v pair until update is done
        s1 = search({"type": "VOTE-REQUEST", "action": "SET", "key": jdata["key"]}, "VOTE")
        s2 = search({"type": "COMMIT", "action": "SET", "key": jdata["key"]}, "COMMIT")

        if not isinstance(s1, dict) and not isinstance(s2, dict):  # neither are dictionaries -> search returned false
            # there are no incoming changes on that key, so make some
            # setting values, so flood to everyone except msg_sock

            message = {"type": "VOTE-REQUEST", "action": "SET", "key": jdata["key"], "val": jdata["val"]}

            # record message in actions dict
            action = {"wait-for": "VOTE", "msg": message, "responds": set(), "total": set(workers),
                      "time": time.time(), "sender": msg_sock}

            actions.append(action)

            # send message
            flood(msg_sock, (json.dumps(message) + "#$#").encode())
            # now just need to wait for votes to come in
        else:  # there is a change on the key in progress
            # fail the set
            message = {"type": "SET-RESPONSE", "success": False}
            msg_sock.sendall((json.dumps(message)).encode())

    elif jdata["type"] == "GET":
        # get a key
        # make sure the k/v pair isn't in the middle of being updated
        # choose a worker to grab the key from, then request it
        # choose worker [0, len(workers) - 1]
        s1 = search({"type": "VOTE-REQUEST", "action": "SET", "key": jdata["key"], "val": ""}, "VOTE")
        s2 = search({"type": "COMMIT", "action": "SET", "key": jdata["key"], "val": ""}, "COMMIT")
        if not isinstance(s1, dict) and not isinstance(s2, dict):
            # no ongoing changes to k/v pair
            index = random.randint(0, len(workers) - 1)
            to_client = workers[index]

            print(" * * * Creating and sending GET message to worker# ", index)
            message = {"type": "GET", "key": jdata["key"], "sender": msg_sock.fileno()}

            to_client.sendall((json.dumps(message) + "#$#").encode())
        else:
            # ongoing changes, queue request
            msgs.append([data, msg_sock])

    elif jdata["type"] == "VOTE-RESPONSE":
        # need all workers to vote: TRUE

        # find the action
        act = search(jdata["msg"], "VOTE")
        if isinstance(act, dict) and jdata["vote"]:  # act not false, found action and isn't a fail vote
            # add msg_sock to set of responded sockets
            act["responds"].add(msg_sock)
            if act["responds"] == act["total"]:
                # all workers have send in their votes

                # create commit message
                message = {"type": "COMMIT", "action": jdata["msg"]["action"], "key": jdata["msg"]["key"],
                           "val": jdata["msg"]["val"]}

                # create commit action
                n_act = {
                    "wait-for": "COMMIT",
                    "msg": message,
                    "responds": set(),
                    "total": set(workers),
                    "time": time.time(),
                    "sender": act["sender"]
                }
                actions.append(n_act)
                # remove act from actions
                actions.remove(act)

                # send message to all workers, not client
                flood(client, (json.dumps(message) + "#$#").encode())

        elif not jdata["vote"]:
            # someone said no, don't commit anything, get rid of act
            message = {"type": "SET-RESPONSE", "success": False}
            act["sender"].sendall((json.dumps(message) + "#$#").encode())
            actions.remove(act)
        # otherwise, act doesn't exist
        else:
            print("Received response for action that doesn't exist")
            print(jdata)

    elif jdata["type"] == "COMMIT-RESPONSE":
        # need all workers to commit: TRUE
        # find the action
        act = search(jdata["msg"], "COMMIT")
        if isinstance(act, dict) and jdata["success"]:  # act not false, found action and isn't a fail commit
            # add msg_sock to set of responded sockets
            act["responds"].add(msg_sock)
            if act["responds"] == act["total"]:
                # all workers have committed value to DB
                # remove act from actions
                actions.remove(act)
                # create set response message for client
                message = {"type": "SET-RESPONSE", "success": True}
                act["sender"].sendall((json.dumps(message)).encode())

                # set request completed
                # now deal w/ waiting get/get-db requests
                backlog(act)

        elif not jdata["success"]:
            # someone failed, no idea what to do, lol
            print("A worker has failed to add something to their database, you may now begin crying")

    elif jdata["type"] == "GET-DB":

        # no changes being made to database
        if len(actions) == 0:
            index = random.randint(0, len(workers) - 1)
            work = workers[index]
            # choose random worker
            message = {"type": "GET-DB", "sender": msg_sock.fileno()}

            work.sendall((json.dumps(message) + "#$#").encode())
        else:
            message = {"type": "GET-DB", "sender": msg_sock.fileno()}
            # queue it for later
            msgs.append([data, msg_sock])

    elif jdata["type"] == "VERIFY":
        message = {"type": "VERIFY", "sender": msg_sock.fileno()}

        act = {
            "wait-for": "VERIFY",
            "msg": message,
            "responds": set(),
            "total": set(workers),
            "time": time.time(),
            "sender": msg_sock,
            "db": []
        }
        actions.append(act)

        if len(actions) == 1:
            # get all the db's from every worker
            flood(msg_sock, (json.dumps(message) + "#$#").encode())
        else:
            msgs.append([data, msg_sock])

    elif jdata["type"] == "VERIFY-RESPONSE":
        act = search(jdata["msg"], "VERIFY")

        if isinstance(act, dict):  # action exists, or there are no actions
            # add msg_sock to set of responded sockets
            act["responds"].add(msg_sock)
            act["db"] = [jdata["data"]] + act["db"]

            if act["responds"] == act["total"]:  # all responses have arrived

                if verify(act["db"]):
                    act["sender"].sendall(json.dumps({"type": "VERIFY-RESPONSE", "success": True}).encode())
                    actions.remove(act)
                else:
                    act["sender"].sendall(json.dumps({"type": "VERIFY-RESPONSE", "success": False}).encode())

    elif jdata["type"] == "SEND":
        # pass this data to the client
        message = {"type": "GET-RESPONSE", "key": jdata["key"], "val": jdata["val"]}
        print(message)
        print(jdata)
        cli_dict[jdata["sender"]].sendall((json.dumps(message)).encode())
        print(" * Sent data to client, request complete.")

    elif jdata["type"] == "SEND-DB":
        message = {"type": "DB-RESPONSE", "data": jdata["data"]}
        cli_dict[jdata["sender"]].sendall((json.dumps(message)).encode())
        print(" * Sent data to client, request complete.")

    else:
        # bad request, ignore
        print("Bad Request")
        msg_sock.sendall(json.dumps({"type": "BAD-REQUEST"}).encode())


print("Coordinator hosted @ localhost:", sys.argv[1])

# connect to workers (fill workers list w/ sockets)
for i in sys.argv[2:]:
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    addrTuple = (i.split(':')[0], int(i.split(':')[1]))

    sock.connect(addrTuple)
    print(" * Created socket connected @ ", addrTuple)
    sock.settimeout(2)
    workers.append(sock)

# Main code loop, loop forever
print("Initializing done, entering main code loop.")
while True:
    try:
        # read from all workers and the coordinator (workers + [s] just appends s to end of client list)
        # write to the workers
        # excepts is empty
        # select will return a subset of the inputs
        reads, writes, excepts = select.select(workers + [s] + clients, workers + clients, [])

        # loop through sockets to read
        for current in reads:
            if current is s:  # incoming client
                # accept connection and input it into list
                (new_sock, new_address) = s.accept()
                print(" * New client @ ", new_address)
                classify = new_sock.recv(1024)
                if classify.decode('utf-8') == "c":
                    clients.append(new_sock)
                    cli_dict.update({new_sock.fileno(): new_sock})
                elif classify.decode('utf-8') == "w":
                    workers.append(new_sock)
            else:  # message from a client
                # receive data
                data = current.recv(2048)
                if data != b'':
                    data = parsedata(data)
                    print(data)
                    for dt in data:
                        if dt != '':
                            print("Received data ", dt)
                            # now decode the data using protocol
                            proto_decode(dt, current)
        # run world's worst time-out
        ww_timeout()

    except KeyboardInterrupt:
        print("Keyboard Interrupt...")
        s.close()
        sys.exit(0)

    except ConnectionError as e:
        print("You're probably seeing this because you disconnected from this socket forcefully")
        print(e)
        s.close()
        sys.exit(0)

    except JSONDecodeError as e:
        print("Error Decoding JSON")
        print("You're probably seeing this because you disconnected from this socket forcefully, or the coordinator received a partial message")
        print(e)
        s.close()
        sys.exit(0)

    except Exception as e:
        print("Something happened.")
        print(e)
