#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <thread>
#include <mutex>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/select.h> 

using namespace std;

mutex mutexLed;
time_t now;
tm *nowinfo; 

int led(string action)
{
	ofstream fled("/sys/class/gpio/gpio21/value");
	if (fled.is_open())
	{
		fled << action;
		fled.close();
		return 1;
	}
	else cout << "LED inaccessible" << endl;
	return 0;
}

int toggle(string source)
{
	now = time(0);
	nowinfo = localtime(&now);
	if(nowinfo->tm_hour<10) cout << "0" << nowinfo->tm_hour <<":";
	else cout << nowinfo->tm_hour <<":";
	if(nowinfo->tm_min<10) cout << "0" << nowinfo->tm_min <<":";
	else cout << nowinfo->tm_min <<":";
	if(nowinfo->tm_sec<10) cout << "0" << nowinfo->tm_sec <<":";
	else cout << nowinfo->tm_sec <<" : ";

	string ledState;
	ifstream fled("/sys/class/gpio/gpio21/value");
	if (fled.is_open())
	{
		getline(fled,ledState);
		fled.close();
	}
	else cout << "Lecture LED impossible" << endl;
	
	switch(ledState[0])
	{
			case '0' : if(led("1")==1) cout << "Led allumee par " << source << endl;break;
			case '1' : if(led("0")==1) cout << "Led eteinte par " << source  << endl;break;
	}	
}

void bySwitch()
{
	int fd;
	fd_set fds;
	char buffer[2];

	// Ouvrir le fichier qui contient l'etat du switch
	if ((fd = open("/sys/class/gpio/gpio20/value", O_RDONLY)) < 0) 
	{
	     perror("Erreur ouverture de value GPIO20");
	     exit(EXIT_FAILURE);
	}
	cout << "Surveillance switch ...OK" << endl;
	while (1) 
	{
        // Preparer la table des evenements exceptionnels attendus
        FD_ZERO(& fds);
        // Avec uniquement le descripteur du fichier.
        FD_SET(fd, & fds);
        // Attente passive (pas de timeout, donc infinie...
        if (select(FD_SETSIZE, NULL, NULL, & fds, NULL) < 0) 
		{
            perror("Erreur de select");
            break;
        }
	    
		
        // Revenir au debut du fichier (lectures successives).
        lseek(fd, 0, SEEK_SET);
		
        // Lire la valeur actuelle du GPIO.
        if (read(fd, buffer, 2) != 2) 
		{
            perror("Erreur de lecture");
            break;
        }
		
		// Verrouiller l'acces au fichier qui contient l'etat de la LED
		mutexLed.lock();
		toggle("Switch");
		mutexLed.unlock();
		// Deverrouiller l'acces au fichier qui contient l'etat de la LED	
	}
	close(fd);
}// fin du thread bySwitch


void byApp()
{
	cout << "Surveillance App ...OK" << endl;

	string orderLed;
	
	while(1)
	{
		cin >> orderLed;
		if(orderLed == "TOGGLE")
		{
			
			// Verrouiller l'acces au fichier qui contient l'etat de la LED
			mutexLed.lock();
			toggle("App");
			mutexLed.unlock();
			// Deverrouiller l'acces au fichier qui contient l'etat de la LED
		}
	}
}// fin du thread byApp

int main()
{	
	//Desactiver la bufferisation
	setbuf(stdout, NULL);
	
	thread th1 (bySwitch);
	thread th2 (byApp);
	
	cout << "Gestion des GPIO 20 et 21" << endl;
	
	th1.join();
	th2.join();
	
}