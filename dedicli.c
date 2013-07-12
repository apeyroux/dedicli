#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <jansson.h>
#include <curl/curl.h>

#define BASE_URL "https://api.online.net/api/v1"
#define LINE_SEPARATOR "----------------------------------------"

struct os_s {
	char *name;
	char *version;
};

typedef struct os_s os_t;

struct location_s {
	char *datacenter;
	char *room;
	char *zone;
	int line;
	int rack;
	char *block;
	int position;
};

typedef struct location_s location_t;

struct network_s {
	char **ip;
	char **private;
	char **ipfo;
};

typedef struct network_s network_t;

enum privilege { OWNER, TECH };
struct privileges_s {
	enum privilege **privileges;	
};

typedef struct privilege_s privilege_t;

struct rescue_credentials_s {
	char *login;
	char *password;
	char *protocol;
	char *ip;
};

typedef struct rescue_credentials_s rescue_credentials_t;

struct server_s {
	int id;
	const char *hostname;
	os_t *os;
	char *power;
	char *boot_mode;
	// date last_reboot
	bool anti_ddos;
	bool proactive_monitoring;
	char *support;
	char *abuse;
	location_t *location;
	network_t *network;
	privilege_t *privileges;
	rescue_credentials_t *rescue_credentials;
};

typedef struct server_s server_t;

/*
 * === start prog ===
 */

/*
 * copie curl rest reslut to variable : http://stackoverflow.com/questions/2329571/c-libcurl-get-output-into-a-string
 */
struct string {
	char *ptr;
	size_t len;
};

void init_string(struct string *s) {
	s->len = 0;
	s->ptr = malloc(s->len+1);
	if (s->ptr == NULL) {
		fprintf(stderr, "malloc() failed\n");
		exit(EXIT_FAILURE);
	}
	s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s)
{
	size_t new_len = s->len + size*nmemb;
	s->ptr = realloc(s->ptr, new_len+1);
	if (s->ptr == NULL) {
		fprintf(stderr, "realloc() failed\n");
		exit(EXIT_FAILURE);
	}
	memcpy(s->ptr+s->len, ptr, size*nmemb);
	s->ptr[new_len] = '\0';
	s->len = new_len;

	return size*nmemb;
}

server_t *newsrv(char *tocken, int serverid) {
	server_t *srv = NULL;
	char *httpheaderauth = NULL;
	char *url = NULL;
	size_t httpheaderauthlength = strlen("Authorization: Bearer ") + strlen(tocken) + 1;
	size_t urllength = strlen("/server/info/") + strlen(BASE_URL) + 5; // 5 == serverid len + 1
	struct string jsonrest;
	// jansson
	json_t *json = NULL;
	json_error_t jsonError;

	// curl
	CURL *curl = NULL;
	struct curl_slist *httpheaderparams = NULL;
	CURLcode res;

	// init party !
	if(NULL == (srv = (server_t *) malloc(sizeof(server_t))))
		return NULL;

	if(NULL == (curl = curl_easy_init())) 
		return NULL;

	if(NULL == (httpheaderauth = (char *) malloc(sizeof(char) * httpheaderauthlength)))
		return NULL;
	
	if(NULL == (httpheaderauth = strcpy(httpheaderauth, "Authorization: Bearer ")))
		return NULL;

	if(NULL == (strncat(httpheaderauth, tocken, httpheaderauthlength)))
		return NULL;

	if(NULL == (url = (char *) malloc(sizeof(char) * urllength)))
		return NULL;

	if(NULL == (json = malloc(sizeof(json))))
        return NULL;

	init_string(&jsonrest);

	// make url
	sprintf(url, "%s%s%d", BASE_URL, "/server/info/", serverid);

	curl_easy_setopt(curl, CURLOPT_URL, BASE_URL);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	// ajout des params d'auth dans les headers http
	if(NULL == (httpheaderparams = curl_slist_append(httpheaderparams, httpheaderauth)))
		return NULL;

	// fprintf(stdout, "%s - %s\n", url, httpheaderauth);

	res = curl_easy_setopt(curl, CURLOPT_URL, url);
	res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, httpheaderparams);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
	res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &jsonrest);

	res = curl_easy_perform(curl);

	if(res != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		return NULL;
	}

	// fprintf(stdout, "json : %s\n", jsonrest.ptr);

	// json_t *json_loads(const char *input, size_t flags, json_error_t *error)
    json = json_loads(jsonrest.ptr, 0, &jsonError);
	if(!json) {
		fprintf(stderr, "error: %s line : %d column : %d [%s]\n", jsonError.source, 
                                                                    jsonError.line, 
                                                                    jsonError.column, 
                                                                    jsonError.text);
		return NULL;
	}

	srv->hostname = strdup(json_string_value(json_object_get(json, "hostname")));
	srv->power = strdup(json_string_value(json_object_get(json, "power")));

	// free party ! 
    curl_easy_cleanup(curl);
    curl_slist_free_all(httpheaderparams);
	free(httpheaderauth);
	httpheaderauth = NULL;
	free(url);
	url = NULL;

	return srv;
}

server_t *newsrv(char *tocken, int serverid);

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
