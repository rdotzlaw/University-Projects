import java.io.InvalidObjectException;

public class List {
    private Node head;
    private int len;

    public List(Node head) {
        this.head = head;
        len = 1;
    }

    public List() {
        this.head = null;
        len = 0;
    }

    public Node getHead() {
        return head;
    }

    public int getLen() {
        return len;
    }

    public Node search(String id){
        Node curr = head;
        while(curr != null && !curr.getData().checkId(id)){
            curr = curr.getNext();
        }
        return curr;
    }

    public void delete(int i){
        if(i >= 0 && i < len) { //not out of bounds for list
            Node n = getIthNode(i - 1); //get node before node to delete
            if (i - 1 < 0) { //if node to delete is head node
                head = head.getNext();
                System.out.println("DELETE SUCCESS");
            } else if (n != null) {

                n.setNext(n.getNext().getNext());
                System.out.println("DELETE SUCCESS");
            }
            len--;
        } else {
            System.out.println("FAIL");
        }
    }

    //replace the ith node's data with a new obj d
    public void replace(int i, Entity d){
        if(i >= 0 && i < len) {
            Node n = getIthNode(i);
            if (n != null) {
                n.setData(d);
                System.out.println("SUCCESS");
            }
        } else {
            System.out.println("FAIL");
        }
    }

    private Node getIthNode(int i){
        Node curr = head;
        if((i >= 0 && i < len) && curr != null){ //node count is [0,len), in this case, i is out of bounds
            int x = 0;
            while (x < i) { //i >=0 and i < len, so curr can never be null
                x++;
                curr = curr.getNext();
            } //curr is now the ith node

        } else {
            //System.out.println("ith Node FAIL");
            curr = null;
        }
        return curr;
    }

    public void print(){
        if(head != null)
            head.print();
    }

    //starts recursive append in node class, appends to end of list
    public void append(Node n){
        if(head == null){
            head = n;
        } else {
            head.append(n);
        }
        len++;
    }

    //returns a Doc with time as close to t as possible, with doc.time <= t
    public Doc searchByTime(int t){
        Doc d = null;

        Node curr = head;
        Node trail = null;
        if(curr != null && curr.getData() instanceof Doc){ //list has nodes and is list of docs
            while(curr != null && ((Doc) curr.getData()).getTime() <= t){
                trail = curr;
                curr = curr.getNext();
            }
            //either trail contains a time <= t, and curr has time > t
            //or curr reached end of list and is null
            //either way, trail contains a time <= t
            assert trail != null;
            d = (Doc) trail.getData();

        } else {
            System.out.println("LIST IS NOT DOC TYPE OR LIST EMPTY");
        }
        return d;
    }

    //recursively copies all nodes, and the data they contain
    public List deepCopy(){
        List l = new List();
        if(head != null){
            l.append(head.deepCopy());
        }
        return l;
    }
}
