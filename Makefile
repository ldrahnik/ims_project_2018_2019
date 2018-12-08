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

clean: rm -f $(PNAME) $(AFULLNAME) *.out

$(PNAME): $(PSOURCES)
	$(CC) $(CFLAGS) $(PSOURCES) $(PFLAGS)

run: ./project 

# TODO: arguments + or empty args means use defaults (+)

################# EXPERIMENTS #####################

# TODO: experiments with set of specific arguments


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
