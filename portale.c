#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include "program.h"

MYSQL *con;
bool *user_type;
UTENTE logged_user;

MYSQL_ROW row;
MYSQL_FIELD *field;
MYSQL_RES *rs_metadata;
MYSQL_STMT *stmt;
int i;
int num_fields;	// number of columns in result
MYSQL_FIELD *fields; // for result set metadata		
MYSQL_BIND *rs_bind; // for output buffers
int status;


unsigned int convert(char *st) {
  char *x;
  for (x = st ; *x ; x++) {
    if (!isdigit(*x))
      return 0L;
  }
  return (strtoul(st, 0L, 10));
}

bool bool_test_stmt_error(MYSQL_STMT * stmt, int status){
	if (status) {
		fprintf(stderr, "\n                            [Error: %s (errno: %d)]\n\n\n",
			mysql_stmt_error(stmt), mysql_stmt_errno(stmt));
		return 1;
	}
	return 0;
}



bool valid(){
	//login

	MYSQL_BIND ps_params[3];	// input parameter buffers
	unsigned long length[3];	// Can do like that because all IN parameters have the same length
	my_bool is_null[3];		// output value nullability
	

	char *cf = malloc(sizeof(char)*16);
	char *psw = malloc(sizeof(char)*64);
	char *_adm = malloc(sizeof(char)*16);
	int *adm = malloc(sizeof(int));
	


	printf("\e[1;1H\e[2J");
	printf("....................................................................................\n");
	printf("....................................................................................\n");
	printf("...............__ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ _..............\n");
	printf("..............|                                                       |.............\n");
	printf("..............|                     LOGIN UTENTE                      |.............\n");
	printf("..............|__ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ _|.............\n");
	printf("....................................................................................\n");
	printf("....................................................................................\n\n");
	printf("                               CODICE FISCALE : ");
	getInput(16, cf, false);
	printf("\n\n                               PASSWORD : ");
	getInput(64, psw, true);
	printf("\n\n                               AMMINISTRATORE [0 = sì | 1 = no] : ");

	getInput(16, _adm, false);
	*adm = atoi(_adm);
	free(_adm);



	length[0] = strlen(cf);
	length[1] = strlen(psw);
	length[2] = sizeof(int); 
	

	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}
	status = mysql_stmt_prepare(stmt, "CALL validazione_accesso(?,?,?)", strlen("CALL validazione_accesso(?,?,?)"));
	bool_test_stmt_error(stmt, status);


	// initialize parameters
	memset(ps_params, 0, sizeof(ps_params));
	// `validazione_accesso` (in cf char(16), in psw varchar(30), in admin boolean)

	ps_params[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[0].buffer = cf;
	ps_params[0].buffer_length = 16;
	ps_params[0].length = &length[0];
	ps_params[0].is_null = 0;

	ps_params[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[1].buffer = psw;
	ps_params[1].buffer_length = 64;
	ps_params[1].length = &length[1];
	ps_params[1].is_null = 0;

	ps_params[2].buffer_type = MYSQL_TYPE_LONG;
	ps_params[2].buffer = adm;
	ps_params[2].buffer_length = sizeof(int);
	ps_params[2].length = &length[2];
	ps_params[2].is_null = 0; 


	// bind input parameters
	status = mysql_stmt_bind_param(stmt, ps_params);
	bool_test_stmt_error(stmt, status);
	// Run the stored procedure
	status = mysql_stmt_execute(stmt);
	bool err = bool_test_stmt_error(stmt, status);
	mysql_stmt_close(stmt);
	*user_type = *adm;
	for (int c = 0; c < 16; c++){
		logged_user.cf[c] = cf[c];
	}
	free(cf);
	free(psw);
	free(adm);
	return err;

}


void sign(){

	MYSQL_BIND ps_params[11];	// input parameter buffers
	unsigned long length[11];	// Can do like that because all IN parameters have the same length
	my_bool is_null[3];		// output value nullability

	MYSQL_TIME *date, *scad_carta;
	date = malloc(sizeof(MYSQL_TIME));
	scad_carta = malloc(sizeof(MYSQL_TIME));

	printf("\e[1;1H\e[2J");
	printf("....................................................................................\n");
	printf("....................................................................................\n");
	printf("...............__ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ _..............\n");
	printf("..............|                                                       |.............\n");
	printf("..............|                      SIGN IN                          |.............\n");
	printf("..............|__ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ _|.............\n");
	printf("....................................................................................\n");
	printf("....................................................................................\n\n");
	fflush(stdout);

	char *_type = malloc(sizeof(char)*32);
	printf("                    TIPOLOGIA UTENTE [ admin:0  | base:1 ] : ");
	fflush(stdout);
	getInput(32, _type, false);
	int *type = malloc(sizeof(int));
	*type = atoi(_type);
	length[0] = sizeof(int);
	free(_type);

	char *cf = malloc(sizeof(char)*16);
	printf("\n                    CODICE FISCALE : ");
	fflush(stdout);
	getInput(16, cf, false);
	length[1] = 16;
	
	char *nome = malloc(sizeof(char)*32);
	printf("\n                    NOME UTENTE : ");
	fflush(stdout);
	getInput(32, nome, false);
	length[2] = strlen(nome);

	char *cognome = malloc(sizeof(char)*32);
	printf("\n                    COGNOME : ");
	fflush(stdout);
	getInput(32, cognome, false);
	length[3] = strlen(cognome);

	//data di nascita suddivisa in 3 sezioni!!!!--

	char *_anno = malloc(sizeof(char)*32);
	printf("\n                    ANNO DI NASCITA : ");
	fflush(stdout);
	getInput(32, _anno, false);
	date->year = atoi(_anno);
	free(_anno);

	char *_mese = malloc(sizeof(char)*32);
	printf("\n                    MESE DI NASCITA : ");
	fflush(stdout);
	getInput(32, _mese, false);
	date->month = atoi(_mese);
	free(_mese);

	char *_giorno = malloc(sizeof(char)*32);
	printf("\n                    GIORNO DI NASCITA : ");
	fflush(stdout);
	getInput(32, _giorno, false);
	date->day = atoi(_giorno);
	free(_giorno);

	length[4] = sizeof(MYSQL_TIME);

	//-------------------    

	char *birthplace = malloc(sizeof(char)*32);
	printf("\n                    LUOGO DI NASCITA : ");
	fflush(stdout);
	getInput(32, birthplace, false);
	length[5] = strlen(birthplace);

	char *numcarta = malloc(sizeof(char)*16);
	printf("\n                    NUMERO DI CARTA : ");
	fflush(stdout);
	getInput(16, numcarta, false);
	length[6] = 16;

	//scadenza carta suddivisa in 2 sezioni!!!!--

	char *_scadanno = malloc(sizeof(char)*32);
	printf("\n                    ANNO SCADENZA : ");
	fflush(stdout);
	getInput(32, _scadanno, false);
	scad_carta->year = atoi(_scadanno);
	free(_scadanno);

	char *_scadmese = malloc(sizeof(char)*32);
	printf("\n                    MESE SCADENZA : ");
	fflush(stdout);
	getInput(32, _scadmese, false);
	scad_carta->month = atoi(_scadmese);
	free(_scadmese);

	length[7] = sizeof(MYSQL_TIME);

	//-------------------

	char *cvv = malloc(sizeof(char)*3);
	printf("\n                    CVV : ");
	fflush(stdout);
	getInput(3, cvv, false);
	length[8] = 3;

	char *indirizzo = malloc(sizeof(char)*32);
	printf("\n                    INDIRIZZO DI CONSEGNA : ");
	fflush(stdout);
	getInput(32, indirizzo, false);
	length[9] = strlen(indirizzo);

	char *pswd = malloc(sizeof(char)*32);
	printf("\n                    PASSWORD DI ACCESSO : ");
	fflush(stdout);
	getInput(32, pswd, false);
	length[10] = strlen(pswd);

	
	
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}
	status = mysql_stmt_prepare(stmt, "CALL registrazione_utente(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", strlen("CALL registrazione_utente(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"));
	bool_test_stmt_error(stmt, status);

	// initialize parameters
	memset(ps_params, 0, sizeof(ps_params));

	ps_params[0].buffer_type = MYSQL_TYPE_LONG;
	ps_params[0].buffer = type;
	ps_params[0].buffer_length = sizeof(int);
	ps_params[0].length = &length[0];
	ps_params[0].is_null = 0;

	ps_params[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[1].buffer = cf;
	ps_params[1].buffer_length = 16;
	ps_params[1].length = &length[1];
	ps_params[1].is_null = 0;

	ps_params[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[2].buffer = nome;
	ps_params[2].buffer_length = 32;
	ps_params[2].length = &length[2];
	ps_params[2].is_null = 0; 

	ps_params[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[3].buffer = cognome;
	ps_params[3].buffer_length = 32;
	ps_params[3].length = &length[3];
	ps_params[3].is_null = 0;

	ps_params[4].buffer_type = MYSQL_TYPE_DATE;
	ps_params[4].buffer = date;
	ps_params[4].buffer_length = 256;
	ps_params[4].length = &length[4];
	ps_params[4].is_null = 0;

	ps_params[5].buffer_type =  MYSQL_TYPE_VAR_STRING;
	ps_params[5].buffer = birthplace;
	ps_params[5].buffer_length = 32;
	ps_params[5].length = &length[5];
	ps_params[5].is_null = 0; 

	ps_params[6].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[6].buffer = numcarta;
	ps_params[6].buffer_length = 16;
	ps_params[6].length = &length[6];
	ps_params[6].is_null = 0;

	ps_params[7].buffer_type = MYSQL_TYPE_DATE;
	ps_params[7].buffer = scad_carta;
	ps_params[7].buffer_length = 256; //provo a sparare alto. non funziona.
	ps_params[7].length = &length[7];
	ps_params[7].is_null = 0;

	ps_params[8].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[8].buffer = cvv;
	ps_params[8].buffer_length = 3;
	ps_params[8].length = &length[8];
	ps_params[8].is_null = 0; 

	ps_params[9].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[9].buffer = indirizzo;
	ps_params[9].buffer_length = 32;
	ps_params[9].length = &length[9];
	ps_params[9].is_null = 0;

	ps_params[10].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[10].buffer = pswd;
	ps_params[10].buffer_length = 32;
	ps_params[10].length = &length[10];
	ps_params[10].is_null = 0;


	// bind input parameters
	status = mysql_stmt_bind_param(stmt, ps_params);
	bool_test_stmt_error(stmt, status);

	// Run the stored procedure
	status = mysql_stmt_execute(stmt); //segmentation fault
	bool err = bool_test_stmt_error(stmt, status);
	mysql_stmt_close(stmt);
	
	free(type);
	free(cf);
	free(nome);
	free(cognome);
	free(date);
	free(birthplace);
	free(numcarta);
	free(scad_carta);
	free(cvv);
	free(indirizzo);
	free(pswd); 
}  






int log_or_sign(){
	
	int isvalid;
	int *logorsign = malloc(sizeof(int));
	char logOrSign[64];

	portale:

	printf("\e[1;1H\e[2J");
	printf("....................................................................................\n");
	printf("....................................................................................\n");
    printf("...............__ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ _..............\n");
	printf("..............|                                                       |.............\n");
	printf("..............|       PORTALE DI ACCESSO: SISTEMA DI ASTE ONLINE      |.............\n");
	printf("..............|__ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ _|.............\n");
	printf("....................................................................................\n");
	printf("....................................................................................\n");
	printf(".........................  Benvenuto! Cosa vuoi fare?   ............................\n");
	printf("....................................................................................\n");
	printf("....................................................................................\n");
	printf("..............__ ___ __..............................__ ___ __......................\n");
	printf(".............|         |............................|         |.....................\n");
	printf(".............|  LOGIN  |............................|  SIGN   |.....................\n");
	printf(".............|   [0]   |............................|   [1]   |.....................\n");
	printf(".............|__ ___ __|............................|__ ___ __|.....................\n");
	printf("....................................................................................\n");
	printf("....................................................................................\n");
	printf("....................................................................................\n");
	printf(".....................| Inserisci il codice di scelta:  ");
	
	getInput(64, logOrSign , false);
	*logorsign = (int) atoi(logOrSign);

	printf("\n\n");

	switch( *logorsign){

		case 0:
			//login
			isvalid = valid(); //se ritorna 0 allora l'accesso è avvenuto correttamente
			if (isvalid == 0){
				return 0;
			}
			
			if(isvalid != 0) 
				goto portale;
			
			

		case 1:
			//sign in
			
			sign();

			return 1;

		default: 
			
			return 2;
	}
}

	