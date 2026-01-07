public class Entity {
    private final String id; //will not change
    private int time; //the time at which this node was created, time++ each time a command is read

    public Entity(String id){
        this.id = id;
    }
    public boolean checkId(String i){
        return i.equals(this.id);
    }

    public String getId(){
        return id;
    }

    public Entity deepCopy(){ //dont need to copy logs
        return new Entity(this.id);
    }

    public void print(){
        System.out.println(id);
    }

}
