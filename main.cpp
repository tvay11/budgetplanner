#include <iostream>
#include <sqlite3.h>
#include <limits>

using namespace std;

double totalAmount = 0.0;

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    for(int i = 0; i < argc; i++) {
        cout << azColName[i] << ": " << argv[i] << "\n";
    }
    cout << "\n";
    return 0;
}

void createTable(sqlite3 *db) {
    char *zErrMsg = nullptr;
    const char *sqlCreateTable = "CREATE TABLE IF NOT EXISTS TRANSACTIONS (" \
                                 "ID INTEGER PRIMARY KEY AUTOINCREMENT," \
                                 "TYPE CHAR(1)," \
                                 "TITLE TEXT NOT NULL," \
                                 "AMOUNT REAL);";
    int rc = sqlite3_exec(db, sqlCreateTable, callback, 0, &zErrMsg);
    if(rc != SQLITE_OK) {
        cout << "SQL error: " << zErrMsg << "\n";
        sqlite3_free(zErrMsg);
    } else {
        cout << "Table created successfully\n";
    }
}

void dropTable(sqlite3 *db) {
    char *zErrMsg = nullptr;
    const char *sqlDropTable = "DROP TABLE IF EXISTS TRANSACTIONS;";
    int rc = sqlite3_exec(db, sqlDropTable, callback, 0, &zErrMsg);
    if(rc != SQLITE_OK) {
        cout << "SQL error: " << zErrMsg << "\n";
        sqlite3_free(zErrMsg);
    } else {
        cout << "Table dropped successfully\n";
    }
}

string center(const string &str, int width) {
    string truncated_str = str.substr(0, width);
    int len = truncated_str.length();

    int padding = (width - len) / 2;
    return string(padding, ' ') + truncated_str + string(padding, ' ') + string((width - len) % 2, ' ');
}


static int callbackTable(void *NotUsed, int argc, char **argv, char **azColName) {
    cout << "|" << center(argv[0] ? argv[0] : "NULL", 5);
    cout << "|" << center(argv[1] ? argv[1] : "NULL", 6);
    cout << "|" << center(argv[2] ? argv[2] : "NULL", 15);

    string amount = argv[3] ? argv[3] : "NULL";
    if (amount != "NULL") {
        double numAmount = stod(amount);
        totalAmount += (argv[1] && string(argv[1]) == "+") ? numAmount : -numAmount;
    }

    if (amount.length() > 10) {
        amount = amount.substr(0, 9) + "-";
    }
    cout << "|" << center(amount, 10) << "|" << endl;

    return 0;
}

void viewTransactions(sqlite3 *db) {
    char *zErrMsg = nullptr;
    totalAmount = 0.0;  // Reset the total amount

    const char *sqlSelect = "SELECT * FROM TRANSACTIONS ORDER BY TYPE;";
    cout << endl;
    cout << "|" << center("ID", 5)
         << "|" << center(" Type", 6)
         << "|" << center("Title", 15)
         << "|" << center("Amount", 10) << "|" << endl;
    cout <<"_________________________________________" <<endl;

    int rc = sqlite3_exec(db, sqlSelect, callbackTable, 0, &zErrMsg);

    if (rc != SQLITE_OK) {
        cout << "SQL error: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
    }

    cout << "Total Amount: " << totalAmount << endl;
}

void addTransaction(sqlite3 *db) {
    char *zErrMsg = nullptr;
    string title;
    float amount;
    string type;

    cout << "Enter title: ";
    cin >> ws;
    getline(cin, title);

    //validate if num
    while (true) {
        cout << "Enter amount: ";
        if (cin >> amount) {
            break;
        } else {
            cout << "Invalid input. Please enter a number for the amount.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    while (true) {
        cout << "Enter transaction type (+/-): ";
        cin >> type;
        if (type == "+" || type == "-") {
            break;
        } else {
            cout << "Invalid input. Please enter either '+' or '-'.\n";
        }
    }


    string sqlInsert = "INSERT INTO TRANSACTIONS (TYPE, TITLE, AMOUNT) VALUES ('"
                       + type + "','" + title.substr(0, 15) + "'," + to_string(amount) + ");";

    int rc = sqlite3_exec(db, sqlInsert.c_str(), callback, 0, &zErrMsg);
    if(rc != SQLITE_OK) {
        cout << "SQL error: " << zErrMsg << "\n";
        sqlite3_free(zErrMsg);
    } else {
        cout << "Record inserted successfully\n";
    }
}

void deleteTransaction(sqlite3 *db) {
    char *zErrMsg = nullptr;
    int id;
    viewTransactions(db);
    cout << "Enter the ID of the transaction you want to delete: ";
    cin >> id;

    string sqlDelete = "DELETE FROM TRANSACTIONS WHERE ID = " + to_string(id) + ";";
    int rc = sqlite3_exec(db, sqlDelete.c_str(), callback, 0, &zErrMsg);

    if(rc != SQLITE_OK) {
        cout << "SQL error: " << zErrMsg << "\n";
        sqlite3_free(zErrMsg);
    } else {
        cout << "Transaction deleted successfully\n";
    }
}

void displayWelcomeMessage() {
    cout << "=========================================" << endl;
    cout << "||    Welcome to Budget Planner!       ||" << endl;
    cout << "=========================================" << endl;
}

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
        cout << "1. Create Table\n";
        cout << "2. Drop Table\n";
        cout << "3. Add Transaction\n";
        cout << "4. View Transactions\n";
        cout << "5. Delete Transaction\n";
        cout << "6. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        if (choice == 1) {
            createTable(db);
        } else if (choice == 2) {
            dropTable(db);
        } else if (choice == 3) {
            addTransaction(db);
        } else if (choice == 4) {
            viewTransactions(db);
        } else if (choice == 5) {
            deleteTransaction(db);
        } else if (choice == 6) {
            break;
        } else {
            cout << "Invalid choice. Try again.\n";
        }
    }
    sqlite3_close(db);
    return 0;
}
