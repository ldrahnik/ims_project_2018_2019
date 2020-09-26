# Name: Drahník Lukáš, Ladislav Bezděčík
# Project: 1. Služby, infrastruktura a energetika
# Date:	21.11.2018
# Email: <xdrahn00@stud.fit.vutbr.cz>, <xbezde13@stud.fit.vutbr.cz>

################# PROJECT #########################

PNAME = project
PDOC = doc/manual.pdf
PDOC_ATTACHEMENTS = doc/attachements/*
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

# exprimenty: 5 let, hledáme vhodnou kapacitu registrací a návštěv při zvyšování generování pacientů
run:
	# experiment 1: 2 sestřičky, 1 zubař, 1 křeslo
	./project -s 2 -z 1 -k 1 -t 5 -r 1800 -n 1100 -i 24 -o experiment1.1.out
	./project -s 2 -z 1 -k 1 -t 5 -r 3200 -n 1200 -i 12 -o experiment1.2.out
	./project -s 2 -z 1 -k 1 -t 5 -r 6300 -n 1200 -i 6 -o experiment1.3.out
	./project -s 2 -z 1 -k 1 -t 5 -r 12500 -n 1200 -i 3 -o experiment1.4.out

 	# experiment 2: 2 sestřičky, 2 zubaři, 2 křesla
	./project -s 2 -z 2 -k 1 -t 5 -r 300 -n 300 -i 24 -o experiment2.1.out
	./project -s 2 -z 2 -k 1 -t 5 -r 600 -n 500 -i 12 -o experiment2.2.out
	./project -s 2 -z 2 -k 1 -t 5 -r 1000 -n 700 -i 6 -o experiment2.3.out
	./project -s 2 -z 2 -k 1 -t 5 -r 1600 -n 900 -i 3 -o experiment2.4.out

	# experiment 3: 3 sestřičky, 2 zubaři, 2 křesla
	./project -s 3 -z 2 -k 2 -t 5 -r 1600 -n 1100 -i 24 -o experiment3.1.out
	./project -s 3 -z 2 -k 2 -t 5 -r 3100 -n 1200 -i 12 -o experiment3.2.out
	./project -s 3 -z 2 -k 2 -t 5 -r 6300 -n 1200 -i 6 -o experiment3.3.out
	./project -s 3 -z 2 -k 2 -t 5 -r 12500 -n 1200 -i 3 -o experiment3.4.out

################# ARCHIVE ##########################

ANAME = 01_xbezde13_xdrahn00
ATYPE = zip
AFULLNAME = $(ANAME).$(ATYPE)
AFILES = Makefile $(PDOC) $(PSOURCES) ${PDOC_ATTACHEMENTS}

zip:
	zip $(AFULLNAME) $(AFILES)

rmzip:
	rm -f $(AFULLNAME)

unzip:
	unzip $(AFULLNAME) -d $(ANAME)

tree:
	tree -a $(ANAME)

################# LATEX ###########################

tex:
	cd doc && make && make manual.ps && make manual.pdf

cleantex: 
	cd doc && make clean
