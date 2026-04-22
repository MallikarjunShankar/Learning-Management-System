# Learning Management System

A simple command-line LMS built with C++ demonstrating OOP principles, inheritance, and STL containers.

## Features

- **Student Management**: Register and login for students
- **Teacher Management**: Register and login for teachers
- **Course Management**: Create and manage courses
- **Enrollments**: Students can enroll in courses (persistent across sessions)
- **Assignments**: Teachers create assignments, students submit them
- **Notes**: Teachers add course notes, students view them
- **Input Validation**: Robust error handling for invalid inputs
- **Data Persistence**: All data saved to files

## Requirements

- C++ compiler (g++, clang, or MSVC)
- C++11 or later

## Compilation

```bash
g++ -Wall -Wextra -g3 main.cpp -o output/main.exe
```

## Running

```bash
./output/main.exe
```

## Usage

### Main Menu
1. Register Student - Create a new student account
2. Register Teacher - Create a new teacher account
3. Login Student - Access student dashboard
4. Login Teacher - Access teacher dashboard
0. Exit - Quit the program

### Student Features
- View available courses
- Enroll in courses
- View course notes
- Submit assignments

### Teacher Features
- Create courses
- Add course notes
- Create assignments

## File Storage

- `users.txt` - Registered students and teachers
- `courses.txt` - Available courses
- `enrollments.txt` - Student enrollments
- `assignments.txt` - Course assignments
- `submissions.txt` - Student submissions
- `{courseName}_notes.txt` - Course notes

## Example Usage

```
Register a student: S001 | John Doe | john@example.com | password123
Login: john@example.com | password123
Enroll: Mathematics 101
View Notes: Mathematics 101
```

## Built With

- C++ Standard Library (iostream, vector, fstream, algorithm)
- Object-Oriented Programming
- Exception Handling
- File I/O
