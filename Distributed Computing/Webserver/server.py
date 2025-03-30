import socket
import threading
import sys
import os
import uuid
import json
import tempfile

'''

    Server Design
    
    Thread to read in requests from sockets
    -> each request create a new thread to deal w/ request
        -> /api/ requests are for the memo api
        -> all other requests are searching for files on the local file system


    API Paths
        GET /api/memo - get a list of all memos
        POST /api/memo - create a new memo on the server. This message will have a body that is json.
        PUT /api/memo/<id> - update a memo on the server. This message will have a body that is json. (This is for testing only)
        DELETE /api/memo/<id> - delete a memo from the server
    Only users with cookies should be able to do anything with memos.


    MEMO JSON
    {
        id: <memo id>,
        content: <content of memo>,
        last-edit: <cookie id of last editor>
    
    }



'''
# cookie_id = 0
threads = []


def get_all(header, response, data):
    header = 'HTTP/1.1 200 OK\n'

    # client should've sent a cookie, b/c they'd get one when first connecting to website
    # don't need to worry about it here, it happens near the end of handle()

    path = r"memos"

    response += "[".encode('utf-8')

    i = 0

    for memo in os.listdir(path):
        if not i == 0:
            response += ",".encode('utf-8')
        file = open(path + "/" + memo, "rb")
        response += file.read()
        file.close()
        i += 1

    response += "]".encode('utf-8')
    response += "\n".encode('utf-8')  # this needs to be added twice, or it doesn't show up at all
    # im not kidding, and i don't understand why

    header += "Content-Type: " + "text/json" + '\n'
    return header, response


def post_memo(header, response, data, memo_id):
    header = 'HTTP/1.1 200 OK\n'

    try:
        # create new memo w/ id
        memo = open("memos/" + str(memo_id), 'w')
        # fill w/ data from body -> all body of request (data variable) is what we put in

        mem = extract_body(data)

        # print(data_list)
        # find cookie
        sesh, flag = check_cookie(data)
        if flag:  # if cookie is there
            # print(sesh.encode())
            final_data = "{\n \"id\": " + str(memo_id) + ",\n" + \
                         "\"memo\": \"" + mem + "\",\n" + \
                         "\"last-edit\": \"" + sesh + "\"\n}"

            memo.write(final_data)
            memo.close()

            pers = open("persistence", "w")  # overwrite persistence value
            pers.write(str(memo_id))
            pers.close()

        else:  # missing cookie, forbidden from doing stuff, man
            print("Request missing cookie, man")
            header, response = err_403("", "")

    except FileExistsError:  # file already exists, wtf??
        print("File's already here, man")
        header, response = err_403("", "")

    return header, response


def extract_body(data):
    # get the memo from the body of data
    if data.__contains__('\r'):
        # do it the stupid way
        data_list = data.split('\r\n\r\n')
        header_list = data_list[0].split("\r\n")
        mem = data_list[1]

    else:  # do it the normal way, like a sane person
        data_list = data.split('\n\n')
        header_list = data_list[0].split("\n")
        mem = data_list[1]
    return mem


def delete_memo(header, response, data, mid):
    cookie, has_cookie = check_cookie(data)

    if has_cookie:
        flag, m_path = find_memo(mid)
        if flag:
            # found the file, it's currently open
            os.remove(m_path)
            header = 'HTTP/1.1 200 OK\n'
            return header, "".encode("utf-8")
        else:
            # no file w/ id found
            return err_404("", "")

    else:
        # forbidden
        print("no cookie")
        return err_403("", "")


def find_memo(mid):
    # find the memo with id == 'id'
    path = r"memos"
    m_path = ""
    flag = False
    for memo in os.listdir(path):
        m_path = path + "/" + memo
        file = open(path + "/" + memo, "r")
        jMemo = json.loads(file.read())
        if jMemo["id"] == mid:
            flag = True
            file.close()
            break
        file.close()
    return flag, m_path


def put_memo(header, response, data, mid):
    cookie, has_cookie = check_cookie(data)

    if has_cookie:
        # find the memo that shares id's with the data body
        # then replace that file's memo and last-edit w/ the data body and cookie
        flag, m_path = find_memo(mid)

        if flag:
            # found memo
            file = open(m_path, "rw")
            # replace contents of file w/ updated contents
            jFile = json.loads(file.read())
            mem = extract_body(data)

            new_j = "{\n \"id\": " + jFile["id"] + ",\n" + \
                    "\"memo\": \"" + mem + "\",\n" + \
                    "\"last-edit\": \"" + cookie + "\"\n}"
            file.write(new_j)
            file.close()
        else:
            # no memo
            return err_404("", "")

    else:
        print("missing cookie")
        return err_403("", "")


def check_cookie(data):
    # checks data, the whole request from a client, for a Session cookie

    if data.__contains__('\r'):
        # do it the stupid way
        data_list = data.split('\r\n\r\n')
        header_list = data_list[0].split("\r\n")

    else:  # do it the normal way, like a sane person
        data_list = data.split('\n\n')
        header_list = data_list[0].split("\n")

    sesh = ""
    flag = False
    for l in header_list:
        if l.startswith("Cookie") and l.__contains__("Session="):
            # flag = True
            cookie_list = l.split(" ")
            for c in cookie_list:
                # print("here")
                # print(c)
                if c.startswith("Session="):
                    flag = True
                    sesh = c.split("=")[1].split(";")[0].split("\n")[
                        0]  # get what comes after 'Session', and remove any ';' chars
                    break
            break
    return sesh, flag


# does any final checks on header
# adding the cookies if the client had them and adding content length
def check_header(data, header, response):
    # check for tracking cookie in request header
    # if no cookie, assign one
    sesh, flag = check_cookie(data)
    if flag:
        header += "Cookie: Session=" + sesh + "\n"
    else:
        # create new uuid
        header += "Set-Cookie: Session=" + str(uuid.uuid4()) + "\n"

    header += "Content-Length: " + str(len(response)) + "\n"
    # now that cookie is added to response, header is done
    header += '\n'
    return header


def err_404(header, response):
    header = 'HTTP/1.1 404 Not Found\n'
    response = '''
                <html>
                    <body>
                        <h3 align="center"> Error 404: File not found </h3>
                    </body>
                </html>
            '''.encode('utf-8')
    return header, response


def err_403(header, response):
    header = 'HTTP/1.1 403 Forbidden\n'
    response = '''
                <html>
                    <body>
                        <h3 align="center"> Error 403: Forbidden </h3>
                    </body>
                </html>
            '''.encode('utf-8')
    return header, response


def handle(sock):
    # has a socket connected to client

    data = sock.recv(2048).decode('utf-8')
    print(data)
    header = ""
    response = "".encode('utf-8')

    req_list = data.split(' ')  # split by spaces
    # req_list[0] is method (GET, POST, etc)
    # req_list[1] is path

    '''
        GET /folder/something.html HTTP:1.1
    
    '''


    # handle request
    # if no /api/, then just get files from file system
    # if /api/ then do api things

    # if they don't have a cookie
    # get -> return with a cookie
    # others -> denied (error 401)

    if req_list[1].__contains__("/api/"):
        # then do api stuff
        if req_list[0] == "GET":
            # get a list of all memos
            header, response = get_all(header, response, data)
        elif req_list[0] == "POST":
            # create new memo on server, data's body is the memo content
            # read persistence for current memo_id
            pers = open("persistence", "r")
            memo_id = int(pers.read()) + 1
            pers.close()
            header, response = post_memo(header, response, data, memo_id)
        elif req_list[0] == "PUT":
            # update memo on server, data's body will be the updated content
            path_list = req_list[1].split('/')
            id = int(path_list[len(path_list) - 1])
            header, response = put_memo(header, response, data, id)
        elif req_list[0] == "DELETE":
            # remove the memo with that id from the server
            path_list = req_list[1].split('/')
            id = int(path_list[len(path_list) - 1])
            header, response = delete_memo(header, response, data, id)
        else:
            # api path doesn't exist
            header, response = err_403(header, response)
    else:
        # grab files from file system
        # doesn't matter if it's a get/post/put/etc, just grab the file and return it
        # should be no query strings, but get rid of them just in case
        file_name = req_list[1].split('?')[0]  # take everything before a '?'
        if not file_name == '/':
            file_name = "files" + file_name  # removes the first '/' character only
        else:
            file_name = "files/index.html"
        print(file_name)
        try:
            file = open(file_name, 'rb')  # read bytes
            response = None
            response = file.read()  # read the whole thang
            file.close()

            # print(response)

            header = 'HTTP/1.1 200 OK\n'

            if file_name.endswith(".jpeg"):
                c_type = 'image/jpeg'
            elif file_name.endswith(".png"):
                c_type = "image/png"
            elif file_name.endswith(".js"):
                c_type = "text/javascript"
            else:
                c_type = 'text/html'

            header += "Content-Type: " + str(c_type) + '\n'


        except FileNotFoundError:
            header, response = err_404(header, response)

    header = check_header(data, header, response)
    print(header)

    # build final response
    final = header.encode('utf-8')
    final += response

    # send response
    # response = 'HTTP/1.1 200 OK\r\nConnection: Close\r\nContent-Length: 11\r\n\r\nHello world'.encode()
    # print(final)
    sock.send(final)


    # close socket
    sock.close()
    # end thread


def server():
    # run the server
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    # bind to local host at given port
    s.bind(('127.0.0.1', 8139))

    while True:
        try:
            print("threads: ", len(threads))

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
                    threads.remove(t)

        except KeyboardInterrupt:
            print("keyboard interrupt, handling current threads, then closing")
            break

        except Exception as e:
            print("Something happened.")
            print(e)
            # then continue, maybe send error to client?

    s.close()
    return


if __name__ == '__main__':
    server()
