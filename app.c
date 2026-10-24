#include "app.h"

#include <stdlib.h>
#include <time.h>

#include "log.h"

static struct App* instance = nullptr;

struct App* getApp() {
	if(instance == nullptr) {
		instance = malloc(sizeof(struct App));
		instance->g_currentAccount = nullptr;
		instance->g_accountBuffer = malloc(sizeof(struct Account*) * 1024);
		instance->latestID = 0;

		for(int i = 0; i < 1024; i++) {
			instance->g_accountBuffer[i] = nullptr;
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

	reciver->balance += ammount;

	print_log(getApp()->g_currentAccount->ID, "Attempt ATM Transaction to: %d for ammount of: %d gr. Successful", reciver->ID, ammount);
	return 0;
}