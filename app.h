#ifndef APP_H

#define APP_H

struct Account {
	int ID;
	long long balance;
	const char* first_name;
	const char* last_name;
	unsigned int ssn;
	unsigned long long dob;
	const char* mothers_name;
	const char* id_num;
	unsigned long long creation_date;
};

struct Command {
	const char* cmd;
	void (*handle)(void);
};

struct App {
	struct Account* g_currentAccount;
	int latestID;

	struct Account** g_accountBuffer;
};

struct Account* new_register_account(int* lastest_id, 
	const char* fname, 
	const char* lname, 
	unsigned int ssn, 
	unsigned long long dob,
	const char* mname, 
	const char* id_num);

struct App* getApp();

int generate_transaction(struct Account* sender, struct Account* reciver, long long ammount);
int atm_terminal_alike_transaction(struct Account* reciver, long long ammount);

#endif