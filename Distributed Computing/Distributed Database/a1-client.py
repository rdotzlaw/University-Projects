import cmd
import sys
import re
import json
import socket


class TestShell(cmd.Cmd):
    intro = 'Welcome to the 3010 verifier shell.   Type help or ? to list commands.\n'
    prompt = '3010 > '
    coordinatorSock = None

    def preloop(self) -> None:
        '''
        Connect to the coordinator
        '''
        try:
            print("Connecting to " + sys.argv[1])
            self.coordinatorSock = socket.socket(
                socket.AF_INET, socket.SOCK_STREAM)
            parts = sys.argv[1].split(':')
            # there is a more pythonic way. Not today!
            who = (parts[0], int(parts[1]))
            self.coordinatorSock.connect(who)
            self.coordinatorSock.sendall("c".encode())
        except Exception as e:
            print("Could not connect. Quitting")
            print(e)
            sys.exit(1)

    def do_set(self, arg):
        '''
        Set a value in the database: set key value
        '''
        matches = re.match("(\S+)\s+(\S+)", arg)
        if matches is None:
            print("set requires two arguments: the key and value")
        else:
            key = matches.group(1)
            value = matches.group(2)

            try:
                content = {"type": "SET", "key": key, "val": value}

                self.coordinatorSock.sendall(json.dumps(content).encode())
                # wait for reply
                print(self.coordinatorSock.recv(1024))
            except Exception as e:
                print("Error sending/receiving")
                print(e)

    def do_db(self, arg):
        '''
        Get the whole database (from a random worker)
        '''
        if len(arg) == 0:
            try:
                content = {"type": "GET-DB"}

                self.coordinatorSock.sendall(json.dumps(content).encode())
                # wait for reply
                print(self.coordinatorSock.recv(1024))
            except Exception as e:
                print("Error sending/receiving")
                print(e)

    def do_rapidset(self, arg):
        '''
        Performs a series of rapid SETs on the same key as a test
        Takes an integer as input (the number of tests), or defaults to 10 tests
        The successful SET-RESPONSE should be at/near the end of the received messages, since it actually takes time
        ** There is an upper limit to the number of tests (24-ish), if the number is too high then the coordinator may receive partial messages
        '''
        if arg == '' or int(arg) >= 24 or int(arg) <= 0:
            arg = 10
        try:
            # t = time.time()
            for i in range(0, int(arg)):
                content = {"type": "SET", "key": 1234, "val": i}
                # The "#$#" is there as a delimiter for the coordinator to help separate messages from clients
                self.coordinatorSock.sendall((json.dumps(content) + "#$#").encode())
            for i in range(0, int(arg)):
                # wait for reply
                reply = self.coordinatorSock.recv(1024)
                print(reply)
                if i < int(arg) - 1:
                    assert not json.loads(reply.decode())["success"]
                else:
                    succ = json.loads(reply.decode())
                    assert succ["success"]
                    # last response should succeed (since it has to actually do something instead of being rejected)
                    # and last response should create a k/v pair with a v == 0
                    self.coordinatorSock.sendall((json.dumps({"type": "GET", "key": 1234}) + "#$#").encode())
                    reply = self.coordinatorSock.recv(1024)
                    assert int(json.loads(reply.decode())["val"]) == 0

        except Exception as e:
            print("Error sending/receiving")
            print(e)

    def do_multiset(self, arg):
        '''
        SETs multiple keys in the database, then verifies to ensure consistency remains
        '''
        if arg == '' or int(arg) >= 11 or int(arg) <= 0:
            arg = 10

        try:
            for i in range(0, int(arg)):
                content = {"type": "SET", "key": i, "val": i}
                # The "#$#" is there as a delimiter for the coordinator to help separate messages from clients
                self.coordinatorSock.sendall((json.dumps(content) + "#$#").encode())
            for i in range(0, int(arg)):
                # wait for reply
                reply = self.coordinatorSock.recv(1024)
                print(reply)
                assert json.loads(reply.decode())["success"]
            # verify consistency
            self.do_verify(self)

        except Exception as e:
            print("Error sending/receiving")
            print(e)

    def do_verify(self, arg):
        '''
        Verifies to ensure all databases on the workers are equal
        '''
        try:
            content = {"type": "VERIFY"}

            self.coordinatorSock.sendall(json.dumps(content).encode())
            # wait for reply
            reply = self.coordinatorSock.recv(1024)
            print(reply)
            assert json.loads(reply.decode())["success"]

        except Exception as e:
            print("Error sending/receiving")
            print(e)

    def do_get(self, arg):
        '''
        Get a value in the database: get key value
        '''

        if len(arg) == 0:
            print("get requires two arguments: the key and value")
        else:

            try:
                content = {"type": "GET", "key": arg}

                self.coordinatorSock.sendall(json.dumps(content).encode())
                # wait for reply
                print(self.coordinatorSock.recv(1024))
            except Exception as e:
                print("Error sending/receiving")
                print(e)

    def do_exit(self, arg):
        print('Later, gator.')
        return True

    def do_EOF(self, arg):
        print('The cool way to exit!')
        return True

    def postloop(self) -> None:
        try:
            if self.coordinatorSock is not None:
                self.coordinatorSock.close()
        except:
            print("Failed to ")


if __name__ == '__main__':
    TestShell().cmdloop()
