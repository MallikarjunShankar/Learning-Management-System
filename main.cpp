#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

class User {

private:

    string id;
    string name;
    string email;
    string password;

public:

    User(){}

    User(string ID, string NAME, string EMAIL, string PASSWORD) {
        id = ID;
        name = NAME;
        email = EMAIL;
        password = PASSWORD;
    }

    void registerUser(string ID, string NAME, string EMAIL, string PASSWORD) {
        id = ID;
        name = NAME;
        email = EMAIL;
        password = PASSWORD;
    }

    bool login(string EMAIL, string PASSWORD) {
        return(EMAIL == email && PASSWORD == password);
    }

    string getName(){
        return name;
    }

    string getEmail(){
        return email;
    }

    string getId(){
        return id;
    }
};