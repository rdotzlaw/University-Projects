public class Wiki {


    private List users;
    private List docs;
    //time counter for wiki, used for keeping track of when stuff happened
    public int time;


    public Wiki(){
        users = new List();
        docs = new List();
        time = 0;
    }

    /* OLD CODE
    //runs each time cmd is executed, keeps track of history and time
    public void log(String id, String dId, String cmd){
        Obj data = new Log(id, dId, cmd, time);
        Node n = new Node(data);
        logs.append(n);
        time++;
    } */

    public List getUsers() {
        return users;
    }

    public List getDocs() {
        return docs;
    }

    public int getTime() {
        return time;
    }

    //is run from wikiIn after each command,
    public void log(String id, String dId, String cmd){
        Node u = users.search(id);
        Node d = docs.search(dId);
        if(u != null && u.getData() instanceof User) {
            ((User) u.getData()).log(cmd);
        }
        if(d != null && d.getData() instanceof Doc){
            ((Doc) d.getData()).log(cmd);
        }
        time++;
    }

    public void createUser(String id) {
        if (users.search(id) == null) {
            users.append(new Node(new User(id)));
            System.out.println("CONFIRMED");

        } else {
            System.out.println("DUPLICATE");
        }
        //time++;
    }

    public void createDoc(String dId, String uId){
        if(docs.search(dId) == null){ //doc doesnt exist
            if(users.search(uId) != null) { //user exists
                docs.append(new Node(new Doc(dId, uId, time)));
                System.out.println("CONFIRMED");
            } else {
                System.out.println("NOT FOUND");
            }
        } else {
            System.out.println("DUPLICATE");
        }
        //time++;

    }

    public void appendToDoc(String dId, String uId, String content){
        Node dNode = docs.search(dId);
        Node uNode = users.search(uId);
        if(dNode != null && uNode != null){
            //Node c = new Node(new Entity(content));
            if( dNode.getData() instanceof Doc) {
                Doc d = (Doc)(dNode.getData()); //cast entity to doc
                d.setTime(time); //makes backup
                d.appendContent(content); //append content to content list in doc d

                System.out.println("SUCCESS");
            }
        } else {
            System.out.println("NOT FOUND");
        }
        //time++; //even failure increments time
    }

    public void replace(String dId, String uId, int lNum, String content){
        Node d = docs.search(dId);
        Node u = users.search(uId);
        if(d != null && u != null) { //doc and user exist
            if(d.getData() instanceof Doc){
                ((Doc) d.getData()).setTime(time);
                ((Doc) d.getData()).replace(lNum, new Entity(content));
            }
        } else {
            System.out.println("NOT FOUND");
        }
        //time++;
    }

    public void delete(String dId, String uId, int lNum){
        Node d = docs.search(dId);
        Node u = users.search(uId);
        if(d != null && u != null){ //doc and user exist
            if(d.getData() instanceof Doc){
                ((Doc) d.getData()).setTime(time);
                ((Doc) d.getData()).delete(lNum); //call doc's delete func
            }
        } else {
            System.out.println("NOT FOUND");
        }
        //time++;
    }

    public void print(String dId){
        if(docs.search(dId) != null) { //doc exists
            Entity e = docs.search(dId).getData();
            docs.search(dId).getData().print(); //since doc dynamic type, will run doc's print()
            //doesn't modify doc, no need to save or update time
        } else {
            System.out.println("NOT FOUND");
        }
        //time++;
    }

    //NOT FOUND if dId or uId dont match and doc/user
    //otherwise, restore doc to version at time t, and print out doc @ time t, or doc at time t+1 if edit was made at t
    public void restore(String uId, String dId, int t){
        Node u = users.search(uId);
        Node d = docs.search(dId);
        if(u != null && d != null){
            if(d.getData() instanceof Doc){
                if(((Doc) d.getData()).exist(t))
                    ((Doc) d.getData()).restore(t);
                else
                    System.out.println("NOT FOUND");
            }
        } else { //returns not found if doc doesnt exist at given time
            System.out.println("NOT FOUND");
        }
    }

    public void history(String dId) {
        printLogs(dId, docs);
        //time++;
    }

    public void report(String uId){
        printLogs(uId, users);
        //time++;
    }

    public void printLogs(String id, List list) {
        if(list.search(id) != null) {
            Entity e = list.search(id).getData();
            if (e.checkId(id)) {
                if (e instanceof Obj) {
                    ((Obj) e).printLogs();
                }
            }
        } else {
            System.out.println("NOT FOUND");
        }
    }

    //Quit command is implemented in WikiIn
    //Comments are managed in WikiIn

}
