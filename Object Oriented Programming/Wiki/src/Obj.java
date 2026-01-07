public class Obj extends Entity{

    private List logs; //list of logs for this entity

    public Obj(String id, List logs) {
        super(id);
        this.logs = logs;
    }
    public Obj(String id){
        super(id);
        this.logs = new List();
    }

    public List getLogs() {
        return logs;
    }

    public void printLogs(){
        logs.print();
    }

    public void log(String cmd){ //cmd is the input line from the file
        Node n = new Node(new Entity(cmd));
        logs.append(n);
    }
}
