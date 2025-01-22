
#### Ryan Dotzlaw

Do tasks 2 and 3

#### Setup

##### Add address to etc/hosts
![](0a.png)

##### Build docker containers
![](0b.png)

##### Start docker containers
![](0c.png)

#### Task 2.1: Webpage Select Injection

On the login page...

![](1a.png)

We can easily login as the admin using the following credentials:
```
	Username: "Admin';-- "
	Password: ""
```
![](1b.png)

We can see we now have access to the `admin` account's home page.

![](1c.png)

#### Task 2.2: Commandline Select Injection

Now we want to send a `curl` to the webpage and pass parameters in the URL to perform a SQL Injection.

With the following command:

```
	curl 'www.seed-server.com/unsafe_home.php?username=Admin%27;--%20&Password='
```

Which results in:

![](1d.png)

Which is hard to read, but theres clearly a logout function and some kind of table, meaning we successfully logged in.


#### Task 2.3: Webpage Append Injection

To run multiple SQL commands, we just need to add a second statement after the semicolon in our 2.1 input.

We can do this using the following credentials:

```
	Username: "Admin'; Update credentials set Nickname='Badmin' where eid=99999;# "
	Password: ""	
```

However, this will always return a syntax error.

![](1e.png)

This is becasue the `PHP` function, `$query` used in the login page cannot perform multiple queries with one input by design.

Here's a snippet from the `PHP` manual on this [topic](https://www.php.net/manual/en/mysqli.quickstart.multiple-statement.php).

![](1f.png)

#### Task 3.1: Increase Alice's Salary

Now we want to increase the salary of the 'Alice' account.

We can change our the salary with the following inputs:

![](2a.png)

As we can see, the changes went through successfully.

![](2b.png)

#### Task 3.2: Set Boby's Salary to 1

To do this we need to modify the salary value, and also change the where clasue.

We can do this with the following inputs.

![](2c.png)

```
PhoneNumber: "', salary='1' where name='boby';-- "
```

As we can see using the docker shell, the changes went through.

![](2d.png)

#### Task 3.3: Change Boby's Password

Now we want to change Boby's password without logging into their account.

We need to change the value in the phone number field to modify the where clause.

Additionally we need to input the new password into the password field.

By modifying the where clause, the password we input will be hashed and applied to Boby's account instead of Alice's

![](2e.png)

```
PhoneNumber: "' where name='boby';-- "
Password: "AliceIs#1"
```

Then, we can confirm this by logging into Boby's account.

![](2f.png)

