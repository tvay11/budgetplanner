# BudgetPlanner

A command-line personal finance management tool written in C. BudgetPlanner helps users track income, expenses, and savings goals through a lightweight, terminal-based interface.

## Project Overview

BudgetPlanner addresses the need for a simple, fast, and portable budgeting application that does not rely on web browsers or graphical interfaces. Many existing budgeting tools are either overly complex, require internet connectivity, or come with unnecessary overhead. This project provides a minimal yet functional solution that runs entirely in the terminal, making it ideal for developers, system administrators, or anyone who prefers working from the command line.

The application stores financial data locally in a structured file format, allowing users to maintain full control over their information. By focusing on core budgeting operations without external dependencies, BudgetPlanner demonstrates efficient memory management, file I/O, and modular code design in C.

## Core Features

- Add, view, and delete income and expense entries with categories and dates
- Calculate and display current balance, total income, and total expenses
- Set monthly savings goals and track progress toward them
- Persistent data storage using a custom file format for reliability and portability
- Interactive command-line menu system with input validation
- Export financial data to a plain text report for external analysis

## Technology Stack

- **C**: The entire application is written in C, chosen for its performance, low-level memory control, and portability across Unix-like systems. C allows the program to remain lightweight with no external runtime dependencies, making it suitable for embedded or minimal environments.

## Architecture and Design Decisions

The codebase is organized into several modules to promote separation of concerns and maintainability:

- **main.c**: Entry point that initializes the application and runs the main menu loop.
- **transaction.c / transaction.h**: Handles creation, storage, and retrieval of income and expense records.
- **budget.c / budget.h**: Manages savings goals and budget calculations.
- **fileio.c / fileio.h**: Implements all file read/write operations using a custom binary format for efficiency.
- **ui.c / ui.h**: Provides terminal-based user interface functions, including menu rendering and input handling.

Design decisions include:

- Use of a linked list data structure for dynamic management of transactions, allowing efficient insertion and deletion without pre-allocated arrays.
- Binary file storage instead of plain text to reduce file size and improve read/write speed, while still allowing export to human-readable format.
- Modular function design with clear interfaces, enabling easy testing and future extension of individual components.

## Installation and Setup

### Prerequisites

- A C compiler (GCC or Clang recommended)
- Make (optional, for build automation)
- Unix-like operating system (Linux, macOS, or Windows Subsystem for Linux)

### Build and Run

1. Clone the repository:

   ```
   git clone https://github.com/tvay11/budgetplanner.git
   cd budgetplanner
   ```

2. Compile the source code:

   ```
   gcc -o budgetplanner main.c transaction.c budget.c fileio.c ui.c -Wall -Wextra
   ```

   Or, if a Makefile is provided:

   ```
   make
   ```

3. Run the application:

   ```
   ./budgetplanner
   ```

4. Follow the on-screen menu to add transactions, view reports, and manage savings goals.

## Future Scope and Key Learnings

This project reinforced fundamental concepts in C programming, including dynamic memory allocation, file I/O, and modular design. Future enhancements could include:

- Support for multiple user profiles with separate data files
- Integration with CSV or JSON export for compatibility with spreadsheet software
- A simple graphical interface using ncurses for improved user experience
- Recurring transaction automation for regular bills or income

BudgetPlanner serves as a solid foundation for exploring more advanced topics such as data encryption, network-based synchronization, or database integration in C.