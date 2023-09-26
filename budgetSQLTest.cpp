#include <gtest/gtest.h>
#include <sqlite3.h>
#include <cstring>
#include <iostream>
#include "budgetSQL.h"
#include <fstream>
#include <vector>

using namespace std;

class BudgetSQLTest : public testing::Test {
protected:
    sqlite3 *db;

    void SetUp() override {
        int rc = sqlite3_open("memory", &db);
        ASSERT_EQ(rc, SQLITE_OK);
    }

    void TearDown() override {
        dropTable(db);
        sqlite3_close(db);
    }
};

TEST_F(BudgetSQLTest, TestCreateTable) {
    cout<< "Test 1. : create Table" <<endl;
    createTable(db);

    char *ErrorMsg = nullptr;
    const char *sqlVerifyTable = "SELECT name FROM sqlite_master WHERE type='table' AND name='BUDGET';";
    bool table_created = false;
    auto verifyCallback = [](void *data, int argc, char **argv, char **azColName) -> int {
        if (argc > 0 && strcmp(argv[0], "BUDGET") == 0) {
            *static_cast<bool *>(data) = true;
        }
        return 0;
    };

    int rc = sqlite3_exec(db, sqlVerifyTable, verifyCallback, &table_created, &ErrorMsg);
    if (rc != SQLITE_OK) {
        sqlite3_free(ErrorMsg);
    }

    EXPECT_TRUE(table_created);
}
TEST_F(BudgetSQLTest, TestDropTable) {
    cout<< "Test 2. : create and drop Table" <<endl;

    createTable(db);
    dropTable(db);

    char *ErrorMsg = nullptr;
    const char *sqlVerifyTable = "SELECT name FROM sqlite_master WHERE type='table' AND name='BUDGET';";
    bool table_dropped = true;
    auto verifyCallback = [](void *data, int argc, char **argv, char **azColName) -> int {
        if (argc > 0 && strcmp(argv[0], "BUDGET") == 0) {
            *static_cast<bool *>(data) = false;
        }
        return 0;
    };

    int rc = sqlite3_exec(db, sqlVerifyTable, verifyCallback, &table_dropped, &ErrorMsg);
    if (rc != SQLITE_OK) {
        sqlite3_free(ErrorMsg);
    }
    EXPECT_TRUE(table_dropped);
}

TEST_F(BudgetSQLTest, TestDropTableWithoutCreating) {
    cout<< "Test 3. :  drop Table" <<endl;
    dropTable(db);

    char *ErrorMsg = nullptr;
    const char *sqlVerifyTable = "SELECT name FROM sqlite_master WHERE type='table' AND name='BUDGET';";
    bool table_dropped = true;
    auto verifyCallback = [](void *data, int argc, char **argv, char **azColName) -> int {
        if (argc > 0 && strcmp(argv[0], "BUDGET") == 0) {
            *static_cast<bool *>(data) = false;
        }
        return 0;
    };

    int rc = sqlite3_exec(db, sqlVerifyTable, verifyCallback, &table_dropped, &ErrorMsg);
    if (rc != SQLITE_OK) {
        sqlite3_free(ErrorMsg);
    }

    EXPECT_TRUE(table_dropped);
}

TEST_F(BudgetSQLTest, TestGetUserInput) {
    cout<< "Test 4. :  get User Input" <<endl;
    istringstream input("TestTitle\n100.5\n");
    streambuf* orig_cin = cin.rdbuf(input.rdbuf());

    auto result = getUserInput({"Food", "-"});

    cin.rdbuf(orig_cin);

    ASSERT_EQ(result.first, "TestTitle");
    ASSERT_FLOAT_EQ(result.second, 100.5);
}

TEST_F(BudgetSQLTest, TestInsertIntoBudgetDB) {
    cout<< "Test 5. :  Insert Into SQL" <<endl;

    createTable(db);
    insertIntoBudgetDB(db, {"Food", "-"}, "Groceries", 200.0);

    sqlite3_stmt *stmt;
    const char *sql = "SELECT TYPE, TITLE, AMOUNT, CATEGORY FROM BUDGET WHERE TITLE='Groceries'";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    ASSERT_EQ(rc, SQLITE_OK) << "Failed to prepare SQL statement.";

    rc = sqlite3_step(stmt);
    ASSERT_EQ(rc, SQLITE_ROW) << "Expected a single row, but got something else.";

    const unsigned char* type_col = sqlite3_column_text(stmt, 0);
    const unsigned char* title_col = sqlite3_column_text(stmt, 1);
    const unsigned char* category_col = sqlite3_column_text(stmt, 3);

    ASSERT_NE(type_col, nullptr) << "Type should not be NULL.";
    ASSERT_NE(title_col, nullptr) << "Title should not be NULL.";
    ASSERT_NE(category_col, nullptr) << "Category should not be NULL.";

    string type = string(reinterpret_cast<const char*>(type_col));
    string title = string(reinterpret_cast<const char*>(title_col));
    float amount = sqlite3_column_double(stmt, 2);
    string category = string(reinterpret_cast<const char*>(category_col));

    ASSERT_EQ(type, "-");
    ASSERT_EQ(title, "Groceries");
    ASSERT_FLOAT_EQ(amount, 200.0);
    ASSERT_EQ(category, "Food");

    sqlite3_finalize(stmt);
}



TEST_F(BudgetSQLTest, DeleteByID) {
    cout<< "Test 6. :  Delete ID" <<endl;

    createTable(db);
    insertIntoBudgetDB(db, {"Food", "-"}, "Groceries", 100.0);

    bool exists = idExists(db, 1);
    ASSERT_TRUE(exists) << "ID should exist in the table.";

    bool deleted = deleteTransactionByID(db, 1);
    ASSERT_TRUE(deleted) << "Should successfully delete transaction by ID.";

    exists = idExists(db, 1);
    ASSERT_FALSE(exists) << "ID should no longer exist in the table.";
}

vector<string> readCSVFile(const string& filename) {
    ifstream file(filename);
    string line;
    vector<string> lines;

    if (file.is_open()) {
        while (getline(file, line)) {
            lines.push_back(line);
        }
        file.close();
    }

    return lines;
}


TEST_F(BudgetSQLTest, TestExportToCSV) {
    cout<< "Test 7. :  export CSV" <<endl;
    createTable(db);
    insertIntoBudgetDB(db, {"Food", "-"}, "Groceries", 200.0);
    insertIntoBudgetDB(db, {"Salary", "+"}, "March", 1000.0);

    exportToCSV(db);

    vector<string> lines = readCSVFile("BUDGET.csv");

    ASSERT_EQ(lines[0], "ID, Type, Category, Title, Amount");
    ASSERT_EQ(lines[1], "1, -, Food, Groceries, 200.0");
    ASSERT_EQ(lines[2], "2, +, Salary, March, 1000.0");
}

TEST_F(BudgetSQLTest, TestGetUserChoice) {
    cout<< "Test 8. :  export CSV" <<endl;
    istringstream input("5\n");
    streambuf* orig_cin = cin.rdbuf(input.rdbuf());
    int choice = getUserChoice();
    cin.rdbuf(orig_cin);
    ASSERT_EQ(choice, 5);
}

TEST_F(BudgetSQLTest, TestUpdateTitle) {
    cout << "Test 9.: Update Title in Database" << endl;

    createTable(db);
    insertIntoBudgetDB(db, {"Food", "-"}, "Groceries", 200.0);

    int id = 1;
    string newTitle = "Snacks";
    string sqlUpdate = "UPDATE BUDGET SET TITLE='" + newTitle + "' WHERE ID=" + to_string(id) + ";";

    bool updateStatus = updateDatabase(db, sqlUpdate);
    ASSERT_TRUE(updateStatus) << "Database should be updated successfully.";

    sqlite3_stmt *stmt;
    const char *sql = "SELECT TITLE FROM BUDGET WHERE ID=1";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    ASSERT_EQ(rc, SQLITE_OK) << "Failed to prepare SQL statement.";
    rc = sqlite3_step(stmt);
    ASSERT_EQ(rc, SQLITE_ROW) << "Expected a single row, but got something else.";

    const unsigned char* title_raw = sqlite3_column_text(stmt, 0);
    ASSERT_NE(title_raw, nullptr) << "Title should not be NULL.";
    string title = string(reinterpret_cast<const char*>(title_raw));
    ASSERT_EQ(title, "Snacks");

    sqlite3_finalize(stmt);
}

TEST_F(BudgetSQLTest, TestUpdateAmount) {
    cout << "Test 10: Update Amount in Database" << endl;

    createTable(db);
    insertIntoBudgetDB(db, {"Food", "-"}, "Groceries", 200.0);

    int id = 1;
    double newAmount = 100.0;
    string sqlUpdate = "UPDATE BUDGET SET AMOUNT=" + to_string(newAmount) + " WHERE ID=" + to_string(id) + ";";

    bool updateStatus = updateDatabase(db, sqlUpdate);
    ASSERT_TRUE(updateStatus) << "Database should be updated successfully.";

    sqlite3_stmt *stmt;
    const char *sql = "SELECT AMOUNT FROM BUDGET WHERE ID=1";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    ASSERT_EQ(rc, SQLITE_OK) << "Failed to prepare SQL statement.";
    rc = sqlite3_step(stmt);
    ASSERT_EQ(rc, SQLITE_ROW) << "Expected a single row, but got something else.";

    float amount = sqlite3_column_double(stmt, 0);
    ASSERT_FLOAT_EQ(amount, 100.0);

    sqlite3_finalize(stmt);
}

TEST_F(BudgetSQLTest, TestUpdateCategory) {
    cout << "Test 11. : Update Category in Database" << endl;

    createTable(db);
    insertIntoBudgetDB(db, {"Food", "-"}, "Groceries", 200.0);

    int id = 1;
    string newCategory = "Rent";
    string sqlUpdate = "UPDATE BUDGET SET CATEGORY='" + newCategory + "' WHERE ID=" + to_string(id) + ";";

    bool updateStatus = updateDatabase(db, sqlUpdate);
    ASSERT_TRUE(updateStatus) << "Database should be updated successfully.";

    sqlite3_stmt *stmt;
    const char *sql = "SELECT CATEGORY FROM BUDGET WHERE ID=1";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    ASSERT_EQ(rc, SQLITE_OK) << "Failed to prepare SQL statement.";
    rc = sqlite3_step(stmt);
    ASSERT_EQ(rc, SQLITE_ROW) << "Expected a single row, but got something else.";

    const unsigned char* category_raw = sqlite3_column_text(stmt, 0);
    ASSERT_NE(category_raw, nullptr) << "Category should not be NULL.";
    string category = string(reinterpret_cast<const char*>(category_raw));
    ASSERT_EQ(category, "Rent");

    sqlite3_finalize(stmt);
}

