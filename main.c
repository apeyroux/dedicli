#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "dedicli.h"

void usage() {
	fprintf(stdout, "usage: -t tocken -s srvid [-i (information)]\n");
	exit(EXIT_SUCCESS);
}

int main(int ac, char **av) {

	int ch = 0;
	int flaginfo = 0;
	unsigned long serverid = 0;
	char *tocken = NULL;

	while(-1 != (ch = getopt(ac, av, "t:s:i"))) {
		switch(ch) {
			case 't':
				tocken = strdup(optarg);
				break;
			case 's':
				serverid = strtoul(optarg, NULL, 10);
				break;
			case 'i':
				flaginfo = 1;
				break;
			default:
				usage();
		}
	}

	if(NULL == tocken)
		usage();

	if(flaginfo && serverid) {
		fprintf(stdout, "%s\n.: %ld information :.\n%s\n", LINE_SEPARATOR, serverid, LINE_SEPARATOR);
		server_t *srv = NULL;
		if(NULL == (srv = newsrv(tocken, serverid))) {
			fprintf(stderr, "error: can't init srv pointer :(\n");
			return EXIT_FAILURE;
		}
		fprintf(stdout, "hostname: %s\npower: %s\n", srv->hostname, srv->power);
	}

	return EXIT_SUCCESS;
}
