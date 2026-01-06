# Long Covid Associative Rule Mining

### Input
This program works explicitly with a cut-down/formatted version of [this census](https://www.census.gov/programs-surveys/household-pulse-survey/datasets.html) from the US Census Bureau, called _US_Week49_COVID.csv_ in the current directory.
The modifications were done to clean up the data and remove irrelevant data.
---
### Output

This program will generate three .csv files:
* _US_Week49_COVID-output.csv_,
  * Which includes all associative rules mined that have "Long Covid: Yes" or "Long Covid: No" as the consequents, sorted in confidence ascending.
* _US_Week49_COVID-LC.csv_
  * Which includes only the associative rules that have "Long Covid: Yes" as the consequent, sorted in confidence ascending.
* _US_Week49_COVID-NoLC.csv_
  * Which includes only the associative rules that have "Long Covid: No" as the consequent, sorted in confidence ascending.

The *LC* and *NoLC* versions of the file are for use with the Associative Rule bar graphs. The choice to label the x-axis of the bar graphs with an index for lookup, instead of the association rule, was done because some association rules were deemed too long to show up in the graph.

The program will also generate many graphs, showing a break-down of the demographics of the dataset for use in the analysis section of the project.

---
### Execution
To run the program, simply ensure that the file *main.py* is in the same directory as the dataset, *US_Week49_COVID.csv*, and then run *main.py* with python.

        python3 main.py
---

### Implementation
This program was implemented using the following libraries:
* mlextend
  * For the apriori frequent pattern mining, and the associative rule mining.
* mathplotlib
  * For creating and rendering the graphs.
* pandas
  * For dataframe manipulation, operations, and output to *.csv* files.
* numpy
  * For extra math operations.
* math
  * For more extra math operations.