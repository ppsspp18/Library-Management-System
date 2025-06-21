# Library-Management-System
The Library Management System is a C++ project designed to efficiently manage library operations, including user authentication, book management, and borrowing/returning processes. It uses file-based storage to maintain data integrity and ensures seamless interaction for both customers and managers with role-based access control.

Features:

User Authentication: Secure signup and login system with role-based access for customers and managers.
Book Management: Add, search, and update book records with details like title, author, availability, and price.
Borrow and Return:
Tracks borrowed books with due dates and calculates a fine for late returns.
Displays comprehensive book details during borrowing and return processes.
Data Persistence: Uses CSV files for managing users, books, and borrowing history.
Modular Design: Built with Object-Oriented Programming principles, allowing easy scalability and maintenance.
Technologies Used:

Programming Language: C++
Concepts: File Handling, Object-Oriented Programming (OOP)
This project is ideal for understanding the implementation of core OOP concepts and file-based data management in C++.

# 📚 Library Management System with Claude Desktop using MCP

This is a simple, file-based **Library Management System** built using the **FastMCP** framework and integrated with **Claude Desktop** using `uv`. It supports basic library operations like adding books, borrowing/returning them, and managing book records.

## 🔧 Features

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

## 🧱 Technologies Used

- Python
- [FastMCP](https://github.com/crewai-dev/mcp) from `mcp.server.fastmcp`
- `uuid` for unique book/order IDs
- `datetime` for handling dates
- JSON files for persistent storage
- `uv` for MCP integration with Claude Desktop

## 📦 Installation

### 1. Install `uv` (if not installed)
```bash
pip install uv
```
### 2. Initialize mcp project
```
uv init Library_management_system
cd Library_management_system
```
### 3. Run the project
```
uv run mcp install main.py
```
close claude desktop and start again it will work 

