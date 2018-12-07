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

#define STALI_PACIENTI 1000

#define NOVY_PACIENT 8

Store Sestricky("Sestricky", SESTRICKY);
Store Zubari("Zubari", ZUBARI);
Store Kresla("Kresla", KRESLA);
Store Kapacita_registraci("Kapacita registraci", KAPACITA_REGISTRACI);
Store Kapacita_navstev("Kapacita navstev", KAPACITA_NAVSTEV);
Store Stali_pacienti("Stali pacienti", STALI_PACIENTI);

Facility Rentgen("Rentgen");

long novy_pacient = 0;
long odmitnut = 0;
long prijmuti = 0;
long v_cekarne = 0;
long pred_zakrokem = 0;
long prohlidka = 0;
long tezky_zakrok = 0;
long lehky_zakrok = 0;
long konec = 0;
long hned_objednani = 0;
long hned_neobjednani = 0;

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
		novy_pacient++;

hovor:

		Enter(Sestricky, 1);

		Wait(Exponential(0.03333333));

		Leave(Sestricky, 1);

		if(!Kapacita_registraci.Empty())
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
			lehky_zakrok++;
			Wait(Exponential(0.5));
			Leave(Kresla, 1);
		}
		(new Siesta)->Activate();
		Wait(Exponential(0.08333333));
bez_placeni:

		if(Random() < 0.2)
		{
			hned_neobjednani++;
			Leave(Sestricky, 1);
			Enter(Stali_pacienti, 1);
		}
		else
		{
			hned_objednani++;
			Wait(Exponential(0.16666667));
		}
vystup:
		konec++;

	}
};

class Generator : public Event
{
	void Behavior()
	{
		(new Vstup)->Activate();
		Activate(Time + Exponential(NOVY_PACIENT));
	}
};

int main()
{

	return 0;
}