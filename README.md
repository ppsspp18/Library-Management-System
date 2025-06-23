# Library Management System

A **console-based Library Management System** built using **C++** and **Object-Oriented Programming (OOP)** concepts. This system allows users to manage books, handle borrowing/returning, and view borrowing history. It also provides additional features for managers like book inventory control and overdue tracking.

---

## Features

### General Users:
- **Signup / Login** – Register and authenticate users.
- **Search Books** – Look up books available in the library.
- **Borrow Book** – Borrow books if available.
- **Return Book** – Return borrowed books.
- **View Borrowed Books** – Check the list of currently borrowed books.
- **View Your History** – See your reading and borrowing history.

### Manager (Admin) Capabilities:
- All user functionalities, plus:
- **Add Book** – Add new books to the library.
- **Delete Book** – Remove a book from the system.
- **Change Book Price/Quantity** – Modify price and available copies.
- **View Borrowed Books (All Users)** – Monitor which users borrowed which books.
- **View History of Any Customer** – Access the past borrowing records of any user.
- **View Overdue Books** – List of books not returned within the due date.
- **View All Returned Records** – All records of books that have been returned.

---

## User Interface (Command Line Interface)

```cpp
while (true) {
    cout << "\n--------------------------------------------------------\n";
    cout << "Library Management System\n";
    cout << "1. Signup\n2. Login\n";
    if(loggedIn) {
        cout << "3. Search book\n4. Borrow book\n5. Return book\n";
        if (loggedIn && user.isManager) { 
            cout << "6. History of customer\n";
            cout << "7. Add book\n";
            cout << "8. View borrowed books of customer\n";
            cout << "9. Delete book from the system\n";
            cout << "10. Change quantity and price of the book\n";
            cout << "11. View overdue books\n";
            cout << "12. View all records of returned books\n";
        } else {
            cout << "6. Your history\n";
            cout << "8. View borrowed books\n";
        }
        cout << "13. Exit\n";
        cout << "--------------------------------------------------------\n";
        cout << "Enter your choice: ";
    }
}
```

# Library Management System Integrated with Claude Desktop using MCP

This is a simple, file-based **Library Management System** built using the **FastMCP** framework and integrated with **Claude Desktop** using `uv`. It supports basic library operations like adding books, borrowing/returning them, and managing book records.

## Features

This project includes the following MCP tools:

- `add_book`
- `borrow_book`
- `return_book`
- `get_book_status`
- `change_book_quantity`
- `delete_book`
- `change_book_price`
- `list_all_book`
- `search_book_by_title`
- `list_all_order`
- `search_order_by_customer`
- `list_all_old_order`

all changes are saved locally in .json file 

## Technologies Used

- Python
- [FastMCP](https://github.com/crewai-dev/mcp) from `mcp.server.fastmcp`
- `uuid` for unique book/order IDs
- `datetime` for handling dates
- JSON files for persistent storage
- `uv` for MCP integration with Claude Desktop

## Installation

### 1. Install `uv` (if not installed)
```bash
pip install uv
```
### 2. Initialize mcp project
```
uv init Library_management_system
cd Library_management_system
```
### 3. Edit main.py file

### 4. Run the project
```
uv run mcp install main.py
```
close claude desktop and start again it will work 

