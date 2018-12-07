/**
 * Name: Drahník Lukáš, Ladislav Bezděčík
 * Project: 1. Služby, infrastruktura a energetika
 * Date: 21.11.2018
 * Email: <xdrahn00@stud.fit.vutbr.cz>, <xbezde13@stud.fit.vutbr.cz>
 */

#include "simlib.h"
#include <iostream>

#define SESTRICKY 3

#define ZUBARI 2

#define KRESLA 2

#define KAPACITA_REGISTRACI 100

#define KAPACITA_NAVSTEV 1000

#define NOVY_PACIENT 8

Store Sestricky("Sestricky", SESTRICKY);
Store Zubari("Zubari", ZUBARI);
Store Kresla("Kresla", KRESLA);
Store Kapacita_registraci("Kapacita registraci", KAPACITA_REGISTRACI);
Store Kapacita_navstev("Kapacita navstev", KAPACITA_NAVSTEV);

Facility Rentgen("Rentgen");

long novy_pacient = 0;
long odmitnut = 0;
long prijmuti = 0;
long v_cekarne = 0;



class Enter : public Process
{
	void Behavior()
	{
		novy_pacient++;

		Enter(Sestricky, 1);

		Wait(Exponential(0.03333333));

		Leave(Sestricky, 1);

		if(!Kapacita_navstev.Empty())
		{
			Enter(Kapacita_navstev, 1);
			prijmuti++;
			Wait(Exponential(2160));

			if(Random() > 0.15)
			{
				v_cekarne++;
			}
			else
			{
				Leave(Kapacita_navstev, 1);
				goto exit;
			}
		}
		else
		{

		}
	}
};

class Generator : public Event
{
	void Behavior()
	{
		(new Enter)->Activate();
		Activate(Time + Exponential(NOVY_PACIENT));
	}
};

int main(int argc, char *argv[])
{
	#if DEBUG
		Print("Hello World\n");
	#endif
}
