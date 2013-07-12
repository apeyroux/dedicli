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
		server_t *srv = NULL;
		if(NULL == (srv = newsrv(tocken, serverid))) {
			fprintf(stderr, "error: can't init srv pointer :(\n");
			return EXIT_FAILURE;
		}
		fprintf(stdout, "--- SRV Information ---\nid: %d\nhostname: %s\npower: %s\nos: %s\nversion: %s\n", srv->id, srv->hostname, srv->power,
															srv->os->name,
															srv->os->version);
		fprintf(stdout, "--- DC location ---\nroom:%s zone:%s line:%d rack:%d block:%s position:%d\n", 
																			srv->location->room, 
																			srv->location->zone,
																			srv->location->line,
																			srv->location->rack,
																			srv->location->block,
																			srv->location->position);
	}

	return EXIT_SUCCESS;
}
