#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>

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
        if (courseName.empty()) {
            throw runtime_error("Course name cannot be empty");
        }

        for (const auto &c : courses) {
            if (c.courseName == courseName) {
                throw runtime_error("Already enrolled in course: " + courseName);
            }
        }

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
    throw runtime_error("Course not found: " + courseName);
}

    void viewCourses() {
    if (courses.empty()) {
        cout << "No courses enrolled\n";
        return;
    }

    cout << "\nEnrolled Courses:\n";
    for (const auto &c : courses) {
        cout << "- " << c.courseName
             << " | Status: "
             << (c.completed ? "Completed" : "In Progress")
             << endl;
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

class Course {

private:

    string courseName;
    vector<string> notesFiles;

public:

    Course() {
    }

    Course(string name) {
        courseName = name;
    }

    string getCourseName() const {
        return courseName;
    }

    void addNoteFile(string filePath) {
        if (filePath.empty()) {
            throw runtime_error("File path cannot be empty");
        }

        notesFiles.push_back(filePath);
    }

    void displayContent() {
        if (notesFiles.empty()) {
            cout << "No notes available for this course" << endl;
            return;
        }

        for (const auto &file : notesFiles) {
            ifstream inFile(file);

            if (!inFile) {
                throw runtime_error("Error opening file: " + file);
            }

            cout << "\nContent from: " << file << " ---\n";

            string line;
            while (getline(inFile, line)) {
                cout << line << endl;
            }

            inFile.close();
        }
    }
};

class AuthController {

private:
    vector<Student> students;
    vector<Teacher> teachers;

public:

    void registerStudent(string id, string name, string email, string password) {
        Student s(id, name, email, password);
        students.push_back(s);

        cout << "Student registered successfully" << endl;
    }

    void registerTeacher(string id, string name, string email, string password, int code, string subject) {
        Teacher t(id, name, email, password, code, subject);
        teachers.push_back(t);

        cout << "Teacher registered successfully" << endl;
    }

    Student* loginStudent(string email, string password) {
        for (auto& s : students) {
            if (s.login(email, password)) {
                cout << "Student login successful." << endl;
                return &s;
            }
        }

        throw runtime_error("Invalid student credentials");
    }

    Teacher* loginTeacher(string email, string password) {
        for (auto& t : teachers) {
            if (t.login(email, password)) {
                cout << "Teacher login successful." << endl;
                return &t;
            }
        }

        throw runtime_error("Invalid teacher credentials");
    }
};

class CourseController {

private:
    vector<Course> courses;

public:

    void createCourse(string courseName) {
        if (courseName.empty()) {
            throw runtime_error("Course name cannot be empty");
        }

        for (const auto &c : courses) {
            if (c.getCourseName() == courseName) {
                throw runtime_error("Course already exists: " + courseName);
            }
        }

        Course c(courseName);
        courses.push_back(c);

        cout << "Course created: " << courseName << endl;
    }

    Course* getCourse(string courseName) {
        for (auto &c : courses) {
            if (c.getCourseName() == courseName) {
                return &c;
            }
        }

        throw runtime_error("Course not found: " + courseName);
    }

    void listCourses() {
        if (courses.empty()) {
            cout << "No courses available.\n";
            return;
        }

        cout << "\nAvailable Courses:\n";
        for (const auto &c : courses) {
            cout << "- " << c.getCourseName() << endl;
        }
    }
};

class NotesController {

private:
    CourseController &courseController;

public:

    NotesController(CourseController &cc) : courseController(cc) {
    }

    void addNoteToCourse(string courseName, string filePath) {
        if (filePath.empty()) {
            throw runtime_error("File path cannot be empty");
        }

        Course* course = courseController.getCourse(courseName);
        course->addNoteFile(filePath);
        cout << "Notes added to course: " << courseName << endl;
    }

    void viewCourseNotes(string courseName) {
        Course* course = courseController.getCourse(courseName);
        course->displayContent();
    }
};