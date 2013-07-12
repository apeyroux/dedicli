#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <jansson.h>
#include <curl/curl.h>

#include "dedicli.h"

/*
 * copie curl rest reslut to variable : http://stackoverflow.com/questions/2329571/c-libcurl-get-output-into-a-string
 */
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

server_t *newsrv(char *token, int serverid) {
	// dedicli
	server_t *srv = NULL;
	os_t *os = NULL;
	location_t *location = NULL;
	// curl
	CURL *curl = NULL;
	CURLcode res;
	char *httpheaderauth = NULL;
	char *url = NULL;
	struct curl_slist *httpheaderparams = NULL;
	size_t httpheaderauthlength = 0; 
	size_t urllength = 0; 
	struct string jsonrest;
	// jansson
	json_t *json = NULL;
	json_error_t jsonError;

	// init party !
	httpheaderauthlength = strlen("Authorization: Bearer ") + strlen(token) + 1;
	urllength = strlen("/server/info/") + strlen(BASE_URL) + 5; // 5 == serverid len + 1 (gruick)
	init_string(&jsonrest);

	if(NULL == (srv = (server_t *) malloc(sizeof(server_t))))
		return NULL;

	if(NULL == (os = (os_t *) malloc(sizeof(os_t))))
		return NULL;

	if(NULL == (location = (location_t *) malloc(sizeof(location_t))))
		return NULL;

	if(NULL == (curl = curl_easy_init())) 
		return NULL;

	if(NULL == (httpheaderauth = (char *) malloc(sizeof(char) * httpheaderauthlength)))
		return NULL;
	
	if(NULL == (httpheaderauth = strcpy(httpheaderauth, "Authorization: Bearer ")))
		return NULL;

	if(NULL == (strncat(httpheaderauth, token, httpheaderauthlength)))
		return NULL;

	if(NULL == (url = (char *) malloc(sizeof(char) * urllength)))
		return NULL;

	if(NULL == (json = malloc(sizeof(json))))
        return NULL;

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
	res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
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

	srv->id = serverid;
	srv->hostname = strdup(json_string_value(json_object_get(json, "hostname")));
	srv->power = strdup(json_string_value(json_object_get(json, "power")));
	// os
	os->name = strdup(json_string_value(json_object_get(json_object_get(json, "os"), "name")));
	os->version = strdup(json_string_value(json_object_get(json_object_get(json, "os"), "version")));
	srv->os = os;
	// location_t
	location->datacenter = strdup(json_string_value(json_object_get(json_object_get(json, "location"), "datacenter")));	
	location->room = strdup(json_string_value(json_object_get(json_object_get(json, "location"), "room")));	
	location->zone = strdup(json_string_value(json_object_get(json_object_get(json, "location"), "zone")));	

	location->line = json_integer_value(json_object_get(json_object_get(json, "location"), "line"));	
	location->rack = json_integer_value(json_object_get(json_object_get(json, "location"), "rack"));	
	location->block = strdup(json_string_value(json_object_get(json_object_get(json, "location"), "block")));	
	location->position = json_integer_value(json_object_get(json_object_get(json, "location"), "position"));	
	srv->location = location;

	// free party ! 
    curl_easy_cleanup(curl);
    curl_slist_free_all(httpheaderparams);
	free(httpheaderauth);
	httpheaderauth = NULL;
	free(url);
	url = NULL;

	return srv;
}

// make freesrv(server_t *srv)
