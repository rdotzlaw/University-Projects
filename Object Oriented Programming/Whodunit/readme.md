
Compile program code with:
	`javac Card.java Guess.java IPlayer.java HPlayer.java CPlayer.java Model.java Main.java`
	
Run the program with:
	`java Main`
Then follow the onscreen instructions to play

Compile the testing suite with:
	`javac -cp .:junit-platform-console-standalone-1.6.0.jar TestGame.java`
	
Run the testing suite with:
	`java -jar junit-platform-console-standalone-1.6.0.jar --class-path . --scan-class-path`
