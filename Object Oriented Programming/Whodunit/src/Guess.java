import java.util.ArrayList;

public class Guess {
    private final boolean isAccusation;
    private final Card weapon;
    private final Card person;
    private final Card place;

    public Guess(boolean a, Card w, Card p, Card pl){
        isAccusation = a;
        weapon = w;
        person = p;
        place = pl;

    }


    public ArrayList<Card> getCards() {
        ArrayList<Card> cards = new ArrayList<>();
        cards.add(weapon);
        cards.add(person);
        cards.add(place);
        return cards;
    }

    public String toString(){
        String s;
        if(isAccusation){
            s = "Accusation: ";
        } else {
            s = "Suggestion: ";
        }
        return s + person.getVal() + " in " + place.getVal() + " with the " + weapon.getVal() + ".";
    }

    public boolean isAccusation() {
        return isAccusation;
    }
}
