
Compile program code with:
	
```
javac Entity.java Obj.java User.java Node.java List.java Doc.java Wiki.java WikiIn.java Main.java
```

Place any .txt file to read in from in the same directory as the .java files

Run the program with:
	`java Main`,
Then just type the file name without any extention to read in from it

Compile the testing suite with:
	`javac -cp .:junit-platform-console-standalone-1.6.0.jar TestWiki.java`

Run the testing suite with:
	`java -jar junit-platform-console-standalone-1.6.0.jar --class-path . --scan-class-path`
	
