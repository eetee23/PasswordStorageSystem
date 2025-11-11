#include "string"
#include "map"
using namespace std;

#ifndef DB_MODIFICATION_H
#define DB_MODIFICATION_H

bool add_password_to_database(string tag, string password);
void create_credentials_to_db();
void delete_entry(int id);
void edit_db_entry(map<string, string> entry);

#endif
