#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <algorithm>

using namespace std;

// Input validation utilities
class InputHandler {
public:
    static string getSafeString(const string& prompt = "") {
        if (!prompt.empty()) cout << prompt;
        string input;
        getline(cin, input);
        
        // Trim whitespace
        input.erase(0, input.find_first_not_of(" \t\n\r"));
        input.erase(input.find_last_not_of(" \t\n\r") + 1);
        
        if (input.empty()) {
            throw runtime_error("Input cannot be empty");
        }
        return input;
    }

    static int getSafeInt(const string& prompt = "") {
        if (!prompt.empty()) cout << prompt;
        int value;
        while (!(cin >> value)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input. Please enter a number: ";
        }
        cin.ignore(10000, '\n');
        return value;
    }

    static string getSafeEmail(const string& prompt = "") {
        string email = getSafeString(prompt);
        if (email.find('@') == string::npos) {
            throw runtime_error("Invalid email format");
        }
        return email;
    }
};

// File I/O utilities with existence checks
class FileManager {
public:
    static bool fileExists(const string& filename) {
        ifstream file(filename);
        return file.good();
    }

    static void saveLine(const string& filename, const string& data) {
        ofstream file(filename, ios::app);
        if (!file.is_open()) {
            throw runtime_error("Cannot open file: " + filename);
        }
        file << data << endl;
        file.close();
    }

    static vector<string> loadFile(const string& filename) {
        vector<string> data;
        if (!fileExists(filename)) {
            return data; // Return empty vector if file doesn't exist
        }
        
        ifstream file(filename);
        if (!file.is_open()) {
            throw runtime_error("Cannot open file: " + filename);
        }
        
        string line;
        while (getline(file, line)) {
            if (!line.empty()) {
                data.push_back(line);
            }
        }
        file.close();
        return data;
    }
};

// Base user
class User {
protected:
    string id, name, email, password;

public:
    User() {}

    User(string ID, string NAME, string EMAIL, string PASSWORD)
        : id(ID), name(NAME), email(EMAIL), password(PASSWORD) {}

    virtual bool login(string EMAIL, string PASSWORD) {
        return EMAIL == email && PASSWORD == password;
    }

    virtual string getRole() = 0;

    string getEmail() { return email; }
    string getName() { return name; }
    string getID() { return id; }
};

// Student role
struct CourseProgress {
    string courseName;
    bool completed;
};

class Student : public User {
private:
    vector<CourseProgress> courses;
    bool enrollmentsLoaded = false;

public:
    Student() {}

    Student(string id, string name, string email, string password)
        : User(id, name, email, password) {}

    string getRole() override { return "Student"; }

    void loadEnrollments() {
        if (enrollmentsLoaded) return;
        
        auto data = FileManager::loadFile("enrollments.txt");
        for (auto &line : data) {
            stringstream ss(line);
            string storedEmail, courseName;
            
            getline(ss, storedEmail, '|');
            getline(ss, courseName, '|');
            
            if (storedEmail == email) {
                auto it = find_if(courses.begin(), courses.end(),
                    [&](CourseProgress c) { return c.courseName == courseName; });
                
                if (it == courses.end()) {
                    courses.push_back({courseName, false});
                }
            }
        }
        enrollmentsLoaded = true;
    }

    void enroll(string courseName) {
        if (courseName.empty()) {
            throw runtime_error("Course name cannot be empty");
        }
        
        auto it = find_if(courses.begin(), courses.end(),
            [&](CourseProgress c) { return c.courseName == courseName; });

        if (it != courses.end())
            throw runtime_error("Already enrolled");

        courses.push_back({courseName, false});
        FileManager::saveLine("enrollments.txt", email + "|" + courseName);

        cout << "Enrolled in " << courseName << endl;
    }

    void viewCourses() {
        loadEnrollments();
        
        if (courses.empty()) {
            cout << "No courses enrolled\n";
            return;
        }

        for (auto &c : courses) {
            cout << c.courseName << " | "
                 << (c.completed ? "Completed" : "In Progress") << endl;
        }
    }
};

// Teacher role
class Teacher : public User {
private:
    int subjectCode;
    string subjectName;

public:
    Teacher() {}

    Teacher(string id, string name, string email, string password, int code, string subject)
        : User(id, name, email, password), subjectCode(code), subjectName(subject) {}

    string getRole() override { return "Teacher"; }

    void createAssignment(string courseName, string title) {
        if (courseName.empty() || title.empty()) {
            throw runtime_error("Course name and title cannot be empty");
        }
        FileManager::saveLine("assignments.txt", courseName + "|" + title);
        cout << "Assignment created.\n";
    }

    void addNotes(string courseName) {
        if (courseName.empty()) {
            throw runtime_error("Course name cannot be empty");
        }
        
        string filename = courseName + "_notes.txt";
        ofstream file(filename, ios::app);
        
        if (!file.is_open()) {
            throw runtime_error("Cannot open notes file");
        }

        cout << "Enter notes (type END on a new line to stop):\n";

        string line;
        while (getline(cin, line)) {
            if (line == "END") break;
            file << line << endl;
        }

        file.close();
        cout << "Notes saved.\n";
    }
};

// Course structure
class Course {
private:
    string courseName;

public:
    Course() {}
    Course(string name) : courseName(name) {}

    string getCourseName() const { return courseName; }
};

// Authentication manager
class AuthController {
private:
    vector<Student> students;
    vector<Teacher> teachers;

public:
    void loadFromFile() {
        auto data = FileManager::loadFile("users.txt");

        for (auto &line : data) {
            stringstream ss(line);
            string id, name, email, password, role;

            getline(ss, id, '|');
            getline(ss, name, '|');
            getline(ss, email, '|');
            getline(ss, password, '|');
            getline(ss, role, '|');

            if (role == "Student") {
                students.emplace_back(id, name, email, password);
            }
            else if (role == "Teacher") {
                teachers.emplace_back(id, name, email, password, 0, "NA");
            }
        }
    }

    void registerStudent(string id, string name, string email, string password) {
        // Validate inputs
        if (id.empty() || name.empty() || email.empty() || password.empty()) {
            throw runtime_error("All fields required");
        }
        if (email.find('@') == string::npos) {
            throw runtime_error("Invalid email format");
        }
        
        students.emplace_back(id, name, email, password);
        FileManager::saveLine("users.txt", id + "|" + name + "|" + email + "|" + password + "|Student");
        cout << "Student registered\n";
    }

    void registerTeacher(string id, string name, string email, string password, int code, string subject) {
        // Validate inputs
        if (id.empty() || name.empty() || email.empty() || password.empty() || subject.empty()) {
            throw runtime_error("All fields required");
        }
        if (email.find('@') == string::npos) {
            throw runtime_error("Invalid email format");
        }
        
        teachers.emplace_back(id, name, email, password, code, subject);
        FileManager::saveLine("users.txt", id + "|" + name + "|" + email + "|" + password + "|Teacher");
        cout << "Teacher registered\n";
    }

    // Use indices instead of pointers to avoid dangling pointers
    int loginStudent(string email, string password) {
        for (size_t i = 0; i < students.size(); i++) {
            if (students[i].login(email, password)) {
                students[i].loadEnrollments(); // Load enrollments on login
                return (int)i;
            }
        }
        throw runtime_error("Invalid student login");
    }

    int loginTeacher(string email, string password) {
        for (size_t i = 0; i < teachers.size(); i++) {
            if (teachers[i].login(email, password)) {
                return (int)i;
            }
        }
        throw runtime_error("Invalid teacher login");
    }

    Student& getStudent(int index) {
        if (index < 0 || index >= (int)students.size()) {
            throw runtime_error("Invalid student index");
        }
        return students[index];
    }

    Teacher& getTeacher(int index) {
        if (index < 0 || index >= (int)teachers.size()) {
            throw runtime_error("Invalid teacher index");
        }
        return teachers[index];
    }
};

// Course manager
class CourseController {
private:
    vector<Course> courses;

public:
    void loadCoursesFromFile() {
        if (!FileManager::fileExists("courses.txt")) {
            return;
        }
        
        auto data = FileManager::loadFile("courses.txt");
        for (auto &c : data) {
            if (!c.empty()) {
                courses.emplace_back(c);
            }
        }
    }

    void createCourse(string name) {
        if (name.empty()) {
            throw runtime_error("Course name cannot be empty");
        }
        
        if (exists(name)) {
            throw runtime_error("Course already exists");
        }
        
        courses.emplace_back(name);
        FileManager::saveLine("courses.txt", name);
        cout << "Course created\n";
    }

    void listCourses() {
        if (courses.empty()) {
            cout << "No courses available\n";
            return;
        }
        
        for (auto &c : courses)
            cout << c.getCourseName() << endl;
    }

    bool exists(string name) {
        return any_of(courses.begin(), courses.end(),
            [&](Course &c) { return c.getCourseName() == name; });
    }
};

// Notes manager
class NotesController {
public:
    void viewNotes(string courseName) {
        if (courseName.empty()) {
            throw runtime_error("Course name cannot be empty");
        }
        
        string filename = courseName + "_notes.txt";
        
        if (!FileManager::fileExists(filename)) {
            cout << "No notes found for this course\n";
            return;
        }
        
        ifstream file(filename);
        if (!file.is_open()) {
            throw runtime_error("Cannot open notes file");
        }
        
        string line;
        bool hasContent = false;
        
        while (getline(file, line)) {
            cout << line << endl;
            hasContent = true;
        }
        
        if (!hasContent) {
            cout << "No notes found for this course\n";
        }
        
        file.close();
    }
};

// Assignment manager
class AssignmentController {
public:
    void submit(string email, string course, string title) {
        if (email.empty() || course.empty() || title.empty()) {
            throw runtime_error("Email, course, and title cannot be empty");
        }
        FileManager::saveLine("submissions.txt", email + "|" + course + "|" + title);
        cout << "Submitted\n";
    }

    void view(string course) {
        if (course.empty()) {
            throw runtime_error("Course name cannot be empty");
        }
        
        auto data = FileManager::loadFile("assignments.txt");
        bool found = false;

        for (auto &line : data) {
            stringstream ss(line);
            string courseName, title;
            
            getline(ss, courseName, '|');
            getline(ss, title, '|');
            
            // Exact match instead of substring
            if (courseName == course) {
                cout << title << endl;
                found = true;
            }
        }
        
        if (!found) {
            cout << "No assignments found for this course\n";
        }
    }
};

// Main execution
int main() {
    AuthController auth;
    CourseController courseCtrl;
    NotesController notesCtrl;
    AssignmentController assignCtrl;

    auth.loadFromFile();
    courseCtrl.loadCoursesFromFile();

    int choice;

    while (true) {
        try {
            cout << "\n==== Learning Management System ====\n";
            cout << "1. Register Student\n2. Register Teacher\n3. Login Student\n4. Login Teacher\n0. Exit\n";
            cout << "Enter choice: ";
            
            choice = InputHandler::getSafeInt();

            if (choice == 1) {
                cout << "\n--- Student Registration ---\n";
                string id = InputHandler::getSafeString("Enter ID: ");
                string name = InputHandler::getSafeString("Enter Name: ");
                string email = InputHandler::getSafeEmail("Enter Email: ");
                string password = InputHandler::getSafeString("Enter Password: ");
                
                auth.registerStudent(id, name, email, password);
            }
            else if (choice == 2) {
                cout << "\n--- Teacher Registration ---\n";
                string id = InputHandler::getSafeString("Enter ID: ");
                string name = InputHandler::getSafeString("Enter Name: ");
                string email = InputHandler::getSafeEmail("Enter Email: ");
                string password = InputHandler::getSafeString("Enter Password: ");
                int code = InputHandler::getSafeInt("Enter Subject Code: ");
                string subject = InputHandler::getSafeString("Enter Subject Name: ");
                
                auth.registerTeacher(id, name, email, password, code, subject);
            }
            else if (choice == 3) {
                cout << "\n--- Student Login ---\n";
                string email = InputHandler::getSafeEmail("Enter Email: ");
                string password = InputHandler::getSafeString("Enter Password: ");

                int studentIdx = auth.loginStudent(email, password);
                Student& student = auth.getStudent(studentIdx);

                int ch;
                do {
                    cout << "\n-- Student Menu --\n";
                    cout << "1. View Courses\n2. Enroll\n3. View Notes\n4. Submit Assignment\n0. Logout\n";
                    cout << "Enter choice: ";
                    
                    ch = InputHandler::getSafeInt();

                    if (ch == 1) {
                        courseCtrl.listCourses();
                    }
                    else if (ch == 2) {
                        string c = InputHandler::getSafeString("Enter course name: ");
                        student.enroll(c);
                    }
                    else if (ch == 3) {
                        string c = InputHandler::getSafeString("Enter course name: ");
                        notesCtrl.viewNotes(c);
                    }
                    else if (ch == 4) {
                        string c = InputHandler::getSafeString("Enter course name: ");
                        string t = InputHandler::getSafeString("Enter assignment title: ");
                        assignCtrl.submit(student.getEmail(), c, t);
                    }
                    else if (ch != 0) {
                        cout << "Invalid choice\n";
                    }

                } while (ch != 0);
                
                cout << "Logged out\n";
            }
            else if (choice == 4) {
                cout << "\n--- Teacher Login ---\n";
                string email = InputHandler::getSafeEmail("Enter Email: ");
                string password = InputHandler::getSafeString("Enter Password: ");

                int teacherIdx = auth.loginTeacher(email, password);
                Teacher& teacher = auth.getTeacher(teacherIdx);

                int ch;
                do {
                    cout << "\n-- Teacher Menu --\n";
                    cout << "1. Create Course\n2. Add Notes\n3. Create Assignment\n0. Logout\n";
                    cout << "Enter choice: ";
                    
                    ch = InputHandler::getSafeInt();

                    if (ch == 1) {
                        string c = InputHandler::getSafeString("Enter course name: ");
                        courseCtrl.createCourse(c);
                    }
                    else if (ch == 2) {
                        string c = InputHandler::getSafeString("Enter course name: ");
                        teacher.addNotes(c);
                    }
                    else if (ch == 3) {
                        string c = InputHandler::getSafeString("Enter course name: ");
                        string tname = InputHandler::getSafeString("Enter assignment title: ");
                        teacher.createAssignment(c, tname);
                    }
                    else if (ch != 0) {
                        cout << "Invalid choice\n";
                    }

                } while (ch != 0);
                
                cout << "Logged out\n";
            }
            else if (choice == 0) {
                cout << "Goodbye!\n";
                break;
            }
            else {
                cout << "Invalid choice. Please try again.\n";
            }

        } catch (exception &e) {
            cout << "Error: " << e.what() << endl;
        }
    }

    return 0;
}