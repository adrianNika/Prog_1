/*
	Function for MCP23017 I2C :
	
	MCP_init :
		- Slave adresse 0x20
	
	MCP_DIR_IO :
		- permet de choisir quelle bit de quelle port sera en entrée ou sortie.
			- pour le MCP23017 le bit 7 (IO7 A/B) est toujours en sortie a 0.
			- si un séléctionner en entrée pour definir avec la fonction suivante
			  si que ces ports en question seront en pull-up/down.
		- utilisation du registre IODIR addr 0x00->A 0x01->B
		
	MCP_GPPU :
		- controlle des pull-up/down dur portA et B
		 - registre addr GPPUA et GPPUB
		- si on décide d'appliquer les pull-up/down
			- elle s'appliqueront que aux port concerné
		
	MCP_IO_IMPUT :
		- en entrée on dois lire le port
		
	
	MCP_IO_OUTPUT :
		- en sortie on doit ecrire sur le port
			but:
				envoyer des donnée en paramètre a la functin
		

*/