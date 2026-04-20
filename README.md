# Hotel-Booking-Management-System
A console-based Hotel Room Booking System in C++ using OOP. It manages rooms, customers, and bookings with automated time-based billing, loyalty-based discounts, and file-based data persistence. Designed for efficient operations, accurate tracking, and scalability for real-world hotel scenarios.
🔹 Introduction

The Hotel Room Booking and Management System is a console-based software application developed in C++ using Object-Oriented Programming (OOP) principles. The system is designed to simulate real-world hotel operations by managing rooms, customers, and bookings in an efficient and structured manner.

Unlike basic booking systems, this implementation incorporates persistent data storage, automated time-based billing, and a customer loyalty management system, making it closer to real-world hotel management solutions. The system ensures data consistency across sessions and enhances user experience through intelligent automation and personalized interactions.

🔹 Objectives
Room Management: Maintain details of room types, pricing, availability, and occupancy status.
Customer Management: Store and track customer information along with their stay history.
Booking Operations: Efficiently handle check-in and check-out processes with proper validation.
Automated Billing: Calculate total charges dynamically based on actual stay duration using system time.
Customer Loyalty System: Implement a tier-based model (Normal, Silver, Gold) to provide discounts for repeat customers.
Data Persistence: Store and retrieve data using file handling to ensure continuity across multiple runs of the system.
Availability Management: Prevent double booking and ensure accurate room allocation.
User-Friendly Interface: Provide a structured, menu-driven interface with formatted output for ease of use.
Scalability: Design the system in a modular way to allow future enhancements such as database integration, analytics, or online booking systems.
🔹 Methodology
1. Class Design

The system is implemented using three core classes:

Room:
Stores room number, type (Standard/Deluxe/Suite), price per night, and occupancy status.
Customer:
Stores customer ID, name, contact details, number of stays, and total expenditure.
Also supports updating customer history for loyalty tracking.
Booking:
Represents the relationship between a customer and a room.
Stores booking ID, customer details, assigned room, check-in timestamp, and booking status.
2. Data Management
Uses vectors to manage collections of rooms, customers, and bookings.
Implements file handling (fstream) to persist data:
Rooms data
Customer data
Booking records
Data is loaded at program startup and saved after every transaction to maintain consistency.
3. Core Functional Operations
Room Availability Check:
Displays all available rooms based on type and occupancy status.
Check-In Process:
Assigns rooms dynamically based on availability and creates new bookings.
Recognizes repeat customers and displays their loyalty status.
Check-Out Process:
Calculates total bill automatically using system time and applies loyalty-based discounts.
Customer Tracking:
Maintains historical data such as total stays and spending for each customer.
Dashboard View:
Displays real-time statistics such as total rooms, occupied rooms, available rooms, and active bookings.
4. User Interface
Menu-driven console interface
Input validation for reliability
Formatted output using iomanip for better readability
🔹 Key Features
Automated time-based billing system
Persistent storage using file handling
Customer loyalty and retention model
Real-time dashboard for system overview
Repeat customer recognition with personalized interaction
Modular and extensible design
🔹 Expected Outcomes
Efficient and structured hotel room management
Accurate and automated billing with minimal human error
Improved customer experience through loyalty-based incentives
Prevention of overbooking and better resource utilization
Reliable data storage and retrieval across sessions
A scalable foundation for future enhancements such as analytics, database integration, and web-based interfaces
