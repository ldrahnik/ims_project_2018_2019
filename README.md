ASSESSMENT 
==========

20/20b (no assessment report)

Modelování a simulace: Stomatologická ordinace
==========

![ScreenShot](https://raw.github.com/ldrahnik/ims_project_2018_2019/master/doc/attachements/petri_nets.png)

## Příklad spuštění:

```
make run
# experiment 1: 2 sestřičky, 1 zubař, 1 křeslo
./project -s 2 -z 1 -k 1 -t 5 -r 1800 -n 1100 -i 24 -o experiment1.1.out
./project -s 2 -z 1 -k 1 -t 5 -r 3200 -n 1200 -i 12 -o experiment1.2.out
./project -s 2 -z 1 -k 1 -t 5 -r 6300 -n 1200 -i 6 -o experiment1.3.out
./project -s 2 -z 1 -k 1 -t 5 -r 12500 -n 1200 -i 3 -o experiment1.4.out
./project -s 2 -z 2 -k 1 -t 5 -r 300 -n 300 -i 24 -o experiment2.1.out
./project -s 2 -z 2 -k 1 -t 5 -r 600 -n 500 -i 12 -o experiment2.2.out
./project -s 2 -z 2 -k 1 -t 5 -r 1000 -n 700 -i 6 -o experiment2.3.out
./project -s 2 -z 2 -k 1 -t 5 -r 1600 -n 900 -i 3 -o experiment2.4.out
# experiment 3: 3 sestřičky, 2 zubaři, 2 křesla
./project -s 3 -z 2 -k 2 -t 5 -r 1600 -n 1100 -i 24 -o experiment3.1.out
./project -s 3 -z 2 -k 2 -t 5 -r 3100 -n 1200 -i 12 -o experiment3.2.out
./project -s 3 -z 2 -k 2 -t 5 -r 6300 -n 1200 -i 6 -o experiment3.3.out
./project -s 3 -z 2 -k 2 -t 5 -r 12500 -n 1200 -i 3 -o experiment3.4.out
```

## Testování programu:

## Odevzdané soubory:

```
01_xbezde13_xdrahn00
├── doc
│   ├── attachements
│   │   └── petri_nets.png
│   └── manual.pdf
├── Makefile
└── src
    └── project.cpp

3 directories, 4 files
```
