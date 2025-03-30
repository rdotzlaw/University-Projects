# Assigment 1
#### Ryan Dotzlaw

## Running and Compilation
Run the worker(s) with:

    python3 a1-worker.py [port number] 

Run the coordinator with:

    python3 a1-coord.py [port number] [address:port](repeated for all the workers) 

Run the client(s) with:

    python3 a1-client.py [address:port](of coordinator) 



## Description
### Worker
* All workers start before coordinator.


* While my workers don't work by connecting to the coordinator, you could, in theory add more workers while the coordinator is running
by connecting to the coordinator and sending "*w*" to let it know you're a worker.


* The workers store the database; a simple dictionary, and take extra time when performing GET or COMMIT operations to simulate a real workload.
### Coordinator
* Coordinator connects to the workers using the command line inputs.


* The coordinator handles the logic and consistency, ensuring that all workers are available using the VOTE message. If any worker is unable to respond, then the SET fails. (Workers that time out are removed from worker pool).

### Client
* Client connects to coordinator and sends it commands


* Immediately after connecting to the coordinator the client sends "*c*" to let the coordinator know a client connected.


* Depending on the command, coordinator will send relevant signals to workers to perform a 2 phase commit, get data, etcetera.


* The client also features the following commands:
  * get [key]: Gets a k/v pair from the database
  * set [key] [val]: Sets a k/v pair in the database
  * db: Gets the whole database
  * verify: Sends a verify command to the coordinator which will let the client know if the database is consistent
  * rapidset [count]: Performs [count] SET operations on the same key (k: 1234), sequentially, and ensures that the first SET operation was completed successfully (checks for k/v pair 1234: 0)
  * multiset [count]: Performs [count] SET operations on different keys in quick succession, then calls *verify* to ensure the database remains consistent

** rapidset and multiset have a limit [count] == 10, because the workers were giving the following error when it exceeded 10:
* [WinError 10053] An established connection was aborted by the software in your host machine
# Protocol

## Worker
#### To coordinator:
* { type: VOTE-RESPONSE, vote: [true/false] msg: [Vote request message], sender: [client]}
  * Notify the coordinator if the previously sent change is OK (true for yes, false for no)


* { type: COMMIT-RESPONSE, success: [true/false], msg: [ Coord Commit message], sender: [client] }
  * Notifies coordinator if a commit is successful or fails (idk how it could fail, its JSON, man)


* { type: SEND, key: [key], val: [val/NULL] }
  * A response to the coordinator's GET message


* { type: GET-DB, data: {DB}, sender: [client] }
  * Returns the whole database in JSON format (DB as k/v pairs)


* { type: VERIFY-RESPONSE, data: {DB}, sender: [client], msg: [message] }
  * Returns the whole database for the purpose of verification


//--------------------------------------------------------------


## Coordinator
#### To Client:
* { type: SET-RESPONSE, success: [true/false] }
  * A response to the client letting them know if their set worked or failed 
  

* { type: GET-RESPONSE, key: [key], val: [val/NULL] }
  * A response to the client with the k/v pair or k/NULL if it doesn't exist in DB 


* { type: DB-RESPONSE, data: {DB} }
  * A copy of the database in dictionary form


* { type: VERIFY-RESPONSE, success: [true/false] }
  * A response to the client letting them know if all the databases are consistent across all workers


#### To Worker:
* { type: VOTE-REQUEST, action: [SET], key: [key], val: [val] }
  * A request to the worker to see if the action is possible
  

* { type: COMMIT, action: [SET], key: [key], val: [val] }
  * Tells the worker do go ahead and perform the action on the database
  

* { type: GET, key: [key] client: [client]}
  * Requests the value of the [key] entry in the database from the worker


* { type: GET-DB, sender: [client]}
  * Request the whole database as k/v pairs


* { type: VERIFY, sender: [client]}
  * Requests the whole database, but also requests a VERIFY-RESPONSE type response

//--------------------------------------------------------------


## Client
#### To Coordinator:
* { type: GET, key: [key] }
  * Requests the value of the [key] entry in the database from the coordinator, (coordinator needs to relay to a worker)


* { type: SET, key: [key], value: [val] }
  * Requests the setting of [key] to [val] in all the worker databases


* { type: GET-DB }
  * Returns the whole database in JSON format (DB as k/v pairs)


* { type: VERIFY }
  * Verifies to ensure all databases on the workers are equal