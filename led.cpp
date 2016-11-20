#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>

#define LED24 "/sys/class/gpio/gpio24/value"
#define BUTTON23 "/sys/class/gpio/gpio23/value"

using namespace std;

int main()
{
	ofstream led24;
	ifstream button23;
	
	while(1)
	{
		button23.open(BUTTON23);
		string buttonState;
		getline(button23,buttonState);
		button23.close();
		if(buttonState=="1")
		{
			led24.open(LED24);	
			led24 << "1";
			led24.close();
			sleep(1);
			led24.open(LED24);
			led24 << "0";
			led24.close();
			sleep(1);
		}
	}
}