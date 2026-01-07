import java.util.ArrayList;
import java.util.Scanner;

public class HPlayer implements IPlayer{
    private int numPlayers;
    private int index;
    private ArrayList<Card> people;
    private ArrayList<Card> places;
    private ArrayList<Card> weapons;
    private ArrayList<Card> cards;

    public void setUp(int numPlayers, int index, ArrayList<Card> ppl, ArrayList<Card> places, ArrayList<Card> weapons) {
        // card lists are lists of ALL cards
        this.numPlayers = numPlayers;
        this.index = index;
        this.people = ppl;
        this.places = places;
        this.weapons = weapons;
        cards = new ArrayList<>();
    }

    public void setCard(Card c) {
        // deals card C to the player
        cards.add(c);
        System.out.println("You received the card: " + c.toString());
    }

    public int getIndex() {
        return index;
    }

    public Card canAnswer(Guess g, IPlayer ip) {
        // takes a guess from a player, and if possible, answers that guess with a card
        // search through cards arraylist to find an instance of any card in 'g'
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
        if(count == 0){
            System.out.println("Player " + ip.getIndex() + " asked you about " + g.toString() +", but you couldn't answer.");
            // send info to ip in model after loop through all/once result != null
        } else if(count == 1){
            result = answers.get(0);
            System.out.println("Player " + ip.getIndex() + " asked you about " + g.toString() +", you only have one card, " + result.getVal() + ", showed it to them.");
            //ip.receiveInfo(this, result);
        } else {
            System.out.println("Player " + ip.getIndex() + " asked you about " + g.toString() +". Which do you show?");
            //Scanner s = new Scanner(System.in);
            printCards(answers);
            int in = getChoiceInt(answers);
            result = answers.get(in);
            //ip.receiveInfo(this, result);
            //s.close();
        }

        return result;
    }

    public Guess getGuess() {
        // can be a guess or accusation
        // take user input to make a guess
        System.out.println("Its your turn.");


        // print out the data necessary to make a guess (all cards)
        // not necessary here
        /*
        System.out.print("Here are the names of all suspects: ");

        for (Card person : people) {
            System.out.print(person.getVal() + ", ");
        }
        System.out.println();

        System.out.print("Here are all the locations: ");
        for (Card place : places) {
            System.out.print(place.getVal() + ", ");
        }
        System.out.println();

        System.out.print("Here are all the weapons: ");
        for (Card weapon : weapons) {
            System.out.print(weapon.getVal() + ", ");
        }
        System.out.println();

         */

        Card p = null;
        Card pl = null;
        Card w = null;

        System.out.println("Which person do you want to suggest?");
        printCards(people);
        p = people.get(getChoiceInt(people));

        System.out.println("Which location do you want to suggest?");
        printCards(places);
        pl = places.get(getChoiceInt(places));

        System.out.println("Which weapon do you want to suggest?");
        printCards(weapons);
        w = weapons.get(getChoiceInt(weapons));


        // get if the user is accusing or not
        boolean a = accuse();

        return new Guess(a, w, p, pl);
    }

    // take user input from System.in,
    // valid input is y/Y or n/N, if invalid input, recursively call until valid input
    private boolean accuse(){
        System.out.println("Is this an accusation (Y/[N])?");
        Scanner s = new Scanner(System.in);
        String in = s.next();
        boolean a;
        if(in.equalsIgnoreCase("Y")){
            a = true;
        } else if(in.equalsIgnoreCase("N")){
            a = false;
        } else {
            System.out.println("Invalid response, try again.");
            a = accuse();
        }
        //s.close();
        return a;
    }

    // takes a int from System.in and returns the int if it is in the bounds of list,
    // otherwise recursively call itself until valid input
    private int getChoiceInt(ArrayList<Card> list){
        int result = 0;
        Scanner s = new Scanner(System.in);
        int in = s.nextInt();
        if(in < 0 || in >= list.size()){
            System.out.println("Invalid response, try again.");
            result = getChoiceInt(list);
        } else {
            result = in;
        }
        return result;
    }

    // take an int from system.in and return a card at list[input] if in bounds
    /* Replaced by more useful getChoiceInt
    private Card getChoice(ArrayList<Card> list){
        Card result = null;
        Scanner s = new Scanner(System.in);
        int in = s.nextInt();
        if(in < 0 || in >= list.size()){
            System.out.println("Invalid response, try again.");
            result = getChoice(list);
        } else {
            result = list.get(in);
        }
        //s.close();
        return result;
    }*/

    private void printCards(ArrayList<Card> list){
        for(int i = 0; i < list.size(); i++){
            System.out.println(i + ": " + list.get(i).getVal());
        }
    }

    public void receiveInfo(IPlayer ip, Card c) {
        if( ip == null && c == null){ // both will equal null, or neither will equal null
            System.out.println("No one could refute your suggestion.");
        } else {
            // player needs to keep track of cards themselves, don't add to cards list
            System.out.println("Player " + ip.getIndex() + " refuted your suggestion by showing you " + c.getVal() + ".");
        }
    }
}
