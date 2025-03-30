import hashlib
import json
import socket
import sys
import time
import uuid
from json import JSONDecodeError
from math import floor
import random

chain_height = 0
temp_chain = {}
chain_built = False
chain = []  # hold the majority blockchain
# format [ {"height": height, "minedBy": miner, "nonce": non, "messages": [...], "hash": hash},
#          ...
#        ]
stats = {}  # keeps track of stats of which blockchain is the 'best'
# format { (hash, height) : [(p1_host, p1_port), (p2_host, p2_port), ... ],
#           ...
#        }
peers_to_get = []

stats_time = 0
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

host = socket.gethostname()
print(host)
port = 8139
name = "r.zlaw"
s.bind((host, port))
TIMOUT = 30
s.settimeout(30)  # every 30s perform timeout event (ping wellknown)

peers = {}
# format:
# {
#  (host, port): { "host": host, "port": port, "name": name, "time": <Time last flood recv>},
#  ...
# }
# wellknown = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
# wellknown.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
wellknown = (socket.gethostbyname('silicon.cs.umanitoba.ca'), 8999)
print(wellknown)

id = str(uuid.uuid4())


def check_peer_timeout():
    # loops through peers list, checking the ["time"] attribute
    # if curr_time - ["time"] >= TIMOUT, remove from peers, otherwise, keep it
    curr_time = time.time()
    to_remove = []
    for p in peers:
        if curr_time - peers.get(p).get("time") >= (TIMOUT * 2):  # give grace period in case of peer having ping offset
            to_remove.append(p)
            print("Removed peer: " + str(p))
    for x in to_remove:
        peers.pop(x)


def new_peer(h, p, n, tm):
    # create new dictionary for peers list according to inputs
    new_p = {
        "host": h,
        "port": p,
        "name": n,
        "time": tm
    }
    return new_p


def flood_reply(p):
    # send flood_reply to peer 'p'
    print("Sending FLOOD-REPLY to: " + str(p))
    f_reply = {
        "type": "FLOOD-REPLY",
        "host": host,
        "port": port,
        "name": name
    }
    s.sendto(json.dumps(f_reply).encode(), p)


def flood_peers(d, msg_type):
    # send 'd' to all peers
    print(" * * Forwarding {} to peers".format(msg_type))
    for p in peers:
        dict_peer = peers.get(p)
        out = (dict_peer["host"], dict_peer["port"])
        s.sendto(d, out)


def get_stat(addr_tuple):
    # (peer_h, peer_p) = addr_tuple
    # ask the peer for stats
    print(" * Asking {} for STATS".format(str(addr_tuple)))
    stats_msg = {
        "type": "STATS"
    }
    s.sendto(json.dumps(stats_msg).encode(), addr_tuple)


def send_stat(p):
    stats_reply = {
        "type": "STATS_REPLY",
        "height": len(chain) - 1,  # height starts at 0, so chain len - 1 is height
        "hash": chain[-1]["hash"]
    }

    s.sendto(json.dumps(stats_reply).encode(), p)


flood_list = {}  # { uuid: peer_tuple, ... }


def add_to_chain(block):
    global chain_height
    global chain_built
    global temp_chain
    # add the current block to the chain, if not done
    if not chain_built:
        if block["height"] == 0 and len(chain) == 0:
            # this is the first block
            chain.append(block)
        elif block["height"] == len(chain):
            # this is the next block in the sequence
            hashBase = hashlib.sha256()
            lastBlock = chain[len(chain) - 1]
            hashBase.update(lastBlock["hash"].encode())
            hashBase.update(block["minedBy"].encode())
            for msg in block["messages"]:
                hashBase.update(msg.encode())

            hashBase.update(str(block["nonce"]).encode())  # str() b/c someone had the nonce as an int
            hash = hashBase.hexdigest()
            if hash == block["hash"]:
                # check difficulty
                if hash[-1 * 8] != '0' * 8:
                    print("Block wasn't difficult enough: {}".format(hash))
                # add it to the chain
                chain.append(block)
                # chain_height = chain_height + 1
                if len(chain) != chain_height:
                    if temp_chain.get(len(chain)) is not None:
                        # if we have a value for the next block
                        # remove it, and add it to the chain
                        add_to_chain(temp_chain.pop(len(chain)))
                else:
                    chain_built = True
                    temp_chain = {}  # clear temp_chain
            else:
                # this block sux
                # get another block from a peer
                # in a working network, would only need 1, but whatever
                get_block(block["height"])
                get_block(block["height"])
                get_block(block["height"])
        else:
            # it's not the first block, and it ain't the next
            # but maybe we'll need it eventually
            # put it into temp_chain
            temp_chain[block["height"]] = block
            if temp_chain.get(len(chain)) is not None:
                # if we have a value for the next block
                # remove it, and add it to the chain
                add_to_chain(temp_chain.pop(len(chain)))
            else:
                # need sum blox
                get_block(len(chain))
                get_block(len(chain))


def print_chain():
    print("PRINTING THE CHAIN!!!! ")

    for b in chain:
        print(
            "{}: {}, {},\n\t {},\n\t {}".format(b["height"], b["minedBy"], b["nonce"], str(b["messages"]), b["hash"]))


def get_block(hi):
    # gets a single block from a random peer
    dat = {
        "type": "GET_BLOCK",
        "height": hi
    }
    s.sendto(json.dumps(dat).encode(), peers_to_get[random.randint(0, len(peers_to_get) - 1)])


def get_blocks(s_key):
    # ask all the peers in peers_to_get for blocks
    global chain_height
    global temp_chain
    (h, height) = s_key
    chain_height = height
    for i in range(0, height):
        dat = {
            "type": "GET_BLOCK",
            "height": i
        }
        s.sendto(json.dumps(dat).encode(), peers_to_get[i % (len(peers_to_get) - 1)])


def handle(dp_tuple):
    (d, p) = dp_tuple
    jdata = json.loads(d.decode('utf-8'))
    global chain
    global chain_built
    global consensus_time
    global peers_to_get
    global stats
    try:
        if jdata["type"] == "FLOOD" and jdata["id"] != id and flood_list.get(
                jdata["id"]) is None:  # if flood, and not our flood, and haven't dealt w/ it before
            # save peer data, if necessary
            flood_list[jdata["id"]] = p
            check_peer_timeout()  # remove peers
            if peers.get((jdata["host"], jdata["port"])) is None:  # don't find this peer in list, add it
                peers[(jdata["host"], jdata["port"])] = new_peer(jdata["host"], jdata["port"], jdata["name"],
                                                                 time.time())
                # if the peer isn't in our peers list, then we've never heard this flood before, so forward it
                flood_peers(d, "FLOOD")  # send 'd' (binary, encoded FLOOD) to all peers
                # if we haven't built a chain yet, then ask this peer for stats
                if len(chain) == 0:
                    get_stat((jdata["host"], jdata["port"]))
            flood_reply((jdata["host"], int(jdata["port"])))
        elif jdata["type"] == "FLOOD" and jdata["id"] != id and flood_list.get(jdata["id"]) is not None:
            print("Already dealt w/ FLOOD")
            # send flood reply to peer

        elif jdata["type"] == "FLOOD-REPLY":
            # add/update peer in peers
            if peers.get((jdata["host"], jdata["port"])) is None:  # don't find this peer in list, add it
                peers[(jdata["host"], jdata["port"])] = new_peer(jdata["host"], jdata["port"], jdata["name"],
                                                                 time.time())
                # if we haven't built a chain yet, then ask this peer for stats
                if len(chain) == 0:
                    get_stat((jdata["host"], jdata["port"]))
        elif jdata["type"] == "STATS":
            if chain_built:  # if we have a chain
                send_stat(p)  # send chain stats to 'p'

        elif jdata["type"] == "STATS_REPLY":
            # if stats_time == 0 and len(peers) > 4:
            # first time receiving a reply and connected to some peers already
            #    stats_time = time.time()
            # add to stats dict according to format
            if stats.get((jdata["hash"], jdata["height"])) is None:  # haven't got stats on this chain
                stats[(jdata["hash"], jdata["height"])] = []  # new list
                print("Adding {} to STATS new entry {}".format(str(p), str((jdata["hash"], jdata["height"]))))
                stats[(jdata["hash"], jdata["height"])].append(p)  # add the peer we got the data from to the stats
            else:
                if p not in stats[(jdata["hash"], jdata["height"])]:
                    stats[(jdata["hash"], jdata["height"])].append(p)
                    print("Adding {} to STATS".format(str(p)))

            # once a list in stats reaches half the peer count, choose that chain
            stat_count = 0
            big = ""
            flag = True
            s_list = []
            for stat in stats:
                (ha, hei) = stat
                stat_count = stat_count + len(stats.get(stat))
                s_list.append((hei, stat))

            if stat_count >= floor(2 * len(peers))/3:
                # 2/3rds of peers have replied ** HERE B/C people don't reply
                best_hi = 0
                best_key = None
                for elem in s_list:
                    (hi, stat) = elem
                    if len(stats.get(stat)) > len(stats.get(best_key)):
                        best_hi


            for stat in stats:
                big = stat
                stat_count = stat_count + len(stats.get(stat))
                if len(stats.get(stat)) > floor(len(peers) / 2):
                    peers_to_get = stats.get(stat)
                    # (h, hi) = stat
                    flag = False
                    get_blocks(stat)
                    break
            if flag and stat_count >= floor(len(peers))/2:
                # find longest stat list or break ties

                for stat in stats:
                    if len(stats.get(stat)) > len(stats.get(big)):
                        big = stat
                peers_to_get = stats.get(big)
                get_blocks(big)



        elif jdata["type"] == "GET_BLOCK":
            reply_block(jdata["height"], p)
        elif jdata["type"] == "GET_BLOCK_REPLY":
            # add the block to the chain, if it works

            block = {
                "height": jdata["height"],
                "minedBy": jdata["minedBy"],
                "nonce": jdata["nonce"],
                "messages": jdata["messages"],
                "hash": jdata["hash"]
            }
            add_to_chain(block)
        elif jdata["type"] == "ANNOUNCE":
            # new block is being added to the chain
            peers_to_get.append(p)
            block = {
                "height": jdata["height"],
                "minedBy": jdata["minedBy"],
                "nonce": jdata["nonce"],
                "messages": jdata["messages"],
                "hash": jdata["hash"]
            }
            add_to_chain(block)
        elif jdata["type"] == "CONSENSUS" and not chain_built:  # consensus message, and chain isn't building
            chain = []
            temp_chain = {}
            chain_built = False
            consensus_time = time.time()
            stats = {}
            flood_peers(json.dumps({"type": "STATS"}).encode(), "STATS")
    except KeyError:
        print("Invalid JSON: KeyError")
    except Exception as excep:
        print(excep)


def reply_block(hi, p):
    dat = {
        "type": "GET_BLOCK_REPLY",
        "height": None,
        "minedBy:": None,
        "nonce": None,
        "messages": None,
        "hash": None
    }
    if hi > len(chain) - 1 or hi < 0:  # height out of bounds
        # dont have
        s.sendto(json.dumps(dat).encode(), p)
    else:  # in bounds
        # search for block with height (even if in bounds, might not have it)
        for link in chain:
            if link["height"] == hi:
                # this is the block
                dat = {
                    "type": "GET_BLOCK_REPLY",
                    "height": link["height"],
                    "minedBy:": link["minedBy"],
                    "nonce": link["nonce"],
                    "messages": link["messages"],
                    "hash": link["hash"]
                }
                break
        s.sendto(json.dumps(dat).encode(), p)


def ping():
    # sends ping message to 'wellknown' address
    flood_msg = {
        "type": "FLOOD",
        "host": host,
        "port": port,
        "id": id,
        "name": name
    }
    s.sendto(json.dumps(flood_msg).encode(), wellknown)


last_timout = time.time()
consensus_time = time.time()
ping()  # join the network
while True:
    try:

        data, peer = s.recvfrom(1024)
        print("Incoming! ----> {}".format(data))
        print("Peers list size: {}".format(len(peers)))
        if data != b'' and data != b'{}':
            handle((data, peer))

        if time.time() - last_timout >= TIMOUT:
            print("Performing timeout...")
            flood_peers(json.dumps({
                "type": "FLOOD",
                "host": host,
                "port": port,
                "id": id,
                "name": name
            }).encode(),
                        "FLOOD")
            last_timout = time.time()
            flood_list = {}  # clear flood list
            id = str(uuid.uuid4())  # gen new uuid
        if time.time() - consensus_time >= (TIMOUT * 4):  # every 4 timeouts, rebuild blockchain (2 mins)
            chain = []
            chain_built = False
            consensus_time = time.time()
            stats = {}

        '''
        thr = threading.Thread(target=handle, args=((data, peer),))
        thr.daemon = False  # no abrupt shutdown
        threads.append(thr)
        thr.start()
        # no join, continue creating new sockets
        # once done clean up dead threads
        for t in threads:
            t.join()
        for t in threads:
            if not t.is_alive():
                threads.remove(t)
                print(" * Removing Thread")'''

        '''
        # listen for connection from client
        s.listen()
        # accept client connection
        (sock, (ip, port)) = s.accept()
        # create thread and pass socket to thread
        thr = threading.Thread(target=handle, args=(sock,))
        thr.daemon = False  # no abrupt shutdown
        threads.append(thr)
        thr.start()
        # no join, continue creating new sockets
        # once done clean up dead threads
        for t in threads:
            t.join()
        for t in threads:
            if not t.is_alive():
                threads.remove(t)'''

        '''
        reads, writes, excepts = select.select(peers + [s], peers, [], 2)

        for current in reads:
            if current is s:
                # someone is contacting ME
                data, peer = s.recvfrom(1024)
                print(data.decode('utf-8'))
                print(peer)'''


    except KeyboardInterrupt:
        print("Keyboard Interrupt...")

        for ent in stats:
            print("({}): {}".format(str(ent), str(stats.get(ent))))

        print_chain()

        print(len(chain))
        print(chain_height)

        for t in temp_chain:
            print(str(temp_chain.get(t)))


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

    except socket.timeout:
        # socket times out, do timeout things
        print("Performing Timeout")
        ping()

    '''except Exception as e:
        print("Something happened.")
        print(e)'''
