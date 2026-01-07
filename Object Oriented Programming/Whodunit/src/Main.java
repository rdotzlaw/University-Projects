
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.Scanner;

public class Main {

    public static void main(String[] args) {
        Scanner s = new Scanner(System.in);
        System.out.println("Enter how many computer opponents: ");
        int i = s.nextInt();
        //s.close();
        // creates players
        ArrayList<IPlayer> players = new ArrayList<>();
        // create human player
        HPlayer p = new HPlayer();
        players.add(p);
        // i is the number of computer players
        // create computer players
        int x = 0;
        while(x < i){
            CPlayer cp = new CPlayer();
            players.add(cp);
            x++;
        }

        // create cards (6) for each category
        // lists of cards used to give out to players
        ArrayList<Card> pp = new ArrayList<>();
        ArrayList<Card> pl = new ArrayList<>();
        ArrayList<Card> wp = new ArrayList<>();
        // create cards
        Card c = new Card("Person", "Bob");
        pp.add(c);
        c = new Card("Person", "John");
        pp.add(c);
        c = new Card("Person", "Sally");
        pp.add(c);
        c = new Card("Person", "Smith");
        pp.add(c);
        c = new Card("Person", "Sam");
        pp.add(c);
        c = new Card("Person", "Dennis");
        pp.add(c);

        c = new Card("Place", "Attic");
        pl.add(c);
        c = new Card("Place", "Library");
        pl.add(c);
        c = new Card("Place", "Kitchen");
        pl.add(c);
        c = new Card("Place", "Basement");
        pl.add(c);
        c = new Card("Place", "Bedroom");
        pl.add(c);
        c = new Card("Place", "Hallway");
        pl.add(c);

        c = new Card("Weapon", "Crowbar");
        wp.add(c);
        c = new Card("Weapon", "Gun");
        wp.add(c);
        c = new Card("Weapon", "Knife");
        wp.add(c);
        c = new Card("Weapon", "Pencil");
        wp.add(c);
        c = new Card("Weapon", "Sword");
        wp.add(c);
        c = new Card("Weapon", "Axe");
        wp.add(c);


        // start model, this is the last thing main does
        Model m = new Model(i+1, pp, pl, wp, players);
        // i+1 b/c i = num of comp players, so add the human player

        s.close();
    }

}
