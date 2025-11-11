#include "string"
#include "map"
using namespace std;

#ifndef DB_VIEW_H
#define DB_VIEW_H

void browse_passwords();
map<string, string> search_by_id(string input);
map<string, string> search_by_name(string input);
bool check_credentials_from_database(string& out_username, string& out_password);


#endif
