# Safe lab
Si vuole realizzare un sistema di intelligenza ambientale basato su dispositivi e tecnologie IoT per il monitoraggio di parametri ambientali e di persone all'interno di una stanza. Nello specifico, il sistema deve essere in grado di:
* Contare il numero di persone attualmente presenti nella stanza. Il rilevamento deve essere effettuato utilizzando opportuna sensoristica (es.
sensori PIR) posti in prossimità del varco/porta d'accesso; ovviamente,
occorre distinguere eventi di entrata/uscita dal varco. Non si richiede
l'utlizzo di camere e di tecniche di computer vision.
* Rilevare ed acquisire parametri ambientali della stanza, quali: temper-
atura, umidità, pressione.
* Prevedere lo storage dei dati acquisiti (numero di persone nell'ambiente,
parametri ambientali di cui sopra) in opportuno time-series database re-
alizzato in INFLUX 2.0.
* Prevedere opportune interfacce di visualizzazione dei dati dei sensori
mediante il tool GRAFANA, o mediante interfacce custom.
* Fornire tecniche di forecasting del numero di utenti nella stanza (in
altre parole: prevedere quanti utenti saranno presenti all'interno della
stanza nei prossimi X minuti) e del valore dei sensori ambientali. I valori
della previsione devono di nuovo essere memorizzati in INFLUX.
* Prevedere meccanismi di alterting nel caso in cui vengano rilevate le
seguenti situazioni di allarme: 
  * il numero di utenti e maggiore di una
soglia prestabilita;
  * i valori di temperatura/umidità/pressione sono
superiori/inferiori a soglie di riferimento. Le notifche di allarme sono in-
dirizzate all'amministratore della struttura e possono consistere nell'invio
di un'email, di un'invocazione di API REST, di un messaggio su profilo
TELEGRAM etc.
* Valutazione delle prestazioni: quanto è accurato il sistema
di contatore degli accessi? qual è il Mean Square Error (MSE) del sistema
di forecasting per i singoli sensori?
* Web of Things: progettare il sistema mediante l'architetut-
tura W3C Web of Things presentata a lezione. Individuare le Things ed
implementare le rispettive Thing Description (TD).
Tecnologie da utilizzare

E' possibile utilizzare un qualsiasi linguaggio di programmazione o
librerie a scelta dello studente. In maniera simile per quanto riguarda la
parte hardware, è possibile utilizzare una qualsiasi piattaforma di prototipiz-
zazione IoT. Se si vuole limitare al minimo la prototipizzazione circuitale, si
consiglia l'ulilizzo di board integrate con sensoristica.
