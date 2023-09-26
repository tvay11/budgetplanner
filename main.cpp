#include <iostream>
#include <sqlite3.h>
#include "budgetSQL.h"

using namespace std;

void displayWelcomeMessage();

int main() {
    sqlite3 *db;
    int rc;

    rc = sqlite3_open("expense_tracker.db", &db);
    if(rc) {
        cout << "Cannot open database: " << sqlite3_errmsg(db) << "\n";
        return 0;
    } else {
        cout << "Opened database successfully\n";
    }

    displayWelcomeMessage();

    int choice;
    while (true) {
        cout << "0. Exit" << endl;
        cout << "1. Create Table" << endl;
        cout << "2. Drop Table" << endl;
        cout << "3. Log New Expense or Income" << endl;
        cout << "4. Show Expense Summary" << endl;
        cout << "5. Remove Specific Entry" << endl;
        cout << "6. Edit Entry by ID" << endl;
        cout << "7. Filter by Category" << endl;
        cout << "8. Search Transactions" << endl;
        cout << "9. Export to CSV" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch(choice) {
            case 0:
                cout << "Exiting program..." << endl;
                sqlite3_close(db);
                return 0;
            case 1:
                createTable(db);
                break;
            case 2:
                dropTable(db);
                break;
            case 3:
                addTransaction(db);
                break;
            case 4:
                viewTransactions(db);
                break;
            case 5:
                deleteTransactionUI(db);
                break;
            case 6:
                updateTransactions(db);
                break;
            case 7:
                filterByCategory(db);
                break;
            case 8:
                searchTransactions(db);
                break;
            case 9:
                exportToCSV(db);
                break;
            default:
                cout << "Invalid choice. Please enter again." << endl;
        }
    }


}

void displayWelcomeMessage() {
    cout << "=========================================" << endl;
    cout << "||    Welcome to Budget Planner!       ||" << endl;
    cout << "=========================================" << endl;
}

