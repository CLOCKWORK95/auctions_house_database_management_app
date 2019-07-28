#pragma once

#include <stdbool.h>

typedef struct{
	char cf[16];
} UTENTE;

struct configuration {
	char *host;
	char *username;
	char *password;
	unsigned int port;
	char *database;
};

extern struct configuration conf;
extern char *config;



extern int parse_config();
extern size_t load_file(char **buffer, char *filename);
extern void dump_config(void);
extern char *getInput(unsigned int lung, char *stringa, bool hide);
extern bool yesOrNo(char *domanda, char yes, char no, bool predef, bool insensitive);
