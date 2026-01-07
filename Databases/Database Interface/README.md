[Source code](Main.java)
[Database Source](everything.csv)
[Database Creation SQL](console.sql)
[EER Diagram Constraints](EER%20Diagram%20Constraints.txt)

*Things to note and Changes from RA Model:*
    
For the location table, "code" column is no longer part of the primary key, 
because some entries don't have a code/country name in the original dataset (despite actually occurring in a country)

For the Source/Report tables, I realized that any combination of attributes in the source table could be null,
meaning I would need an ID column for the Source table, but since the majority of rows in the Source are already unique (thanks to the urls),
meaning there were already roughly 10,000+ rows, I decided to just combine Source and Report and use eId as the primary key

In general, I noticed some issues with my relational model when trying to insert data into tables,
mostly due to the fact that some entries in my dataset are missing data for certain attributes or just plain wrong.

Speaking of wrong data, I didn't put the country 'Czechia' in the country table, because 'Czechia' is shorthand
for 'Czech Republic' which was already in the table. Preventing a unique constraint violation for the country table PK.

Also, I believe it was mentioned in the previous (1.2) part, but the columns EVENT_TIME and EVENT_IMPORT_ID where removed
during normalization due to the fact that EVENT_TIME was completely null, and EVENT_IMPORT_ID was always equal to EVENT_ID,
making it redundant.

*Also*, my part 1.2 RA model has a BasedIn table, but I actually removed that from the EER Diagram, because I figured it was
a pretty poor relationship, that may not hold, I just *might* have forgotten to remove it from my RA Model.

**Also** *, also*, after inserting data into tables, I decided it would be better to keep the During and Is_A tables from the EER Diagram,
instead of combining Category/eId and Storm/eID together like I did in my RA model.

Take a look at console.sql if you want to see me creating tables and inserting data, though it is fairly messy.

Also, take a look at everything.csv if you want to see the original dataset (not recomended, it's *really* messy)

# Compiling and Running

To compile the code, *Main.java* needs to be in the same folder as *hsqldb.jar* and all the *localhost* files.

Then compile with:
    
    javac Main.java

Then run with (for windows):

    java -cp .;hsqldb.jar Main
    // This is the same command as used to run A3Q1 (java version)
    // But with the file name changed to "Main"



#*Part 4*


#Summary of the data


*Why was it chosen?*


I chose the landslide database because it had both a lot of entries, and a lot of attributes, all in one massive table.
This allowed me to break up the monolithic table into multiple smaller tables thanks 
to its large number of attributes, and I was also able to have a lot of data to work with for my queries.


*What does it consist of?*


The dataset consists of information on landslides that occurred around the globe.
This information includes:

* Why the landslide occurred
* Where the landslide occurred (including coordinates, and country)
* When the landslide occurred
* Damages in the form of injuries and fatalities
* Sources that reported on the landslide
* Extra info such as a title, description, notes, links to pictures, and metadata (such as import and edit times, etc.)


*How large is it?*

The dataset has 11,033 entries, with 31 unique columns, and its CVS file is 8.28 MB in size.

# Discussion of Data Model

*Why was it broken down into those tables?*

The dataset was broken into the following tables:
* Category
* Country
* Division
* During
* Events
* Import
* Is_A
* Location
* Report
* Storm

The tables were broken up this way to separate unrelated data, and to minimize the amount of duplicate data in the database, 
while still retaining the relationships between the tables.

For cardinality ratios, in general the Events table has a 1-to-m relationship with the tables it's related to (Is_A, During, Location), since multiple Events can have the same category/occur during the same storm/happen at the same location.
Location and Division also has a 1-to-m relationship with Country, since multiple entries in both of them can occur in the same country.
Source/Import and Events are the exceptions, they have a 1-to-1 relationship, since each Event can only be reported on by a single source or imported once.

There wasn't much to think about for the participation constraints, by nature of the data domain, no matter how I broke the tables up
they had to participate in a relationship with a landslide event (with Events table), otherwise they wouldn't be in the dataset.
The only outlier was Location and Events, where both had mandatory participation, for the reasons stated above and because all landslides
have to happen at some location.

I kept the Is_A and During relationship tables instead of just combining them with Category or Storm because it cut the size of the Storm table in half, and 
reduced the size of the Category table from over 10,000, to under 1,000, getting rid of a lot of duplicate data.



# Summary of Database

*List of tables, with cardinality(rows) and arity(columns)*

1. Category:
   1. Cardinality of: 846
   2. Arity of: 5
2. Country:
   1. Cardinality of: 140
   2. Arity of: 2
3. Division:
   1. Cardinality of: 9,471
   2. Arity of: 6
4. During:
   1. Cardinality of: 577
   2. Arity of: 2
5. Events:
   1. Cardinality of: 11,033
   2. Arity of: 7
6. Import:
   1. Cardinality of: 11,033
   2. Arity of: 5
7. Is_A:
   1. Cardinality of: 10,945
   2. Arity of: 2
8. Location:
   1. Cardinality of: 11,033
   2. Arity of: 6
9. Report:
   1. Cardinality of: 11,033
   2. Arity of: 4
10. Storm:
    1. Cardinality of: 218
    2. Arity of: 2

*Technically also*
11. Everything:
    1. Cardinality of: 11,033
    2. Arity of: 31

*But that's only there in case something got deleted during coding, or for debugging purposes, not because it's in use.*

# List of Queries

1. Count the number of landslides that occurred in each country.
2. What is the total number of fatalities caused by each storm?
3. Find the names of all countries that have no landslides caused by storms.
4. Find the number of 'Christmas Miracles': count the landslides that occurred on Christmas with no injuries or deaths (0 or null).
5. Count the number of news reports made by each source.
6. Which country has the most accurate location measurements: count the 'exact' location accuracies that occur in each country.
7. Find the danger zones: For each closest gazetteer, find the average distance to a landslide, the smaller the average, the more 'dangerous'
8. Count the landslides caused by natural disaster type triggers for each country
9. Count the number of large to catastrophic landslides for each country
10. What is the total amount of injuries and fatalities reported by each source?
11. Find each event's degree distance from 'NULL Island' (lat: 0, long: 0)

In addition to select statements for all 10 individual tables.

*Note that I didn't make any queries for the import table since it's mostly metadata and not very interesting to work with.*

*Also note, for 11, I'm not sure how distance works for maps (considering the Earth is a sphere),
so I just treated lat and long as coords on a flat plane (because I don't want to learn how to find distance between points on the surface of a sphere).*

# Summary of the interface

The interface was written in Java using the java.awt and javax.swing libraries. It should be fairly simple to use, given its simple design.
The top drop down is for selecting a query, and the "Run" button is for running the currently selected query.
The result of the query will be output to the blank space in the center of the screen.

The second window that opens contains a single button that says "Export". By pressing this button the text area of the first
window will be exported to a .csv file with the name "export[current datetime].csv", only if the text area isn't empty.