import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Scanner;

public class TestGame {
    ArrayList<Card> pp;
    ArrayList<Card> pl;
    ArrayList<Card> wp;

    @BeforeEach
    void before(){
        // create cards (6) for each category
        // lists of cards used to give out to players
        pp = new ArrayList<>();
        pl = new ArrayList<>();
        wp = new ArrayList<>();
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

        Collections.shuffle(pp);
        Collections.shuffle(pl);
        Collections.shuffle(wp);
    }

    @Test
    void test1(){
        IPlayer cp = new CPlayer();
        cp.setUp(2, 0, pp, pl, wp);
        IPlayer other = new CPlayer();
        other.setUp(2, 1, pp, pl, wp);
        Guess g = new Guess(false, wp.get(0), pp.get(0), pl.get(0));
        assert cp.canAnswer(g, other) == null;
    }

    @Test
    void test2(){
        IPlayer cp = new CPlayer();
        IPlayer other = new CPlayer();

        cp.setUp(2, 0, pp, pl, wp);
        cp.setCard(pp.get(0)); // give card
        other.setUp(2, 1, pp, pl, wp);
        Guess g = new Guess(false, wp.get(0), pp.get(0), pl.get(0));
        assert cp.canAnswer(g, other) == pp.get(0);
    }

    @Test
    void test3(){
        IPlayer cp = new CPlayer();
        IPlayer other = new CPlayer();

        cp.setUp(2, 0, pp, pl, wp);
        cp.setCard(pp.get(0)); // give card
        cp.setCard(wp.get(0));
        other.setUp(2, 1, pp, pl, wp);
        Guess g = new Guess(false, wp.get(0), pp.get(0), pl.get(0));
        assert cp.canAnswer(g, other) == pp.get(0) || cp.canAnswer(g, other) == wp.get(0);
    }

    @Test
    void test4(){
        //an initial guess from CPlayer shouldn't contain any cards they've been given
        IPlayer cp = new CPlayer();
        Card wep = wp.get(0);
        Card per = pp.get(0);
        Card pla = pl.get(0);
        // set up cp
        cp.setUp(2, 0, pp, pl, wp);

        cp.setCard(wep);
        cp.setCard(per);
        cp.setCard(pla);
        //guess shouldn't contain wep, per, or pla
        Guess g = cp.getGuess();
        // cards card types are : weapon, person, place
        ArrayList<Card> cards = g.getCards();
        assert wep != cards.get(0) && per != cards.get(1) && pla != cards.get(2);
    }

    @Test
    void test5(){
        // if comp is missing only 1 card from each category, (3 cards total), comp should return correct accusation
        IPlayer cp = new CPlayer();
        Card wep = wp.get(0);
        Card per = pp.get(0);
        Card pla = pl.get(0);
        // set up cp
        cp.setUp(2, 0, pp, pl, wp);

        // remove answer cards
        pp.remove(0);
        wp.remove(0);
        pl.remove(0);

        // add all but answer cards to cp
        for(Card c : pp)
            cp.setCard(c);
        for(Card c : pl)
            cp.setCard(c);
        for(Card c : wp)
            cp.setCard(c);

        // check to see if cp.getGuess gives correct accusation
        Guess g = cp.getGuess();
        assert g.isAccusation() && g.getCards().contains(wep) && g.getCards().contains(per) && g.getCards().contains(pla);

    }

    @Test
    void test6(){
        IPlayer cp = new CPlayer();
        Card wep = wp.get(0);
        Card ext = wp.get(1);
        Card per = pp.get(0);
        Card pla = pl.get(0);
        // set up cp
        cp.setUp(2, 0, pp, pl, wp);

        // remove answer cards, and an extra
        pp.remove(0);
        wp.remove(0);
        wp.remove(0);
        pl.remove(0);

        // add all but answer cards and extra to cp
        for(Card c : pp)
            cp.setCard(c);
        for(Card c : pl)
            cp.setCard(c);
        for(Card c : wp)
            cp.setCard(c);

        Guess g = cp.getGuess();
        // g should not be an accusation
        assert !g.isAccusation();
        // cp receives extra card from another player
        cp.receiveInfo(new CPlayer(), ext);
        g = cp.getGuess();
        // g should now be correct accusation
        assert g.isAccusation() && g.getCards().contains(wep) && g.getCards().contains(per) && g.getCards().contains(pla);
    }

    @Test
    void test7(){
        // HPlayer.canAnswer is called and HPlayer instance has one or more cards that are included in the guess
        // canAnswer should return a card included in the guess
        // ie, an HPlayer cannot answer with a card that they don't have in their hand
        Scanner s = new Scanner(System.in);
        HPlayer hp = new HPlayer();
        hp.setUp(1, 0, pp, pl, wp);

        // just add 2 cards to hp
        hp.setCard(wp.get(0));
        hp.setCard(pp.get(0));
        // create guess
        Guess g = new Guess(false, wp.get(0), pp.get(0), pl.get(0));
        // grab answer
        Card answer = hp.canAnswer(g, new CPlayer());
        // answer is a card that hp has
        assert answer == wp.get(0) || answer == pp.get(0);

    }
}
