#include "iostream"
#include "random"
#include "chrono"
#include "string"
#include "vector"
#include "map"

#include <utils.h>
#include <globals.h>
#include <sqlite3.h>

using namespace std;

bool add_password_to_database(string tag, string password) {
    sqlite3* db;

    int exit = sqlite3_open("database/passwords.db", &db);

    if (exit != SQLITE_OK) {
        cerr << "ERROR opening database for credential check" << sqlite3_errmsg(db) << endl;
        return false;
    }

    exit = sqlite3_exec(db, ("PRAGMA key = '" + std::string(db_key) + "';").c_str(), nullptr, nullptr, nullptr);

    if (exit != SQLITE_OK) {
        std::cerr << "Failed to set key when creating DB." << std::endl;
        sqlite3_close(db);
        return 1;
    }

    string password_insert ("INSERT INTO PASSWORDS (NAME, PASSWORD) VALUES(?, ?);");
    sqlite3_stmt* stmt;

    exit = sqlite3_prepare_v2(db, password_insert.c_str(), -1, &stmt, nullptr);

    if (exit != SQLITE_OK) {
        cerr << "Failed to prepare insert statement" << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return false;
    }

    sqlite3_bind_text(stmt, 1, tag.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_TRANSIENT);

    exit = sqlite3_step(stmt);

    if (exit != SQLITE_DONE) {
        cerr << "ERROR in new password insert: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return false;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return true;
}

void create_credentials_to_db() {
    sqlite3* db;
    char* messageError;

    cout << "creating credentials" << endl;

    int exit = sqlite3_open("database/passwords.db", &db);

    if (exit != SQLITE_OK) {
        cerr << "ERROR opening database for  creating credentials" << sqlite3_errmsg(db) << endl;
        return;
    }

    exit = sqlite3_exec(db, ("PRAGMA key = '" + std::string(db_key) + "';").c_str(), nullptr, nullptr, nullptr);

    if (exit != SQLITE_OK) {
        std::cerr << "Failed to set key when creating DB." << std::endl;
        sqlite3_close(db);
        return;
    }

    string username, password;

    login(username, password);
    cout << "username " << username << endl;
    cout << "password " << password << endl;
    string sql_credentials("INSERT INTO credentials (TABLE_NAME, USERNAME, PASSWORD) VALUES(?, ?, ?);");
    sqlite3_stmt* stmt;

    exit = sqlite3_prepare_v2(db, sql_credentials.c_str(), -1, &stmt, nullptr);

    if (exit != SQLITE_OK) {
        cerr << "Failed to prepare insert statement" << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_text(stmt, 1, "main", -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, password.c_str(), -1, SQLITE_TRANSIENT);

    exit = sqlite3_step(stmt);

    if (exit != SQLITE_DONE) {
        cerr << "Login credential insert ERROR" << sqlite3_errmsg(db) << endl;
        sqlite3_free(messageError);
    }
    else {
        cout << "Login credentials added" << endl;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void delete_entry(int id) {
    sqlite3* db;
    char* message_error;

    int exit = sqlite3_open("database/passwords.db", &db);

    if (exit != SQLITE_OK) {
        cerr << "ERROR opening database for deleting passwords" << sqlite3_errmsg(db) << endl;
        return;
    }

    exit = sqlite3_exec(db, ("PRAGMA key = '" + std::string(db_key) + "';").c_str(), nullptr, nullptr, nullptr);

    if (exit != SQLITE_OK) {
        std::cerr << "Failed to set key when creating DB." << std::endl;
        sqlite3_close(db);
        return;
    }

    string delete_password = "DELETE FROM passwords WHERE ID = ?;";
    sqlite3_stmt* stmt;

    exit = sqlite3_prepare_v2(db, delete_password.c_str(), -1, &stmt, nullptr);

    if (exit != SQLITE_OK) {
        cerr << "Failed to prepare select statement" << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_int(stmt, 1, id);

    exit = sqlite3_step(stmt);

    if (exit == SQLITE_DONE) {
        cout << "Entry deletion compelete" << endl;
    } else {
        cerr << "Entry deletion FAILED" << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return;
}

void edit_db_entry(map<string, string> entry) {
    sqlite3* db;
    
    int exit = sqlite3_open("database/passwords.db", &db);

    if(exit != SQLITE_OK) {
        cerr << "ERROR opening database for entry editing" << sqlite3_errmsg(db) << endl;
        return;
    }

    exit = sqlite3_exec(db, ("PRAGMA key = '" + std::string(db_key) + "';").c_str(), nullptr, nullptr, nullptr);

    if (exit != SQLITE_OK) {
        std::cerr << "Failed to set key when creating DB." << std::endl;
        sqlite3_close(db);
        return;
    }

    for (const auto& pair : entry) {
        cout << "key: " << pair.first << " value: " << pair.second << endl; 
    }

    auto id_entry = entry.find("id");
    auto name_entry = entry.find("name");
    auto password_entry = entry.find("password");

    int id = stoi(id_entry->second);
    entry.erase(id_entry);

    sqlite3_stmt* stmt;    
    for (const auto& pair : entry) {
        string key = pair.first;
        string value = pair.second;

        string edit_entry = "UPDATE passwords SET "+ key +" = ? WHERE ID = ?;";

        exit = sqlite3_prepare(db, edit_entry.c_str(), -1, &stmt, nullptr);

        if (exit != SQLITE_OK) {
            cerr << "Failed to prepare UPDATE statement " << sqlite3_errmsg(db) << endl;
            sqlite3_close(db);
            return;
        }
    
        sqlite3_bind_text(stmt, 1, value.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 2, id);

        exit = sqlite3_step(stmt);

        if (exit != SQLITE_DONE) {
            cerr << "Entry editing FAILED try again " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(stmt);
            break;
        }
        sqlite3_finalize(stmt);
    }
    sqlite3_close(db);
    return;
}