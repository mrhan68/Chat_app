#ifndef __DB_H__
#define __DB_H__

#include "network.h"
#include <sqlite3.h>
#define MAX_SQL_SIZE 3072
#define MAX_FRIEND_SIZE 50

sqlite3 *Create_room_sqlite(Package *pkg);
void save_chat(Package *pkg);
void drop_table(int group_id);
void create_friends_table();
void save_friend(char *sender, char *receiver);
void remove_friend(char *sender, char *receiver);
char *get_friends_list(char *username);
void printf_friends_list(char **friends);
int callback(void *data, int argc, char **argv, char **azColName);
int check_friends_table(char *sender, char *receiver);
int callback_2(void *data, int argc, char **argv, char **azColName);

#endif
