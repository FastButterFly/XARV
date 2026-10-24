#include "app.h"

#include <stdlib.h>
#include <time.h>

#include <stdio.h>

#include "log.h"

#include "sqlite3.h"

static struct App* instance = nullptr;
static sqlite3* database = nullptr;

bool register_account_from_database_index(int id) {
	if(database == nullptr) return false;
	sqlite3_stmt* v = nullptr;
	sqlite3_prepare_v2(database, "SELECT * FROM accounts WHERE id = ?", -1, &v, NULL);
	sqlite3_bind_int(v, 1, id);
	if(sqlite3_step(v) == SQLITE_ROW) {
		struct Account* ptr = malloc(sizeof(struct Account));
		ptr->ID = sqlite3_column_int(v, 0);
		ptr->balance = sqlite3_column_int64(v, 1);

		for(int i=0;i<1024;i++) {
			if(getApp()->g_accountBuffer[i] == nullptr) {
				getApp()->g_accountBuffer[i] = ptr;
			}
		}

		ptr->first_name = (const char*)sqlite3_column_text(v, 2);
		ptr->last_name = (const char*)sqlite3_column_text(v, 3);
		ptr->ssn = sqlite3_column_int(v, 4);
		ptr->dob = sqlite3_column_int64(v, 5);
		ptr->mothers_name = (const char*)sqlite3_column_text(v, 6);
		ptr->id_num = (const char*)sqlite3_column_text(v, 7);
		ptr->creation_date = sqlite3_column_int64(v, 8);

		int print_id = -1;

		if(getApp()->g_currentAccount != nullptr) {
			print_id = getApp()->g_currentAccount->ID;
		}

		print_log(print_id, "New account registered with (FROM DB):\n\tID: %d\n\tFirst name: %s\n\tLast name: %s\n\tSSN: %d\n\tDOB: %lld\n\tMother Name: %s\n\tID Number: %s\n", 
			ptr->ID, 
			ptr->first_name, 
			ptr->last_name, 
			ptr->ssn, 
			ptr->dob,
			ptr->mothers_name,
			ptr->id_num);

		getApp()->latestID += 1;
	} else {
		sqlite3_finalize(v);
		return false;
	}

	sqlite3_finalize(v);
	return true;
}

struct Account* get_account_by_id(int id) {
	if(id > getApp()->latestID) return nullptr;
	for(int i =0; i < getApp()->latestID; i++) {
		if(getApp()->g_accountBuffer[i]->ID == id) {
			return getApp()->g_accountBuffer[i];
		}
	}

	if(!register_account_from_database_index(id)) {
		return nullptr;
	} else {
		for(int i =0; i < getApp()->latestID; i++) {
			if(getApp()->g_accountBuffer[i]->ID == id) {
				return getApp()->g_accountBuffer[i];
			}
		}
	}

	return nullptr;
}

struct App* getApp() {
	if(instance == nullptr) {
		instance = malloc(sizeof(struct App));
		instance->g_currentAccount = nullptr;
		instance->g_accountBuffer = malloc(sizeof(struct Account*) * 1024);
		instance->latestID = 0;

		for(int i = 0; i < 1024; i++) {
			instance->g_accountBuffer[i] = nullptr;
		}

		if(sqlite3_open("db/data.db", &database) != SQLITE_OK) {
			print_log(-2, "Failed to open database file");
			printf("* Failed to open database file. Changes will not be saved!");
			database = nullptr;
		} else {
			sqlite3_exec(database, "CREATE TABLE IF NOT EXISTS accounts (id INTEGER PRIMARY KEY, balance INTEGER, FIRST_NAME TEXT, LAST_NAME TEXT, SSN NUMBER, DOB NUMBER, MOTHERS_NAME TEXT, ID_NUM TEXT, CREATION_DATE NUMBER);", nullptr, nullptr, nullptr);
			sqlite3_stmt* v = nullptr;
			sqlite3_prepare_v2(database, "SELECT max(id) FROM accounts;", -1, &v, NULL);
			if(sqlite3_step(v) == SQLITE_ROW) {
				if(sqlite3_column_type(v, 0) != SQLITE_NULL) {
					getApp()->latestID = sqlite3_column_int(v, 0);
				}
			}

			sqlite3_finalize(v);
		}
	}

	return instance;
}

struct Account* new_register_account(int* lastest_id, 
	const char* fname, 
	const char* lname, 
	unsigned int ssn, 
	unsigned long long dob,
	const char* mname, 
	const char* id_num) 
{
	struct Account* ptr = malloc(sizeof(struct Account));
	ptr->balance = 0;
	ptr->ID = (*lastest_id);
	if(getApp()->g_accountBuffer[(*lastest_id)] == nullptr) {
		getApp()->g_accountBuffer[(*lastest_id)] = ptr;
	}
	(*lastest_id) += 1;
	ptr->first_name = fname;
	ptr->last_name = lname;
	ptr->ssn = ssn;
	ptr->dob = dob;
	ptr->mothers_name = mname;
	ptr->id_num = id_num;
	ptr->creation_date = (unsigned long long)time(NULL);

	int print_id = -1;

	if(getApp()->g_currentAccount != nullptr) {
		print_id = getApp()->g_currentAccount->ID;
	}

	print_log(print_id, "New account registered with:\n\tID: %d\n\tFirst name: %s\n\tLast name: %s\n\tSSN: %d\n\tDOB: %lld\n\tMother Name: %s\n\tID Number: %s\n", 
		ptr->ID, 
		fname, 
		lname, 
		ssn, 
		dob,
		mname,
		id_num);

	if(database != nullptr) {
		sqlite3_stmt* v = nullptr;
		sqlite3_prepare_v2(database, "INSERT INTO accounts (id, balance, FIRST_NAME, LAST_NAME, SSN, DOB, MOTHERS_NAME, ID_NUM, CREATION_DATE) values (?, ?, ?, ?, ?, ?, ?, ?, ?)", -1, &v, NULL);
		sqlite3_bind_int(v, 1, ptr->ID);
		sqlite3_bind_int64(v, 2, ptr->balance);
		sqlite3_bind_text(v, 3, ptr->first_name, -1, SQLITE_TRANSIENT);
		sqlite3_bind_text(v, 4, ptr->last_name, -1, SQLITE_TRANSIENT);
		sqlite3_bind_int(v, 5, ptr->ssn);
		sqlite3_bind_int64(v, 6, ptr->dob);
		sqlite3_bind_text(v, 7, ptr->mothers_name, -1, SQLITE_TRANSIENT);
		sqlite3_bind_text(v, 8, ptr->id_num, -1, SQLITE_TRANSIENT);
		sqlite3_bind_int64(v, 9, ptr->creation_date);

		if(sqlite3_step(v) != SQLITE_DONE) {
			printf(" * Database error: %s\n", sqlite3_errmsg(database));
		}
		sqlite3_finalize(v);
	}

	return ptr;
}

int generate_transaction(struct Account* sender, struct Account* reciver, long long ammount) {
	print_log(getApp()->g_currentAccount->ID, "Attempt Transaction between accounts: %d %d for ammount of: %d gr", getApp()->g_currentAccount->ID, reciver->ID, ammount);

	if(ammount < 0) {
		print_log(getApp()->g_currentAccount->ID, "Attempt Transaction between accounts: %d %d for ammount of: %d gr. Failed: Ammount is 0 or under", getApp()->g_currentAccount->ID, reciver->ID, ammount);
		return 1;
	}

	if(ammount > sender->balance) {
		print_log(getApp()->g_currentAccount->ID, "Attempt Transaction between accounts: %d %d for ammount of: %d gr. Failed: Balance too low", getApp()->g_currentAccount->ID, reciver->ID, ammount);
		return 2;
	}

	if(sender->ID == reciver->ID) {
		print_log(getApp()->g_currentAccount->ID, "Attempt Transaction between accounts: %d %d for ammount of: %d gr. Failed: Same IDs", getApp()->g_currentAccount->ID, reciver->ID, ammount);
		return 3;
	}

	if(database != nullptr) {
		sqlite3_stmt* v = nullptr;
		sqlite3_prepare_v2(database, "UPDATE accounts SET balance = ? WHERE id = ?", -1, &v, NULL);
		sqlite3_bind_int64(v, 1, sender->balance - ammount);
		sqlite3_bind_int(v, 2, sender->ID);

		if(sqlite3_step(v) != SQLITE_DONE) {
			printf(" * Database error: %s\n", sqlite3_errmsg(database));
			return 4;
		}
		sqlite3_finalize(v);

		sqlite3_stmt* v2 = nullptr;
		sqlite3_prepare_v2(database, "UPDATE accounts SET balance = ? WHERE id = ?", -1, &v2, NULL);
		sqlite3_bind_int64(v2, 1, reciver->balance + ammount);
		sqlite3_bind_int(v2, 2, reciver->ID);

		if(sqlite3_step(v2) != SQLITE_DONE) {
			printf(" * Database error: %s\n", sqlite3_errmsg(database));
			return 4;
		}
		sqlite3_finalize(v2);
	}

	sender->balance -= ammount;
	reciver->balance += ammount;

	print_log(getApp()->g_currentAccount->ID, "Attempt Transaction between accounts: %d %d for ammount of: %d gr. Successful", getApp()->g_currentAccount->ID, reciver->ID, ammount);

	return 0;
}

int atm_terminal_alike_transaction(struct Account* reciver, long long ammount) {
	print_log(getApp()->g_currentAccount->ID, "Attempt ATM Transaction to: %d for ammount of: %d gr", reciver->ID, ammount);

	if(ammount < 0) {
		print_log(getApp()->g_currentAccount->ID, "Attempt ATM Transaction to: %d for ammount of: %d gr. Failed: Ammounnt is 0 or under", reciver->ID, ammount);
		return 1;
	}

		sqlite3_stmt* v2 = nullptr;
		sqlite3_prepare_v2(database, "UPDATE accounts SET balance = ? WHERE id = ?", -1, &v2, NULL);
		sqlite3_bind_int64(v2, 1, reciver->balance + ammount);
		sqlite3_bind_int(v2, 2, reciver->ID);

		if(sqlite3_step(v2) != SQLITE_DONE) {
			printf(" * Database error: %s\n", sqlite3_errmsg(database));
			return 2;
		}
		sqlite3_finalize(v2);

	reciver->balance += ammount;

	print_log(getApp()->g_currentAccount->ID, "Attempt ATM Transaction to: %d for ammount of: %d gr. Successful", reciver->ID, ammount);
	return 0;
}