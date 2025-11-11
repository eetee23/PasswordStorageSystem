#include "iostream"
#include "random"
#include "chrono"
#include "string"
#include "vector"
#include "map"

#include <db_creation.h>
#include <globals.h>
#include <sqlite3.h>

using namespace std;

int sqlite_data_base_creation() {
    sqlite3* db;

    int exit = sqlite3_open("database/passwords.db", &db);

    if (exit != SQLITE_OK) {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        return 1;
    } else {
        cout << "Opened SQLite database successfully!\n";
    }

    exit = sqlite3_exec(db, ("PRAGMA key = '" + std::string(db_key) + "';").c_str(), nullptr, nullptr, nullptr);

    if (exit != SQLITE_OK) {
        std::cerr << "Failed to set key when creating DB." << std::endl;
        sqlite3_close(db);
        return 1;
    }

    string db_password_table =  "CREATE TABLE IF NOT EXISTS PASSWORDS(ID INTEGER PRIMARY KEY AUTOINCREMENT," \
                                "NAME STRING NOT NULL," \
                                "PASSWORD STRING NOT NULL)";
    char* message_error;
    exit = sqlite3_exec(db, db_password_table.c_str(), NULL, 0, &message_error);
    if (exit != SQLITE_OK) {
        cerr << "ERROR in Creating password table: " << message_error << endl;
        sqlite3_free(message_error);
        message_error = nullptr;
        return 1;
    } else{
        cout << "Password table was successfully created" << endl;
    }
    string db_user_credentials_table =  "CREATE TABLE IF NOT EXISTS CREDENTIALS(ID INTEGER PRIMARY KEY AUTOINCREMENT," \
                                        "TABLE_NAME TEXT NOT NULL," \
                                        "USERNAME TEXT NOT NULL," \
                                        "PASSWORD TEXT NOT NULL);";
    exit = sqlite3_exec(db, db_user_credentials_table.c_str(), NULL, 0, &message_error);
    if (exit != SQLITE_OK) {
        cerr << "ERROR in Creating credentials table: " << message_error << endl;
        sqlite3_free(message_error);
        message_error = nullptr;
        return 1;
    } else{
        cout << "Credentials table was successfully created" << endl;
    }
    sqlite3_close(db);
    return 0;
}
