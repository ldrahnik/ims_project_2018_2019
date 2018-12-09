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

#define NOVY_PACIENT 24

#define RUNTIME 10

Store Sestricky("Sestricky", SESTRICKY);
Store Zubari("Zubari", ZUBARI);
Store Kresla("Kresla", KRESLA);
Store Kapacita_registraci("Kapacita registraci", KAPACITA_REGISTRACI);
Store Kapacita_navstev("Kapacita navstev", KAPACITA_NAVSTEV);

Facility Rentgen("Rentgen");

Histogram cekarna_rtg("Pacient ceka na rentgen", 0, 5, 20);
Histogram cekarna_zakrok("Pacient ceka na zakrok", 0, 5, 20);
Histogram sestr("Sledovani sestricek", 0, 5, 20);

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
unsigned long stal_obj = 0;
unsigned long test = 0;

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
		double begin_time;
		double rand = 0;
		double start;
		novy++;

hovor:

		start=Time;
		Enter(Sestricky, 1);
		Wait(Exponential(2 MINUT));
		Leave(Sestricky, 1);
		sestr((Time-start)*60);

		if(!Kapacita_registraci.Full())
		{
			Enter(Kapacita_registraci, 1);
			prijmuti++;
			Wait(Exponential(90 DEN));

			if(Random() > 0.15)
			{
				begin_time = Time;
				v_cekarne++;
				start=Time;
				Seize(Rentgen);
				Enter(Sestricky, 1);
				cekarna_rtg((Time - begin_time)*60);
				Wait(Exponential(15 MINUT));
				Release(Rentgen);
				Leave(Sestricky, 1);
				sestr((Time-start)*60);

				Wait(Exponential(35 MINUT));
			}
			else
			{
				Leave(Kapacita_registraci, 1);
				goto vystup;
			}
		}
		else
		{
			odmitnut++;
				goto vystup;
		}

zakrok:
		begin_time = Time;
		pred_zakrokem++;
		start =Time;
		Enter(Kresla, 1);
		Enter(Sestricky, 1);
		Wait(Exponential(5 MINUT));
		cekarna_zakrok((Time - begin_time)*60);
		rand = Random();

		if(rand < 0.2)
		{
			Enter(Zubari, 1);
			double st = Time;
			Enter(Sestricky, 1);
			tezky_zakrok++;
			Wait(Exponential(90 MINUT));
			Leave(Kresla, 1);
			Leave(Sestricky, 1);
			sestr((Time-start)*60);

		}
		else if(rand < 0.5)
		{
			Enter(Zubari, 1);
			prohlidka++;
			Wait(Exponential(20 MINUT));
			Leave(Kresla, 1);
			(new Siesta)->Activate();
			goto bez_placeni;
		}
		else
		{
			Enter(Zubari, 1);
			lehky_zakrok++;
			Wait(Exponential(30 MINUT));
			Leave(Kresla, 1);
		}
		(new Siesta)->Activate();

		Wait(Exponential(5 MINUT));
bez_placeni:
		if(Random() < 0.2)
		{
			hned_neobjednani++;
			Leave(Sestricky, 1);
			sestr((Time-start)*60);

			stali++;
		}
		else
		{
			Enter(Kapacita_navstev, 1);
			hned_objednani++;
			Wait(Exponential(10 MINUT));
			Leave(Sestricky, 1);
			sestr((Time-start)*60);

			goto ceka_termin;
		}
		if(Random() < 0.005)
			goto vystup;
		Wait(Exponential(6 MESIC));

objednava_se:
		stal_obj++;
		start = Time;
		Enter(Sestricky, 1);
		Wait(Exponential(0.5 MINUT));
		Leave(Sestricky, 1);
		sestr((Time-start)*60);

		if(!Kapacita_navstev.Full())
		{
			objednan++;
			Enter(Kapacita_navstev, 1);
ceka_termin:
			Wait(Exponential(90 DEN));
			Leave(Kapacita_navstev, 1);
			rand = Random();
			if(rand < 0.01)
				goto vystup;
			else if(rand < 0.10)
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
				Wait(Exponential(5 DEN));
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
	unsigned long runtime = RUNTIME ROK;
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
				runtime = (static_cast<unsigned long>(std::atol(optarg)) ROK);
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
	Print("Novi Pacienti: %d\n", novy);
	Print("Prijmuti pacienti na registraci: %d\n", prijmuti);
	Print("Odmitnuti pacienti na registraci: %d\n", odmitnut);
	Print("V cekarne na rtg: %d\n", v_cekarne);
	Print("V cekarne na zakrok: %d\n", pred_zakrokem);
	Print("Test: %d\n", test);
	Print("Objednani stali pacienti: %d\n", stal_obj);
	Print("Neobjednan stali pacienti: %d\n", neobjednan);

	cekarna_rtg.Output();
	cekarna_zakrok.Output();

	Sestricky.Output();
	Zubari.Output();
	Kresla.Output();
	Kapacita_registraci.Output();
	Kapacita_navstev.Output();

	sestr.Output();

	return EXIT_SUCCESS;
}
