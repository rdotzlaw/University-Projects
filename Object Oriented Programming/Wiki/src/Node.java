public class Node {
    private Node next;
    private Entity data;
    private int time;

    public Node(Entity d){
        this.data = d;
        this.next = null;
        time = -1;
    }

    public Node(Entity d, Node n){
        this.data = d;
        this.next = n;
        time = -1;
    }

    public Node(Entity data, int time) {
        this.data = data;
        this.time = time;
    }

    public void print(){
        this.data.print();
        if(next != null)
            next.print();
    }

    public Entity getData(){
        return data;
    }

    public Node getNext(){
        return next;
    }

    public void setData(Entity data) {
        this.data = data;
    }

    public void setNext(Node next) {
        this.next = next;
    }

    //recursive append to end of list
    public void append(Node n){
        if(next == null){
            next = n;
        } else {
            next.append(n);
        }
    }

    public Node deepCopy(){
        Node n = new Node(this.data.deepCopy());
        if(this.next != null)
            n.setNext(this.next.deepCopy()); //this will reach end of list, eventually
        return n;
    }

}
