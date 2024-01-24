#ifndef __ACCOUNT_MANAGER_H__
#define __ACCOUNT_MANAGER_H__

#define USERNAME_SIZE 256
#define PASSWORD_SIZE 256

typedef struct Account_
{
    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
    int is_signed_in;
    struct Account_ *next;
} Account;

void write_to_file(Account *list);
Account *read_account_list();
Account *find_account(Account *list, char *username);
void add_to_list(Account *list, Account *new_acc);

#endif