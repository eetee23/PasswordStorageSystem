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

void browse_passwords() {
    sqlite3* db;
    char* message_error;
    vector<map<string, string>> result_rows;

    int exit = sqlite3_open("database/passwords.db", &db);

    if (exit != SQLITE_OK) {
        cerr << "ERROR opening database for browsing passwords" << sqlite3_errmsg(db) << endl;
        return;
    }

    exit = sqlite3_exec(db, ("PRAGMA key = '" + std::string(db_key) + "';").c_str(), nullptr, nullptr, nullptr);

    if (exit != SQLITE_OK) {
        std::cerr << "Failed to set key when creating DB." << std::endl;
        sqlite3_close(db);
        return;
    }

    string get_passwords = "SELECT * from passwords;";

    exit = sqlite3_exec(db, get_passwords.c_str(), callback, &result_rows, &message_error);

    if (exit != SQLITE_OK) {
        cerr << "ERROR in selecting from passwords table" << message_error << endl;
        sqlite3_free(message_error);
        sqlite3_close(db);
        return;
    }

    sqlite3_close(db);

    if (result_rows.empty()) {
        cout << "Password storage is empty" << endl;
        return;
    }

    for (const auto& row : result_rows) {
        auto id = row.find("ID");
        auto tag = row.find("NAME");
        if (id != row.end() && tag != row.end()) {
            cout << id->second << ": " << tag->second << endl;
        }
    }
    return;
}

map<string, string> search_by_id(string input) {
    sqlite3* db;
    char message_error;
    int id = stoi(input);
    int exit = sqlite3_open("database/passwords.db", &db);

    if (exit != SQLITE_OK) {
        cerr << "ERROR opening database for password id search" << sqlite3_errmsg(db) << endl;
    }

    exit = sqlite3_exec(db, ("PRAGMA key = '" + std::string(db_key) + "';").c_str(), nullptr, nullptr, nullptr);

    if (exit != SQLITE_OK) {
        std::cerr << "Failed to set key when creating DB." << std::endl;
        sqlite3_close(db);
        return {};
    }

    string check_by_id = "SELECT * FROM passwords WHERE ID = ?;";
    sqlite3_stmt* stmt;
    
    exit = sqlite3_prepare_v2(db, check_by_id.c_str(), -1, &stmt, nullptr);

    if (exit != SQLITE_OK) {
        cerr << "Failed to prepare select statement" << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return {};
    }

    sqlite3_bind_int(stmt, 1, id);

    exit = sqlite3_step(stmt);

    map<string, string> result;

    if (exit == SQLITE_ROW) {
        int fetched_id = sqlite3_column_int(stmt, 0);
        const unsigned char* name = sqlite3_column_text(stmt, 1);
        const unsigned char* password = sqlite3_column_text(stmt, 2);

        cout << "id: " << fetched_id << " name: " << name << endl;


        result["id"] = to_string(fetched_id);
        result["name"] = name ? reinterpret_cast<const char*>(name): "";
        result["password"] = password ? reinterpret_cast<const char*>(password): "";
    } else {
        cerr << "ERROR in selecting password: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return {};
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return result;
}

map<string, string> search_by_name(string input) {
    sqlite3* db;
    char message_error;
    int exit = sqlite3_open("database/passwords.db", &db);

    if (exit != SQLITE_OK) {
        cerr << "ERROR opening database for password name search" << sqlite3_errmsg(db) << endl;
    }

    exit = sqlite3_exec(db, ("PRAGMA key = '" + std::string(db_key) + "';").c_str(), nullptr, nullptr, nullptr);

    if (exit != SQLITE_OK) {
        std::cerr << "Failed to set key when creating DB." << std::endl;
        sqlite3_close(db);
        return {};
    }

    string check_by_id = "SELECT * FROM passwords WHERE NAME = ?;";
    sqlite3_stmt* stmt;
    
    exit = sqlite3_prepare_v2(db, check_by_id.c_str(), -1, &stmt, nullptr);

    if (exit != SQLITE_OK) {
        cerr << "Failed to prepare select statement" << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return {};
    }

    sqlite3_bind_text(stmt, 1, input.c_str(), -1, SQLITE_TRANSIENT);

    vector<map<string, string>> all_rows;
    map<string,string> row;
    map<string,string> result;

    for (int i = 0; i < 99; i++) {
        exit = sqlite3_step(stmt);
        if (exit == SQLITE_ROW) {
            int fetched_id = sqlite3_column_int(stmt, 0);
            const unsigned char* name = sqlite3_column_text(stmt, 1);
            const unsigned char* password = sqlite3_column_text(stmt, 2);

            row.clear();
            row["id"] = to_string(fetched_id);
            row["name"] = name ? reinterpret_cast<const char*>(name): "";
            row["password"] = password ? reinterpret_cast<const char*>(password): "";
            all_rows.push_back(row);

            cout << "id: " << fetched_id << " name: " << name << endl;
        } else if (exit == SQLITE_DONE) {
            if (i == 0) {
                cout << "Password name not found" << endl;
            }
            else if (i > 1) {
                int id;
                cout << "Give id of the password you would like to copy: ";
                cin >> id;
                
                for (const auto& result_row : all_rows) {
                    auto id_check_row = result_row.find("id");
                    if (id_check_row != result_row.end()) {
                        int id_check = stoi(id_check_row->second);
                        if (id == id_check) {
                            result = result_row;
                            break;
                        }
                    }
                }
            } else {
                result = row;
            }
            break;
        }else {
            cerr << "ERROR in selecting password: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return {};
        }
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return result;
}


bool check_credentials_from_database(string& out_username, string& out_password) {
    sqlite3* db;
    vector<map<string, string>> result_rows;

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

    string check_credentials = "SELECT * FROM credentials WHERE TABLE_NAME = 'main'";
    char* message_error;
    exit = sqlite3_exec(db, check_credentials.c_str(), callback, &result_rows, &message_error);
    if (exit != SQLITE_OK) {
        cerr << "ERROR in main credentials table select" << endl;
        sqlite3_free(message_error);
        sqlite3_close(db);
        message_error = nullptr;
        return false;
    }

    sqlite3_close(db);

    if (result_rows.empty()) {
        cout << "creadentials is empty" << endl;
        return false;
    }

    for (const auto& row : result_rows) {
        auto it_user = row.find("USERNAME");
        auto it_pass = row.find("PASSWORD");

        if (it_user != row.end() && !it_user->second.empty() &&
            it_pass != row.end() && !it_pass->second.empty()) {
            out_username = it_user->second;
            out_password = it_pass->second;
            return true;
        }
    }

    return false;
}

