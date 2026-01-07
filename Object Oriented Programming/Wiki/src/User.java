public class User extends Obj{

    //More or less just for the label
    //Obj class has all the needed functions for user including
    //a list of logs to keep track of commands
    //this class could be removed, and Obj could be renamed to User,
    //but then having Doc extend User would be confusing
    public User(String id) {
        super(id);
    }
}
