# Name: Drahník Lukáš, Ladislav Bezděčík
# Project: 1. Služby, infrastruktura a energetika
# Date:	21.11.2018
# Email: <xdrahn00@stud.fit.vutbr.cz>, <xbezde13@stud.fit.vutbr.cz>

################# PROJECT #########################

PNAME = project
PDOC = doc/manual.pdf
PSOURCES = src/*.cpp
PFLAGS = -lsimlib -lm

################# FLAGS ###########################

CC = g++
CFLAGS = -g -O2

################# SCRIPTS #########################

all: $(PNAME)

rebuild: clean all

clean: 
	rm -f $(PNAME) $(AFULLNAME) *.out

$(PNAME): $(PSOURCES)
	$(CC) $(CFLAGS) $(PSOURCES) $(PFLAGS) -o ./$(PNAME)


################# EXPERIMENTS #####################

# exprimenty: 5 let, 1 nový pacient / den, hledáme vhodnou kapacitu registrací a návštěv
run:
	# experiment 1: 2 sestřičky, 1 zubař, 1 křeslo
	./project -s 2 -z 1 -k 1 -t 5 -r 100 -n 100 -i 24 -o experiment1.out # TODO: vhodná kapacita

 	# experiment 2: 2 sestřičky, 2 zubaři, 1 křeslo
	./project -s 2 -z 2 -k 1 -t 5 -r 100 -n 100 -i 24 -o experiment2.out # TODO: vhodná kapacita

	# experiment 3: 2 sestřičky, 2 zubaři, 2 křesla
	./project -s 2 -z 2 -k 2 -t 5 -r 100 -n 100 -i 24 -o experiment3.out # TODO: vhodná kapacita

 	# experiment 4: 3 sestřičky, 2 zubaři, 2 křesla
	./project -s 2 -z 2 -k 2 -t 5 -r 100 -n 100 -i 24 -o experiment4.out # TODO: vhodná kapacita


################# ARCHIVE ##########################

ANAME = 01_xbezde13_xdrahn00
ATYPE = zip
AFULLNAME = $(ANAME).$(ATYPE)
AFILES = Makefile $(PDOC) $(PSOURCES)

zip:
	zip $(AFULLNAME) $(AFILES)

rmzip:
	rm -f $(AFULLNAME)

################# LATEX ###########################

tex:
	cd doc && make && make manual.ps && make manual.pdf

cleantex: 
	cd doc && make clean
