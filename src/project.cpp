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

#define KAPACITA_NAVSTEV 100

#define NOVY_PACIENT 8		//Pacienti kteri se chteji zaregistrovat volaji v intervalech Exp(1/3 dne)

#define RUNTIME 10			//Doba, po kterou bude simulace spustena. V rocich.

Store Sestricky("Sestricky", SESTRICKY);
Store Zubari("Zubari", ZUBARI);
Store Kresla("Kresla", KRESLA);
Store Kapacita_registraci("Kapacita registraci", KAPACITA_REGISTRACI);		//Maximalni pocet pacientu, ktere bude mit ordinace ve svych zaznamech
Store Kapacita_navstev("Kapacita navstev", KAPACITA_NAVSTEV);				//Maximalni pocet registrovanych pacientu, kteri muzou byt v jednu chvili objednani

Facility Rentgen("Rentgen");

Histogram cekarna_rtg_time("Pacient ceka na rentgen", 0, 5, 20);		//Sleduje, jak dlouho pacienti cekaji na rentgen
Histogram cekarna_zakrok_time("Pacient ceka na zakrok", 0, 5, 20);	//Sleduje, jak dlouho pacienti cekaji na zakrok
Histogram sestricky_time("Sledovani sestricek", 0, 5, 20);		//Sleduje, jak dlouho trvaji sestrickam tasky

unsigned long novy_pacient = NOVY_PACIENT;

unsigned long nova_registrace = 0;
unsigned long odmitnuta_registrace = 0;
unsigned long prijmuta_registrace = 0;
unsigned long cekarna_rtg = 0;
unsigned long cekarna_zakrok = 0;
unsigned long prohlidka = 0;
unsigned long tezky_zakrok = 0;
unsigned long lehky_zakrok = 0;
unsigned long vystoupeny = 0;
unsigned long neobjednan = 0;
unsigned long objednan = 0;

double naplnena_kapacita = 0;

//proces ktery uvolni zubare po 5 minutach
class Siesta : public Process
{
	void Behavior()
	{
		Wait(Exponential(5 MINUT));
		Leave(Zubari, 1);
		this->Terminate();
	}
};

class Vstup : public Process
{
	void Behavior()
	{
		double start_cekarna;		//promena pro pocatecni cas v cekarne
		double start_sestricka;		//promena pro pocatecni cas tasku sestricky

		double rand = 0;			//promena pro pravdepodobnost

		nova_registrace++;

hovor:

		start_sestricka = Time;
		Enter(Sestricky, 1);
		Wait(Exponential(2 MINUT));		//telefoni hovor se setrickou pred registraci
		Leave(Sestricky, 1);
		sestricky_time((Time-start_sestricka)*60);

		if(!Kapacita_registraci.Full())
		{
			Enter(Kapacita_registraci, 1);
			prijmuta_registrace++;
			Wait(Exponential(90 DEN));		//cekani na termin

			if(Random() > 0.15)
			{
				start_cekarna = Time;
				start_sestricka = Time;
				cekarna_rtg++;
				Seize(Rentgen);
				Enter(Sestricky, 1);
				cekarna_rtg_time((Time - start_cekarna)*60);
				Wait(Exponential(15 MINUT));		//prubeh rentgenu
				Release(Rentgen);
				Leave(Sestricky, 1);
				sestricky_time((Time-start_sestricka)*60);

				Wait(Exponential(35 MINUT));		//vyplneni registrace
			}
			else		//pacient nedorazil na registraci
			{
				Leave(Kapacita_registraci, 1);
				goto vystup;
			}
		}
		else		//pacient se nevesel do registracni kapacity
		{
			odmitnuta_registrace++;
			goto vystup;
		}

		if(Kapacita_registraci.Full() && (naplnena_kapacita == 0))		//zaznamenaci casu zaplneni kapacity registrace
		{
			naplnena_kapacita = Time - StartTime;
		}

zakrok:

		start_cekarna = Time;
		start_sestricka = Time;
		cekarna_zakrok++;
		Enter(Kresla, 1);
		Enter(Sestricky, 1);
		Wait(Exponential(5 MINUT));		//prichystani pacienta
		cekarna_zakrok_time((Time - start_cekarna)*60);

		rand = Random();
		if(rand < 0.2)
		{
			Enter(Zubari, 1);
			Enter(Sestricky, 1);
			tezky_zakrok++;
			Wait(Exponential(90 MINUT));		//prubeh tezkeho zakroku
			Leave(Kresla, 1);
			Leave(Sestricky, 1);
			sestricky_time((Time-start_sestricka)*60);
		}
		else if(rand < 0.5)
		{
			Enter(Zubari, 1);
			prohlidka++;
			Wait(Exponential(20 MINUT));		//prubeh prohlidky
			Leave(Kresla, 1);
			(new Siesta)->Activate();
			goto bez_placeni;
		}
		else
		{
			Enter(Zubari, 1);
			lehky_zakrok++;
			Wait(Exponential(30 MINUT));		//prubeh lehkeho zakroku
			Leave(Kresla, 1);
		}
		(new Siesta)->Activate();
		Wait(Exponential(5 MINUT));			//placeni

bez_placeni:

		if(Random() < 0.2)
		{
			Leave(Sestricky, 1);
			sestricky_time((Time-start_sestricka)*60);
		}
		else				//pacient se po zakroku objedna
		{
			Enter(Kapacita_navstev, 1);
			Wait(Exponential(10 MINUT));
			Leave(Sestricky, 1);
			sestricky_time((Time-start_sestricka)*60);
			goto ceka_termin;
		}

		if(Random() < 0.005) 		//stavajici pacient uz neprijde
			goto vystup;

		Wait(Exponential(6 MESIC));		//doba nez se stavajici pacient opet objedna

objednava_se:

		start_sestricka = Time;
		Enter(Sestricky, 1);
		Wait(Exponential(0.5 MINUT));		//objednani se
		Leave(Sestricky, 1);
		sestricky_time((Time-start_sestricka)*60);

		if(!Kapacita_navstev.Full())
		{
			objednan++;
			Enter(Kapacita_navstev, 1);

ceka_termin:

			Wait(Exponential(90 DEN));		//doba terminu objednani
			Leave(Kapacita_navstev, 1);

			rand = Random();
			if(rand < 0.01)			//neprijde na termin
				goto vystup;
			else if(rand < 0.10)		//preobjedna se
				goto objednava_se;
			else					//prijde na termin
				goto zakrok;
		}
		else		//kapacita objednani je plna
		{
			neobjednan++;
			if (Random() < 0.01)	//uz se neobjedna
				goto vystup;

			else		//objedna se znovu za 5 dni
			{
				Wait(Exponential(5 DEN));
				goto objednava_se;
			}
		}
vystup:			//pacient vystoupi ze systemu
		vystoupeny++;
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
			case 's':		//pocet sestricek
				sestricky = static_cast<unsigned long>(std::atol(optarg));
				break;
			case 'z':		//pocet zubaru
				zubari = static_cast<unsigned long>(std::atol(optarg));
				break;
			case 'k':		//pocet kresel
				kresla = static_cast<unsigned long>(std::atol(optarg));
				break;
			case 'r':		//pocet registraci
				registrace = static_cast<unsigned long>(std::atol(optarg));
				break;
			case 'n':		//pocet navstev
				navstevy = static_cast<unsigned long>(std::atol(optarg));
				break;
			case 'i':		//doba mezi novymi pacienty
				novy_pacient = static_cast<unsigned long>(std::atol(optarg));
				break;
			case 'o':		//vystupni soubor
				filename = optarg;
				break;
			case 't':		//doba simulace v rocich
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
	Print("Novi Pacienti: %d\n", nova_registrace);
	Print("Prijmuti pacienti na registraci: %d\n", prijmuta_registrace);
	Print("Odmitnuti pacienti na registraci: %d\n", odmitnuta_registrace);
	Print("V cekarne na rtg: %d\n", cekarna_rtg);
	Print("V cekarne na zakrok: %d\n", cekarna_zakrok);
	Print("Objednani stali pacienti: %d\n", objednan);
	Print("Neobjednan stali pacienti: %d\n", neobjednan);
	Print("Naplneni kapacity registraci: %f dni\n", naplnena_kapacita/24);
	Print("Pacienti na lehkem zakroku: %d\n", lehky_zakrok);
	Print("Pacienti na tezkem zakroku: %d\n", tezky_zakrok);
	Print("Pacienti na prohlidce: %d\n", prohlidka);
	Print("Pacienti vystoupeni ze systemu: %d\n", vystoupeny);

	cekarna_rtg_time.Output();
	cekarna_zakrok_time.Output();

	Sestricky.Output();
	Zubari.Output();
	Kresla.Output();
	Kapacita_registraci.Output();
	Kapacita_navstev.Output();

	sestricky_time.Output();

	return EXIT_SUCCESS;
}
