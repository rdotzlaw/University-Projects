
import org.junit.jupiter.api.Test;

public class TestWiki {

    /*
        TEST HELPER FUNCTIONS
     */
    //checks 2 lists and returns true if all node.data has the same id
    boolean listEquals(List l1, List l2){
        Node c1 = l1.getHead();
        Node c2 = l2.getHead();
        boolean result = true;
        while(c1 != null && c2 != null && result){
            String e1 = c1.getData().getId();
            String e2 = c2.getData().getId();
            if(!e1.equals(e2)){
                result = false;
            }
            c1 = c1.getNext();
            c2 = c2.getNext();
        }
        return result;
    }
    //makes a list of size i
    List fillList(int i){
       int x = 0;
       List result = new List();
       while(x < i){
           char j = (char)x;
           Entity e = new Entity(String.valueOf(x));
           result.append(new Node(e));
           x++;
       }
       return result;
    }


    /* ====================
        LIST TESTS
     */
    @Test
    void listAppend() {
        List l = new List();
        l.append(new Node(new Entity("1")));
        assert l.getHead() != null && l.getHead().getData().getId().equals("1");
    }
    @Test
    void listReplace1(){
        //replace 1st and replace last
        Node n1 = new Node(new Entity("1"));
        Entity e1 = new Entity("4");
        List l = new List(n1);
        l.replace(0, e1);
        assert l.getHead().getData().checkId("4");
    }
    @Test
    void listReplace2(){
        //replace mid

        Entity e1 = new Entity("4");
        List l = fillList(10);
        l.replace(2, e1);
        Node n1 = l.getHead();
        Node test = n1.getNext().getNext();
        assert test.getData().checkId("4");
    }
    @Test
    void listDeepCopy(){
        List l = fillList(10);
        List l2 = l.deepCopy();
        assert listEquals(l, l2);
    }
    @Test
    void listDelete1(){
        List l = fillList(10);
        l.delete(9);
        l.delete(8);
        List l2 = fillList(8);
        assert listEquals(l, l2);
    }



    /* ====================
        ENTITY TESTS
     */
    @Test
    void entityCheckId1(){
        Entity e = new Entity("AAA");
        assert e.checkId("AAA");
    }
    @Test
    void entityCheckId2(){
        Entity e = new Entity("AAA");
        assert !e.checkId("");
    }

    /* ====================
        OBJ TESTS
     */
    //Obj class only adds printLogs and log functions, both which return void,
    //nothing to test

    /* ====================
        USER TESTS
     */
    //User class is basically just a rename Entity class,
    //User adds nothing, but having Doc extend User would be weird,
    //So for easier understandability, both User and Entity class exist
    //Despite being the exact same (more or less)

    /* ====================
        DOC TESTS
     */
    //Doc delete, replace, and appendContent are just calling functions from the list class on doc.content
    //no need to test them
    @Test
    void docExist1(){
        //check exist for t < time
        Doc d = new Doc("1", "A", 5);
        d.setTime(6); //saves doc, so history.head != null
        assert !d.exist(4);
    }
    @Test
    void docExist2(){
        //check exist for t > time
        Doc d = new Doc("1", "A", 5);
        d.setTime(6); //saves doc, so history.head != null
        assert d.exist(10);
    }
    @Test
    void docSetTime(){
        Doc d = new Doc("1", "a", 0);
        //d.history is empty, with history.head == null
        d.setTime(1);
        assert d.getHistory().getHead() != null && d.getTime() == 1;
    }
    @Test
    void docRestore1(){
        //restore from the middle of history
        Doc d = new Doc("1", "A", 0);
        d.setTime(1); //mandatory for each action
        d.appendContent("l1");
        d.setTime(2);
        d.appendContent("l2");
        List cpy = d.getContent().deepCopy();
        d.setTime(3);
        d.appendContent("l3");
        d.restore(2);
        assert listEquals(cpy, d.getContent());
    }
    @Test
    void docRestore2(){
        //restore from the end of history (no change)
        Doc d = new Doc("1", "A", 0);
        d.setTime(1); //mandatory for each action
        d.appendContent("l1");
        d.setTime(2);
        d.appendContent("l2");
        d.setTime(3);
        d.appendContent("l3");
        List cpy = d.getContent().deepCopy();
        d.restore(3);
        assert listEquals(cpy, d.getContent());
    }
    @Test
    void docRestore3(){
        //restore from the start of history (empty)
        Doc d = new Doc("1", "A", 0);
        List cpy = d.getContent().deepCopy();
        d.setTime(1); //mandatory for each action
        d.appendContent("l1");
        d.setTime(2);
        d.appendContent("l2");
        d.setTime(3);
        d.appendContent("l3");
        d.restore(0);
        assert listEquals(cpy, d.getContent());
    }
    @Test
    void docRestore4(){
        //restore from beyond end of history (no change
        Doc d = new Doc("2", "A", 0);
        d.setTime(1); //mandatory for each action
        d.appendContent("l1");
        d.setTime(2);
        d.appendContent("l2");
        d.setTime(3);
        d.appendContent("l3");
        List cpy = d.getContent().deepCopy();
        d.restore(100);
        assert listEquals(cpy, d.getContent());
    }
    /* ====================
        WIKI TESTS
     */
    //Aside from these tests, the other methods in wiki just call functions from other classes
    //with some logic to print out to the console
    @Test
    void wikiCreateUser(){
        Wiki w = new Wiki();
        w.createUser("1");
        assert w.getUsers().search("1").getData().checkId("1");
    }
    @Test
    void wikiCreateDoc(){
        Wiki w = new Wiki();
        w.createUser("A");
        w.createDoc("1", "A");
        assert w.getDocs().search("1").getData().checkId("1");
    }
    @Test
    void wikiLog(){
        //make sure wiki.log() places a log in the logs list for both
        //relevant user & doc
        Wiki w = new Wiki();
        w.createUser("A");
        w.createDoc("d1", "A");
        w.log("A", "d1", "command" );
        Node docNode = w.getDocs().getHead();
        Node userNode = w.getUsers().getHead();
        Doc d = (Doc) docNode.getData();
        User u = (User) userNode.getData();
        Entity dLog = d.getLogs().getHead().getData();
        Entity uLog = u.getLogs().getHead().getData();
        assert uLog.getId().equals("command") && dLog.getId().equals("command");
    }

}
