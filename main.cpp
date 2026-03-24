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

struct CourseProgress {
    string courseName;
    bool completed;
};

class Student : public User {

private:

    vector<CourseProgress> courses;

public:

    Student(){
    }

    Student(string id, string name, string email, string password) : User(id, name, email, password) {
    }

    void enroll(string courseName) {
        CourseProgress cp;
        cp.courseName = courseName;
        cp.completed = false;

        courses.push_back(cp);

        cout << "Enrolled in course: " << courseName << endl;
    }

    void markComplete(string courseName){
        for (auto &c : courses) {
            if (c.courseName == courseName) {
                c.completed = true;
                cout << "Course marked complete: " << courseName << endl;
                return;
            }
        }
        cout << "Course not found" << endl;
    }

    void viewCourses() {
        cout << "\nEnrolled Courses: \n";
        for (const auto &c : courses) {
            cout << "- " << c.courseName << " | Status: " << (c.completed ? "Completed" : "In Progress") << endl;
        }
    }

    void submitAssignment(string courseName) {
        cout << "Assignment submitted for: " << courseName << endl;
    }
};

class Teacher : public User {

private:

    int subjectCode;
    string subjectName;

public:

    Teacher() {
    }

    Teacher(string id, string name, string email, string password, int code, string subject)
        : User(id, name, email, password) {
        subjectCode = code;
        subjectName = subject;
    }

    void createAssignment(string courseName) {
        cout << "Assignment created for: " << courseName << endl;
    }

    void addNotes(string filePath) {
        cout << "Note added: " << filePath << endl;
    }

    string getSubjectName() {
        return subjectName;
    }
};