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

#include <utils.h>
#include <sqlite3.h>

using namespace std;

pair<string, string> generate_new_password(){
    string password_tag, password;
    int password_length;
    const string characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!#¤%&";
    auto seed = chrono::steady_clock::now().time_since_epoch().count();
    mt19937 generator(seed);

    uniform_int_distribution<> distribution(0, characters.size() - 1);

    cout << "Give tag for the password: ";
    cin >> password_tag;

    cout << "Give password lenght: ";
    cin >> password_length;

    cout << "Generating password" << endl;
    string random_string;
    for (int i = 0; i < password_length; ++i) {
        random_string += characters[distribution(generator)];
    }

    return make_pair(password_tag, random_string);
}


void copy_to_clipboard(const string& random_password) {
    #ifdef _WIN32
        const size_t lrp = random_password.length() + 1;
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, lrp);
        if (!hMem) {
            cout << "Failed to allocate memory for coping" << endl;
            return;
        }
        memcpy(GlobalLock(hMem), random_password.c_str(), lrp);
        GlobalUnlock(hMem);

        if (OpenClipboard(0)) {
            EmptyClipboard();
            SetClipboardData(CF_TEXT, hMem);
            CloseClipboard();
            cout << "Password copied to clip board" << endl;
        } else {
            cout << "Failed to open clipbroad" << endl;
            GlobalFree(hMem);
        }
    #else
        string cmd = "echo \"" + random_password + "\" | xclip -selection clipboard";
    #endif
}

int callback(void* data, int argc, char** argv, char** azColName) {
    // Data is the return value to calling function
    // Number of columns in the current row
    // Array of strings representing the values of each column in the current row
    // Array of strings representing the names of each column in the current row
    auto* results = static_cast<vector<map<string, string>>*>(data);

    map<string, string> row;

    for (int i = 0; i < argc; i++) {
        string key = azColName[i];
        string value = (argv[i] ? argv[i] : "NULL");

        row[key] = value;
    }

    results->push_back(row);

    return 0;
}

void login (string &login_username, string &login_password) {
    cout << "Enter username: ";
    cin >> login_username;

    cout << "Enter passsword: ";
    cin >> login_password;
}

map<string, string>edit_entry_fields(map<string, string> entry) {
    auto id_entry = entry.find("id");
    auto name_entry = entry.find("name");
    auto password_entry = entry.find("password");
    string entry_value;
    map<string, string> result = {};
    for (int i = 0; i < 99; i++) {
        int input;
        string user_input;

        cout << "1. Name" << endl;
        cout << "2. Password" << endl;
        cout << "3. Close" << endl;
        cout << "Enter number for what you would like to edit: ";
        cin >> input;

        switch(input) {
            case 1:
                if (result.find("id") == result.end()) {
                    cout << "id: " << id_entry->second << endl;
                    string id_value = id_entry->second;
                    result["id"] = id_value;
                }
                if (result.find("name") != result.end()) {
                    cout << "Current name: " << result["name"] << endl;
                    cout << "Enter new name: ";
                    cin >> user_input;
                    result["name"] = user_input;
                } else {
                    entry_value = name_entry->second;
                    cout << "Current name: " << entry_value << endl;
                    cout << "Enter new name: ";
                    cin >> user_input;
                    result["name"] = user_input;
                }
                break;
            case 2:
                if (result.find("id") == result.end()) {
                    string id_value = id_entry->second;
                    result["id"] = id_value;
                }
                if (result.find("password") != result.end()) {
                    cout << "Current password: " << result["password"] << endl;
                    cout << "Enter new password: ";
                    cin >> user_input;
                    result["password"] = user_input;
                } else {
                    entry_value = password_entry->second;
                    cout << "Current password " << entry_value << endl;
                    cout << "Enter new password: ";
                    cin >> user_input;
                    result["password"] = user_input;
                }
                break;
            case 3:
                i = 99;
                break;
            default:
                cout << "Invalid input: "<< input << endl;
        }
    }
    return result;
}