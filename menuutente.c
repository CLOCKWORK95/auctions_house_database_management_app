#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include "program.h"


bool *user_type;
MYSQL *con;
UTENTE logged_user;

// Inizializzazione delle variabili per la gestione delle stored procedures
MYSQL_RES *result;
MYSQL_ROW row;
MYSQL_FIELD *field;
MYSQL_RES *rs_metadata;
my_bool is_null[3]; // output value nullability
int i;
int num_fields;	// number of columns in result
MYSQL_FIELD *fields; // for result set metadata		
MYSQL_BIND *rs_bind; // for output buffers




//gestione segnali di errori dal dalle procedure (sqlstate...)
static int test_stmt_error(MYSQL_STMT * stmt, int status){
	if (status) {
		fprintf(stderr, "Errore: %s (errno: %d)\n",
			mysql_stmt_error(stmt), mysql_stmt_errno(stmt));
		return(1);
	}
	else{
		return 0;
	}
}


//Ciclo di stampa degli output di una stored procedure, comune a tutte le operazioni.
void procedure_output(MYSQL_STMT * stmt, int status){

	MYSQL_TIME *date;

    // This is a general piece of code, to show how to deal with
	// generic stored procedures. A lighter version tailored to
	// a single specific stored procedure is shown below, for
	// the second stored procedure used in the application
	
	// process results until there are no more
	do {
		/* the column count is > 0 if there is a result set */
		/* 0 if the result is only the final status packet */
		num_fields = mysql_stmt_field_count(stmt);

		if (num_fields > 0) {
			
			// what kind of result set is this?
			if (con->server_status & SERVER_PS_OUT_PARAMS)
				//gestione della stampa parametri I/O stored procedure: se non li voglio allora break;
				printf("The stored procedure has returned output in OUT/INOUT parameter(s)\n");

			// Get information about the outcome of the stored procedure
			rs_metadata = mysql_stmt_result_metadata(stmt);
			if(test_stmt_error(stmt, rs_metadata == NULL) == 1){
				printf("\nPremi invio per continuare...\n");
				while(getchar()!= '\n'){}
				return;
			} 
			
			// Retrieve the fields associated with OUT/INOUT parameters
			fields = mysql_fetch_fields(rs_metadata);
			rs_bind = (MYSQL_BIND *)malloc(sizeof(MYSQL_BIND) * num_fields);
			if (!rs_bind) {
				printf("Cannot allocate output buffers\n");
				exit(1);
			}
			//initialize result set_bind structure allocated as null (or zero)
			memset(rs_bind, 0, sizeof(MYSQL_BIND) * num_fields);

			// set up and bind result set output buffers
			for (i = 0; i < num_fields; ++i) {
				
				//void *memory = malloc(fields[i].length);
				void *memory = malloc(256); //numero grosso di byte per contenere chiunque dei campi di una tabella!
				
				rs_bind[i].buffer_type = fields[i].type;
				rs_bind[i].is_null = &is_null[i];

				switch (fields[i].type) {
					//associazione del type della colonna i al corrispettivo tipo C (parsing)
					//inoltre dice dove va a finire l'elemento di ogni riga corrispondente a quella colonna:
					// cio implica che se due colonne sono di tipo stringa punteranno allo stesso indirizzo di memoria (&token[0])
					//bisogna risolverlo!

					case MYSQL_TYPE_VAR_STRING:
						rs_bind[i].buffer = memory;
						rs_bind[i].buffer_length = fields[i].length;
						break;
					
					case MYSQL_TYPE_TINY:
						rs_bind[i].buffer = memory;
						rs_bind[i].buffer_length = fields[i].length;
						break;
					
					
					case MYSQL_TYPE_STRING:
						rs_bind[i].buffer = memory;
						rs_bind[i].buffer_length = fields[i].length;
						break;

					case MYSQL_TYPE_LONG:
						rs_bind[i].buffer = memory;
						rs_bind[i].buffer_length = fields[i].length;
						break;
					
					case MYSQL_TYPE_TIMESTAMP:
						rs_bind[i].buffer = memory;
						rs_bind[i].buffer_length = fields[i].length;
						break;

					case MYSQL_TYPE_DATETIME:
						rs_bind[i].buffer = memory;
						rs_bind[i].buffer_length = fields[i].length;
						break;

					case MYSQL_TYPE_BLOB:
						rs_bind[i].buffer = memory;
						rs_bind[i].buffer_length = fields[i].length;
						break;
					
					
					case MYSQL_TYPE_SHORT:
						rs_bind[i].buffer = memory;
						rs_bind[i].buffer_length = fields[i].length;
						break;
					
					case MYSQL_TYPE_NEWDECIMAL:
						rs_bind[i].buffer = memory;
						rs_bind[i].buffer_length = fields[i].length;
						break; 
					
					case MYSQL_TYPE_FLOAT:
						rs_bind[i].buffer = memory;
						rs_bind[i].buffer_length = fields[i].length;
						break; 

					case MYSQL_TYPE_DOUBLE:
						rs_bind[i].buffer = memory;
						rs_bind[i].buffer_length = fields[i].length;
						break; 
				

					default:
						fprintf(stderr,	"ERROR: unexpected type column %d: %d.\n", i,fields[i].type);
						exit(1);
				}


			}


			status = mysql_stmt_bind_result(stmt, rs_bind);
			if(test_stmt_error(stmt, status) == 1){
				printf("\nPremi invio per continuare...\n");
				while(getchar()!= '\n'){}
				return;
			} 

			for (i = 0; i < num_fields; ++i) {
				printf("                     Colonna.%d", i);   // (strlen = 30)
			}

			printf("\n\n");
			// fetch and display result set rows
			while (1) {
				
				status = mysql_stmt_fetch(stmt); 
				

				if (status == 1 || status == MYSQL_NO_DATA)
					break;
				

				for (i = 0; i < num_fields; ++i) {
					
					switch (rs_bind[i].buffer_type) {

						case MYSQL_TYPE_VAR_STRING: 
							if (*rs_bind[i].is_null){
								printf("%30s", "NULL");
								fflush(stdout);
							}
							else{
								printf("%30s", (char*) rs_bind[i].buffer);
								fflush(stdout);
								}
							break;
						
						case MYSQL_TYPE_DATE: 
							if (*rs_bind[i].is_null){
								printf("%30s", "NULL");
								fflush(stdout);}
							else{
								date = rs_bind[i].buffer;
								printf("%15d-%d-%d %d:%d:%d", date->year, date->month, date->day, date->hour, date->minute, date->second);
								fflush(stdout);
							}
							break;
						
						case MYSQL_TYPE_STRING: 
							if (*rs_bind[i].is_null){
								printf("%30s", "NULL");
								fflush(stdout);
							}
							else{
								printf("%30s", (char*) rs_bind[i].buffer);
								fflush(stdout);
								}
							break;

						case MYSQL_TYPE_FLOAT: 
							if (*rs_bind[i].is_null){
								printf("%30s", "NULL");
								fflush(stdout);
							}
							else{
								printf("%30.2f", *(float*) rs_bind[i].buffer);
								fflush(stdout);
								}
							break;
						
						case MYSQL_TYPE_DOUBLE: 
							if (*rs_bind[i].is_null){
								printf("%30s", "NULL");
								fflush(stdout);
							}
							else{
								printf("%30.2f", *(double*) rs_bind[i].buffer);
								}
							break;

						case MYSQL_TYPE_LONG: 
							if (*rs_bind[i].is_null){
								printf("%30s", "NULL");
								fflush(stdout);
							}
							else{
								printf("%30d", *(int*) rs_bind[i].buffer);
								fflush(stdout);
								}
							break;
						
						case MYSQL_TYPE_TIMESTAMP: 
							if (*rs_bind[i].is_null){
								printf("%30s", "NULL");
								fflush(stdout);
							}
							else{
								date = rs_bind[i].buffer;
								printf("%21d-%d-%d %d:%d:%d",date->year, date->month, date->day, date->hour, date->minute, date->second);
							}
							break;

						case MYSQL_TYPE_BLOB: 
							if (*rs_bind[i].is_null){
								printf("%30s", "NULL");
								fflush(stdout);
							}
							else{
								printf("%30s", (char*) rs_bind[i].buffer);
								fflush(stdout);
								}
							break;
						
						case MYSQL_TYPE_DATETIME: 
							if (*rs_bind[i].is_null){
								printf("%30s", "NULL");
								fflush(stdout);
							}
							else{
								printf("%30s", (char*) rs_bind[i].buffer);
								fflush(stdout);
								}
							break;
						
						
						case MYSQL_TYPE_SHORT: 
							if (*rs_bind[i].is_null){
								printf("%30s", "NULL");
								fflush(stdout);
							}
							else{
								printf("%30d", *(int*) rs_bind[i].buffer);
								fflush(stdout);
								}
							break;


						case MYSQL_TYPE_TINY: 
							if (*rs_bind[i].is_null){
								printf("%30s", "NULL");
								fflush(stdout);
							}
							else{
								printf("%30d", *(int*) rs_bind[i].buffer);
								fflush(stdout);
								}
							break;
						
						case MYSQL_TYPE_NEWDECIMAL: 
							if (*rs_bind[i].is_null){
								printf("%30s", "NULL");
								fflush(stdout);
							}
							else{
								printf("%30.6lf", *(float*) rs_bind[i].buffer);
								fflush(stdout);
								}
							break;


						default:
							printf("ERROR: unexpected type (%d)\n", rs_bind[i].buffer_type);
					}
				}
				printf("\n");
			}
			mysql_free_result(rs_metadata);	// free metadata

			for (int j = 0; j < num_fields; ++j){
				free(rs_bind[j].buffer);
			}
			
			free(rs_bind);	// free output buffers
		} else {
			// no columns = final status packet
			printf("\n\n%20s", "--------------------fine dell'output-------------------------\n");
		}

		// more results? -1 = no, >0 = error, 0 = yes (keep looking)
		status = mysql_stmt_next_result(stmt);
		if (status > 0)
			if(test_stmt_error(stmt, status) == 1){
				printf("\nPremi invio per continuare...\n");
				while(getchar()!= '\n'){}
				return;
			} 
	} while (status == 0);

	mysql_stmt_close(stmt);
}




/* ----------------------------SEZIONE DEDICATA ALL'UTENTE BASE------------------------------------ */


void op_a1();

//RICERCA ASTA PER NOME OGGETTO
void op_1(){

    MYSQL_STMT *stmt;
    int status;
    MYSQL_BIND ps_params[1];	// input parameter buffers
    unsigned long length[6];	// Can do like that because all IN parameters have the same length
    
	top:
    //titolo : operazione
	printf("\e[1;1H\e[2J");
    printf("\n\n**** VISUALIZZAZIONE ASTE: RICERCA PER OGGETTO ****\n\n");

	//define input variable
	char nome[64];
	printf("ricerca aste per nome: ");
	getInput(64, nome, false);
	length[0] = strlen(nome);

    //inizializzazione statement procedurale.
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}
    
	status = mysql_stmt_prepare(stmt, "CALL visualizza_aste_per_nome_oggetto(?)", strlen("CALL visualizza_aste_per_nome_oggetto(?)"));
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		goto top;
	} 



	// initialize parameters
	memset(ps_params, 0, sizeof(ps_params));

	// `visualizza_aste_per_nome_oggetto` (in nomeoggetto varchar(20))
	ps_params[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[0].buffer = nome;
	ps_params[0].buffer_length = 64;
	ps_params[0].length = &length[0];
	ps_params[0].is_null = 0;


	// bind input parameters
	status = mysql_stmt_bind_param(stmt, ps_params);
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		goto top;
	} 
	// Run the stored procedure
	status = mysql_stmt_execute(stmt);
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		goto top;
	} 

    procedure_output( stmt, status);
}

//VISUALIZZAZIONE ASTE ATTIVE
void op_2(){

    MYSQL_STMT *stmt;
    int status;

	top:
    //titolo : operazione
	printf("\e[1;1H\e[2J");
    printf("\n\n**** VISUALIZZAZIONE ASTE ATTIVE ****\n\n");

    //inizializzazione statement procedurale.
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}
	status = mysql_stmt_prepare(stmt, "CALL visualizza_aste_attive()", strlen("CALL visualizza_aste_attive()"));
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		goto top;
	} 



	// Run the stored procedure
	status = mysql_stmt_execute(stmt);
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		return;
	} 
    procedure_output(stmt, status);
}

//VISUALIZZAZIONE STATO DI UN ASTA
void op_3(){

    MYSQL_STMT *stmt;
    int status;
    MYSQL_BIND ps_params[1];	// input parameter buffers
    unsigned long length[1];	// Can do like that because all IN parameters have the same length
    
	top:
    //titolo : operazione
	printf("\e[1;1H\e[2J");
    printf("\n\n**** VISUALIZZAZIONE ASTE: VISUALIZZA LO STATO DI UN'ASTA ****\n\n");

	//define input variable
	int *id = malloc(sizeof(int));
	char* _id = malloc(sizeof(char)*64);
	printf("inserire il codice dell'asta: ");
	getInput(64, _id, false);
	*id =  atoi(_id);
	free(_id); 
	length[0] = sizeof(int);

    //inizializzazione statement procedurale.
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}
    
	status = mysql_stmt_prepare(stmt, "CALL visualizza_stato_asta(?)", strlen("CALL visualizza_stato_asta(?)"));
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		free(id);
		goto top;
	} 

	// initialize parameters
	memset(ps_params, 0, sizeof(ps_params));

	// `visualizza_stato_asta` (in id int)
	ps_params[0].buffer_type = MYSQL_TYPE_LONG;
	ps_params[0].buffer = id;
	ps_params[0].buffer_length = sizeof(int);
	ps_params[0].length = &length[0];
	ps_params[0].is_null = 0;



	// bind input parameters
	status = mysql_stmt_bind_param(stmt, ps_params);
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		free(id);
		goto top;
	} 
	// Run the stored procedure
	status = mysql_stmt_execute(stmt);
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		free(id);
		return;
	} 

	
    procedure_output( stmt, status);
	free(id);

}

//VISUALIZZAZIONE OGGETTI AGGIUDICATI
void op_4(){

	MYSQL_STMT *stmt;
    int status;
    MYSQL_BIND ps_params[1];	// input parameter buffers
    unsigned long length[1];	// Can do like that because all IN parameters have the same length
    
	top:
    //titolo : operazione
	printf("\e[1;1H\e[2J");
    printf("\n\n**** VISUALIZZAZIONE OGGETTI AGGIUDICATI ****\n\n");


	//define input variable
	length[0] = 16;


    //inizializzazione statement procedurale.
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}
    
	status = mysql_stmt_prepare(stmt, "CALL visualizza_oggetti_aggiudicati(?)", strlen("CALL visualizza_oggetti_aggiudicati(?)"));
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		goto top;
	} 

	// initialize parameters
	memset(ps_params, 0, sizeof(ps_params));

	// `visualizza_stato_asta` (in id int)
	ps_params[0].buffer_type = MYSQL_TYPE_STRING;
	ps_params[0].buffer = logged_user.cf;
	ps_params[0].buffer_length = 16;
	ps_params[0].length = &length[0];
	ps_params[0].is_null = 0;

	

	// bind input parameters
	status = mysql_stmt_bind_param(stmt, ps_params);
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		goto top;
	} 

	// Run the stored procedure
	status = mysql_stmt_execute(stmt);
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		return;
	} 
	procedure_output( stmt, status);

}

//REGISTRAZIONE OFFERTA SU UN'ASTA
void op_5(){

    MYSQL_STMT *stmt;
    int status;
    MYSQL_BIND ps_params[4];	// input parameter buffers
    unsigned long length[4];	// Can do like that because all IN parameters have the same length
    
	top:
    //titolo : operazione
	printf("\e[1;1H\e[2J");
    printf("\n\n**** REGISTRAZIONE OFFERTA SU UN'ASTA ****\n\n");


	//define input variable
	length[0] = 16;


	char* _id = malloc(sizeof(char)*64);
	printf("inserire il codice dell'asta: ");
	getInput(64, _id, false);
	int *id = malloc(sizeof(int));
	*id = (int) atoi(_id);
	length[1] = sizeof(int);
	free(_id);
	
	char* _offerta = malloc(sizeof(char)*64);
	printf("\ninserire l'importo dell'offerta: ");
	getInput(64, _offerta, false);
	float *offerta = malloc(sizeof(float));
	*offerta = (float) atof(_offerta);
	length[2] = sizeof(float);
	free(_offerta);

	char* _importo_controff = malloc(sizeof(char)*64);
	printf("\ninserire un importo massimo di controfferta: ");
	getInput(64, _importo_controff, false);
	float *importo_controff = malloc(sizeof(float));
	*importo_controff = (float) atof(_offerta);
	length[2] = sizeof(float);
	free(_importo_controff);




    //inizializzazione statement procedurale.
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}
    
	status = mysql_stmt_prepare(stmt, "CALL registra_offerta(?, ?, ?, ?)", strlen("CALL registra_offerta(?, ?, ?, ?)"));
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		free(id);
		free(offerta);
		free(importo_controff);
		goto top;
	} 

	// initialize parameters
	memset(ps_params, 0, sizeof(ps_params));

	// `visualizza_stato_asta` (in id int)
	ps_params[0].buffer_type = MYSQL_TYPE_STRING;
	ps_params[0].buffer = logged_user.cf;
	ps_params[0].buffer_length = 16;
	ps_params[0].length = &length[0];
	ps_params[0].is_null = 0;

	ps_params[1].buffer_type = MYSQL_TYPE_LONG;
	ps_params[1].buffer = id;
	ps_params[1].buffer_length = sizeof(int);
	ps_params[1].length = &length[1];
	ps_params[1].is_null = 0;

	ps_params[2].buffer_type = MYSQL_TYPE_FLOAT;
	ps_params[2].buffer = offerta;
	ps_params[2].buffer_length = sizeof(float);
	ps_params[2].length = &length[2];
	ps_params[2].is_null = 0;

	ps_params[3].buffer_type = MYSQL_TYPE_FLOAT;
	ps_params[3].buffer = importo_controff;
	ps_params[3].buffer_length = sizeof(float);
	ps_params[3].length = &length[3];
	ps_params[3].is_null = 0;

	

	// bind input parameters
	status = mysql_stmt_bind_param(stmt, ps_params);
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		free(id);
		free(offerta);
		free(importo_controff);
		goto top;
	} 

	// Run the stored procedure
	status = mysql_stmt_execute(stmt);
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		free(id);
		free(offerta);
		free(importo_controff);
		return;
	} 

	printf("L'offerta è stata regolarmente registrata.\n");
	
	free(id);
	free(offerta);
	free(importo_controff);
}

//RICERCA ASTE PER ESPOSITORE
void op_6(){

	MYSQL_STMT *stmt;
    int status;
    MYSQL_BIND ps_params[1];	// input parameter buffers
    unsigned long length[1];	// Can do like that because all IN parameters have the same length
    
	top:
    //titolo : operazione
	printf("\e[1;1H\e[2J");
    printf("\n\n**** RICERCA ASTE PER ESPOSITORE ****\n\n");


	//define input variable
	
	char* cf_esp = malloc(sizeof(char)*16);
	printf("inserire il codice fiscale dell'espositore: ");
	getInput(16, cf_esp, false);
	length[0] = 16;
	

    //inizializzazione statement procedurale.
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}
    
	status = mysql_stmt_prepare(stmt, "CALL visualizza_aste_per_espositore(?)", strlen("CALL visualizza_aste_per_espositore(?)"));
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		free(cf_esp);
		goto top;
	}

	// initialize parameters
	memset(ps_params, 0, sizeof(ps_params));

	// `visualizza_stato_asta` (in id int)
	ps_params[0].buffer_type = MYSQL_TYPE_STRING;
	ps_params[0].buffer = cf_esp;
	ps_params[0].buffer_length = 16;
	ps_params[0].length = &length[0];
	ps_params[0].is_null = 0;

	
	

	// bind input parameters
	status = mysql_stmt_bind_param(stmt, ps_params);
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		free(cf_esp);
		goto top;
	}

	// Run the stored procedure
	status = mysql_stmt_execute(stmt);
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		free(cf_esp);
		return;
	}

	procedure_output(stmt, status);
	
	free(cf_esp);
	
}

//RICERCA ASTE PER CATEGORIA
void op_7(){

	MYSQL_STMT *stmt;
    int status;
    MYSQL_BIND ps_params[1];	// input parameter buffers
    unsigned long length[6];	// Can do like that because all IN parameters have the same length
    
	top:
    //titolo : operazione
	printf("\e[1;1H\e[2J");
    printf("\n\n**** VISUALIZZAZIONE ASTE: RICERCA PER CATEGORIA ****\n\n");

	//define input variable
	char* categoria = malloc(sizeof(char)*64);
	printf("inserire il nome di una categoria esistente (o parte di esso): ");
	getInput(64, categoria, false);
	length[0] = strlen(categoria);

    //inizializzazione statement procedurale.
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}
    
	status = mysql_stmt_prepare(stmt, "CALL visualizza_aste_per_categoria(?)", strlen("CALL visualizza_aste_per_categoria(?)"));
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		free(categoria);
		goto top;
	} 

	// initialize parameters
	memset(ps_params, 0, sizeof(ps_params));

	// `visualizza_aste_per_nome_oggetto` (in nomeoggetto varchar(20))
	ps_params[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[0].buffer = categoria;
	ps_params[0].buffer_length = strlen(categoria);
	ps_params[0].length = &length[0];
	ps_params[0].is_null = 0;


	// bind input parameters
	status = mysql_stmt_bind_param(stmt, ps_params);
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		free(categoria);
		goto top;
	} 
	// Run the stored procedure
	status = mysql_stmt_execute(stmt);
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		free(categoria);
		return;
	} 

    procedure_output(stmt, status);

	free(categoria);
}

//RICERCA ASTE PER PARTECIPAZIONE
void op_9(){

	MYSQL_STMT *stmt;
    int status;
    MYSQL_BIND ps_params[1];	// input parameter buffers
    unsigned long length[1];	// Can do like that because all IN parameters have the same length
    
	top:
    //titolo : operazione
	printf("\e[1;1H\e[2J");
    printf("\n\n**** RICERCA ASTE PER PARTECIPAZIONE ****\n\n");


	//define input variable
	length[0] = 16;


    //inizializzazione statement procedurale.
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}
    
	status = mysql_stmt_prepare(stmt, "CALL visualizza_partecipazione_aste (?)", strlen("CALL visualizza_partecipazione_aste (?)"));
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		goto top;
	} 

	// initialize parameters
	memset(ps_params, 0, sizeof(ps_params));

	// `visualizza_stato_asta` (in id int)
	ps_params[0].buffer_type = MYSQL_TYPE_STRING;
	ps_params[0].buffer = logged_user.cf;
	ps_params[0].buffer_length = 16;
	ps_params[0].length = &length[0];
	ps_params[0].is_null = 0;

	

	// bind input parameters
	status = mysql_stmt_bind_param(stmt, ps_params);
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		goto top;
	} 

	// Run the stored procedure
	status = mysql_stmt_execute(stmt);
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		return;
	} 
	procedure_output( stmt, status);
}


//INTERFACCIA UTENTE : UTENTE BASE
void menu_utente_base(){

    char *operation;
    start: operation = malloc(64*sizeof(char));
    int op_code;
	printf("\e[1;1H\e[2J");
    printf("....................................................................................\n");
	printf("....................................................................................\n");
	printf("..............................|      MENU UTENTE      |.............................\n");
	printf("....................................................................................\n");
	printf("....................................................................................\n\n\n");

	printf(" _____ ________ ________ _____Operazioni Disponibili_____ ________ ________ _______\n");
	printf("|                                                                                   |\n");
	printf("|                                                                    [LOG OUT: 0]   |\n");
	printf("|                                                                                   |\n");
    printf("|   OPERAZIONE 1 : Ricerca aste per nome dell' oggetto                              |\n");
    printf("|   OPERAZIONE 2 : Ricerca aste attive                                              |\n");
	printf("|   OPERAZIONE 3 : Visualizzazione dello stato di un'asta                           |\n");
    printf("|   OPERAZIONE 4 : Visualizzazione oggetti aggiudicati                              |\n");
	printf("|   OPERAZIONE 5 : Registrazione offerta per un'asta                                |\n");
	printf("|   OPERAZIONE 6 : Ricerca aste per espositore                                      |\n");
	printf("|   OPERAZIONE 7 : Ricerca aste per categoria di afferenza                          |\n");
	printf("|   OPERAZIONE 8 : Visualizzazione categorie                                        |\n");
	printf("|   OPERAZIONE 9 : Visualizzazione delle aste a cui ho partecipato                  |\n");
	printf("|____ ________ ________ ________ ________ ________ ________ ________ ________ ______|\n\n");
    printf("   Inserisci il codice dell'operazione : ");
    getInput(64, operation, false);
    op_code = atoi(operation);
    switch((int)op_code){
		case 0:
			break;

        case 1:
            op_1();
            printf("\n\n Premi invio per tornare al Menù Utente...\n");
            while(getchar() != '\n'){}
            free(operation);
			printf("\e[1;1H\e[2J");
            goto start;
            break;
        
        case 2:
            op_2();
            printf("\n\n Premi invio per tornare al Menù Utente...\n");
            while(getchar() != '\n'){}
            free(operation);
			printf("\e[1;1H\e[2J");
            goto start;
            break;

		case 3:
            op_3();
            printf("\n\n Premi invio per tornare al Menù Utente...\n");
            while(getchar() != '\n'){}
            free(operation);
			printf("\e[1;1H\e[2J");
            goto start;
            break;

		case 4:
            op_4();
            printf("\n\n Premi invio per tornare al Menù Utente...\n");
            while(getchar() != '\n'){}
            free(operation);
			printf("\e[1;1H\e[2J");
            goto start;
            break;

		case 5:
            op_5();
            printf("\n\n Premi invio per tornare al Menù Utente...\n");
            while(getchar() != '\n'){}
            free(operation);
			printf("\e[1;1H\e[2J");
            goto start;
            break;

		case 6:
            op_6();
            printf("\n\n Premi invio per tornare al Menù Utente...\n");
            while(getchar() != '\n'){}
            free(operation);
			printf("\e[1;1H\e[2J");
            goto start;
            break;

		case 7:
            op_7();
            printf("\n\n Premi invio per tornare al Menù Utente...\n");
            while(getchar() != '\n'){}
            free(operation);
			printf("\e[1;1H\e[2J");
            goto start;
            break;
		
		case 8:
            op_a1();
            printf("\n\n Premi invio per tornare al Menù Utente...\n");
            while(getchar() != '\n'){}
            free(operation);
			printf("\e[1;1H\e[2J");
            goto start;
            break;
		
		case 9:
            op_9();
            printf("\n\n Premi invio per tornare al Menù Utente...\n");
            while(getchar() != '\n'){}
            free(operation);
			printf("\e[1;1H\e[2J");
            goto start;
            break;

        default:
            free(operation);
			printf("\e[1;1H\e[2J");
            goto start;
            break;
            
    } 
	return;
}







/* -----------------------SEZIONE DEDICATA ALL'UTENTE AMMINISTRATORE---------------------------- */



/*                              *** Gestione categorie ***                                        */


//VISUALIZZA TITOLARIO GERARCHICO
void op_a1(){

    MYSQL_STMT *stmt;
    int status;
    MYSQL_BIND ps_params[1];	// input parameter buffers
    unsigned long length[6];	// Can do like that because all IN parameters have the same length
    
	top:
    //titolo : operazione
	printf("\e[1;1H\e[2J");
    printf("\n\n    **** VISUALIZZAZIONE CATEGORIE : TITOLARIO GERARCHICO ****\n\n");

	

    //inizializzazione statement procedurale.
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}
    
	status = mysql_stmt_prepare(stmt, "CALL visualizzazione_titolario_gerarchico()", strlen("CALL visualizzazione_titolario_gerarchico()"));
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		goto top;
	} 


	// bind input parameters
	status = mysql_stmt_bind_param(stmt, ps_params);
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		goto top;
	} 
	// Run the stored procedure
	status = mysql_stmt_execute(stmt);
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		goto top;
	} 

    procedure_output( stmt, status);
}

//INSERIMENTO CATEGORIA
void op_a2(){

    MYSQL_STMT *stmt;
    int status;
    
	top:
    //titolo : operazione
	printf("\e[1;1H\e[2J"); 
    printf("\n\n           **** INSERIMENTO CATEGORIA ****\n\n");
	printf(" _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ \n");
	printf("|                                                        |\n");
	printf("| In questa sezione è possibile inserire nuove categorie |\n");
	printf("| nel titolario corrente. Il titolario è strutturato in  |\n");
	printf("| una gerarchia a tre livelli:                           |\n");
	printf("| Lv 1 : categorie effettivamente associabili ad oggetti |\n");
	printf("| Lv 2 : categorie genitori di categorie di Lv 1         |\n");
	printf("| Lv 3 : categorie genitori di categorie di Lv 2         |\n");
	printf("| E' possibile inserire una nuova categoria in uno       |\n");
	printf("| qualsiasi di questi livelli, indicando il suo nome     |\n");
	printf("| e quello della categoria genitore, se esistente.       |\n");
	printf("| _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _| \n");
	printf("\nDigitare il livello del titolario in cui si desidera inserire la nuova categoria.\n");
	printf("\nAltrimenti, digitare il codice '0' per visionare il titolario corrente... ");

	char *_lv = malloc(sizeof(char) * 32);
	int lv;
	getInput(32, _lv, false);
	lv = atoi(_lv);
	free(_lv);

	int answer;

	switch(lv){
		
		case 0:
			op_a1();
            printf("\n\n Premi invio per tornare alla scelta del livello...\n");
            while(getchar() != '\n'){}
			printf("\e[1;1H\e[2J");
            goto top;
            break;

		
		case 1:
			printf("\e[1;1H\e[2J");
			printf("INSERIMENTO CATEGORIA: LIVELLO 1 \n");

			printf("Si desidera associare la categoria a un genitore al livello 2 ?\n");
			printf("Digitare il codice di risposta [ 0 = sì  |  1 = no ] : ");

			char* _answer = malloc(sizeof(char)*32);
			getInput(32, _answer, false);
			answer = atoi(_answer);
			free(_answer);

			if(answer == 0){

				MYSQL_BIND ps_params[2];	// input parameter buffers
    			unsigned long length[2];	// Can do like that because all IN parameters have the same length
				
				//define input variable
				char nome[64];
				printf("\nInserire il nome della nuova categoria: ");
				getInput(64, nome, false);
				length[0] = strlen(nome);

				char nomePadre[64];
				printf("\nInserire il nome della categoria genitore: ");
				getInput(64, nomePadre, false);
				length[1] = strlen(nomePadre);


				//inizializzazione statement procedurale.
				stmt = mysql_stmt_init(con);
				if (!stmt) {
					printf("Could not initialize statement\n");
					exit(1);
				}
				
				status = mysql_stmt_prepare(stmt, "CALL inserimento_categoria3(?, ?)", strlen("CALL inserimento_categoria3(?, ?)"));
				if(test_stmt_error(stmt, status) == 1){
					printf("\nPremi invio per continuare...\n");
					while(getchar()!= '\n'){}
					goto top;
				} 


				// initialize parameters
				memset(ps_params, 0, sizeof(ps_params));

				// `visualizza_aste_per_nome_oggetto` (in nomeoggetto varchar(20))
				ps_params[0].buffer_type = MYSQL_TYPE_VAR_STRING;
				ps_params[0].buffer = &nome;
				ps_params[0].buffer_length = 64;
				ps_params[0].length = &length[0];
				ps_params[0].is_null = 0;

				
				ps_params[1].buffer_type = MYSQL_TYPE_VAR_STRING;
				ps_params[1].buffer = &nomePadre;
				ps_params[1].buffer_length = 64;
				ps_params[1].length = &length[1];
				ps_params[1].is_null = 0;


				// bind input parameters
				status = mysql_stmt_bind_param(stmt, ps_params);
				if(test_stmt_error(stmt, status) == 1){
					printf("\nPremi invio per continuare...\n");
					while(getchar()!= '\n'){}
					goto top;
				} 
				// Run the stored procedure
				status = mysql_stmt_execute(stmt);
				if(test_stmt_error(stmt, status) == 1){
					printf("\nPremi invio per continuare...\n");
					while(getchar()!= '\n'){}
					goto top;
				}
			} else{

				MYSQL_BIND ps_params[1];	// input parameter buffers
    			unsigned long length[1];	// Can do like that because all IN parameters have the same length
				
				//define input variable
				char nome[64];
				printf("\nInserire il nome della nuova categoria: ");
				getInput(64, nome, false);
				length[0] = strlen(nome);


				//inizializzazione statement procedurale.
				stmt = mysql_stmt_init(con);
				if (!stmt) {
					printf("Could not initialize statement\n");
					exit(1);
				}
				
				status = mysql_stmt_prepare(stmt, "CALL inserimento_categoria3(?, null)", strlen("CALL inserimento_categoria3(?, null)"));
				if(test_stmt_error(stmt, status) == 1){
					printf("\nPremi invio per continuare...\n");
					while(getchar()!= '\n'){}
					goto top;
				} 


				// initialize parameters
				memset(ps_params, 0, sizeof(ps_params));

				// `visualizza_aste_per_nome_oggetto` (in nomeoggetto varchar(20))
				ps_params[0].buffer_type = MYSQL_TYPE_VAR_STRING;
				ps_params[0].buffer = &nome;
				ps_params[0].buffer_length = 64;
				ps_params[0].length = &length[0];
				ps_params[0].is_null = 0;


				// bind input parameters
				status = mysql_stmt_bind_param(stmt, ps_params);
				if(test_stmt_error(stmt, status) == 1){
					printf("\nPremi invio per continuare...\n");
					while(getchar()!= '\n'){}
					goto top;
				} 

				// Run the stored procedure
				status = mysql_stmt_execute(stmt);
				if(test_stmt_error(stmt, status) == 1){
					printf("\nPremi invio per continuare...\n");
					while(getchar()!= '\n'){}
					goto top;
				}
			}
			return;
			break;

		
		case 2:

			printf("\e[1;1H\e[2J");
			printf("INSERIMENTO CATEGORIA: LIVELLO 2 \n");

			printf("Si desidera associare la categoria a un genitore al livello 3 ?\n");
			printf("Digitare il codice di risposta [ 0 = sì  |  1 = no ] : ");

			char* _answer2 = malloc(sizeof(char)*32);
			getInput(32, _answer2, false);
			answer = atoi(_answer2);
			free(_answer2);

			if(answer == 0){

				MYSQL_BIND ps_params[2];	// input parameter buffers
    			unsigned long length[2];	// Can do like that because all IN parameters have the same length
				
				//define input variable
				char nome[64];
				printf("\nInserire il nome della nuova categoria: ");
				getInput(64, nome, false);
				length[0] = strlen(nome);

				char nomePadre[64];
				printf("\nInserire il nome della categoria genitore: ");
				getInput(64, nomePadre, false);
				length[1] = strlen(nomePadre);


				//inizializzazione statement procedurale.
				stmt = mysql_stmt_init(con);
				if (!stmt) {
					printf("Could not initialize statement\n");
					exit(1);
				}
				
				status = mysql_stmt_prepare(stmt, "CALL inserimento_categoria2(?, ?)", strlen("CALL inserimento_categoria2(?, ?)"));
				if(test_stmt_error(stmt, status) == 1){
					printf("\nPremi invio per continuare...\n");
					while(getchar()!= '\n'){}
					goto top;
				} 


				// initialize parameters
				memset(ps_params, 0, sizeof(ps_params));

				// `visualizza_aste_per_nome_oggetto` (in nomeoggetto varchar(20))
				ps_params[0].buffer_type = MYSQL_TYPE_VAR_STRING;
				ps_params[0].buffer = &nome;
				ps_params[0].buffer_length = 64;
				ps_params[0].length = &length[0];
				ps_params[0].is_null = 0;

				
				ps_params[1].buffer_type = MYSQL_TYPE_VAR_STRING;
				ps_params[1].buffer = &nomePadre;
				ps_params[1].buffer_length = 64;
				ps_params[1].length = &length[1];
				ps_params[1].is_null = 0;


				// bind input parameters
				status = mysql_stmt_bind_param(stmt, ps_params);
				if(test_stmt_error(stmt, status) == 1){
					printf("\nPremi invio per continuare...\n");
					while(getchar()!= '\n'){}
					goto top;
				} 
				// Run the stored procedure
				status = mysql_stmt_execute(stmt);
				if(test_stmt_error(stmt, status) == 1){
					printf("\nPremi invio per continuare...\n");
					while(getchar()!= '\n'){}
					goto top;
				}
			} else{

				MYSQL_BIND ps_params[1];	// input parameter buffers
    			unsigned long length[1];	// Can do like that because all IN parameters have the same length
				
				//define input variable
				char nome[64];
				printf("\nInserire il nome della nuova categoria: ");
				getInput(64, nome, false);
				length[0] = strlen(nome);


				//inizializzazione statement procedurale.
				stmt = mysql_stmt_init(con);
				if (!stmt) {
					printf("Could not initialize statement\n");
					exit(1);
				}
				
				status = mysql_stmt_prepare(stmt, "CALL inserimento_categoria2(?, null)", strlen("CALL inserimento_categoria2(?, null)"));
				if(test_stmt_error(stmt, status) == 1){
					printf("\nPremi invio per continuare...\n");
					while(getchar()!= '\n'){}
					goto top;
				} 


				// initialize parameters
				memset(ps_params, 0, sizeof(ps_params));

				// `visualizza_aste_per_nome_oggetto` (in nomeoggetto varchar(20))
				ps_params[0].buffer_type = MYSQL_TYPE_VAR_STRING;
				ps_params[0].buffer = &nome;
				ps_params[0].buffer_length = 64;
				ps_params[0].length = &length[0];
				ps_params[0].is_null = 0;


				// bind input parameters
				status = mysql_stmt_bind_param(stmt, ps_params);
				if(test_stmt_error(stmt, status) == 1){
					printf("\nPremi invio per continuare...\n");
					while(getchar()!= '\n'){}
					goto top;
				} 

				// Run the stored procedure
				status = mysql_stmt_execute(stmt);
				if(test_stmt_error(stmt, status) == 1){
					printf("\nPremi invio per continuare...\n");
					while(getchar()!= '\n'){}
					goto top;
				}
			}
			return;
			break;

		case 3:

			printf("\e[1;1H\e[2J");
			printf("INSERIMENTO CATEGORIA: LIVELLO 3 \n");

			MYSQL_BIND ps_params[1];	// input parameter buffers
			unsigned long length[1];	// Can do like that because all IN parameters have the same length
			
			//define input variable
			char nome[64];
			printf("\nInserire il nome della nuova categoria: ");
			getInput(64, nome, false);
			length[0] = strlen(nome);


			//inizializzazione statement procedurale.
			stmt = mysql_stmt_init(con);
			if (!stmt) {
				printf("Could not initialize statement\n");
				exit(1);
			}
			
			status = mysql_stmt_prepare(stmt, "CALL inserimento_categoria1(?)", strlen("CALL inserimento_categoria1(?)"));
			if(test_stmt_error(stmt, status) == 1){
				printf("\nPremi invio per continuare...\n");
				while(getchar()!= '\n'){}
				goto top;
			} 


			// initialize parameters
			memset(ps_params, 0, sizeof(ps_params));

			// `visualizza_aste_per_nome_oggetto` (in nomeoggetto varchar(20))
			ps_params[0].buffer_type = MYSQL_TYPE_VAR_STRING;
			ps_params[0].buffer = &nome;
			ps_params[0].buffer_length = 64;
			ps_params[0].length = &length[0];
			ps_params[0].is_null = 0;


			// bind input parameters
			status = mysql_stmt_bind_param(stmt, ps_params);
			if(test_stmt_error(stmt, status) == 1){
				printf("\nPremi invio per continuare...\n");
				while(getchar()!= '\n'){}
				goto top;
			} 

			// Run the stored procedure
			status = mysql_stmt_execute(stmt);
			if(test_stmt_error(stmt, status) == 1){
				printf("\nPremi invio per continuare...\n");
				while(getchar()!= '\n'){}
				goto top;
			}

			return;
			break;
		
		default:
			goto top;
			break;

	}


}

//AGGIORNAMENTO CATEGORIA
void op_a3(){

	MYSQL_STMT *stmt;
    int status;
	
	//inizializzazione statement procedurale.
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}
    
	top:
    //titolo : operazione
	printf("\e[1;1H\e[2J"); 
    printf("\n\n           **** AGGIORNAMENTO CATEGORIA ****\n\n");
	printf(" _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ \n");
	printf("|                                                        |\n");
	printf("| In questa sezione è possibile modificare il nome di    |\n");
	printf("| una categoria e/o associarla ad una categoria genitore,|\n");
	printf("| scelta tra quelle esistenti al livello superiore del   |\n");
	printf("| titolario. Si scelga la tipologia di aggiornamento     |\n");
	printf("| tra le seguenti:                                       |\n");
	printf("| 1. Modifica del nome di una categoria                  |\n");
	printf("| 2. Associa un padre ad una categoria (orfana)          |\n");
	printf("| 3. Cambia padre ad una categoria                       |\n");
	printf("| _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _| \n");
	printf("\nDigitare il codice relativo alla tipologia di aggiornamento desiderata.\n");
	printf("\nAltrimenti, digitare il codice '0' per visionare il titolario corrente... ");

	char *_cod = malloc(sizeof(char) * 32);
	int cod;
	getInput(32, _cod, false);
	cod = atoi(_cod);
	free(_cod);

	MYSQL_BIND ps_params[3];	// input parameter buffers
    unsigned long length[3];
	char nome[64];
	char nuovo_nome[64];
	char nomePadre[64];
	char new_nomePadre[64];

	switch(cod){

		case 0:
			op_a1();
            printf("\n\n Premi invio per tornare alla scelta del livello...\n");
            while(getchar() != '\n'){}
			printf("\e[1;1H\e[2J");
            goto top;
            break;

		case 1:

			//define input variable
			printf("\nInserire il nome attuale della categoria : ");
			getInput(64, nome, false);
			length[0] = strlen(nome);

			printf("\nInserire il nuovo nome : ");
			getInput(64, nuovo_nome, false);
			length[1] = strlen(nuovo_nome);
			
			status = mysql_stmt_prepare(stmt, "CALL aggiornamento_categoria(?, ?, null, null)", strlen("CALL aggiornamento_categoria(?, ?, null, null)"));
			if(test_stmt_error(stmt, status) == 1){
				printf("\nPremi invio per continuare...\n");
				while(getchar()!= '\n'){}
				goto top;
			} 

			// initialize parameters
			memset(ps_params, 0, sizeof(ps_params));

			// `visualizza_aste_per_nome_oggetto` (in nomeoggetto varchar(20))
			ps_params[0].buffer_type = MYSQL_TYPE_VAR_STRING;
			ps_params[0].buffer = &nome;
			ps_params[0].buffer_length = 64;
			ps_params[0].length = &length[0];
			ps_params[0].is_null = 0;
			
			ps_params[1].buffer_type = MYSQL_TYPE_VAR_STRING;
			ps_params[1].buffer = &nuovo_nome;
			ps_params[1].buffer_length = 64;
			ps_params[1].length = &length[1];
			ps_params[1].is_null = 0;


			// bind input parameters
			status = mysql_stmt_bind_param(stmt, ps_params);
			if(test_stmt_error(stmt, status) == 1){
				printf("\nPremi invio per continuare...\n");
				while(getchar()!= '\n'){}
				goto top;
			} 
			// Run the stored procedure
			status = mysql_stmt_execute(stmt);
			if(test_stmt_error(stmt, status) == 1){
				printf("\nPremi invio per continuare...\n");
				while(getchar()!= '\n'){}
				goto top;
			}
			return;
			break;

		case 2:
			//define input variable
			printf("\nInserire il nome della categoria : ");
			getInput(64, nome, false);
			length[0] = strlen(nome);

			printf("\nInserire il nome della categoria che si vuole rendere padre : ");
			getInput(64, nomePadre, false);
			length[1] = strlen(nomePadre);

			status = mysql_stmt_prepare(stmt, "CALL aggiornamento_categoria(?, null, null, ?)", strlen("CALL aggiornamento_categoria(?, null, null, ?)"));
			if(test_stmt_error(stmt, status) == 1){
				printf("\nPremi invio per continuare...\n");
				while(getchar()!= '\n'){}
				goto top;
			} 

			// initialize parameters
			memset(ps_params, 0, sizeof(ps_params));

			// `visualizza_aste_per_nome_oggetto` (in nomeoggetto varchar(20))
			ps_params[0].buffer_type = MYSQL_TYPE_VAR_STRING;
			ps_params[0].buffer = &nome;
			ps_params[0].buffer_length = 64;
			ps_params[0].length = &length[0];
			ps_params[0].is_null = 0;

			
			ps_params[1].buffer_type = MYSQL_TYPE_VAR_STRING;
			ps_params[1].buffer = &nomePadre;
			ps_params[1].buffer_length = 64;
			ps_params[1].length = &length[1];
			ps_params[1].is_null = 0;


			// bind input parameters
			status = mysql_stmt_bind_param(stmt, ps_params);
			if(test_stmt_error(stmt, status) == 1){
				printf("\nPremi invio per continuare...\n");
				while(getchar()!= '\n'){}
				goto top;
			} 
			// Run the stored procedure
			status = mysql_stmt_execute(stmt);
			if(test_stmt_error(stmt, status) == 1){
				printf("\nPremi invio per continuare...\n");
				while(getchar()!= '\n'){}
				goto top;
			}
			return;
			break;
	


		case 3:

			//define input variable
			printf("\nInserire il nome della categoria : ");
			getInput(64, nome, false);
			length[0] = strlen(nome);
			
			printf("\nInserire il nome della attuale categoria padre : ");
			getInput(64, nomePadre, false);
			length[1] = strlen(nomePadre);
			
			printf("\nInserire il nome della categoria che si vuole rendere padre : ");
			getInput(64, new_nomePadre, false);
			length[2] = strlen(new_nomePadre);
		
			status = mysql_stmt_prepare(stmt, "CALL aggiornamento_categoria(?, null, ?, ?)", strlen("CALL aggiornamento_categoria(?, null, ?, ?)"));
			if(test_stmt_error(stmt, status) == 1){
				printf("\nPremi invio per continuare...\n");
				while(getchar()!= '\n'){}
				goto top;
			} 

			// initialize parameters
			memset(ps_params, 0, sizeof(ps_params));

			// `visualizza_aste_per_nome_oggetto` (in nomeoggetto varchar(20))
			ps_params[0].buffer_type = MYSQL_TYPE_VAR_STRING;
			ps_params[0].buffer = &nome;
			ps_params[0].buffer_length = 64;
			ps_params[0].length = &length[0];
			ps_params[0].is_null = 0;
		
			ps_params[1].buffer_type = MYSQL_TYPE_VAR_STRING;
			ps_params[1].buffer = &nomePadre;
			ps_params[1].buffer_length = 64;
			ps_params[1].length = &length[1];
			ps_params[1].is_null = 0;

			ps_params[2].buffer_type = MYSQL_TYPE_VAR_STRING;
			ps_params[2].buffer = &new_nomePadre;
			ps_params[2].buffer_length = 64;
			ps_params[2].length = &length[2];
			ps_params[2].is_null = 0;

			// bind input parameters
			status = mysql_stmt_bind_param(stmt, ps_params);
			if(test_stmt_error(stmt, status) == 1){
				printf("\nPremi invio per continuare...\n");
				while(getchar()!= '\n'){}
				goto top;
			} 
			// Run the stored procedure
			status = mysql_stmt_execute(stmt);
			if(test_stmt_error(stmt, status) == 1){
				printf("\nPremi invio per continuare...\n");
				while(getchar()!= '\n'){}
				goto top;
			}
			return;
			break;


	}
}

//CANCELLAZIONE CATEGORIA ****
void op_a4(){

	MYSQL_STMT *stmt;
    int status;
	MYSQL_BIND ps_params[1];	// input parameter buffers
	unsigned long length[1];	// Can do like that because all IN parameters have the same length
	
	
	//define input variable
	char nome[64];
	top:
	printf("\nInserire il nome della categoria da eliminare: ");
	getInput(64, nome, false);
	length[0] = strlen(nome);


	//inizializzazione statement procedurale.
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}
	
	status = mysql_stmt_prepare(stmt, "CALL cancellazione_categoria(?)", strlen("CALL cancellazione_categoria(?)"));
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		goto top;
	} 


	// initialize parameters
	memset(ps_params, 0, sizeof(ps_params));

	// `visualizza_aste_per_nome_oggetto` (in nomeoggetto varchar(20))
	ps_params[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[0].buffer = &nome;
	ps_params[0].buffer_length = 64;
	ps_params[0].length = &length[0];
	ps_params[0].is_null = 0;


	// bind input parameters
	status = mysql_stmt_bind_param(stmt, ps_params);
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		goto top;
	} 

	// Run the stored procedure
	status = mysql_stmt_execute(stmt);
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		goto top;
	}
}


/*                                  *** Gestione aste ***                                        */


//INIZIALIZZAZIONE NUOVA ASTA
void op_a5(){

    MYSQL_STMT *stmt;
    int status;
    MYSQL_BIND ps_params[9];	// input parameter buffers
    unsigned long length[9];	// Can do like that because all IN parameters have the same length
    
	top:
    //titolo : operazione
	printf("\e[1;1H\e[2J");
    printf("\n\n**** INIZIALIZZAZIONE NUOVA ASTA ****\n\n");


	//define input variable
	
	char* nome = malloc(sizeof(char)*64);
	printf("inserire il nome dell'oggetto all'asta: ");
	getInput(64, nome, false);
	length[0] = strlen(nome);

	char* cat = malloc(sizeof(char)*64);
	printf("\ninserire la categoria di afferenza dell'oggetto: ");
	getInput(64, cat, false);
	length[1] = strlen(cat);

	char* desc = malloc(sizeof(char)*64);
	printf("\ninserire una breve descrizione dell' oggetto: ");
	getInput(64, desc, false);
	length[2] = strlen(desc);

	char* stato = malloc(sizeof(char)*64);
	printf("\ninserire lo stato attuale dell'oggetto (nuovo/usato/buone condizioni/etc...): ");
	getInput(64, stato, false);
	length[3] = strlen(stato);

	char* colore = malloc(sizeof(char)*64);
	printf("\ninserire il colore dell'oggetto: ");
	getInput(64, colore, false);
	length[4] = strlen(colore);

	length[5] = 16;

	char* dimens = malloc(sizeof(char)*64);
	printf("\ninserire le dimensioni dell'oggetto: ");
	getInput(64, dimens, false);
	length[6] = strlen(dimens);
	
	char* _prezzobase = malloc(sizeof(char)*64);
	printf("\ninserire il prezzo di partenza per l'oggetto all'asta: ");
	getInput(64, _prezzobase, false);
	float *prezzobase = malloc(sizeof(float));
	*prezzobase = (float) atof(_prezzobase);
	length[7] = sizeof(float);
	free(_prezzobase);

	char* _durata = malloc(sizeof(char)*64);
	printf("\ninserire la durata, in giorni, dell'asta (il range da considerare è [1, 7] ) : ");
	getInput(64, _durata, false);
	int *durata = malloc(sizeof(int));
	*durata = (int) atoi(_durata);
	length[8] = sizeof(int);
	free(_durata);




    //inizializzazione statement procedurale.
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}
    
	status = mysql_stmt_prepare(stmt, "CALL inizializzazione_asta(?, ?, ?, ?, ?, ?, ?, ?, ?)", strlen("CALL inizializzazione_asta(?, ?, ?, ?, ?, ?, ?, ?, ?)"));
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		free(nome);
		free(cat);
		free(desc);
		free(stato);
		free(colore);
		free(dimens);
		free(prezzobase);
		free(durata);
		goto top;
	} 

	// initialize parameters
	memset(ps_params, 0, sizeof(ps_params));

	
	ps_params[0].buffer_type = MYSQL_TYPE_STRING;
	ps_params[0].buffer = nome;
	ps_params[0].buffer_length = 64;
	ps_params[0].length = &length[0];
	ps_params[0].is_null = 0;

	ps_params[1].buffer_type = MYSQL_TYPE_STRING;
	ps_params[1].buffer = cat;
	ps_params[1].buffer_length = 64;
	ps_params[1].length = &length[1];
	ps_params[1].is_null = 0;

	ps_params[2].buffer_type = MYSQL_TYPE_STRING;
	ps_params[2].buffer = desc;
	ps_params[2].buffer_length = 64;
	ps_params[2].length = &length[2];
	ps_params[2].is_null = 0;

	ps_params[3].buffer_type = MYSQL_TYPE_STRING;
	ps_params[3].buffer = stato;
	ps_params[3].buffer_length = 64;
	ps_params[3].length = &length[3];
	ps_params[3].is_null = 0;

	ps_params[4].buffer_type = MYSQL_TYPE_STRING;
	ps_params[4].buffer = colore;
	ps_params[4].buffer_length = 64;
	ps_params[4].length = &length[4];
	ps_params[4].is_null = 0;

	ps_params[5].buffer_type = MYSQL_TYPE_STRING;
	ps_params[5].buffer = logged_user.cf;
	ps_params[5].buffer_length = 16;
	ps_params[5].length = &length[5];
	ps_params[5].is_null = 0;

	ps_params[6].buffer_type = MYSQL_TYPE_STRING;
	ps_params[6].buffer = dimens;
	ps_params[6].buffer_length = 64;
	ps_params[6].length = &length[6];
	ps_params[6].is_null = 0;

	ps_params[7].buffer_type = MYSQL_TYPE_FLOAT;
	ps_params[7].buffer = prezzobase;
	ps_params[7].buffer_length = sizeof(float);
	ps_params[7].length = &length[7];
	ps_params[7].is_null = 0;

	ps_params[8].buffer_type = MYSQL_TYPE_LONG;
	ps_params[8].buffer = durata;
	ps_params[8].buffer_length = sizeof(int);
	ps_params[8].length = &length[8];
	ps_params[8].is_null = 0;

	

	// bind input parameters
	status = mysql_stmt_bind_param(stmt, ps_params);
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		free(nome);
		free(cat);
		free(desc);
		free(stato);
		free(colore);
		free(dimens);
		free(prezzobase);
		free(durata);
		goto top;
	} 

	// Run the stored procedure
	status = mysql_stmt_execute(stmt);
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		free(nome);
		free(cat);
		free(desc);
		free(stato);
		free(colore);
		free(dimens);
		free(prezzobase);
		free(durata);
		return;
	} 

	printf("L'Asta è stata correttamente inizializzata.\n");
	
	free(nome);
		free(cat);
		free(desc);
		free(stato);
		free(colore);
		free(dimens);
		free(prezzobase);
		free(durata);
}

//REPORT ASTA
void op_a6(){

    MYSQL_STMT *stmt;
    int status;
    MYSQL_BIND ps_params[1];	// input parameter buffers
    unsigned long length[1];	// Can do like that because all IN parameters have the same length
    
	top:
    //titolo : operazione
	printf("\e[1;1H\e[2J");
    printf("\n\n**** REPORT ASTA ****\n\n");

	//define input variable
	int *id = malloc(sizeof(int));
	char* _id = malloc(sizeof(char)*64);
	printf("inserire il codice dell'asta: ");
	getInput(64, _id, false);
	*id =  atoi(_id);
	free(_id); 
	length[0] = sizeof(int);

    //inizializzazione statement procedurale.
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}
    
	status = mysql_stmt_prepare(stmt, "CALL report_asta(?)", strlen("CALL report_asta(?)"));
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		free(id);
		goto top;
	} 

	// initialize parameters
	memset(ps_params, 0, sizeof(ps_params));

	// `visualizza_stato_asta` (in id int)
	ps_params[0].buffer_type = MYSQL_TYPE_LONG;
	ps_params[0].buffer = id;
	ps_params[0].buffer_length = sizeof(int);
	ps_params[0].length = &length[0];
	ps_params[0].is_null = 0;



	// bind input parameters
	status = mysql_stmt_bind_param(stmt, ps_params);
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		free(id);
		goto top;
	} 
	// Run the stored procedure
	status = mysql_stmt_execute(stmt);
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		free(id);
		return;
	} 

	
    procedure_output( stmt, status);
	free(id);

}

//VISUALIZZAZIONE ASTE INDETTE
void op_a7(){

	MYSQL_STMT *stmt;
    int status;
    MYSQL_BIND ps_params[1];	// input parameter buffers
    unsigned long length[1];	// Can do like that because all IN parameters have the same length
    
	top:
    //titolo : operazione
	printf("\e[1;1H\e[2J");
    printf("\n\n**** VISUALIZZAZIONE ASTE INDETTE ****\n\n");


	//define input variable
	length[0] = 17;


    //inizializzazione statement procedurale.
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}
    
	status = mysql_stmt_prepare(stmt, "CALL visualizza_aste_per_espositore(?)", strlen("CALL visualizza_aste_per_espositore(?)"));
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		goto top;
	} 

	// initialize parameters
	memset(ps_params, 0, sizeof(ps_params));

	// `visualizza_stato_asta` (in id int)
	ps_params[0].buffer_type = MYSQL_TYPE_STRING;
	ps_params[0].buffer = logged_user.cf;
	ps_params[0].buffer_length = 17;
	ps_params[0].length = &length[0];
	ps_params[0].is_null = 0;

	

	// bind input parameters
	status = mysql_stmt_bind_param(stmt, ps_params);
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		goto top;
	} 

	// Run the stored procedure
	status = mysql_stmt_execute(stmt);
	if(test_stmt_error(stmt, status) == 1){
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		return;
	} 
	procedure_output( stmt, status);

}



//INTERFACCIA UTENTE: UTENTE AMMINISTRATORE
void menu_utente_admin(){

    char *operation;
    start: operation = malloc(64*sizeof(char));
    int op_code;

	printf("\e[1;1H\e[2J");
    printf("....................................................................................\n");
	printf("....................................................................................\n");
	printf("............................|     MENU AMMINISTRATORE    |..........................\n");
	printf("....................................................................................\n");
	printf("....................................................................................\n\n\n");

	printf(" ____ ________ ________ ____ Gestione delle Categorie ____ ________ ________ _______\n");
	printf("|                                                                                   |\n");
	printf("|                                                                    [LOG OUT: 0]   |\n");
	printf("|                                                                                   |\n");
    printf("|   OPERAZIONE 1 : Visualizzazione titolario completo                               |\n");
    printf("|   OPERAZIONE 2 : Inserimento categoria                                            |\n");
	printf("|   OPERAZIONE 3 : Aggiornamento categoria                                          |\n");
    printf("|   OPERAZIONE 4 : Cancellazione categoria                                          |\n");
	printf("|____ ________ ________ ________ ________ ________ ________ ________ ________ ______|\n\n");

	printf(" _____ ________ ________ ______ Gestione delle Aste ______ ________ ________ _______\n");
	printf("|                                                                                   |\n");
    printf("|   OPERAZIONE 5 : Inizializzazione di una nuova asta                               |\n");
    printf("|   OPERAZIONE 6 : Generazione Report di un'asta                                    |\n");
	printf("|   OPERAZIONE 7 : Visualizzazione aste indette                                     |\n");
    printf("|   OPERAZIONE 8 : Visualizzazione dello stato di un'asta                           |\n");
	printf("|____ ________ ________ ________ ________ ________ ________ ________ ________ ______|\n\n");
    printf("   Inserisci il codice dell'operazione : ");
    getInput(64, operation, false);
    op_code = atoi(operation);
    switch((int)op_code){
		case 0:
			break;

        case 1:
            op_a1();
            printf("\n\n Premi invio per tornare al Menù Amministratore...\n");
            while(getchar() != '\n'){}
            free(operation);
			printf("\e[1;1H\e[2J");
            goto start;
            break;

		case 2:
            op_a2();
            printf("\n\n Premi invio per tornare al Menù Amministratore...\n");
            while(getchar() != '\n'){}
            free(operation);
			printf("\e[1;1H\e[2J");
            goto start;
            break;

		case 3:
            op_a3();
            printf("\n\n Premi invio per tornare al Menù Amministratore...\n");
            while(getchar() != '\n'){}
            free(operation);
			printf("\e[1;1H\e[2J");
            goto start;
            break;

		case 4:
            op_a4();
            printf("\n\n Premi invio per tornare al Menù Amministratore...\n");
            while(getchar() != '\n'){}
            free(operation);
			printf("\e[1;1H\e[2J");
            goto start;
            break;
		
		case 5:
            op_a5();
            printf("\n\n Premi invio per tornare al Menù Amministratore...\n");
            while(getchar() != '\n'){}
            free(operation);
			printf("\e[1;1H\e[2J");
            goto start;
            break;

		case 6:
            op_a6();
            printf("\n\n Premi invio per tornare al Menù Amministratore...\n");
            while(getchar() != '\n'){}
            free(operation);
			printf("\e[1;1H\e[2J");
            goto start;
            break;
		
		case 7:
            op_a7();
            printf("\n\n Premi invio per tornare al Menù Amministratore...\n");
            while(getchar() != '\n'){}
            free(operation);
			printf("\e[1;1H\e[2J");
            goto start;
            break;
		
		case 8:
            op_3();
            printf("\n\n Premi invio per tornare al Menù Amministratore...\n");
            while(getchar() != '\n'){}
            free(operation);
			printf("\e[1;1H\e[2J");
            goto start;
            break;
        

        default:
            free(operation);
			printf("\e[1;1H\e[2J");
            goto start;
            break;
            
    } 
	return;
}


