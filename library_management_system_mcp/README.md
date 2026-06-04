# Library Management System using FastMCP and Claude Desktop

## Overview

The Library Management System is an AI-enabled application built using **Python**, **FastMCP**, and **Claude Desktop**. The project demonstrates how a Large Language Model (LLM) can interact directly with a local database through the **Model Context Protocol (MCP)**.

Instead of using a traditional graphical user interface, users can manage books, borrow books, return books, search inventory, and track transactions through natural language conversations with Claude Desktop.

This project serves as a practical example of AI tool calling, MCP architecture, backend development, and local database integration.

---

## Problem Statement

Traditional library systems require dedicated web or desktop interfaces for interaction.

The goal of this project was to:

* Enable natural language interaction with a library database.
* Learn and implement the Model Context Protocol (MCP).
* Integrate Claude Desktop with local data.
* Build an AI-powered backend system.
* Understand tool calling and resource sharing in MCP.

---

## Features

### Book Management

* Add new books
* Delete books
* Update book quantity
* Update book price
* View book details
* Search books by title
* List all books

### Borrowing Management

* Borrow books
* Return books
* Track active borrowings
* Calculate rental costs
* Apply late return fines

### Transaction History

* Store completed borrowing records
* Maintain payment history
* View old transactions

### AI Integration

* Natural language interaction through Claude Desktop
* Automatic tool invocation
* Resource sharing using MCP

---

## Technology Stack

### Backend

* Python
* FastMCP

### AI Layer

* Claude Desktop
* Model Context Protocol (MCP)

### Storage

* JSON Database

### Libraries Used

```python
fastmcp
datetime
uuid
json
os
typing
```

---

## System Architecture

```text
+------------------+
|      User        |
+------------------+
          |
          v
+------------------+
| Claude Desktop   |
+------------------+
          |
          v
+------------------+
|   MCP Client     |
+------------------+
          |
          v
+------------------+
| FastMCP Server   |
+------------------+
          |
          v
+------------------+
| Local Database   |
+------------------+
```

---

## Project Structure

```text
library_management_system/

│
├── main.py
│
├── books_data.json
│
├── orders_data.json
│
├── old_orders_data.json
│
└── README.md
```

---

## Database Design

### Books Database

Stores information about all books available in the library.

| Field    | Description          |
| -------- | -------------------- |
| id       | Unique Book ID       |
| title    | Book Title           |
| author   | Author Name          |
| price    | Rental Price Per Day |
| quantity | Available Copies     |

---

### Orders Database

Stores currently borrowed books.

| Field          | Description     |
| -------------- | --------------- |
| id             | Order ID        |
| title          | Book Title      |
| customer_name  | Borrower Name   |
| start_date     | Borrow Date     |
| end_date       | Due Date        |
| number_of_days | Borrow Duration |

---

### Old Orders Database

Stores completed transactions.

| Field             | Description    |
| ----------------- | -------------- |
| id                | Transaction ID |
| title             | Book Title     |
| customer_name     | Borrower Name  |
| start_date        | Borrow Date    |
| end_date          | Return Date    |
| total_amount_paid | Amount Paid    |

---

## Core Classes

### Book

Represents a book in the library.

Attributes:

* id
* title
* author
* price
* quantity

---

### Order

Represents an active borrowing transaction.

Attributes:

* id
* title
* customer_name
* start_date
* end_date
* number_of_days

---

### OldOrder

Represents a completed transaction.

Attributes:

* id
* title
* customer_name
* start_date
* end_date
* total_amount_paid

---

## MCP Tools Implemented

### Book Tools

| Tool                   | Description           |
| ---------------------- | --------------------- |
| add_book()             | Add a new book        |
| get_book_status()      | View book information |
| list_all_books()       | List all books        |
| search_book_by_title() | Search books          |
| change_book_quantity() | Update inventory      |
| change_book_price()    | Update price          |
| delete_book()          | Delete book           |

---

### Borrowing Tools

| Tool                       | Description            |
| -------------------------- | ---------------------- |
| borrow_book()              | Borrow a book          |
| return_book()              | Return a book          |
| list_all_orders()          | View active borrowings |
| search_order_by_customer() | Search customer orders |

---

### History Tools

| Tool                  | Description                 |
| --------------------- | --------------------------- |
| list_all_old_orders() | View completed transactions |

---

## MCP Resources

### Borrowed Books Resource

```python
borrowed_books://{user}
```

Returns all books currently borrowed by a specific user.

Example:

```text
borrowed_books://Prakhar
```

---

## Business Logic

### Borrow Book Workflow

1. Validate book exists.
2. Check available quantity.
3. Reduce inventory count.
4. Create order record.
5. Save data.

---

### Return Book Workflow

1. Locate active order.
2. Increase inventory count.
3. Calculate rental amount.
4. Calculate late fine.
5. Create historical record.
6. Remove active order.
7. Save data.

---

## Fine Calculation

### Rental Cost

```text
Rental Cost = Book Price × Borrow Days
```

### Late Fine

```text
Fine = (Book Price + 5) × Extra Days
```

### Total Payment

```text
Total Amount = Rental Cost + Fine
```

---

## Data Persistence

The application uses JSON files for persistent storage.

### Startup

```text
Load JSON Files
       ↓
Populate Memory Database
```

### Update

```text
Modify Data
      ↓
Save Back To JSON
```

This ensures data remains available even after restarting the server.

---

## Error Handling

The system handles:

* Invalid Book ID
* Invalid Order ID
* Out-of-stock books
* Negative inventory updates
* Invalid price updates
* Deleting borrowed books

---

## How to Run

### Create Project

```bash
uv init library_management_system_mcp
```

### Install MCP Server

```bash
uv run mcp install main.py
```

### Start Claude Desktop

After installation, Claude Desktop can discover and use the MCP tools automatically.

---

## Example Natural Language Commands

```text
Add a new book called Clean Code.
```

```text
Show all books available in the library.
```

```text
Borrow Atomic Habits for 7 days.
```

```text
Return my borrowed book.
```

```text
Search for books related to Python.
```

---

## Challenges Faced

* Understanding MCP architecture
* Designing AI-callable tools
* Maintaining consistency across databases
* Implementing persistence
* Managing inventory updates
* Handling business rules and fines

---

## Key Learnings

Through this project I learned:

* Model Context Protocol (MCP)
* FastMCP Framework
* Tool Calling Architecture
* Claude Desktop Integration
* CRUD Operations
* Database Persistence
* Backend Development
* AI Agent Development

---

## Future Improvements

* SQLite Database
* PostgreSQL Support
* Authentication System
* User Roles
* Book Reservation System
* Recommendation Engine
* Analytics Dashboard
* Email Notifications
* Cloud Deployment

---

## Summary

This project demonstrates how Large Language Models can interact with external systems through the Model Context Protocol. Using FastMCP, I exposed library operations as AI-callable tools and connected them with Claude Desktop. The project includes inventory management, borrowing workflows, transaction history, fine calculation, data persistence, and AI-powered natural language interaction, providing hands-on experience in Agentic AI, MCP architecture, and backend system design.
