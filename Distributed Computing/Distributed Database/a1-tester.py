import json
import socket
import sys

try:
    print("Connecting to " + sys.argv[1])
    coordinatorSock = socket.socket(
        socket.AF_INET, socket.SOCK_STREAM)
    parts = sys.argv[1].split(':')
    # there is a more pythonic way. Not today!
    who = (parts[0], int(parts[1]))
    coordinatorSock.connect(who)
    coordinatorSock.setblocking(True)
except Exception as e:
    print("Could not connect. Quitting")
    print(e)
    sys.exit(1)

print("Tester will now begin testing...")

print(" * Testing basic functionality")

# Get returns null when no value
c1 = {"type": "GET", "key": "key"}
print(c1)
# "#$#" is there as a delimiter, because sometimes coordinator receives multiple 'sends' combined
coordinatorSock.sendall((json.dumps(c1) + "#$#").encode())
r = coordinatorSock.recv(1024)
print(r)
d1 = json.loads(r.decode())
print(d1)
assert d1 == {"type": "GET-RESPONSE", "key": "key", "val": None}

# Setting a value
c1 = {"type": "SET", "key": "key", "val": "value"}
print(c1)
# "#$#" is there as a delimiter, because sometimes coordinator receives multiple 'sends' combined
coordinatorSock.sendall((json.dumps(c1) + "#$#").encode())
d1 = json.loads(coordinatorSock.recv(1024).decode())
print(d1)
assert d1 == {"type": "SET-RESPONSE", "success": True}

# Getting the database

c1 = {"type": "GET-DB"}
print(c1)
# "#$#" is there as a delimiter, because sometimes coordinator receives multiple 'sends' combined
coordinatorSock.sendall((json.dumps(c1) + "#$#").encode())
d1 = json.loads(coordinatorSock.recv(1024).decode())
print(d1)
assert d1 == {"type": "DB-RESPONSE", "data": {"key": "value"}}

# Get returns the correct k/v pair


# Rapidly Sending Two SETS
print(" * Sending two rapid SETs")
c1 = {"type": "SET", "key": "key", "val": "value1"}
c2 = {"type": "SET", "key": "key", "val": "value2"}
print(c1)
print(c2)

# "#$#" is there as a delimiter
coordinatorSock.sendall((json.dumps(c1) + "#$#").encode())
coordinatorSock.sendall((json.dumps(c2) + "#$#").encode())

d1 = json.loads(coordinatorSock.recv(1024).decode())
d2 = json.loads(coordinatorSock.recv(1024).decode())

print(d1)
print(d2)

# Due to the time that it will take for the valid SET request to complete,
# the 1st response should be a failure
assert d1 == {"type": "SET-RESPONSE", "success": False}
assert d2 == {"type": "SET-RESPONSE", "success": True}






