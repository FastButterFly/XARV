#include "shell.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <time.h>

#include "log.h"

static struct Account* g_currentAccount;
static int latestID = 0;

static struct Account** g_accountBuffer;

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

	if(g_accountBuffer[(*lastest_id)] == nullptr) {
		g_accountBuffer[(*lastest_id)] = ptr;
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

	if(g_currentAccount != nullptr) {
		print_id = g_currentAccount->ID;
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

void create_account_command_handler() {
	printf(" * Preparing.....\n");
	char* first_name = malloc(sizeof(char) * 1024);
	char* last_name = malloc(sizeof(char) * 1024);
	unsigned int ssn = 0;
	unsigned long long dob = 0;
	char* mother_name = malloc(sizeof(char) * 1024);
	char* id_num = malloc(sizeof(char) * 1024);

	printf(" * First Name >> ");
	if(scanf("%1023s", first_name) == 0) {
		printf(" * Invalid value. \n");
		return;
	}
	printf(" * Last Name >> ");
	if(scanf("%1023s", last_name) == 0) {
		printf(" * Invalid value. \n");
		return;
	}
	printf(" * SSN >> ");
	if(scanf("%d", &ssn) == 0) {
		printf(" * Invalid value. \n");
		return;
	}
	printf(" * Date of birth [UNIX Timestamp] >> ");
	if(scanf("%lld", &dob) == 0) {
		printf(" * Invalid value. \n");
		return;
	}
	printf(" * Mother name >> ");
	if(scanf("%1023s", mother_name) == 0) {
		printf(" * Invalid value. \n");
		return;
	}
	printf(" * ID Card num >> ");
	if(scanf("%1023s", id_num) == 0) {
		printf(" * Invalid value. \n");
		return;
	}

	new_register_account(
		&latestID,
		first_name,
		last_name,
		ssn,
		dob,
		mother_name,
		id_num
	);

	printf("Account of ID %d was created\n", latestID - 1);
}

struct Command* new_command(const char* cmd, void (*handle)(void)) {
	struct Command* ptr = malloc(sizeof(struct Command));
	ptr->cmd = cmd;
	ptr->handle = handle;
	return ptr;
}

void register_command(struct Command* cmd, struct Command** buffer, int* current_i, int max_i) {
	printf(" * Registering command: %s\n", cmd->cmd);
	if((*current_i) >= max_i - 1) {
		printf(" * Too many commands registered. Operation Dropped.\n");
		return;
	}

	buffer[*current_i] = cmd;
	*current_i += 1;
}

void set_account_command() {
	int id = 0;

	printf(" * Please enter account ID >> ");
	if(scanf("%d", &id) == 0) {
		printf(" * Not a number!\n");
		return;
	}

	for (int i = 0; i < latestID; i++) {
		if(g_accountBuffer[i]->ID == id) {
			g_currentAccount = g_accountBuffer[i];
			return;
		}
	}

	printf(" * No account of ID %d was found\n", id);
}

void balance_command_handler() {
	if(g_currentAccount == nullptr) {
		printf(" * Select account first\n");
		return;
	}

	printf(" * Balance: %.2f PLN\n", (double)g_currentAccount->balance / 100.0f);
	return;
}

bool generate_transaction(struct Account* sender, struct Account* reciver, long long ammount) {

	print_log(g_currentAccount->ID, "Attempt Transaction between accounts: %d %d for ammount of: %d gr", g_currentAccount->ID, reciver->ID, ammount);

	if(ammount < 0) {
		print_log(g_currentAccount->ID, "Attempt Transaction between accounts: %d %d for ammount of: %d gr. Failed: Ammount is 0 or under", g_currentAccount->ID, reciver->ID, ammount);
		return false;
	}

	if(ammount > sender->balance) {
		print_log(g_currentAccount->ID, "Attempt Transaction between accounts: %d %d for ammount of: %d gr. Failed: Balance too low", g_currentAccount->ID, reciver->ID, ammount);
		return false;
	}

	if(sender->ID == reciver->ID) {
		print_log(g_currentAccount->ID, "Attempt Transaction between accounts: %d %d for ammount of: %d gr. Failed: Same IDs", g_currentAccount->ID, reciver->ID, ammount);
		return false;
	}

	sender->balance -= ammount;
	reciver->balance += ammount;

	print_log(g_currentAccount->ID, "Attempt Transaction between accounts: %d %d for ammount of: %d gr. Successful", g_currentAccount->ID, reciver->ID, ammount);

	return true;
}

bool atm_terminal_alike_transaction(struct Account* reciver, long long ammount) {

	print_log(g_currentAccount->ID, "Attempt ATM Transaction to: %d for ammount of: %d gr", reciver->ID, ammount);

	if(ammount < 0) {
		print_log(g_currentAccount->ID, "Attempt ATM Transaction to: %d for ammount of: %d gr. Failed: Ammounnt is 0 or under", reciver->ID, ammount);
		return false;
	}

	reciver->balance += ammount;

	print_log(g_currentAccount->ID, "Attempt ATM Transaction to: %d for ammount of: %d gr. Successful", reciver->ID, ammount);
	return true;
}

void generate_atm_alike_transaction_command_handler() {
	if(g_currentAccount == nullptr) {
		printf(" * Select account first\n");
		return;
	}

	long long ammount = 0;
	printf( " * Select ammount to generate in transaction [in gr not PLN] >> ");
	if(scanf("%lld", &ammount) == 0) {
		printf("Not a number\n");
		return;
	}

	if(ammount < 0) {
		printf(" * Values below 0 are not allowed\n");
		return;
	}

	if(atm_terminal_alike_transaction(g_currentAccount, ammount)) {
		printf(" * Transaction was succesful\n");
	} else {
		printf(" * Transaction failed\n");
	}

	return;
}

void generate_transaction_command_handler() {
	if(g_currentAccount == nullptr) {
		printf(" * Select account first\n");
		return;
	}

	int target_account_id = 0;
	printf(" * Select target account ID >> ");
	if(scanf("%d", &target_account_id) == 0) {
		printf(" * Not a number");
		return;
	}

	struct Account* targetAccount = nullptr;

	for(int i = 0; i < latestID; i++) {
		if(g_accountBuffer[i]->ID == target_account_id) {
			targetAccount = g_accountBuffer[i];
		}
	}

	if(targetAccount == nullptr) {
		printf(" * Invalid account ID\n");
		return;
	}

	if(g_currentAccount->ID == targetAccount->ID) {
		printf(" * Account IDs must be diffrent from eachother\n");
		return;
	}

	long long ammount = 0;
	printf(" * Preparing transaction between account %d and %d\n", g_currentAccount->ID, targetAccount->ID);
	printf(" * Provide ammount [in gr not PLN] >> ");
	scanf("%lld", &ammount);

	if(ammount < 0) {
		printf(" * Values under 0 are not allowed\n");
		return;
	}

	if(ammount > g_currentAccount->balance) {
		printf(" * Insufficient balance to begin transaction\n");
		return;
	}

	if(generate_transaction(g_currentAccount, targetAccount, ammount)) {
		printf(" * Transaction succesful\n");
	} else {
		printf(" * Transaction failed\n");
	}
	return;
}

void register_commands(int* var, struct Account* currentAccount, struct Command** buf) {
	struct Command* target_account_command = new_command("target", &set_account_command);
	struct Command* create_account_command = new_command("create", &create_account_command_handler);
	struct Command* ball_account_command = new_command("balance", &balance_command_handler);
	struct Command* trans_account_command = new_command("run-transaction", &generate_transaction_command_handler);
	struct Command* atm_account_command = new_command("gen-atm-alike-transaction", &generate_atm_alike_transaction_command_handler);
	register_command(target_account_command, buf, var, 1024);
	register_command(create_account_command, buf, var, 1024);
	register_command(ball_account_command, buf, var, 1024);
	register_command(trans_account_command, buf, var, 1024);
	register_command(atm_account_command, buf, var, 1024);
}

void handle(const char* const cmd, int* val, struct Command** buf) {
	for(int i = 0; i < (*val); i++) {
		if(strcmp(cmd, buf[i]->cmd) == 0) {
			buf[i]->handle();
			return;
		}
	}

	printf(" * Invalid Command\n");
	return;
}

bool run_shell() {
	int commands_registered = 0;
	g_currentAccount = nullptr;

	struct Command** buf = malloc(sizeof(struct Command*) * 1024);

	g_accountBuffer = malloc(sizeof(struct Account*) * 1024);

	for(int i = 0; i < 1024; i++) {
		buf[i] = nullptr;
	}

	for(int i = 0; i < 1024; i++) {
		g_accountBuffer[i] = nullptr;
	}

	register_commands(&commands_registered, g_currentAccount, buf);

	printf(" * Running shell\n");

	char* cmd = malloc(4096);

	for(int i = 0; i < 4096; i++) {
		cmd[i] = 0x0;
	}

	int account_id_shown = 0;

	for(;;) {
		if(g_currentAccount == nullptr) {
			account_id_shown = -1;
		} else {
			account_id_shown = g_currentAccount->ID;
		}

		printf("@ Target account: %d >> ", account_id_shown);
		scanf("%4095s", cmd);

		handle(cmd, &commands_registered, buf);
	}

	return false;
}