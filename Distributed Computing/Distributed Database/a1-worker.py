import select
import socket
import sys
import json
import time
import random
from json import JSONDecodeError

database = {
    "a": "one",
    "b": "two",
    "c": "three",
    "d": "four",
    "e": "five"
}

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

s.bind(('localhost', int(sys.argv[1])))

s.listen()

# coordinator connects
conn, addr = s.accept()

print("Connected @ ", addr)


def proto_decode(d):
    print(" * Dealing with message")
    jdata = json.loads(d)
    dtype = jdata["type"]
    print(" * Message type: ", dtype)

    if dtype == "VOTE-REQUEST":
        # I see no reason why a worker should send a 'false' vote
        message = {"type": "VOTE-RESPONSE", "vote": True, "msg": jdata}
        conn.sendall((json.dumps(message) + "#$#").encode())

    elif dtype == "COMMIT":
        time.sleep(random.random() / 8 + 0.01)
        # set the k/v pair or add it if it isn't there
        database.update({jdata["key"]: jdata["val"]})
        # if there was an actual database, this could fail, but im 99% sure this can never fail
        message = {"type": "COMMIT-RESPONSE", "success": True, "msg": jdata}
        conn.sendall((json.dumps(message) + "#$#").encode())

    elif dtype == "GET":
        # get the data and send it
        time.sleep(random.random() / 8 + 0.01)
        k = jdata["key"]
        message = {"type": "SEND", "key": k, "val": database.get(k),
                   "sender": jdata["sender"]}  # will return none if not there
        conn.sendall((json.dumps(message) + "#$#").encode())

    elif dtype == "GET-DB":
        # return whole db
        message = {"type": "SEND-DB", "data": database, "sender": jdata["sender"]}
        conn.sendall((json.dumps(message) + "#$#").encode())

    elif dtype == "VERIFY":
        # return whole db
        message = {"type": "VERIFY-RESPONSE", "data": database, "sender": jdata["sender"], "msg": jdata}
        conn.sendall((json.dumps(message) + "#$#").encode())

    else:
        print("Bad Request")


def parsedata(data):
    return data.decode().split("#$#")


# main worker loop
print("Worker done initializing, starting main loop.")
while True:
    try:
        reads, writes, excepts = select.select([conn] + [s], [conn] + [s], [])

        # loop through sockets to read
        for current in reads:
            # receive data
            """data = current.recv(2048)
            print("Received data ", data)
            # now decode the data using protocol
            proto_decode(data)
        """
            data = current.recv(1024)
            if data != b'':
                data = parsedata(data)
                print(data)
                for dt in data:
                    if dt != '':
                        print("Received data ", dt)
                        # now decode the data using protocol
                        proto_decode(dt)

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
        print("You're probably seeing this because you disconnected from this socket forcefully")
        print(e)
        s.close()
        sys.exit(0)

    except Exception as e:
        print("Something happened.")
        print(e)
