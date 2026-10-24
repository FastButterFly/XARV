#include "shell.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "app.h"

#include <time.h>

#include "log.h"

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
		&getApp()->latestID,
		first_name,
		last_name,
		ssn,
		dob,
		mother_name,
		id_num
	);

	printf("Account of ID %d was created\n", getApp()->latestID - 1);
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

	getApp()->g_currentAccount = get_account_by_id(id);

	if(getApp()->g_currentAccount == nullptr) {
		printf(" * No account of ID %d was found\n", id);
	}
	
	return;
}

void balance_command_handler() {
	if(getApp()->g_currentAccount == nullptr) {
		printf(" * Select account first\n");
		return;
	}

	printf(" * Balance: %.2f PLN\n", (double)getApp()->g_currentAccount->balance / 100.0f);
	return;
}

void generate_atm_alike_transaction_command_handler() {
	if(getApp()->g_currentAccount == nullptr) {
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

	if(atm_terminal_alike_transaction(getApp()->g_currentAccount, ammount) == 0) {
		printf(" * Transaction was succesful\n");
	} else {
		printf(" * Transaction failed\n");
	}

	return;
}

void generate_transaction_command_handler() {
	if(getApp()->g_currentAccount == nullptr) {
		printf(" * Select account first\n");
		return;
	}

	int target_account_id = 0;
	printf(" * Select target account ID >> ");
	if(scanf("%d", &target_account_id) == 0) {
		printf(" * Not a number");
		return;
	}

	struct Account* targetAccount = get_account_by_id(target_account_id);

	if(targetAccount == nullptr) {
		printf(" * Invalid account ID\n");
		return;
	}

	if(getApp()->g_currentAccount->ID == targetAccount->ID) {
		printf(" * Account IDs must be diffrent from eachother\n");
		return;
	}

	long long ammount = 0;
	printf(" * Preparing transaction between account %d and %d\n", getApp()->g_currentAccount->ID, targetAccount->ID);
	printf(" * Provide ammount [in gr not PLN] >> ");
	scanf("%lld", &ammount);

	if(ammount < 0) {
		printf(" * Values under 0 are not allowed\n");
		return;
	}

	if(ammount > getApp()->g_currentAccount->balance) {
		printf(" * Insufficient balance to begin transaction\n");
		return;
	}

	if(generate_transaction(getApp()->g_currentAccount, targetAccount, ammount) == 0) {
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

	struct Command** buf = malloc(sizeof(struct Command*) * 1024);

	for(int i = 0; i < 1024; i++) {
		buf[i] = nullptr;
	}

	register_commands(&commands_registered, getApp()->g_currentAccount, buf);

	printf(" * Running shell\n");

	char* cmd = malloc(4096);

	for(int i = 0; i < 4096; i++) {
		cmd[i] = 0x0;
	}

	int account_id_shown = 0;

	for(;;) {
		if(getApp()->g_currentAccount == nullptr) {
			account_id_shown = -1;
		} else {
			account_id_shown = getApp()->g_currentAccount->ID;
		}

		printf("@ Target account: %d >> ", account_id_shown);
		scanf("%4095s", cmd);

		handle(cmd, &commands_registered, buf);
	}

	return false;
}