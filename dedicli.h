#include <stdbool.h>

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

struct string {
	char *ptr;
	size_t len;
};


void init_string(struct string *s);
size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s);
server_t *newsrv(char *token, int serverid);
