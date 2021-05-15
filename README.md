# AUCTIONS_HOUSE
Computerised system for an Auctions House.
Realizzazione di un sistema software per la gestione on-line di una casa 
d’aste. Gli attori del sistema sono di fatto gli utenti stessi, che si suddividono in utenti base ed utenti 
amministratori. Gli utenti amministratori caricano on-line le aste, gli utenti partecipano alle stesse rilanciando 
offerte. Alla stregua di un progetto di ingegneria del software, sono state necessarie fasi iterative di analisi e 
modellazione per raggiungere il prodotto finale. 
Le fasi principali della progettazione sono:
▪ Analisi dei requisiti, che comprende la disambiguazione della specifica utente; la stesura di un 
glossario dei termini e il raggruppamento dei requisiti in insiemi omogenei, per una corretta 
pianificazione del lavoro da svolgere.
▪ Progettazione concettuale, per la creazione di uno schema Entity-Relationship grezzo, che risponda ai 
requisiti concettuali di leggibilità, minimalità, correttezza e completezza, per la piena navigabilità 
dello stesso. 
▪ Progettazione logica, che prevede l’identificazione delle operazioni da implementare; l’analisi del 
volume dei dati e del costo delle operazioni. 
In questa fase, inoltre, è prevista la modifica dello schema E-R prodotto nella fase precedente, 
attraverso passaggi incrementali, quali: analisi delle ridondanze; eliminazione delle generalizzazioni; 
traduzione di entità e associazioni e costruzione di uno schema relazionale pulito.
Ritengo che questa sezione sia un passaggio cruciale nello sviluppo di un software per la gestione di 
una base di dati: lavorare in modo accurato durante questa fase può risultare la chiave di volta per i 
passaggi successivi. 
▪ Normalizzazione dello schema relazionale, per arrivare ad una rappresentazione vera e propria delle 
tabelle da modellare nel database.
L’attenzione riposta nella fase di progettazione logica ha permesso la costruzione di uno schema 
relazionale in forma normale di Boyce&Codd: di fatto, nel mio caso, non è stato necessario applicare 
algoritmi di normalizzazione allo schema. 
▪ Progettazione fisica.
La fase di progettazione fisica e la conseguente fase d’implementazione, sono strettamente correlate. 
Nella progettazione fisica, vengono identificati i privilegi relativi agli utenti, e vengono generate strutture di 
ottimizzazione dei tempi d’esecuzione, quali gli indici (di primo e di secondo livello) associati alle tabelle 
dello schema.
Il DBMS utilizzato è Mariadb, installato su sistema operativo della famiglia UNIX (Ubuntu 18.04). 
L’applicazione è sviluppata in linguaggio C, facendo uso delle API offerte dalla libreria mysql.h, su ambiente 
di sviluppo Visual Studio Code per la parte di front-end (da terminale).
