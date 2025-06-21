from mcp.server.fastmcp import FastMCP
from typing import List
from datetime import date, datetime, timedelta
from uuid import uuid4
import json
import os

# File path for storing data
DATA_FILE = os.path.join(os.path.dirname(__file__), "orders_data.json")
DATA_FILE2 = os.path.join(os.path.dirname(__file__), "books_data.json")
DATA_FILE3 = os.path.join(os.path.dirname(__file__), "old_orders_data.json")


# In-memory database
orders_db = {}
books_db = {}
old_orders_db = {}

todays_date = date.today() # in format 2025-06-19

# Book schema
class Order:
    def __init__(self, title: str, customer_name: str, number_of_days: int):
        self.id = str(uuid4())
        self.title = title
        self.start_date = todays_date
        self.number_of_days = number_of_days
        self.customer_name = customer_name
        self.end_date =todays_date + timedelta(days=number_of_days)

    def to_dict(self):
        return {
            "id": self.id,
            "title": self.title,
            "start_date":self.start_date.isoformat(),
            "end_date":self.end_date.isoformat(),
            "number_of_days":self.number_of_days,
            "customer_name": self.customer_name
        }
    
class Book:
    def __init__(self, title: str, author: str, price: int, quantity: int):
        self.id = str(uuid4())
        self.title = title
        self.author = author
        self.price = price
        self.quantity = quantity

    def to_dict(self):
        return {
            "id": self.id,
            "title": self.title,
            "author": self.author,
            "price": self.price,
            "quantity": self.quantity
        }
    
class OldOrder:
    def __init__(self, title: str, customer_name: str, start_date: str, end_date: str, total_amount_paid: int):
        self.id = str(uuid4())
        self.title = title
        self.customer_name = customer_name
        self.start_date = start_date
        self.end_date = end_date
        self.total_amount_paid = total_amount_paid

    def to_dict(self):
        return {
            "id": self.id,
            "title": self.title,
            "customer_name": self.customer_name,
            "start_date":self.start_date,
            "end_date":self.end_date,
            "total_amount_paid":self.total_amount_paid
        }

# Load order data from file
def load_orders_data():
    if os.path.exists(DATA_FILE):
        with open(DATA_FILE, "r") as f:
            data = json.load(f)
            for item in data:
                order = Order(title=item["title"], customer_name=item["customer_name"],
                               number_of_days=item["number_of_days"])
                order.start_date = datetime.strptime(item["start_date"], "%Y-%m-%d").date()
                order.end_date = datetime.strptime(item["end_date"], "%Y-%m-%d").date()
                order.id = item["id"]
                orders_db[order.id] = order

# Load books data from file
def load_books_data():
    if os.path.exists(DATA_FILE2):
        with open(DATA_FILE2, "r") as f:
            data = json.load(f)
            for item in data:
                book = Book(title=item["title"], author=item["author"], price=item["price"], 
                            quantity=item["quantity"])
                book.id = item["id"]
                books_db[book.id] = book

# Load old orders data from file
def load_old_orders_data():
    if os.path.exists(DATA_FILE3):
        with open(DATA_FILE3, "r") as f:
            data = json.load(f)
            for item in data:
                old_order = OldOrder(title=item["title"], customer_name=item["customer_name"], 
                                     start_date=item["start_date"], end_date=item["end_date"], 
                                     total_amount_paid=item["total_amount_paid"])
                old_order.id = item["id"]
                old_orders_db[old_order.id] = old_order

# Save orders data to file
def save_orders_data():
    with open(DATA_FILE, "w") as f:
        json.dump([order.to_dict() for order in orders_db.values()], f, indent=4)

# Save books in the library
def save_books_data():
    with open(DATA_FILE2, "w") as f:
        json.dump([book.to_dict() for book in books_db.values()], f, indent=4)

# Save old orders data to file
def save_old_orders_data():
    with open(DATA_FILE3, "w") as f:
        json.dump([order.to_dict() for order in old_orders_db.values()], f, indent=4)

# Create MCP server
mcp = FastMCP("LibraryManagement")

# Load data on start
load_orders_data()
load_books_data()
load_old_orders_data()

# Tool to add book
@mcp.tool()
def add_book(title: str, author: str, price: int, quantity: int) -> str:
    """To add book using book title and author in string, price and quantity in integer."""
    book = Book(title, author, price, quantity)
    books_db[book.id] = book
    save_books_data()
    return f"Book added with ID: {book.id}"

# Tool to borrow book
@mcp.tool()
def borrow_book(book_id: str, user: str, number_of_days: int) -> str:
    """
    Allows a user to borrow a book using the book's ID and number of days in integer.
    Validates input and updates the books and orders databases.
    """

    # Check if book exists
    book = books_db.get(book_id)
    if not book:
        return f"❌ Book with ID '{book_id}' not found."

    # Check if book is available
    if book.quantity <= 0:
        return f"❌ Book '{book.title}' is currently not available."

    # Proceed to borrow
    book.quantity -= 1
    order = Order(title=book.title, customer_name=user, number_of_days=number_of_days)
    orders_db[order.id] = order

    # Save changes
    save_books_data()
    save_orders_data()

    return f"✅ Book '{book.title}' successfully borrowed by {user} until {number_of_days}."


# Tool to return book
@mcp.tool()
def return_book(order_id: str) -> str:
    """Return a borrowed book using the order ID."""
    order = orders_db.get(order_id)
    if not order:
        return "❌ Order ID not found."

    # Find the book by title
    book_title = order.title
    book = next((b for b in books_db.values() if b.title == book_title), None)
    if not book:
        return "❌ Book not found in library."

    # Increase book quantity
    book.quantity += 1

    # Parse dates
    start = order.start_date
    end = order.end_date
    today = datetime.today().date()

    # Calculate charges
    rental_days = (end - start).days
    price = int(rental_days) * int(book.price)

    additional_price = 0
    if today > end:
        additional_days = (today - end).days
        additional_price = int(additional_days) * int((book.price) + 5)  # fine

    total_amount_paid = price + additional_price

    # Create old order record
    old_order = OldOrder(
        title=order.title,
        customer_name=order.customer_name,
        start_date=start.isoformat(),
        end_date=today.isoformat(),
        total_amount_paid=total_amount_paid
    )
    old_orders_db[old_order.id] = old_order

    # Remove from active orders
    del orders_db[order_id]

    # Save data
    save_orders_data()
    save_books_data()
    save_old_orders_data()

    if additional_price > 0:
        return f"✅ Book '{book.title}' returned with total ₹{total_amount_paid} in which fine is ₹{additional_price}."
    return f"✅ Book '{book.title}' returned successfully with amount ₹{total_amount_paid}."


# Tool to get book status
@mcp.tool()
def get_book_status(book_id: str) -> dict:
    """this tool return particular book the book title, author, quantity and price"""
    book = books_db.get(book_id)
    if book:
        return book.to_dict()
    return {"error": "Book ID not found"}

# Tool to change book quantity
@mcp.tool()
def change_book_quantity(book_id: str, change: int) -> dict:
    """this tool change particular book quantity
    change will be negative to lower the quantity"""
    book = books_db.get(book_id)
    if book:
        if (change < 0) and ((-change)>book.quantity) :
            return {"error": "Not feasible"}
        book.quantity += change
        save_books_data()
        return book.to_dict()
    return {"error": "Book ID not found"}

# Tool to delete book
@mcp.tool()
def delete_book(book_id: str) -> dict:
    """this tool useed to delete particular book"""
    book = books_db.get(book_id)
    if book:
        book_title = book.title
        order = next((b for b in orders_db.values() if b.title == book_title), None)
        if not order:
            del books_db[book.id]
            save_books_data()
            return book.to_dict()
        return {"error": "book is borrowed"}
    return {"error": "Book ID not found"}

# Tool to change book price
@mcp.tool()
def change_book_price(book_id: str, change: int) -> dict:
    """this tool change particular book price 
    change will be negative to lower price"""
    book = books_db.get(book_id)
    if book:
        if (change < 0) and ((-change)> book.price) :
            return {"error": "Not feasible"}
        book.price += change
        save_books_data()
        return book.to_dict()
    return {"error": "Book ID not found"}

# Tool to list all books
@mcp.tool()
def list_all_books() -> List[dict]:
    """this tool provide information about all the books"""
    return [book.to_dict() for book in books_db.values()]

# Tool to search book 
@mcp.tool()
def search_book_by_title(query: str) -> List[dict]:
    """
    this tool Search for books by title (case-insensitive, partial match).
    """
    result = []
    for book in books_db.values():
        if query.lower() in book.title.lower():
            result.append(book.to_dict())
    return result

# Tool to list all orders
@mcp.tool()
def list_all_orders() -> List[dict]:
    """ this tool return all orders"""
    return [order.to_dict() for order in orders_db.values()]

# Tool to search customer
@mcp.tool()
def search_order_by_customer(query: str) -> List[dict]:
    """
    this tool Search for books by title (case-insensitive, partial match).
    """
    result = []
    for order in orders_db.values():
        if query.lower() in order.customer_name.lower():
            result.append(order.to_dict())
    return result

# Tool to list all old orders
@mcp.tool()
def list_all_old_orders() -> List[dict]:
    """ this tool return all old orders"""
    return [order.to_dict() for order in old_orders_db.values()]

# Tool to list all borrowed books by a user
@mcp.resource("borrowed_books://{user}")
def borrowed_books_by_user(user: str) -> List[dict]:
    return [book.to_dict() for book in orders_db.values() if book.customer_name == user]

if __name__ == "__main__":
    mcp.run()



"""
uv init library_management_system_mcp

to run
uv run mcp install main.py

"""

