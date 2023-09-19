# Budget Planner

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Usage](#usage)
- [Known Issues](#known-issues)
- [Author](#author)

## Overview

This is a simple budget planner application built using C++ and SQLite. The application allows you to create a table, add transactions, view transactions, delete transactions, and drop the table.

## Features

- **Create Table**: Initialize a new transactions table.
- **Drop Table**: Remove an existing transactions table.
- **Add Transaction**: Add new transactions to the table.
- **View Transactions**: Display all transactions in the table.
- **Delete Transaction**: Remove specific transactions from the table.

## Prerequisites

- C++ Compiler (e.g., g++)
- SQLite3 library

## Installation

1. **Clone the Repository**

    ```
    git clone https://github.com/tvay11/budgetplanner.git
    ```

2. **Navigate to the Project Directory**

    ```
    cd budgetplanner
    ```

3. **Compile the Project**

    ```
    g++ -o budgetplanner main.cpp -lsqlite3
    ```

## Usage

1. **Run the Application**

    ```
    ./my_program
    ```

2. **Follow On-Screen Instructions**

    The application will guide you through the options for managing your transactions.

## Known Issues

- dockerfile will run in a infinite loop.


## Author

- [tvay11](https://github.com/tvay11)
