#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <ctime>
#include <iomanip>

using namespace std;

string getCurrentDate() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    stringstream ss;
    ss << 1900 + ltm->tm_year << "-"
       << 1 + ltm->tm_mon << "-"
       << ltm->tm_mday;
    return ss.str();
}


string calculateDueDate(int daysAfter) {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    ltm->tm_mday += daysAfter;

    mktime(ltm);

    stringstream ss;
    ss << 1900 + ltm->tm_year << "-"
       << setfill('0') << setw(2) << 1 + ltm->tm_mon << "-"
       << setfill('0') << setw(2) << ltm->tm_mday;
    return ss.str();
}


class User {
public:
    string username;
    string password;
    bool isManager;

    void signup();
    bool login();
};

void User::signup() {
    cout << "Enter username: "<<endl;
    cin.ignore();
    getline(cin, username);
    cout << "Enter password: "<<endl;
    cin >> password;
    cout << "Are you a manager? (1 for Yes, other for No): "<<endl;
    string temp;
    cin >> temp;
    if(temp=="1") isManager = true;
    else isManager = false;

    ofstream file("users.csv", ios::app);
    if (file.is_open()) {
        file << username << "," << password << "," << isManager << "\n";
        file.close();
        cout << "Signup successful!\n";
    } else {
        cout << "Error opening file.\n";
    }
}

bool User::login() {
    cout << "Enter username: \n";
    cin.ignore();
    getline(cin, username);
    cout << "Enter password: \n";
    cin >> password;

    ifstream file("users.csv");
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string storedUsername, storedPassword, storedIsManager;
        getline(ss, storedUsername, ',');
        getline(ss, storedPassword, ',');
        getline(ss, storedIsManager, ',');
        if (username == storedUsername && password == storedPassword) {
            isManager = (storedIsManager == "1");
            file.close();
            cout << "\nLogin successful!\n\n";
            return true;
        }
    }
    file.close();
    cout << "\nInvalid username or password.\n";
    return false;
}

class Book {
public:
    int id;
    string title;
    string author;
    bool isAvailable;

    void searchBook();
    bool borrowBook(int bookID, const string &username);
    void returnBook(int bookID, const string &username);
    void addBook();
};

void Book::searchBook() {
    cout << "Enter title or author to search: \n";
    string query;
    cin.ignore();
    getline(cin, query);

    ifstream file("books.csv");
    string line;
    bool found = false;

    while (getline(file, line)) {
        stringstream ss(line);
        string id, title, author, availability, amount;
        getline(ss, id, ',');
        getline(ss, title, ',');
        getline(ss, author, ',');
        getline(ss, availability, ',');
        getline(ss, amount, ',');

        if (title.find(query) != string::npos || author.find(query) != string::npos) {
            found = true;
            cout << "ID: " << id 
                 << ", Title: " << title
                 << ", Author: " << author
                 << ", Available: " << (availability == "1" ? "Yes" : "No")
                 << ", Amount: " << amount << " INR\n";
        }
    }

    file.close();

    if (!found) {
        cout << "No matching books found.\n";
    }
}

bool Book::borrowBook(int bookID, const string &username) {
    ifstream file("books.csv");
    ofstream temp("temp.csv");
    string line;
    bool borrowed = false;

    while (getline(file, line)) {
        stringstream ss(line);
        string id, title, author, availability, amount;
        getline(ss, id, ',');
        getline(ss, title, ',');
        getline(ss, author, ',');
        getline(ss, availability, ',');
        getline(ss, amount, ',');

        if (stoi(id) == bookID && availability == "1") {
            borrowed = true;

            temp << id << "," << title << "," << author << ",0," << amount << "\n";

            ofstream borrowFile("borrow_records.csv", ios::app);
            borrowFile << username << "," << id << "," << getCurrentDate() << "," << calculateDueDate(15) << "\n"; // 15 days after current date
            borrowFile.close();

            cout << "Book borrowed successfully!\n";
            cout << "Title: " << title << "\n";
            cout << "Author: " << author << "\n";
            cout << "Amount: " << amount << " INR\n";
        } else {
            temp << line << "\n";
        }
    }

    file.close();
    temp.close();
    remove("books.csv");
    rename("temp.csv", "books.csv");

    if (!borrowed) {
        cout << "Book is not available or invalid ID.\n";
        return false;
    }

    return true;
}

void Book::returnBook(int bookID, const string &username) {
    ifstream borrowFile("borrow_records.csv");
    ofstream tempBorrow("temp_borrow.csv");
    string line;
    bool returned = false;

    while (getline(borrowFile, line)) {
        stringstream ss(line);
        string user, id, borrowDate, dueDate;
        getline(ss, user, ',');
        getline(ss, id, ',');
        getline(ss, borrowDate, ',');
        getline(ss, dueDate, ',');

        if (user == username && stoi(id) == bookID) {
            returned = true;

            string currentDate = getCurrentDate();
            if (currentDate > dueDate) { 
                cout << "Book returned late.\n";
                cout << "Fine amount: 100 INR\n";
            } else {
                cout << "Book returned on time. No fine.\n";
            }
        } else {
            tempBorrow << line << "\n";
        }
    }

    borrowFile.close();
    tempBorrow.close();
    remove("borrow_records.csv");
    rename("temp_borrow.csv", "borrow_records.csv");

    if (returned) {
        ifstream bookFile("books.csv");
        ofstream tempBook("temp_book.csv");
        string bookDetails;

        while (getline(bookFile, line)) {
            stringstream ss(line);
            string id, title, author, availability, amount;
            getline(ss, id, ',');
            getline(ss, title, ',');
            getline(ss, author, ',');
            getline(ss, availability, ',');
            getline(ss, amount, ',');

            if (stoi(id) == bookID) {
                bookDetails = "Title: " + title + ", Author: " + author + ", Amount: " + amount + " INR";
                tempBook << id << "," << title << "," << author << ",1," << amount << "\n";
            } else {
                tempBook << line << "\n";
            }
        }

        bookFile.close();
        tempBook.close();
        remove("books.csv");
        rename("temp_book.csv", "books.csv");

        cout << "Book returned successfully!\n";
        if (!bookDetails.empty()) {
            cout << "Book Details: " << bookDetails << "\n";
        }
    } else {
        cout << "No record found for this book ID.\n";
    }
}


void Book::addBook() {
    string title, author;
    double amount;

    cout << "Enter book title: ";
    cin.ignore();
    getline(cin, title);

    cout << "Enter book author: ";
    getline(cin, author);

    cout << "Enter book amount (price): ";
    cin >> amount;

    ifstream file("books.csv");
    int lastID = 0;
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        string id;
        getline(ss, id, ',');
        lastID = stoi(id);
    }
    file.close();

    ofstream outFile("books.csv", ios::app);

    outFile << lastID + 1 << "," << title << "," << author << ",1," << fixed << setprecision(2) << amount << "\n";
    outFile.close();

    cout << "\nBook added successfully with amount: " << fixed << setprecision(2) << amount << " INR.\n";
}


class Library {
private:
    User user;
    Book book;

public:
    void start();
};

void Library::start() {
    int choice;
    bool loggedIn = false;

    while (true) {
        cout << "\nLibrary Management System\n";
        cout << "1. Signup\n2. Login\n";
        if (loggedIn) {
            cout << "3. Search Book\n4. Borrow Book\n5. Return Book\n";
            if (user.isManager) {
                cout << "6. Add Book (Manager Only)\n";
            }
        }
        cout << "7. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            user.signup();
            break;
        case 2:
            loggedIn = user.login();
            break;
        case 3:
            if (loggedIn) {
                book.searchBook();
            } else {
                cout << "Please login first.\n";
            }
            break;
        case 4:
            if (loggedIn) {
                int bookID;
                cout << "Enter book ID to borrow: ";
                cin >> bookID;
                book.borrowBook(bookID, user.username);
            } else {
                cout << "Please login first.\n";
            }
            break;
        case 5:
            if (loggedIn) {
                int bookID;
                cout << "Enter book ID to return: ";
                cin >> bookID;
                book.returnBook(bookID, user.username);
            } else {
                cout << "Please login first.\n";
            }
            break;
        case 6:
            if (loggedIn && user.isManager) {
                book.addBook();
            } else if (!loggedIn) {
                cout << "Please login first.\n";
            } else {
                cout << "You do not have permission to add books.\n";
            }
            break;
        case 7:
            cout << "Exiting...\n";
            return;
        default:
            cout << "Invalid choice. Try again.\n";
        }
    }
}

int main() {
    Library library;
    library.start();
    return 0;
}
