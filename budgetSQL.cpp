//
// Created by Thang Vay on 9/21/23.
//

#include "budgetSQL.h"
#include <fstream>

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
    char *ErrorMsg = nullptr;
    const char *sqlCreateTable = "CREATE TABLE IF NOT EXISTS BUDGET (" \
                                 "ID INTEGER PRIMARY KEY AUTOINCREMENT," \
                                 "TYPE CHAR(1)," \
                                 "TITLE TEXT NOT NULL," \
                                 "AMOUNT REAL," \
                                 "CATEGORY TEXT);";
    int rc = sqlite3_exec(db, sqlCreateTable, callback, 0, &ErrorMsg);
    if(rc != SQLITE_OK) {
        cout << "SQL error: " << ErrorMsg << "\n";
        sqlite3_free(ErrorMsg);
    } else {
        cout << "Table created successfully\n";
    }
}

void dropTable(sqlite3 *db) {
    char *ErrorMsg = nullptr;
    const char *sqlDropTable = "DROP TABLE IF EXISTS BUDGET;";
    int rc = sqlite3_exec(db, sqlDropTable, callback, 0, &ErrorMsg);
    if(rc != SQLITE_OK) {
        cout << "SQL error: " << ErrorMsg << "\n";
        sqlite3_free(ErrorMsg);
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
    cout << "|" << center(argv[0] ? argv[0] : "NULL", 5);  // ID
    cout << "|" << center(argv[1] ? argv[1] : "NULL", 6);  // Type
    cout << "|" << center(argv[4] ? argv[4] : "NULL", 15); // Category
    cout << "|" << center(argv[2] ? argv[2] : "NULL", 15); // Title
    cout << "|" << center(argv[3] ? argv[3] : "NULL", 10) << "|"; // Amount

    string amount = argv[3] ? argv[3] : "NULL";
    if (amount != "NULL") {
        double numAmount = stod(amount);
        totalAmount += (argv[1] && string(argv[1]) == "+") ? numAmount : -numAmount;
    }

    cout << endl;
    return 0;
}



void viewTransactions(sqlite3 *db) {
    char *ErrorMsg = nullptr;
    totalAmount = 0.0;  // Reset the total amount

    const char *sqlSelect = "SELECT * FROM BUDGET ORDER BY TYPE;";
    cout << endl;
    cout << "|" << center("ID", 5)
         << "|" << center("Type", 6)
         << "|" << center("Category", 15)
         << "|" << center("Title", 15)
         << "|" << center("Amount", 10) << "|" << endl;
    cout << "_________________________________________________________" << endl;

    int rc = sqlite3_exec(db, sqlSelect, callbackTable, 0, &ErrorMsg);

    if (rc != SQLITE_OK) {
        cout << "SQL error: " << ErrorMsg << endl;
        sqlite3_free(ErrorMsg);
    }
    cout<<endl;
    cout << "Total Amount: " << totalAmount << endl;
    cout<<endl;
}

pair<string, string> selectCategory() {
    cout << "Select a category:" << endl
         << "0. Exit" << endl
         << "1. Food" << endl
         << "2. Rent" << endl
         << "3. Utilities" << endl
         << "4. Entertainment" << endl
         << "5. Travel" << endl
         << "6. Other Expense" << endl
         << "7. Salary" << endl
         << "8. Other Income" << endl;

    cout << "Enter a number:";
    int categoryChoice;
    cin >> categoryChoice;

    string category;
    string type;

    if (categoryChoice == 0) {
        return make_pair("Exit", "Exit");
    }

    switch (categoryChoice) {
        case 1:
            category = "Food";
            break;
        case 2:
            category = "Rent";
            break;
        case 3:
            category = "Utilities";
            break;
        case 4:
            category = "Entertainment";
            break;
        case 5:
            category = "Travel";
            break;
        case 6:
            category = "Other Expense";
            break;
        case 7:
            category = "Salary";
            break;
        case 8:
            category = "Other Income";
            break;
        default:
            cout << "Invalid choice, setting category to 'Other Expense'\n";
            category = "Other Expense";
            break;
    }

    if (categoryChoice == 7 || categoryChoice == 8) {
        type = "+";
    } else {
        type = "-";
    }

    return make_pair(category, type);
}

pair<string, float> getUserInput(const pair<string, string>& choice) {
    string title;
    float amount;

    cout << "Enter title: ";
    cin >> ws;
    getline(cin, title);

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

    return make_pair(title, amount);
}


void addTransaction(sqlite3 *db) {
    pair<string, string> choice = selectCategory();  // first is category, second is type

    if (choice.first == "Exit") {
        cout << "Exiting the program." << endl;
        return;
    }

    auto userInput = getUserInput(choice);  // first is title, second is amount
    insertIntoBudgetDB(db, choice, userInput.first, userInput.second);
}

void insertIntoBudgetDB(sqlite3 *db, const pair<string, string>& choice, const string& title, float amount) {
    char *ErrorMsg = nullptr;

    string sqlInsert = "INSERT INTO BUDGET (TYPE, TITLE, AMOUNT, CATEGORY) VALUES ('"
                       + choice.second + "','" + title.substr(0, 15) + "'," + to_string(amount) + ",'" + choice.first + "');";

    int rc = sqlite3_exec(db, sqlInsert.c_str(), callback, 0, &ErrorMsg);
    if (rc != SQLITE_OK) {
        cout << "SQL error: " << ErrorMsg << "\n";
        sqlite3_free(ErrorMsg);
    } else {
        cout << "Record inserted successfully\n";
    }
}

bool idExists(sqlite3 *db, int id) {
    string sqlQuery = "SELECT COUNT(*) FROM BUDGET WHERE ID=" + to_string(id) + ";";
    sqlite3_stmt *stmt;
    bool exists = false;

    if (sqlite3_prepare_v2(db, sqlQuery.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            exists = sqlite3_column_int(stmt, 0) > 0;
        }
        sqlite3_finalize(stmt);
    }

    return exists;
}

bool deleteTransactionByID(sqlite3 *db, int id) {
    string sqlDelete = "DELETE FROM BUDGET WHERE ID = " + to_string(id) + ";";
    char *ErrorMsg = nullptr;
    int rc = sqlite3_exec(db, sqlDelete.c_str(), nullptr, 0, &ErrorMsg);

    if (rc != SQLITE_OK) {
        cout << "SQL error: " << ErrorMsg << "\n";
        sqlite3_free(ErrorMsg);
        return false;
    }
    cout <<"Successfully deleted Id "<<endl;
    return true;
}

void deleteTransactionUI(sqlite3 *db) {
    viewTransactions(db);
    int id;
    cout << "Enter the ID that you want to delete or 0 to return: ";
    cin >> id;
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (id == 0) {
        return;
    }

    if (idExists(db, id)) {
        if (deleteTransactionByID(db, id)) {
            cout << "Transaction deleted successfully.\n";
        } else {
            cout << "Failed to delete transaction.\n";
        }
    } else {
        cout << "Invalid ID. Please try again.\n";
    }
}


void filterByCategory(sqlite3 *db) {
    char *ErrorMsg = nullptr;
    string category;

    cout << "Select a category:" << endl
         << "0. Return" << endl  // <-- New option
         << "1. Food" << endl
         << "2. Rent" << endl
         << "3. Utilities" << endl
         << "4. Entertainment" << endl
         << "5. Travel" << endl
         << "6. Other Expense" << endl
         << "7. Salary" << endl
         << "8. Other Income" << endl;

    cout << "Enter a number:";
    int choice;
    cin >> choice;

    if (choice == 0) {
        return;
    }

    switch (choice) {
        case 1:
            category = "Food";
            break;
        case 2:
            category = "Rent";
            break;
        case 3:
            category = "Utilities";
            break;
        case 4:
            category = "Entertainment";
            break;
        case 5:
            category = "Travel";
            break;
        case 6:
            category = "Other Expense";
            break;
        case 7:
            category = "Salary";
            break;
        case 8:
            category = "Other Income";
            break;
        default:
            cout << "Invalid choice.\n";
            return;
    }

    cout << endl;
    cout << "|" << center("ID", 5)
         << "|" << center("Type", 6)
         << "|" << center("Category", 15)
         << "|" << center("Title", 15)
         << "|" << center("Amount", 10) << "|" << endl;
    cout << "_________________________________________________________" << endl;

    string sqlSelect = "SELECT * FROM BUDGET WHERE CATEGORY = '" + category + "' ORDER BY TYPE;";

    int rc = sqlite3_exec(db, sqlSelect.c_str(), callbackTable, 0, &ErrorMsg);

    if (rc != SQLITE_OK) {
        cout << "SQL error: " << ErrorMsg << endl;
        sqlite3_free(ErrorMsg);
    }
}

bool getValidID(sqlite3 *db, int &id) {
    while (true) {
        cout << "Enter ID that you want to update or 0 to return: ";
        cin >> id;

        if(id == 0) {
            return false;
        }

        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        string sqlQuery = "SELECT COUNT(*) FROM BUDGET WHERE ID=" + to_string(id) + ";";
        sqlite3_stmt *stmt;

        if (sqlite3_prepare_v2(db, sqlQuery.c_str(), -1, &stmt, 0) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                int count = sqlite3_column_int(stmt, 0);
                if (count > 0) {
                    sqlite3_finalize(stmt);
                    return true;
                } else {
                    cout << "Invalid ID. Please try again.\n";
                }
            }
            sqlite3_finalize(stmt);
        } else {
            cout << "SQL error: " << sqlite3_errmsg(db) << endl;
        }
    }
}

int getUserChoice() {
    int choice;
    cout << "What would you like to update?\n";
    cout << "1. Name" << endl;
    cout << "2. Amount" << endl;
    cout << "3. Category" << endl;
    cout << "Enter your choice (1-3): ";
    cin >> choice;
    return choice;
}

bool updateDatabase(sqlite3 *db, const string &sqlUpdate) {
    char *ErrorMsg = nullptr;
    int rc = sqlite3_exec(db, sqlUpdate.c_str(), nullptr, 0, &ErrorMsg);
    if(rc != SQLITE_OK) {
        cout << "SQL error: " << ErrorMsg << endl;
        sqlite3_free(ErrorMsg);
        return false;
    } else {
        return true;
    }
}

void updateTransactions(sqlite3 *db) {
    viewTransactions(db);
    int id;
    if (!getValidID(db, id)) {
        return;
    }

    int choice = getUserChoice();

    string sqlUpdate;

    switch(choice) {
        case 1: {
            string newTitle;
            cout << "Enter new Title: ";
            cin >> newTitle;
            sqlUpdate = "UPDATE BUDGET SET TITLE='" + newTitle + "' WHERE ID=" + to_string(id) + ";";
            break;
        }
        case 2: {
            double newAmount;
            cout << "Enter new Amount: ";
            cin >> newAmount;
            sqlUpdate = "UPDATE BUDGET SET AMOUNT=" + to_string(newAmount) + " WHERE ID=" + to_string(id) + ";";
            break;
        }
        case 3: {
            pair<string, string> newCatAndType = selectCategory(); //get new cat and type
            if (newCatAndType.first == "Exit") {
                cout << "Exiting the update process.\n";
                return;
            }
            sqlUpdate = "UPDATE BUDGET SET CATEGORY='" + newCatAndType.first + "', TYPE='" + newCatAndType.second + "' WHERE ID=" + to_string(id) + ";";
            break;
        }
        default:
            cout << "Invalid choice.\n";
            return;
    }

    if (updateDatabase(db, sqlUpdate)) {
        cout << "Update successful!\n";
    }
}


void searchTransactions(sqlite3 *db) {
    char *ErrorMsg = nullptr;
    string keyword;
    cout << "Enter the keyword to search by title: ";
    cin >> keyword;

    cout << endl;
    cout << "|" << center("ID", 5)
         << "|" << center("Type", 6)
         << "|" << center("Category", 15)
         << "|" << center("Title", 15)
         << "|" << center("Amount", 10) << "|" << endl;
    cout << "_________________________________________________________" << endl;

    string sqlSearch = "SELECT * FROM BUDGET WHERE TITLE LIKE '%" + keyword + "%' ORDER BY TYPE;";

    int rc = sqlite3_exec(db, sqlSearch.c_str(), callbackTable, 0, &ErrorMsg);

    if(rc != SQLITE_OK) {
        cout << "SQL error: " << ErrorMsg << "\n";
        sqlite3_free(ErrorMsg);
    }
}

static int callbackCSV(void *NotUsed, int argc, char **argv, char **azColName) {
    ofstream* csvFile = static_cast<ofstream*>(NotUsed);

    for (int i = 0; i < argc; i++) {
        if (i != 0) {
            *csvFile << ", ";
        }
        if (argv[i]) {
            *csvFile << argv[i];
        } else {
            *csvFile << "NULL";
        }
    }
    *csvFile << endl;

    return 0;
}


void exportToCSV(sqlite3 *db) {
    char *ErrorMsg = nullptr;
    ofstream csvFile("BUDGET.csv");

    if (!csvFile.is_open()) {
        cout << "Could not open transactions.csv for writing." << endl;
        return;
    }

    csvFile << "ID, Type, Category, Title, Amount" << endl;

    const char *sqlSelect = "SELECT ID, Type, Category, Title, Amount FROM BUDGET ORDER BY ID;";

    int rc = sqlite3_exec(db, sqlSelect, callbackCSV, (void*)&csvFile, &ErrorMsg);

    if (rc != SQLITE_OK) {
        cout << "SQL error: " << ErrorMsg << endl;
        sqlite3_free(ErrorMsg);
    } else {
        cout << "Data exported successfully to transactions.csv" << endl;
    }
    csvFile.close();
}

