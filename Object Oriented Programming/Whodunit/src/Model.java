import java.util.*;

public class Model {
    private final ArrayList<Card> people;
    private final ArrayList<Card> places;
    private final ArrayList<Card> weapons;
    private final ArrayList<IPlayer> players = new ArrayList<>();
    private final ArrayList<IPlayer> removedPlayers;
    private final ArrayList<IPlayer> allPlayers = new ArrayList<>();
    private final Card person;
    private final Card weapon;
    private final Card place;
    private final int playerCount;

    // TODO:
    //  DONE Implement CPlayer methods, including working AI
    //  Ensure AI for CPlayer passes all necessary tests
    //  Test HPlayer and CPlayer interactions, along with CPlayer and CPlayer interactions

    public Model(int i, ArrayList<Card> pp, ArrayList<Card> pl, ArrayList<Card> wp, ArrayList<IPlayer> p) {
        playerCount = i;
        people = pp;
        places = pl;
        weapons = wp;
        players.addAll(p);
        allPlayers.addAll(p);
        removedPlayers = new ArrayList<>();
        // all players get a list of all possible cards
        // set up the players
        int x = 0;
        while(x < playerCount){ // i
            IPlayer temp = players.remove(0);
            temp.setUp(playerCount, x, people, places, weapons);
            players.add(temp);
            x++;
        } // all players set up

        // shuffle decks
        Collections.shuffle(weapons);
        Collections.shuffle(people);
        Collections.shuffle(places);
        // get answer cards
        weapon = weapons.remove(new Random().nextInt(weapons.size()));
        person = people.remove(new Random().nextInt(people.size()));
        place = places.remove(new Random().nextInt(places.size()));

        // its possible for a player to get all one type of card, so put all cards in a list together
        // and deal from that
        ArrayList<Card> cards = new ArrayList<>();
        cards.addAll(weapons);
        cards.addAll(people);
        cards.addAll(places);

        /*
        x = 0;
        while(x < weapons.size()){
            cards.add(weapons.get(x));
            cards.add(people.get(x));
            cards.add(places.get(x));
            x++;
        }*/

        System.out.print("Here are the names of all suspects: ");
        print(people);
        System.out.print("Here are all the locations: ");
        print(places);
        System.out.print("Here are all the weapons: ");
        print(weapons);

        // divide cards up for the players, deal using setCard method
        // loop until no more cards
        Collections.shuffle(cards);
        while(!cards.isEmpty()){
            // choose a card, then give to the player first in queue, then put that player at the back
            // remove player from front of players list
            IPlayer temp = players.remove(0);
            // remove card from front of list
            // and deal the card
            temp.setCard(cards.remove(0));
            // put player at back of list
            players.add(temp);
        }
        // add cards back to decks now that the cards have been dealt
        weapons.add(weapon);
        places.add(place);
        people.add(person);
        // shuffle again (otherwise answer is just at the end of list)
        Collections.shuffle(weapons);
        Collections.shuffle(people);
        Collections.shuffle(places);

        // play the game
        play();
    }

    private void play(){
        // s may or may not be needed, probably not tho
        //Scanner s = new Scanner(System.in);
        int turn = 0;
        boolean over = false;
        while(players.size() > 1 && !over){
            IPlayer currentPlayer = players.remove(0);
            allPlayers.remove(currentPlayer); // so loop doesn't ask current player to answer own guess
            int index = currentPlayer.getIndex();
            // prints out currentPlayer's index
            System.out.println("Current turn: " + turn);
            // ask player for guess
            Guess g = currentPlayer.getGuess();
            // print guess
            System.out.println("Player " + index + ": " + g.toString());
            // if player is accusing
            if(g.isAccusation()){ // if accuse
                // see if accusation is correct
                ArrayList<Card> accuse = g.getCards();
                // the accuse arraylist contains all the correct cards
                if(accuse.contains(weapon) && accuse.contains(person) && accuse.contains(place)){
                    // accusation is right, currentPlayer wins
                    System.out.println("Player " + index + " won the game.");
                    over = true; // this will end the while loop, not really good coding practice, but idk what else to use
                } else {
                    // the accusation is wrong, do not put currentPlayer back into players list
                    // put them into removedPlayers list instead
                    System.out.println("Player " + index + " made a bad accusation and was removed from the game.");
                    removedPlayers.add(currentPlayer);
                }
            } else {// if not accuse
                // take the guess and work through players and then removedPlayers by index
                int x = 0;
                //int pind = 0;
                //int rind = 0;
                Card result = null;
                IPlayer ip = null;
                // TODO: loop through all players in order of index
                while(x < playerCount -1 && result == null){ // playercount -1, because the currentPlayer isnt counted

                    // loop through allPlayers, which is sorted by index by default and check each player
                    System.out.println("Asking Player " + allPlayers.get(x).getIndex() + ".");
                    result = allPlayers.get(x).canAnswer(g, currentPlayer);
                    if(result != null){
                        // report answer
                        ip = allPlayers.get(x);
                        System.out.println("Player " + ip.getIndex() + " answered.");
                    }
                    /*
                    if(removedPlayers.size() > 0 && players.get(pind).getIndex() < removedPlayers.get(rind).getIndex()){
                        // then check players[pind]
                        System.out.println("Asking Player " + players.get(pind).getIndex() + ".");
                        result = players.get(pind).canAnswer(g, currentPlayer);
                        if(result != null) {
                            // report answer
                            ip = players.get(pind);
                            System.out.println("Player " + ip.getIndex() + " answered.");
                        }
                        pind++;
                    } else { // should NEVER have the same indices, so here removedPlayers[rind] < players[pind]
                        // check removedPlayers[rind]
                        //noinspection DuplicatedCode
                        System.out.println("Asking Player " + removedPlayers.get(rind).getIndex() + ".");
                        result = removedPlayers.get(rind).canAnswer(g, currentPlayer);
                        if(result != null) {
                            // report answer
                            ip = removedPlayers.get(rind);
                            System.out.println("Player " + ip.getIndex() + " answered.");
                        }
                        rind++;
                    } // else

                     */
                    x++;

                } // while, loop until all players checked or result != null


                // now result should be something, if someone has result as a card,
                // or it should be null if no one has result (ie guess was the correct answer, or currentPlayer has some of the cards in guess)
                if(ip == null && result == null){
                    System.out.println("No one could answer.");
                }
                // add current player back to players
                players.add(currentPlayer);
                currentPlayer.receiveInfo(ip, result);
            } // else ( if not accuse)
            allPlayers.add(currentPlayer); // add at end of list,
            turn++;
        } // while

        if(players.size() == 1 && !over) {
            // game ended b/c everyone else got kicked out,
            // and game hasn't already finished
            IPlayer currentPlayer = players.remove(0);
            int index = currentPlayer.getIndex();
            System.out.println("Player " + index + " won the game.");
        } // no else needed
    }

    private void print(ArrayList<Card> places){
        for (Card place : places) {
            System.out.print(place.getVal() + ", ");
        }
        System.out.println();
    }

    /*
    private Card searchHelp(IPlayer ret, ArrayList<IPlayer> list, int ind, Guess g, IPlayer curr){

        return null;
    }
    */

    /*
    private IPlayer checkFor(ArrayList<IPlayer> list, int i, Guess g, IPlayer p){
        Card c = null;
        IPlayer ip = null;
        System.out.println("Asking Player " + list.get(i).getIndex() + ".");
        c = list.get(i).canAnswer(g, p);
        if(c != null) {
            // report answer
            ip = list.get(i);
            System.out.println("Player " + ip.getIndex() + " answered.");
        }
    }*/

}
