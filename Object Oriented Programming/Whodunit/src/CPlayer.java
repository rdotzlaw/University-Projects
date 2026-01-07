import java.util.ArrayList;
import java.util.Random;

public class CPlayer implements IPlayer{
    private int numPlayers;
    private int index;
    private ArrayList<Card> people;
    private ArrayList<Card> places;
    private ArrayList<Card> weapons;
    // these 3 are a list of all the 'missing' cards, the cards that the AI doesn't know about
    private ArrayList<Card> pcards;
    private ArrayList<Card> plcards;
    private ArrayList<Card> wcards;
    // a list of the known cards, will not contain any card from cards list
    private ArrayList<Card> known;
    private ArrayList<Card> cards;
    public void setUp(int numPlayers, int index, ArrayList<Card> ppl, ArrayList<Card> places, ArrayList<Card> weapons) {
        this.numPlayers = numPlayers;
        this.index = index;
        this.people = ppl;
        this.places = places;
        this.weapons = weapons;
        // when CPlayers receive an answer to their guesses, add the Card to the cards list if it isn't already there
        cards = new ArrayList<>();
        known = new ArrayList<>();
        // initially, all cards are unknown
        pcards = new ArrayList<>();
        pcards.addAll(ppl);
        plcards = new ArrayList<>();
        plcards.addAll(places);
        wcards = new ArrayList<>();
        wcards.addAll(weapons);
    }

    public void setCard(Card c) {
        cards.add(c);
        // remove card from list of unknown cards via type
        if(c.getType().equals("Weapon")) {
            wcards.remove(c);
        } else if(c.getType().equals("Person")){
            pcards.remove(c);
        } else {
            plcards.remove(c);
        }

    }

    public int getIndex() {
        return index;
    }

    public Card canAnswer(Guess g, IPlayer ip) {
        // if this has one of the cards in g, then return it, otherwise return null
        // return null if cards is empty
        // return a card if that is the only card the computer has
        Card result = null;
        ArrayList<Card> guess = g.getCards();
        ArrayList<Card> answers = new ArrayList<>();
        int count = 0;
        for(Card gCard : guess){
            if(cards.contains(gCard)){
                answers.add(gCard);
                count++;
            }
        }


        if(count > 0) {
            // only 1 answer or more than 1 answer
            // just give 1st possible answer
            // could do more advanced logic, such as repeatedly showing the same card instead of just the 1st card
            result = answers.get(0);
        }


        return result;
    }

    public Guess getGuess() {
        // guess printed out in model, only need to make guess
        // automatically make a guess that is 'realistic' (passes 7 tests in assignment desc.)
        // if cards is missing 3 cards from the full list of cards, then accuse using the missing cards (ONLY IF THOSE 3 CARDS ARE OF DIFFERENT TYPES)
        // if cards is missing 4 cards, after a missing card is gotten from receiveInfo, the next getGuess should be an accusation

        boolean accuse = false;
        // if this is missing 1 weapon, 1 person and 1 place card
        // accuse using the missing cards
        if(wcards.size() == 1 && plcards.size() == 1 && pcards.size() == 1){
            // make accusation
            accuse = true;
        }
        // not possible to know all cards, since answer cards will not be passed into setCard,
        // that means that the lists of missing cards will never be empty
        // keep asking for missing cards until there are no more missing cards
        Card w = guessHelp(wcards);
        Card p = guessHelp(pcards);
        Card pl = guessHelp(plcards);
        return new Guess(accuse, w, p, pl);
    }

    private Card guessHelp(ArrayList<Card> list){
        // grabs a random card from the list
        return list.get(new Random().nextInt(list.size()));
    }

    // originally this was adding to cards instead of known
    // but then this would be able to answer with cards that weren't in this' hand
    public void receiveInfo(IPlayer ip, Card c) {
        // gets called after all players (excluding this) have been searched for cards in this' guess
        // ip is sending this card C which is a card that appeared in this' guess
        // add c to known if c is not in known already
        // by adding c to known, the getGuess can use it to make reasonable guesses

        // only add if not already in known and not null
        if(ip != null && c != null && !known.contains(c)) {
            // add to known cards
            known.add(c);
            // remove from unknown cards
            if(c.getType().equals("Weapon")) {
                wcards.remove(c);
            } else if(c.getType().equals("Person")){
                pcards.remove(c);
            } else {
                plcards.remove(c);
            }
        }
        // maybe more logic if splitting cards this has into their type?
        // ^ maybe helpful for getGuess
        // ^^ done
    }
}
