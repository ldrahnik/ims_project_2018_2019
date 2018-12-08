/**
 * Name: Drahník Lukáš, Ladislav Bezděčík
 * Project: 1. Služby, infrastruktura a energetika
 * Date: 21.11.2018
 * Email: <xdrahn00@stud.fit.vutbr.cz>, <xbezde13@stud.fit.vutbr.cz>
 */

#include "simlib.h"
#include <iostream>
#include <getopt.h>
#include <stdio.h>
#include <cstdlib>

#define DEN * 24

#define MESIC * 730

#define ROK * 8760

#define MINUT / 60

#define SESTRICKY 3

#define ZUBARI 2

#define KRESLA 2

#define KAPACITA_REGISTRACI 100

#define KAPACITA_NAVSTEV 1000

#define NOVY_PACIENT 8

#define RUNTIME 3650

Store Sestricky("Sestricky", SESTRICKY);
Store Zubari("Zubari", ZUBARI);
Store Kresla("Kresla", KRESLA);
Store Kapacita_registraci("Kapacita registraci", KAPACITA_REGISTRACI);
Store Kapacita_navstev("Kapacita navstev", KAPACITA_NAVSTEV);

Facility Rentgen("Rentgen");

unsigned long novy_pacient = NOVY_PACIENT;

unsigned long novy = 0;
unsigned long odmitnut = 0;
unsigned long prijmuti = 0;
unsigned long v_cekarne = 0;
unsigned long pred_zakrokem = 0;
unsigned long prohlidka = 0;
unsigned long tezky_zakrok = 0;
unsigned long lehky_zakrok = 0;
unsigned long konec = 0;
unsigned long hned_objednani = 0;
unsigned long hned_neobjednani = 0;
unsigned long neobjednan = 0;
unsigned long objednan = 0;
unsigned long stali = 0;

class Siesta : public Process
{
	void Behavior()
	{
		Wait(Exponential(0.08333333));
		Leave(Zubari, 1);
		this->Terminate();
	}
};

class Vstup : public Process
{
	void Behavior()
	{
		double rand = 0;
		novy++;

hovor:

		Enter(Sestricky, 1);

		Wait(Exponential(0.03333333));
		Leave(Sestricky, 1);

		if(!Kapacita_registraci.Full())
		{
			Enter(Kapacita_registraci, 1);
			prijmuti++;
			Wait(Exponential(2160));
			Leave(Kapacita_registraci, 1);

			if(Random() > 0.15)
			{
				v_cekarne++;
				Enter(Sestricky, 1);
				Seize(Rentgen);
				Wait(Exponential(0.25));
				Release(Rentgen);
				Leave(Sestricky, 1);
				Wait(Exponential(0.58333333));
			}
			else
			{
				goto vystup;
			}
		}
		else
		{
			odmitnut++;
			if(Random() > 0.5)
			{
				Wait(Exponential(120));
				goto hovor;
			}
			else
				goto vystup;
		}

zakrok:

		pred_zakrokem++;
		Enter(Sestricky, 1);
		Enter(Kresla, 1);
		rand = Random();

		if(rand < 0.2)
		{
			Enter(Zubari, 1);
			Enter(Sestricky, 1);
			tezky_zakrok++;
			Wait(Exponential(0.83333333));
			Leave(Kresla, 1);
		}
		else if(rand < 0.5)
		{
			Enter(Zubari, 1);
			prohlidka++;
			Wait(Exponential(0.33333333));
			Leave(Kresla, 1);
			(new Siesta)->Activate();
			goto bez_placeni;
		}
		else
		{
			Enter(Zubari, 1);
			lehky_zakrok++;
			Wait(Exponential(0.5));
			Leave(Kresla, 1);
		}
		(new Siesta)->Activate();

		Wait(Exponential(0.08333333));
bez_placeni:
		Wait(Exponential(0.08333333));
		if(Random() < 0.2)
		{
			hned_neobjednani++;
			Leave(Sestricky, 1);
			stali++;
		}
		else
		{
			Enter(Kapacita_navstev, 1);
			hned_objednani++;
			Wait(Exponential(0.16666667));
			Leave(Sestricky, 1);
			goto ceka_termin;
		}
		if(Random() < 0.005)
			goto vystup;
		Wait(Exponential(2190));
objednava_se:
		Enter(Sestricky, 1);
		Wait(Exponential(0.00833333));
		Leave(Sestricky, 1);

		if(!Kapacita_navstev.Full())
		{
			objednan++;
			Enter(Kapacita_navstev, 1);
ceka_termin:
			Wait(Exponential(2160));
			Leave(Kapacita_navstev, 1);
			rand = Random();
			if(rand < 0.01)
				goto vystup;
			else if(rand < 0.09)
				goto objednava_se;
			else
				goto zakrok;

		}
		else
		{
			neobjednan++;
			if (Random() < 0.01)
				goto vystup;
			else
			{
				Wait(Exponential(120));
				goto objednava_se;
			}
		}
vystup:
		konec++;
		stali--;

	}
};

class Generator : public Event
{
	void Behavior()
	{
		(new Vstup)->Activate();
		Activate(Time + Exponential(novy_pacient));
	}
};

int main(int argc, char *argv[])
{
	int opt;

	unsigned long sestricky = SESTRICKY;
	unsigned long zubari = ZUBARI;
	unsigned long kresla = KRESLA;
	unsigned long registrace = KAPACITA_REGISTRACI;
	unsigned long navstevy = KAPACITA_NAVSTEV;
	unsigned long runtime = RUNTIME DEN;
	std::string filename = "output.out";

	while((opt = getopt(argc, argv, "s:z:k:r:n:i:o:t:")) != -1)
	{
		switch(opt)
		{
			case 's':
				sestricky = static_cast<unsigned long>(std::atol(optarg));
				break;
			case 'z':
				zubari = static_cast<unsigned long>(std::atol(optarg));
				break;
			case 'k':
				kresla = static_cast<unsigned long>(std::atol(optarg));
				break;
			case 'r':
				registrace = static_cast<unsigned long>(std::atol(optarg));
				break;
			case 'n':
				navstevy = static_cast<unsigned long>(std::atol(optarg));
				break;
			case 'i':
				novy_pacient = static_cast<unsigned long>(std::atol(optarg));
				break;
			case 'o':
				filename = optarg;
				break;
			case 't':
				runtime = (static_cast<unsigned long>(std::atol(optarg)) DEN);
				break;
			default:
				break;
		}
	}

	Sestricky.SetCapacity(sestricky);
	Zubari.SetCapacity(zubari);
	Kresla.SetCapacity(kresla);
	Kapacita_registraci.SetCapacity(registrace);
	Kapacita_navstev.SetCapacity(navstevy);

	SetOutput(filename.c_str());

	Init(0, runtime);

	(new Generator)->Activate();

	Run();

	Print("Runtime: %d\n", runtime);
	Print("Novych Pacientu: %d\n", novy);

	Sestricky.Output();
	Zubari.Output();
	Kresla.Output();
	Kapacita_registraci.Output();
	Kapacita_navstev.Output();

	return EXIT_SUCCESS;
}