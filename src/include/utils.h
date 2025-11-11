#include "string"
#include "map"
using namespace std;

#ifndef UTILS_H
#define UTILS_H

pair<string, string> generate_new_password();
void copy_to_clipboard(const string& random_password);
int callback(void* data, int argc, char** argv, char** azColName);
void login (string &login_username, string &login_password);
map<string, string>edit_entry_fields(map<string, string> entry);

#endif
