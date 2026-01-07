
import java.io.*;
import java.util.*;
import java.lang.*;

public class WikiIn {

    private boolean quit;
    private final Wiki wiki; //will not change

    //upon creation, run the read method
    public WikiIn(){
        quit = false;
        wiki = new Wiki();
        this.read();
    }

    /*
        Command list:
            USER - creates a user if no duplicate exists
            CREATE - creates a doc if no duplicate exists
            APPEND
            REPLACE
            DELETE
            PRINT
            RESTORE
            HISTORY
            USERREPORT
            QUIT
            COMMENTS - Lines that start with # are ignored

     */
    private void read(){

        Scanner frame = new Scanner(System.in);
        System.out.println("Enter file name (without file extension)");
        String name = frame.nextLine();
        //skip over tabs and white space at start of line if it exists
        //and lines that start with # are just comments, and should be ignored
        try {
            //Executes commands for the wiki object after parsing
            File file = new File(name + ".txt");
            Scanner s = new Scanner(file);
            System.out.println("File found, starting wiki.");

            //keep going until hit quit command, or out of lines
            while(s.hasNextLine() && !quit){
                this.parse(s.nextLine());
            }

            if(!quit){
                System.out.println("QUIT command missing, exiting program.");
            } else {
                System.out.println("Bye.");
            }

        } catch (FileNotFoundException e){
            System.out.println("File was not found, make sure file is in the same directory.");
            this.read();
        }

    }

    private String concatContent(String[] arr, int start){
        String content = "";
        for(int i = start; i < arr.length-1; i++){
            content = content.concat(arr[i] + " "); //ith element with a space after
        }
        content = content.concat(arr[arr.length-1]); //last element w/out space after
        return content;
    }

    //if parse quit command, set quit = true
    private void parse(String line){
        //comments start with #, ignore these
        if(line.charAt(0) != '#'){
            String[] split = line.split("\\s+"); // \\s+ will remove any amount of white space
            /* for debug
            for(int i = 0; i < split.length; i++){
                System.out.println(split[i]);
            } */
            String cmd = split[0];
            if(cmd.equals("USER")){
                wiki.createUser(split[1]);
                wiki.log(split[1], "", line);
            } else if(cmd.equals("CREATE")){
                wiki.createDoc(split[1], split[2]);
                wiki.log(split[2], split[1], line);
            } else if(cmd.equals("APPEND")){
                String content = concatContent(split, 3);
                wiki.appendToDoc(split[1], split[2], content);
                wiki.log(split[2], split[1], line);
            } else if(cmd.equals("REPLACE")){
                String content = concatContent(split, 4);
                wiki.replace(split[1], split[2], Integer.parseInt(split[3]), content);
                wiki.log(split[2], split[1], line);
            } else if(cmd.equals("DELETE")){
                wiki.delete(split[1], split[2], Integer.parseInt(split[3]));
                wiki.log(split[2], split[1], line);
            } else if(cmd.equals("PRINT")){
                wiki.print(split[1]);
                wiki.log("", split[1], line);
            } else if(cmd.equals("RESTORE")){
                wiki.restore(split[1], split[2], Integer.parseInt(split[3]));
                wiki.log(split[1], split[2], line);
            } else if(cmd.equals("HISTORY")){
                wiki.history(split[1]);
                wiki.log("", split[1], line);
            } else if(cmd.equals("USERREPORT")){
                wiki.report(split[1]);
                wiki.log(split[1], "", line);
            } else if(cmd.equals("QUIT")){
                quit = true;
            }

            /*
            //this switch statement works fine on intelij, but will not work on command line
            //so i changed it for an if
            switch (cmd) {
                case "USER" -> {


                } case "CREATE" -> {
                    wiki.createDoc(split[1], split[2]);
                    wiki.log(split[2], split[1], line);

                } case "APPEND" -> {
                    //concat the string to append to the doc
                    String content = concatContent(split, 3);
                    wiki.appendToDoc(split[1], split[2], content);
                    wiki.log(split[2], split[1], line);

                } case "REPLACE" -> {
                    String content = concatContent(split, 4);
                    wiki.replace(split[1], split[2], Integer.parseInt(split[3]), content);
                    wiki.log(split[2], split[1], line);

                } case "DELETE" -> {
                    wiki.delete(split[1], split[2], Integer.parseInt(split[3]));
                    wiki.log(split[2], split[1], line);

                } case "PRINT" -> {
                    wiki.print(split[1]);
                    wiki.log("", split[1], line);

                } case "RESTORE" -> {
                    wiki.restore(split[1], split[2], Integer.parseInt(split[3]));
                    wiki.log(split[1], split[2], line);

                } case "HISTORY" -> {
                    wiki.history(split[1]);
                    wiki.log("", split[1], line);

                } case "USERREPORT" -> {
                    wiki.report(split[1]);
                    wiki.log(split[1], "", line);

                } case "QUIT" -> quit = true;

            }*/

        }
    }


}
