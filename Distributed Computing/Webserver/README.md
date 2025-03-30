
### Ryan Dotzlaw

## Part 1 - The Server
###Running the Server
The server can be run by using the following command:
            
        python3 server.py

This will start the server on the address: localhost:8139

### Server Methods
#### server
This method is the 'main' method of the server, it loops until it receives a keyboard interrupt, each loop searching for a request through the socket. 
Once a request is received, it will then create a thread that runs the handle method and pass the connection to the thread.

#### err_404
This method returns a (header, response) tuple containing a basic error 404 response, for when the server can't find a file in the file system, or the memo the user is trying to delete/put to doesn't exist
#### err_403
This method returns a (header, response) tuple containing a basic error 403 response, for when the user tries to make an API request without the valid credentials (a cookie called Session)

#### handle
This method does multiple things
* It parses the incoming request, either sending it to one of the API helper functions, or grabbing a file from the file system and putting it into the response body
* It will also call helper functions such as check_header to add important details to the response header
* Lastly, it will concatenate the header and body of the response, send it to the client, and then close the socket

#### check_cookie
This method checks a request for the existence of a cookie called 'Session'. Should it find Session, it will return a tuple of (Cookie["Session"], true), otherwise it returns ([Empty String], false)
#### check_header
This method helps add the finishing touches to a response header, adding Content-Length, and any Cookies (setting a cookie if needed)
#### put_memo
This method returns a (header, response) tuple, either header and response forming the header and body of a successful PUT response, or a 404/403 if the file isn't found or the PUT request didn't have any Session cookie
#### find_memo
This method searches through all the memos in the file system to find one that matches the given ID, if it finds one it returns (True, [File path]), otherwise it returns (False, [Empty String])
#### delete_memo
This method returns a (header, response) tuple, either header and response forming the header and body of a successful DELETE response, or a 404/403 if the file isn't found or the DELETE request didn't have any Session cookie
#### extract_body
This method takes the raw data from a request and separates the header from the body, either by spliting on '/r/n/r/n' or '/n/n', depending on which is valid.
#### post_memo
This method returns a (header, response) tuple, with the header being a 200 OK header, and response being empty, showing that it successfully created a new memo on the server. Provided the method didn't run into conditions that would lead to a 403 Error.
#### get_all
This method returns a (header, response) tuple, with the header being a 200 OK header, and response being the body of the response, which is a list of JSON memo objects



## Part 2 - The Client
### Running the Client
To run the client, travel to the following address with your browser:

        localhost:8139

This will allow you to use the website, and it's functionality. Along with using the memo API and searching the server's file system.

### Client Functionality
The client website has two parts:
1. The input field for posting a new memo
   * This can be used by entering a memo in the input field, then hitting the button next to it to submit the memo to the server.
   * These memos are persistent and stored in the 'memos' directory with an incrementing id automatically assigned
2. The list of all memos
   * By hitting the 'Get all Memos' button, an XMLHTTPRequest is sent to the server to grab all the memos using the memos API, the response is then used to create list elements for an unordered list, which is then updated without the page refreshing.

## Part 3 - The Client 2: Electric Boogaloo
The second client is a C program that will test the basic API functionality of the server

It does this by
 * Connecting to the server @localhost:8139
 * Sending a POST message
 * Sending a GET message to verify the memo was posted
 * Sending a DELETE message
 * Sending a PUT message to verify the memo was deleted

Clean the client2 binary with:

      make bin_clean


Compile the second client with the following command(s):

(The make clean is optional, see below)

      make clean
      make

The clean rule will remove any client2 binaries, empty the memos folder, and reset the persistence file.

This is so the client2 will be able to test the functionality without having to worry about other memos containing the same "memo" key/value pair (since this is how client2 checks to confirm the POST).

However, if you aren't worried about that, and your memo for client2 is unique, then the clean rule isn't necessary

From there, run the client while server.py is running using the following command

      ./client2 <Session ID> <The> <Parts> <Of> <The> <Memo> ...

The Session ID can be anything

What ever follows the Session ID is parsed into a single string, with each argument separated by spaces.
This is the memo that will be sent to server.py.