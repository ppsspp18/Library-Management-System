#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <unordered_map>
#include <iomanip>
#include <ctime>

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

time_t parseDateToTime(const string& dateStr) {
    tm timeStruct = {};
    sscanf(dateStr.c_str(), "%d-%d-%d", &timeStruct.tm_year, &timeStruct.tm_mon, &timeStruct.tm_mday);
    timeStruct.tm_year -= 1900;  
    timeStruct.tm_mon -= 1;      
    timeStruct.tm_hour = 0;
    timeStruct.tm_min = 0;
    timeStruct.tm_sec = 0;
    return mktime(&timeStruct);
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
    void viewReadingHistory(const string &username);
    void viewAllOldRecords();
    void addBook();
    void viewBorrowedBooks(const string &username); 
    void deleteBook();
    void updateBookDetails();
    void viewOverdueBooks();

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
            int avail = stoi(availability);
            cout << "ID: " << id 
                 << ", Title: " << title
                 << ", Author: " << author
                 << ", Available: " << (avail >= 1 ? "Yes" : "No")
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
    bool isbook = false;

    while (getline(file, line)) {
        stringstream ss(line);
        string id, title, author, availabilityStr, amount;
        getline(ss, id, ',');
        getline(ss, title, ',');
        getline(ss, author, ',');
        getline(ss, availabilityStr, ',');
        getline(ss, amount, ',');

        int availability = stoi(availabilityStr);
        int price = stoi(amount);
        if(stoi(id) == bookID ) isbook = true;
        if (stoi(id) == bookID && availability > 0) {
            borrowed = true;
            availability--;  // decrease quantity by 1

            temp << id << "," << title << "," << author << "," << availability << "," << amount << "\n";
            
            int numnber_of_days;
            cout << "enter number of days : ";
            cin >> numnber_of_days;

            int total_amount = numnber_of_days*price;
            ofstream borrowFile("borrow_records.csv", ios::app);
            borrowFile << username << "," << id << "," << title << "," << getCurrentDate() << "," << calculateDueDate(numnber_of_days) << "\n";
            borrowFile.close();

            cout << "Book borrowed successfully!\n";
            cout << "Title: " << title << "\n";
            cout << "Author: " << author << "\n";
            cout << "perday amount: " << amount << "\n";
            cout << "Total Amount: " << total_amount << " INR\n";
        } else {
            temp << line << "\n";
        }
    }

    file.close();
    temp.close();
    remove("books.csv");
    rename("temp.csv", "books.csv");
    
    if (!isbook){
        cout << "invalid book id. \n";
        return false;
    }
    if (!borrowed) {
        cout << "Book is not available. \n";
        return false;
    }

    return true;
}


void Book::returnBook(int bookID, const string &username) {
    ifstream borrowFile("borrow_records.csv");
    ofstream tempBorrow("temp_borrow.csv");
    ofstream oldOrder("old_order.csv", ios::app); 
    string line;
    bool returned = false;
    string borrowDate, dueDate, bookTitle;
    int daysLate;
    while (getline(borrowFile, line)) {
        stringstream ss(line);
        string user, id, title, bDate, dDate;
        getline(ss, user, ',');
        getline(ss, id, ',');
        getline(ss, title, ',');
        getline(ss, bDate, ',');
        getline(ss, dDate, ',');

        if (user == username && stoi(id) == bookID) {
            returned = true;
            borrowDate = bDate;
            dueDate = dDate;

            string currentDate = getCurrentDate();

            time_t current = parseDateToTime(currentDate);
            time_t due = parseDateToTime(dueDate);
            
            double secondsLate = difftime(current, due);
            daysLate = static_cast<int>(secondsLate / (60 * 60 * 24));

            if (difftime(current, due) > 0) {
                cout << "Book returned late.\n";
            } else {
                cout << "Book returned on time. No fine.\n";
            }

            oldOrder << username << "," << bookID << "," << title << "," 
                     << borrowDate << "," << dueDate << "," << currentDate << "\n";

        } else {
            tempBorrow << line << "\n";
        }
    }

    borrowFile.close();
    tempBorrow.close();
    oldOrder.close();
    remove("borrow_records.csv");
    rename("temp_borrow.csv", "borrow_records.csv");

    if (returned) {
        ifstream bookFile("books.csv");
        ofstream tempBook("temp_book.csv");
        string bookLine;

        while (getline(bookFile, bookLine)) {
            stringstream ss(bookLine);
            string id, title, author, availabilityStr, amount;
            getline(ss, id, ',');
            getline(ss, title, ',');
            getline(ss, author, ',');
            getline(ss, availabilityStr, ',');
            getline(ss, amount, ',');

            int availability = stoi(availabilityStr);
            int price = stoi(amount);
            price += 10;
            int fine = price * daysLate;
            if (stoi(id) == bookID) {
                availability++;  
                cout << "Extra fine due to delay(10 Inr extra for every day): " << fine << "\n";
                bookTitle = title;  
                tempBook << id << "," << title << "," << author << "," << availability << "," << amount << "\n";
            } else {
                tempBook << bookLine << "\n";
            }
        }

        bookFile.close();
        tempBook.close();
        remove("books.csv");
        rename("temp_book.csv", "books.csv");

        cout << "Book returned successfully!\n";
        cout << "Book Details: Title: " << bookTitle << "\n";
    } else {
        cout << "No record found for this book ID.\n";
    }
}


void Book::viewReadingHistory(const string &username) {
    ifstream file("old_order.csv");
    string line;
    bool found = false;

    cout << "\nReading History for user: " << username << "\n";
    cout << "--------------------------------------------------\n";
    cout << "BookID | Title | Borrowed | Due | Returned\n";

    while (getline(file, line)) {
        stringstream ss(line);
        string user, id, title, bDate, dDate, retDate;
        getline(ss, user, ',');
        getline(ss, id, ',');
        getline(ss, title, ',');
        getline(ss, bDate, ',');
        getline(ss, dDate, ',');
        getline(ss, retDate, ',');

        if (user == username) {
            found = true;
            cout << id << " | " << title << " | " << bDate << " | " << dDate << " | " << retDate << "\n";
        }
    }

    if (!found) {
        cout << "No reading history found.\n";
    }
}

void Book::viewAllOldRecords() {
    ifstream file("old_order.csv");
    string line;
    bool found = false;

    cout << "\n--- All Returned Book Records ---\n";
    cout << "Username | BookID | Title | Borrow Date | Due Date | Return Date\n";
    cout << "---------------------------------------------------------\n";

    while (getline(file, line)) {
        stringstream ss(line);
        string user, id, title, bDate, dDate, retDate;
        getline(ss, user, ',');
        getline(ss, id, ',');
        getline(ss, title, ',');
        getline(ss, bDate, ',');
        getline(ss, dDate, ',');
        getline(ss, retDate, ',');

        found = true;
        cout << user << " | " << id << " | " << title << " | " << bDate << " | " << dDate << " | " << retDate << "\n";
    }

    if (!found) {
        cout << "No old records found.\n";
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

    cout << "Enter book amount (price per day): ";
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

void Book::viewBorrowedBooks(const string &username) {
    ifstream borrowFile("borrow_records.csv");
    if (!borrowFile) {
        cout << "No borrow records found.\n";
        return;
    }

    ifstream booksFile("books.csv");
    string line;

    bool found = false;

    cout << "Borrowed Books for User: " << username << "\n";
    cout << "----------------------------------------------------------\n";
    cout << "Book ID | Title                      | Borrow Date | Due Date\n";

    while (getline(borrowFile, line)) {
        stringstream ss(line);
        string uname, bookID, title, borrowDate, dueDate;

        getline(ss, uname, ',');
        getline(ss, bookID, ',');
        getline(ss, title, ',');
        getline(ss, borrowDate, ',');
        getline(ss, dueDate, ',');

        if (uname == username) {
            cout << left << setw(8) << bookID << " | "
                 << setw(25) << title << " | "
                 << setw(12) << borrowDate << " | "
                 << dueDate << "\n";
            found = true;
        }
    }

    borrowFile.close();

    if (!found) {
        cout << "No borrowed books found for this user.\n";
    }
}

void Book::deleteBook() {
    int deleteID;
    cout << "Enter the Book ID to delete: ";
    cin >> deleteID;

    ifstream inFile("books.csv");
    ofstream tempFile("temp.csv");

    string line;
    bool found = false;

    while (getline(inFile, line)) {
        stringstream ss(line);
        string idStr;
        getline(ss, idStr, ',');
        int id = stoi(idStr);

        if (id != deleteID) {
            tempFile << line << "\n";  // Keep the book
        } else {
            found = true;  // Mark as found
        }
    }

    inFile.close();
    tempFile.close();

    remove("books.csv");
    rename("temp.csv", "books.csv");

    if (found) {
        cout << "\n Book with ID " << deleteID << " deleted successfully.\n";
    } else {
        cout << "\n Book with ID " << deleteID << " not found.\n";
    }
}

void Book::updateBookDetails() {
    int bookID;
    double newPrice;
    int newQuantity;

    cout << "Enter Book ID to update: ";
    cin >> bookID;

    ifstream inFile("books.csv");
    ofstream tempFile("temp.csv");

    string line;
    bool found = false;

    while (getline(inFile, line)) {
        stringstream ss(line);
        string idStr, title, author, quantityStr, priceStr;

        getline(ss, idStr, ',');
        getline(ss, title, ',');
        getline(ss, author, ',');
        getline(ss, quantityStr, ',');
        getline(ss, priceStr, ',');

        int id = stoi(idStr);

        if (id == bookID) {
            found = true;
            cout << "Current price is : " << priceStr<< " enter new price: ";
            cin >> newPrice;
            cout << "Current quantity is : " << quantityStr << " enter new quantity: ";
            cin >> newQuantity;
            tempFile << id << "," << title << "," << author << "," 
                     << newQuantity << "," << fixed << setprecision(2) << newPrice << "\n";
        } else {
            tempFile << line << "\n";
        }
    }

    inFile.close();
    tempFile.close();

    remove("books.csv");
    rename("temp.csv", "books.csv");

    if (found) {
        cout << "\n Book details updated successfully.\n";
    } else {
        cout << "\n Book with ID " << bookID << " not found.\n";
    }
}

void Book::viewOverdueBooks() {
    ifstream borrowFile("borrow_records.csv");
    if (!borrowFile) {
        cout << "No borrow records found.\n";
        return;
    }

    string line;

    string todayStr = getCurrentDate();
    time_t today = parseDateToTime(todayStr);
    bool found = false;

    cout << "\nOverdue Books:\n";
    cout << "--------------------------------------------------------------------\n";
    cout << "Username | Book ID | Title                    | Due Date\n";

    while (getline(borrowFile, line)) {
        stringstream ss(line);
        string uname, bookID, title, borrowDate, dueDate;

        getline(ss, uname, ',');
        getline(ss, bookID, ',');
        getline(ss, borrowDate, ',');
        getline(ss, dueDate, ',');

        time_t due = parseDateToTime(dueDate);

        if (difftime(today, due) > 0) { 
            cout << left << setw(9) << uname << " | "
                 << setw(7) << bookID << " | "
                 << setw(25) << title << " | "
                 << dueDate << "\n";
            found = true;
        }
    }

    borrowFile.close();

    if (!found) {
        cout << "No overdue books.\n";
    }
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
        cout << "\n--------------------------------------------------------\n";
        cout << "Library Management System\n";
        cout << "1. Signup\n2. Login\n";
        if(loggedIn)
        {
        cout << "3. Search book\n4. Borrow book\n5. Return book\n";
        if (loggedIn && user.isManager) {
            cout << "6. history of customer\n";
            cout << "7. Add book\n";
            cout << "8. View borrowed books of customer \n";
            cout << "9. Delete book from the system \n";
            cout << "10. Change quantity and price of the book \n";
            cout << "11. View overdue books\n";
            cout << "12. View all records of returned books\n";
        }
        else{
            cout << "6. your history : \n";
            cout << "8. View borrowed books \n";
        }
        cout << "13. Exit\n";
        cout << "--------------------------------------------------------\n";
        cout << "Enter your choice: ";
        }
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
            if (loggedIn) {
                if (user.isManager){
                    string username;
                    cout << "enter username of customer: ";
                    cin >> username;
                    book.viewReadingHistory(username);
                }
                else{
                book.viewReadingHistory(user.username);
                }
            } 
            else if (!loggedIn) {
                cout << "Please login first.\n";
            } 
            break;
        case 7:
            if (loggedIn && user.isManager) {
                book.addBook();
            } else if (!loggedIn) {
                cout << "Please login first.\n";
            } else {
                cout << "You do not have permission to add books.\n";
            }
            break;
        case 8:
            if (loggedIn) {
                if (user.isManager){
                    string username;
                    cout << "enter username of customer: ";
                    cin >> username;
                    book.viewBorrowedBooks(username);
                }
                else{
                    book.viewBorrowedBooks(user.username);
                }
            } 
            else {
                cout << "Please login first.\n";
            }
            break;
        case 9:
            if (loggedIn && user.isManager) {
                book.deleteBook();
            } else if (!loggedIn) {
                cout << "Please login first.\n";
            } else {
                cout << "You do not have permission to add books.\n";
            }
            break;
        case 10:
            if (loggedIn && user.isManager) {
                book.updateBookDetails();
            } else if (!loggedIn) {
                cout << "Please login first.\n";
            } else {
                cout << "You do not have permission to add books.\n";
            }
            break;
        case 11:
            if (loggedIn && user.isManager) {
                book.viewOverdueBooks();
            } else if (!loggedIn) {
                cout << "Please login first.\n";
            } else {
                cout << "You do not have permission to add books.\n";
            }
            break;
        case 12:
            if (loggedIn && user.isManager) {
                book.viewAllOldRecords();
            } else if (!loggedIn) {
                cout << "Please login first.\n";
            } else {
                cout << "You do not have permission to add books.\n";
            }
            break;
        case 13:
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