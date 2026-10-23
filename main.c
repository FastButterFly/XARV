#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shell.h"

int main() {
	printf("ZARV - Banking system.\nCreated by: F4stbutterfly\n\n");

	printf(" * User name >> ");

	char* buf = malloc(4096);

	scanf("%4095s", buf);

	printf(" * Password >> ");

	char* passwd = malloc(4096);

	scanf("%4095s", passwd);

	if(strcmp(buf, "admin") != 0 || strcmp(passwd, "admin") != 0) {
		printf(" * Invalid credentials. Access denied....\n");
		return 0;
	}

	free(buf);
	free(passwd);

	printf(" * Mouting shell\n\n");

	run_shell();

	printf(" * Shell unmounted ending user session\n");

	return 0;
}