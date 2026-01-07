public class Card {
    private final String type; // neither should ever be changed
    private final String val;

    public Card(String t, String v){
        val = v;
        type = t;
    }

    public String getVal() {
        return val;
    }
    public String getType() {
        return type;
    }

    public String toString(){
        return val;
    }
}

