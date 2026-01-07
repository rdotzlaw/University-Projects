public class Doc extends Obj{

    private List content; //list of strings
    private final String creator;
    private List history; //list of docs
    private int time; //time starts at 0, 1st command is @ time 0

    public Doc(String id, String c, int t) {
        super(id);
        this.creator = c;
        this.content = new List();
        this.time = t;
        this.history = new List();
    }

    public Doc(String id, List content, String creator, int time) {
        super(id);
        this.content = content;
        this.creator = creator;
        this.time = time;
        this.history = new List();
    }



    public void print(){
        content.print();
    }

    private void save(){
        //deep copy all objects in content list to create a new list object
        //then create a Doc object and append it to the end of the history list
        Doc d;
        List c = null;
        c = content.deepCopy(); //creates a new list, that contains the same stuff, but isn't tied by pointers
        //since the list content is only of entity type, there is no need to create more versions of deepCopy() for other types
        d = new Doc(super.getId(), c, this.creator, time); //creates a new doc using the unlinked content
        this.history.append(new Node(d)); //saves the new unlinked doc to the history list
    }

    public void setTime(int t){ //should be called before each operation on a doc in the wiki class
        //save content into history, then update time for current instance
        this.save();
        this.time = t;
    }

    public void delete(int i){
        content.delete(i);
    }

    public void replace(int i, Entity line){
        content.replace(i, line);
    }

    public List getContent(){
        return content;
    }
    public List getHistory(){
        return history;
    }

    public int getTime(){
        return time;
    }

    //returns true if the history.head.getData().getTime() <= t
    public boolean exist(int t){
        boolean result = false;
        Node n = history.getHead();
        if(n != null && n.getData() instanceof Doc) {
            result = ((Doc) n.getData()).getTime() <= t;
        }
        return result;
    }

    //set content to history's (t)th node, which contains this.content at time t
    //When restoring to a t >= time for most recent change,
    //restores the 2nd most recent change
    //for restore(5), when looking thru history, there is no backup for t=5
    //this is b/c the content for t=5 is stored in the current doc, meaning there is no need for a backup
    public void restore(int t){
        if(t < this.time) { //if t >= this.time, no changes have been made since time
            //so the most recent edit will be in this.content
            Doc restorePoint = this.history.searchByTime(t);
            if (restorePoint != null) {
                this.content = restorePoint.content; //overwrite this content with content from time t
            }
        }
    }

    public void appendContent(String line){
        content.append(new Node(new Entity(line)));

    }
}
