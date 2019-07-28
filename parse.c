#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsmn.h"
#include "program.h"

char *config;


static int jsoneq(const char *json, jsmntok_t *tok, const char *s)
{
	if (tok->type == JSMN_STRING
	    && (int) strlen(s) == tok->end - tok->start
	    && strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

int parse_config(void)
{
	int i;
	int r;
	jsmn_parser p;
	jsmntok_t t[128]; /* We expect no more than 128 tokens */

	jsmn_init(&p);
	r = jsmn_parse(&p, config, strlen(config), t, sizeof(t)/sizeof(t[0]));
	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return 1;
	}

	/* Assume the top-level element is an object */
	if (r < 1 || t[0].type != JSMN_OBJECT) {
		printf("Object expected\n");
		return 1;
	}

	/* Loop over all keys of the root object */
	for (i = 1; i < r; i++) {
		if (jsoneq(config, &t[i], "host") == 0) {
			/* We may use strndup() to fetch string value */
			conf.host = strndup(config + t[i+1].start, t[i+1].end-t[i+1].start);
			i++;
		} else if (jsoneq(config, &t[i], "username") == 0) {
			conf.username = strndup(config + t[i+1].start, t[i+1].end-t[i+1].start);
			i++;
		} else if (jsoneq(config, &t[i], "password") == 0) {
			conf.password = strndup(config + t[i+1].start, t[i+1].end-t[i+1].start);
			i++;
		} else if (jsoneq(config, &t[i], "port") == 0) {
			conf.port = strtol(config + t[i+1].start, NULL, 10);
			i++;
		} else if (jsoneq(config, &t[i], "database") == 0) {
			conf.database = strndup(config + t[i+1].start, t[i+1].end-t[i+1].start);
			i++;
		} else {
			printf("Unexpected key: %.*s\n", t[i].end-t[i].start, config + t[i].start);
		}
	}
	return EXIT_SUCCESS;
}

size_t load_file(char **buffer, char *filename)
{
	FILE *f = fopen(filename, "rb");
	if(f == NULL) {
		fprintf(stderr, "Unable to open file %s\n", filename);
		exit(1);
	}
	
	fseek(f, 0, SEEK_END);
	size_t fsize = ftell(f);
	fseek(f, 0, SEEK_SET);  //same as rewind(f);
	
	*buffer = malloc(fsize + 1);
	fread(*buffer, fsize, 1, f);
	fclose(f);
	
	(*buffer)[fsize] = 0;
	return fsize;
}

	
void dump_config(void)
{
	puts("*** Current Configuration ***");
	printf("Host: %s\n", conf.host);
	printf("Username: %s\n", conf.username);
	printf("Password: %s\n", conf.password);
	printf("Port: %u\n", conf.port);
	printf("database: %s\n", conf.database);
}
