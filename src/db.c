#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "db.h"

char *friends[MAX_FRIEND_SIZE];
char **pointer;
// char friends_list[MAX_FRIEND_SIZE][30];
int num_of_friends = 0;
int is_existed = 0;
char friends_list[MAX_SQL_SIZE];

sqlite3 *Create_room_sqlite(Package *pkg)
{
    sqlite3 *database;
    char name[23];
    sprintf(name, "%d.db", pkg->group_id);
    int ret = sqlite3_open(name, &database);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(database));
        exit(0);
    }
    return database;
}

void save_chat(Package *pkg)
{
    sqlite3 *database = Create_room_sqlite(pkg);

    char *errmsg = NULL;
    char buf[MAX_SQL_SIZE] = "create table if not exists chat(time TEXT, sender TEXT, message TEXT)";
    int ret = sqlite3_exec(database, buf, NULL, NULL, &errmsg);
    if (ret != SQLITE_OK)
    {
        printf("Open table failed\n");
        return;
    }

    time_t t;
    t = time(&t);
    char time[100];
    strcpy(time, ctime(&t));
    int len = strlen(time);
    time[len - 1] = '\0';
    sprintf(buf, "insert into chat values('%s','%s','%s')", time, pkg->sender, pkg->msg);
    ret = sqlite3_exec(database, buf, NULL, NULL, &errmsg);
    if (ret != SQLITE_OK)
    {
        printf("Failed to insert data\n");
        return;
    }
    sqlite3_close(database);
}

void drop_table(int group_id)
{
    sqlite3 *database;
    char name[23];
    sprintf(name, "%d.db", group_id);
    int ret = sqlite3_open(name, &database);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(database));
        exit(0);
    }

    char *errmsg = NULL;

    ret = sqlite3_exec(database, "drop table IF EXISTS chat", 0, 0, &errmsg);

    if (ret != SQLITE_OK)
    {

        printf("Error in SQL statement: %s\n", errmsg);

        sqlite3_free(errmsg);
        sqlite3_close(database);
        return;
    }
    printf("Chat table dropped successfully\n");
    sqlite3_close(database);
}

void create_friends_table()
{
    sqlite3 *db;
    int ret = sqlite3_open("friends.db", &db);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }

    char *errmsg = NULL;
    char *sql = "CREATE TABLE if not exists Friends(sender TEXT ,receiver TEXT);";
    ret = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    if (ret != SQLITE_OK)
    {
        printf("Open table failed\n");
        sqlite3_close(db);
        return;
    }
    sqlite3_close(db);
    return;
}

void save_friend(char *sender, char *receiver)
{
    sqlite3 *db;
    char *err_msg = NULL;

    int rc = sqlite3_open("friends.db", &db);

    if (rc != SQLITE_OK)
    {

        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
    char sql[MAX_SQL_SIZE];
    sprintf(sql, "INSERT INTO Friends VALUES('%s', '%s');", sender, receiver);
    printf("In database insert");
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK)
    {

        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);

        return;
    }

    sqlite3_close(db);

    return;
}

void remove_friend(char *sender, char *receiver)
{
    sqlite3 *db;
    char *err_msg = NULL;

    int rc = sqlite3_open("friends.db", &db);

    if (rc != SQLITE_OK)
    {

        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
    char sql[MAX_SQL_SIZE];
    sprintf(sql, "DELETE FROM Friends WHERE (sender='%s' and receiver='%s') or (sender='%s' and receiver='%s');", sender, receiver, receiver, sender);
    printf("In database insert");
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK)
    {

        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);

        return;
    }

    sqlite3_close(db);

    return;
}

int check_friends_table(char *sender, char *receiver)
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char sql[MAX_SQL_SIZE];
    is_existed = 0;

    const char *data = "Callback function ";
    /* Open database */
    rc = sqlite3_open("friends.db", &db);

    if (rc)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    else
    {
        fprintf(stderr, "Opened database successfully\n");
    }

    // sprintf(sql, "SELECT * from friends where sender='%s' and receiver='%s'", username, username);
    sprintf(sql, "SELECT * from friends where (sender='%s' and receiver='%s') or (sender='%s' and receiver='%s')", sender, receiver, receiver, sender);
    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, callback_2, (void *)data, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else
    {
        fprintf(stdout, "Operation done successfully\n");
    }
    sqlite3_close(db);
    return is_existed;
}

int callback_2(void *data, int argc, char **argv, char **azColName)
{
    int i;
    fprintf(stderr, "%s: ", (const char *)data);
    fprintf(stderr, "%d: ", argc);

    if (argc > 0)
    {
        is_existed = 1;
    }
    else
    {
        is_existed = 0;
    }
    return 0;
}

char *get_friends_list(char *username)
{
    num_of_friends = 0;
    sqlite3 *db;
    char *err_msg = 0;
    char sql[MAX_SQL_SIZE];
    int rc = sqlite3_open("friends.db", &db);

    if (rc != SQLITE_OK)
    {

        fprintf(stderr, "Cannot open database: %s\n",
                sqlite3_errmsg(db));
        sqlite3_close(db);

        return NULL;
    }

    sprintf(sql, "SELECT * from friends where sender='%s' or receiver='%s'", username, username);

    rc = sqlite3_exec(db, sql, callback, (void *)username, &err_msg);

    if (rc != SQLITE_OK)
    {

        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);

        return NULL;
    }
    sqlite3_close(db);

    printf("\n%s\n", friends_list);

    return friends_list;
}

int callback(void *data, int argc, char **argv,
             char **azColName)
{
    char str[200];
    char *username = (char *)data;
    printf("username: %s\n", username);
    for (int i = 0; i < argc; i++)
    {

        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        if (strcmp(username, argv[i]) != 0)
        {
            friends[num_of_friends] = argv[i];
            strcat(str, argv[i]);
            strcat(str, " ");
            printf("\nargv[i]: %s\n", friends[num_of_friends]);
            num_of_friends++;
        }
    }
    strcat(str, "\n");
    // printf("\nfriends[0]:%s\n", friends[0]);
    // printf("\nfriends[1]:%s\n", friends[1]);

    sprintf(friends_list, "%d\n", num_of_friends);
    strcat(friends_list, str);

    return 0;
}

void printf_friends_list(char **list)
{
    for (int i = 0; i < num_of_friends; i++)
    {
        printf("%s ...", *list++);
    }
}
