#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include "program.h"


extern bool *user_type;
extern MYSQL *con;
struct configuration conf;


UTENTE logged_user;


void menu_utente_base();

void menu_utente_admin();

int log_or_sign();



static void test_error(MYSQL * con, int status){
	if (status) {
		fprintf(stderr, "Error: %s (errno: %d)\n", mysql_error(con),
			mysql_errno(con));
		exit(1);
	}
}



int main(int argc, char **argv){
	
	user_type = malloc(sizeof(bool));
	con = mysql_init(NULL);
	
	
	//Inizializzazione della prima connessione (phantom user), per connettersi al db -> Login table.

	load_file(&config, "config.json");
	parse_config();
	if (con == NULL) {
		fprintf(stderr, "Initilization error: %s\n", mysql_error(con));
		exit(1);
	}
	if (mysql_real_connect(con, conf.host, conf.username, conf.password, conf.database, conf.port, NULL, 0) == NULL) {
		fprintf(stderr, "Connection error: %s\n", mysql_error(con));
		exit(1);
	}



	//Operazioni sul Portale d'Accesso: Login / Sign in

	int logOrSign;
	portal: logOrSign = log_or_sign();

	switch((int)logOrSign){
		case 0: //login
			if (*user_type == 0){ //login -> admin
				//cambio utente: accesso al database per l'utente amministratore
				if(mysql_change_user(con,"utenteamm","utenteamm","sistemaaste") != 0){
					//inserire gestione errore connessione
				}
				menu_utente_admin();
			} else if (*user_type == 1){ // login -> standard user
				//cambio utente: accesso al database per l'utente base
				if(mysql_change_user(con,"utentebase","utentebase","sistemaaste") != 0){
					//inserire gestione errore connessione
				}
				menu_utente_base();
			}

			goto portal;
		
		case 1: //sign in
			printf("\nUtente Registrato!");
			goto portal;


		default: //quit
			break;

	}


	

	mysql_close(con);
	free(user_type);
	return 0;
}
