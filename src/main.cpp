#include "iostream"
#include "random"
#include "chrono"
#include "string"
#include "vector"
#include "map"

#ifdef _WIN32
#include <windows.h>
#else
#include <cstdlib>
#endif

#include <db_view.h>
#include <db_modification.h>
#include <db_creation.h>
#include <globals.h>
#include <utils.h>
#include <sqlite3.h>

using namespace std;

const char *db_key = "mysecretkey";

int check_credentials(string username, string password) {
    int result = 0;
    string login_username, login_password;

    cout << "Enter username: ";
    cin >> login_username;

    cout << "Enter password: ";
    cin >> login_password;

    if (username == login_username && password == login_password) {
        result = 1;
        cout << "correct credentials" << endl;
        return result;
    } else {
        cout << "username or password is incorrect" << endl;
        return result;
    }
}

void new_password() {
    string password, tag;
    pair<string, string> np;
    bool db_add;

    np = generate_new_password();
    tag = np.first;
    password = np.second;
    db_add = add_password_to_database(tag, password);
    if (db_add == true) {
        cout << "new password created under tag " << tag << endl;
        copy_to_clipboard(password);
    } else {
        cout << "Failed to generate password for " << tag << endl;
    }
}

void get_password() {
    string search_input;
    bool str_value = false;
    map<string, string> result;

    cout << "Input password id or name: ";
    cin >> search_input;

    for (char ch : search_input) {
        if (!(ch >= 48 && ch <= 57)) {
            str_value = true;
            break;
        }
    }

    if (str_value != true) {
        result = search_by_id(search_input);
     } else {
        result = search_by_name(search_input);
    }
    if (!result.empty()) {
        auto id_entry = result.find("id");
        auto name_entry = result.find("name");
        cout << "id: " << id_entry->second << " name: " << name_entry->second << endl;
        auto pw_entry = result.find("password");
        string password = pw_entry->second;
        copy_to_clipboard(password);
    }
}

void delete_password() {
    string input;
    bool str_value = false;
    map<string, string> result;
    cout << "For deleting give entry id or name: ";
    cin >> input;

    for (char ch : input) {
        if (!(ch >= 48 && ch <= 57)) {
            str_value = true;
            break;
        }
    }

    if (str_value != true) {
        int int_input = stoi(input);
        delete_entry(int_input);
    } else {
        result = search_by_name(input);
        if (!result.empty()) {
            auto id_entry = result.find("id");
            int id = stoi(id_entry->second);
            delete_entry(id);
        }
    }
}

void edit_entry() {
    string input;
    bool val_string = false;
    map<string, string> search_result, edited_entry;

    cout << "Enter id or name for the password you would like to edit: ";
    cin >> input;

    for (char ch : input) {
        if (!(ch >= 48 && ch <= 57)) {
            val_string = true;
            break;
        }
    }

    if (!val_string) {
        search_result = search_by_id(input);
    } else {
        search_result = search_by_name(input);
    }
    if (!search_result.empty()) {
        edited_entry = edit_entry_fields(search_result);
        if (!edited_entry.empty()) {
            edit_db_entry(edited_entry);
        }
    }
}

int main () {
    int result, check_login, db_check, db_credentials, start;
    string login_username, login_password;
    string username, password;

    start = sqlite_data_base_creation();
    if (start != 0) {
        return 0;
    }

    for (int i = 0; i < 2; i++) {
        bool found = check_credentials_from_database(username, password);
        if (!found) {
            cout << "Credentials not found, creating new..." << endl;
            create_credentials_to_db();
            continue;
        } else {
            break;
        }
    }
    cout << "Login to password manager" << endl;
    check_login = check_credentials(username, password);
    if (check_login == 1) {
        int action;
        bool program = true;
        while (program == true) {
            cout << "What would you like to do" << endl;
            cout << "1. Get password by id or name" << endl;
            cout << "2. Browse passwords" << endl;
            cout << "3. Generate passwords" << endl;
            cout << "4. Edit password entry" << endl;
            cout << "5. Delete password by name" << endl;
            cout << "6. Close program" << endl;
            cout << "Input number for the action you would like to do: ";
            cin >> action;
            switch(action) {
                case 1:
                    get_password();
                    break;
                case 2:
                    browse_passwords();
                    break;
                case 3:
                    new_password();
                    break;
                case 4:
                    edit_entry();
                    break;
                case 5:
                    delete_password();
                    break;
                case 6:
                    program = false;
                    break;
                default:
                    cout << "Invalid action given: " << action << endl;
                    break;
            }
        }
    } else {
        return 0;
    }
    return 0;
}