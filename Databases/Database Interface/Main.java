import javax.swing.*;
import java.awt.*;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.sql.*;
import java.util.Date;



public class Main {
    //private static final String connectionURL = "jdbc:sqlite:identifier.sqlite";
    private static Connection connection = null;

    // entry main
    public static void main(String[] args) {
        // try to connect to DB
        try {
            // connect to db

            Class.forName("org.hsqldb.jdbc.JDBCDriver");
            // files required are all "localhost.(whatever)" files in Proj directory
            connection = DriverManager.getConnection("jdbc:hsqldb:file:localhost;hsqldb.lock_file=false", "admin", "admin");
            System.out.println("Connection established.");
            System.out.println("Current Schema: " + connection.getSchema() + ".");

            /* Prints all tables in db
            connection.setSchema("INFORMATION_SCHEMA");
            System.out.println("The following tables exist in the schema.");
            PreparedStatement ps = connection.prepareStatement("select TABLE_NAME from TABLES where TABLE_SCHEMA = 'PUBLIC'");
            ResultSet rs = ps.executeQuery();
            while(rs.next()) {
                System.out.println(rs.getString(1));
            }
            connection.setSchema("PUBLIC");
             */

            // create JFrame
            JFrame f = new JFrame("Databases Group (of one) Project");
            f.setSize(1200, 800);
            f.setLocation(700, 240);

            // create text are for query result
            final JTextArea textArea = new JTextArea(400, 400);
            textArea.setEditable(false);
            Font f1 = textArea.getFont();
            Font f2 = new Font(f1.getFontName(), f1.getStyle(), 18);
           textArea.setFont(f2);

            final JScrollPane jsp = new JScrollPane(textArea);
            f.getContentPane().add(BorderLayout.CENTER, jsp);

            JFrame frame = new JFrame("Export result to CSV");
            frame.setSize(200, 200);
            frame.setLocation(200, 400);
            final JButton b = new JButton("Export result to CSV");
            frame.getContentPane().add(BorderLayout.CENTER, b);

            b.addActionListener( e -> {
                String s = textArea.getText(); // grabs the text area
                if(!s.equals("")){ // something in the text area
                    Date d = new Date();
                    System.out.println(d.toString());
                    File file = new File("export_" + d.getYear() +"_" + d.getMonth() +"_" + d.getDay() +"_" + d.getHours() +"_" + d.getMinutes() +"_" + d.getSeconds() +"_" + ".csv");
                    try {
                        file.createNewFile();
                    } catch (IOException ex) {
                        ex.printStackTrace();
                    }
                    try {
                        FileWriter fw = new FileWriter(file);
                        // write s to file with fw
                        fw.write(s);
                        fw.close();
                    } catch (IOException ex) {
                        ex.printStackTrace();
                    }
                }
            });


            // create drop down menu for selecting a query
            String[] optionsToChoose = {
                    "Count the number of landslides that occurred in each country.",
                    "What is the total number of fatalities caused by each storm?",
                    "Find the names of all countries that have no landslides caused by storms.",
                    "Find the number of 'Christmas Miracles': count the landslides that occurred on Christmas with no injuries or deaths (0 or null)",
                    "Count the number of news reports made by each source.",
                    "Which country has the most accurate location measurements: count the 'exact' location accuracies that occur in each country.",
                    "Find the danger zones: For each closest gazetteer, find the average distance to a landslide, the smaller the average, the more 'dangerous'",
                    "Count the landslides caused by natural disaster type triggers for each country",
                    "Count the number of large to catastrophic landslides for each country",
                    "What is the total amount of injuries and fatalities reported by each source?",
                    "Find each event's degree distance from 'NULL Island' (distance from point 0,0 on globe)",
                    "Get all entries from Category table.",
                    "Get all entries from Country table.",
                    "Get all entries from Division table.",
                    "Get all entries from Events table.",
                    "Get all entries from Import table.",
                    "Get all entries from Location table.",
                    "Get all entries from Report table.",
                    "Get all entries from Storm table.",
                    "Get all entries from During table.",
                    "Get all entries from Is_A table."
            }; //Division", "Events", "Import", "Location", "Report", "Source", "Storm"
            JComboBox<String> jComboBox = new JComboBox<>(optionsToChoose);
            f.getContentPane().add(BorderLayout.NORTH, jComboBox);

            // create button for running selected query
            final JButton button = new JButton("Run");
            f.getContentPane().add(BorderLayout.SOUTH, button);
            button.addActionListener(e -> {
                // run the selected command and result into text box
                try {
                    textArea.setText(execute(jComboBox.getSelectedIndex()));
                } catch (SQLException throwable) {
                    // idk what this does, but intellij want this so whatever
                    throwable.printStackTrace();
                }
            });

            frame.setVisible(true);
            f.setVisible(true);

        } catch (Exception ex) {
            System.out.println("Unable to connect to database.");
            ex.printStackTrace();
        }
    }

    public static String execute(int i) throws SQLException {
        // based on the index (i), set query to specific string
        connection.setSchema("PUBLIC");
        System.out.println("Executing " + i + "th statement...");
        if(i == 14){
            System.out.println("This can take a while (15+ sec, sometimes).");
        }
        String output;
        String queryString = switch (i) {
            case 0 -> "select c.countryName, count(l.eId) as landslide_count from Location l, Country c where l.code = c.code group by countryName order by landslide_count desc;";
            case 1 -> "select s.stormName, sum(isNull(e.fatality, 0)) as death_count from  Events e, During d, Storm s  where e.eId = d.eId  and d.sId = s.sId group by s.stormName order by death_count desc;";
            case 2 -> "select distinct countryName from Country where code in( select distinct code from Country except ( select distinct code from Location where eId in ( select eId from DURING)));";
            case 3 -> "select count(eId) as Christmas_Miracles, eDate from Events where regexp_matches(convert(eDate, varchar(64)), '^12/25/.*') and (Fatality is null or Fatality = 0) and (Injury is null or Injury = 0) group by eDate;";
            case 4 -> "select sourceName, count(url) as reports from REPORT group by sourceName order by reports desc";
            case 5 -> "select c.countryName, count(l.lAcc) as acc_count from Location l, Country c where c.code = l.code and l.lAcc = 'exact' group by c.countryName order by acc_count desc;";
            case 6 -> "select GCLOSEST, avg(GDIST) as Average_Dist from DIVISION group by GCLOSEST order by Average_Dist";
            case 7 -> "select c.countryName, count(x.\"trigger\") as times_smited from Country c, Location l, IS_A i, Category x where c.code = l.code and l.eId = i.eId and i.CID = x.cId and x.\"trigger\" in ('volcano', 'earthquake', 'flooding', 'monsoon', 'tropical_cyclone', 'dam_embankment_collapse') group by c.countryName order by times_smited desc;";
            case 8 -> "select countryName, count(category.size) as Big_Ones from Country, Location, IS_A, Category where Country.code = Location.code and Location.eId = IS_A.eId and IS_A.cId = CATEGORY.cId and category.size in ('catastrophic', 'very_large', 'large') group by countryName order by Big_Ones desc;";
            case 9 -> "select r.sourceName, sum(isNull(e.fatality, 0)) as sum_fatality, sum(isNull(e.injury, 0)) as sum_injury from REPORT r, Events e where r.eId = e.eId group by r.sourceName order by sum_fatality desc;";
            case 10 -> "select eTitle, sqrt(power(lat, 2) + power(long, 2)) as Distance from Location, Events where LOCATION.EID = EVENTS.EID order by Distance;";
            case 11 -> "select * from Category;";
            case 12 -> "select * from Country;";
            case 13 -> "select * from Division;";
            case 14 -> "select * from Events;";
            case 15 -> "select * from Import;";
            case 16 -> "select * from Location;";
            case 17 -> "select * from Report;";
            case 18 -> "select * from Storm;";
            case 19 -> "select * from During;";
            case 20 -> "select * from IS_A;";
            default -> "select eId from Events limit 10"; // if you somehow manage to break something
        };

        // run query
        PreparedStatement s = connection.prepareStatement(queryString);
        StringBuilder sb = new StringBuilder();
        try( ResultSet rs = s.executeQuery()){
            int size = rs.getMetaData().getColumnCount();
            for(int x = 1; x <= size; x++){
                sb.append(rs.getMetaData().getColumnLabel(x)).append(", ");
            }
            sb.deleteCharAt(sb.lastIndexOf(",")); // remove last comma
            sb.append('\n');
            while(rs.next()){
                for(int x = 1; x <= size; x++){
                    sb.append(rs.getString(x)).append(", ");
                }
                sb.deleteCharAt(sb.lastIndexOf(",")); // remove last comma
                sb.append('\n');
            }
        } catch (SQLException e){
            System.out.println("Something went wrong when executing the query.");
            e.printStackTrace();
        }

        // put results of query into string
        output = sb.toString();

        // return results
        return output;
    }

}
