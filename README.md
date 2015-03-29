# Introduction #
Robocar World Championship is intended to offer a common research platform for developing urban traffic control algorithms and for investigating the relationship between smart cities and robot cars with particular attention to spread of robot cars of the near future. At the heart of this initiative is the Robocar City Emulator. It will enable researchers to test and validate their theories and models.

The following figure shows a high-level "Tetris" plan of the project
![http://m.cdn.blog.hu/pr/progpater/image/tetris_plan2.400x.png](http://m.cdn.blog.hu/pr/progpater/image/tetris_plan2.400x.png)

  * **Map**: Competitions are played on an OSM ([OpenStreetMap](https://www.openstreetmap.org/about)) map.
  * **City**: Each competition is assigned to a city.
  * **ASA**: (Automated Sensor Annotations) This subsystem will automatically collect traffic data using video cameras. The collected data will be used in the [smartcity](https://code.google.com/p/robocar-emulator/source/browse/justine/rcemu/src/smartcity.hpp) program.
  * **HSA**: (Human controlled Sensor Annotations)
  * **Robocar City Emulator**:
  * **The competition**:
  * **Results**:
  * **Monitors**:

We already have a working prototype called [Justine](https://github.com/nbatfai/robocar-emulator/tree/master/justine). In the spirit of RERO (_"release early, release often"_) it has been released in _"version 0.1 Police edition"_ for the competition called _"Debrecen 1"_. This initial rapid prototype has very limited features but we think it can be used to start building a community of people who share an interest in this subject.
  * Each competition is assigned to a city. Competitions are played on a rectangular area of the OSM ([OpenStreetMap](https://www.openstreetmap.org)) map of the given city that is defined by GPS coords minlat, minlon, maxlat and maxlon. This area is referred to as the "City Operating Area".
  * [Routine cars](https://github.com/nbatfai/robocar-emulator/blob/master/justine/rcemu/src/car.hpp), [smart cars and guided cars](https://github.com/nbatfai/robocar-emulator/blob/master/justine/rcemu/src/car.hpp) may be placed onto the City Operating Area in which cop multiagents pursue gangster agents for ten minutes. Cops are guided cars and these are controlled by the competing teams.

# How to Participate #

## Installation of related software ##
First of all, you will need a map. For example, let's choose Berlin. The OSM Berlin map can be found at https://www.openstreetmap.org/export#map=10/52.5074/13.4225 The OSM XML file can be exported from this page. (If the selected region is too large you can use the mirror links around the map.) In the following, it is assumed that you will save the exported OSM map in the `berlin.osm` file.

The following steps can also be seen in a YouTube video at http://youtu.be/_FNoFqlygyE

<a href='http://www.youtube.com/watch?feature=player_embedded&v=_FNoFqlygyE' target='_blank'><img src='http://img.youtube.com/vi/_FNoFqlygyE/0.jpg' width='560' height=315 /></a>

### Robocar City Emulator ###
You can obtain the source code by cloning the git repo with command `git clone https://github.com/nbatfai/robocar-emulator.git`

```
[nbatfai@desteny temp]$ git clone https://code.google.com/p/robocar-emulator/
Cloning into 'robocar-emulator'...
remote: Counting objects: 473, done.
remote: Compressing objects: 100% (216/216), done.
remote: Total 473 (delta 204), reused 468 (delta 204), pack-reused 0
Receiving objects: 100% (473/473), 291.21 KiB | 0 bytes/s, done.
Resolving deltas: 100% (204/204), done.
[nbatfai@desteny temp]$
```


The `rcemu` directory contains the sources of Robocar City Emulator that is a GNU autoconf based project.
Change to this directory and enter the command `autoreconf --install` to generate the `configure` script.

```
[nbatfai@desteny rcemu]$ autoreconf --install
libtoolize: putting auxiliary files in `.'.
libtoolize: copying file `./ltmain.sh'
configure.ac:42: installing './config.guess'
configure.ac:42: installing './config.sub'
configure.ac:22: installing './install-sh'
configure.ac:22: installing './missing'
src/Makefile.am: installing './depcomp'
configure.ac: installing './ylwrap'
[nbatfai@desteny rcemu]$
```

The configure script checks for presence of headers and libraries required to build Robocar City Emulator:

```
[nbatfai@desteny rcemu]$ ./configure 
...
checking for the Boost thread library... (cached) yes
checking boost/asio.hpp usability... yes
checking boost/asio.hpp presence... yes
checking for boost/asio.hpp... yes
checking for osmium/osm/relation.hpp... yes
checking for osmium/io/any_input.hpp... yes
checking for library containing shm_open... -lrt
checking for shm_open... yes
checking for flex... flex
checking lex output file root... lex.yy
checking lex library... none needed
checking whether yytext is a pointer... no
checking for flex... flex
checking that generated files are newer than configure... done
configure: creating ./config.status
config.status: creating Makefile
config.status: creating src/Makefile
config.status: executing depfiles commands
config.status: executing libtool commands
[nbatfai@desteny rcemu]$ 
```

It is likely you will see some error messages due to missing packages like libosmium or Boost. (If you cannot fix them you may find them in the [FAQ](FAQ.md) or you can ask on the [mailing list](https://groups.google.com/forum/?hl=hu#!forum/rcemu).)

If you have no errors you can build the program using `make`:

```
[nbatfai@desteny rcemu] make
...
make[1]: Leaving directory `/home/nbatfai/ROBOCAR/temp/robocar-emulator/justine/rcemu/src'
make[1]: Entering directory `/home/nbatfai/ROBOCAR/temp/robocar-emulator/justine/rcemu'
make[1]: Nothing to be done for `all-am'.
make[1]: Leaving directory `/home/nbatfai/ROBOCAR/temp/robocar-emulator/justine/rcemu'
[nbatfai@desteny rcemu]
```

After compiling, you can run the program `smartcity`.

```
[nbatfai@desteny rcemu]$ src/smartcity --osm=../berlin.osm --city=Berlin --shm=BerlinSharedMemory --node2gps=../berlin-lmap.txt
```

To run the **traffic server**, open another command window and start the command `traffic`:

```
[nbatfai@desteny rcemu]$ src/traffic --port=10007 --shm=BerlinSharedMemory
```


### Car Window ###

To build the display program, open a third command window and change to directory `rcwin`. It is a Java project based on Maven. It can be built using the command `mvn`:

```
[nbatfai@desteny rcwin]$ mvn clean compile package site assembly:assembly
```

To run the display program, enter the following command:

```
[nbatfai@desteny rcwin]$ java -jar target/site/justine-rcwin-0.0.16-jar-with-dependencies.jar ../berlin-lmap.txt 
```

### Testing the components together ###
Open a further terminal window and using the following command line:

```
[nbatfai@desteny ~]$ (sleep 1; echo "<init Norbi 500 g>"; sleep 1)|telnet localhost 10007
```

500 gangster agents will be connected to the traffic server.

And finally start our own agent program

```
[nbatfai@desteny rcemu]$ src/samplemyshmclient --port=10007 --shm=BerlinSharedMemory --team=BerlinPolice
```

If everything is OK, you will see all agents in the display window as it is shown in the following pictures:

![http://robocar.inf.unideb.hu/~norbi/images/berlinpolice1.png](http://robocar.inf.unideb.hu/~norbi/images/berlinpolice1.png)

![http://robocar.inf.unideb.hu/~norbi/images/berlinpolice2.png](http://robocar.inf.unideb.hu/~norbi/images/berlinpolice2.png)

## Submission ##
The submission process is divided into the following two steps.

### Submission of Team Qualification Paper ###
Each team must fill out the [Team Qualification Paper](https://github.com/nbatfai/robocar-emulator/tree/master/doc/qualification) and return it to [Robocar World Championship - Robocar City Emulator](https://groups.google.com/forum/?hl=hu#!forum/rcemu) mailing list. The file `rcarwcup-qualification.xml` is a _**DocBook 5.1 XML**_ document that can be edited by any text editor. Please submit it in PDF form. Creating PDF is as simple as typing `make` in the `qualification` directory:

```
[nbatfai@desteny qualification]$ make
rm -vf rcarwcup-qualification.pdf rcarwcup-qualification.epub
xmllint --xinclude rcarwcup-qualification.xml --output output.xml
xmllint --relaxng http://docbook.org/xml/5.1CR3/rng/docbookxi.rng output.xml --noout
output.xml validates
rm -f output.xml
dblatex -T native rcarwcup-qualification.xml -p robocaremu-hu.xls
Build the book set list...
Build the listings...
XSLT stylesheets DocBook - LaTeX 2e (0.3.5)
===================================================
Stripping NS from DocBook 5/NG document.
Processing stripped document.
Image 'dblatex' not found
Image '' not found
Build rcarwcup-qualification.pdf
'rcarwcup-qualification.pdf' successfully built
[nbatfai@desteny qualification]$
```

### Submission of the source code ###
Your software must be released under an open source license (like the GPL) and should run under GNU/Linux systems. The preferred build tools for building your software are CMake, GNU autoconf/automake or Apache Maven. Your program can only be submitted as source package created by one of these build tools. Submissions may be sent directly to Organizers or to [Robocar World Championship - Robocar City Emulator](https://groups.google.com/forum/?hl=hu#!forum/rcemu) mailing list.

#### The royal road to programming your own team ####
A sample program (called [SampleClient](https://github.com/nbatfai/robocar-emulator/blob/master/justine/rcemu/src/myshmclient.hpp)) has been written to demonstrate how client agent can communicate with traffic server through TCP. It is written in C++11 and uses Boost Graph Library to implement routing. It creates the BGL graph from the map graph that is placed in the shared memory segment by the [smartcity](https://github.com/nbatfai/robocar-emulator/blob/master/justine/rcemu/src/smartcity.hpp) program.

If you use this sample program as the basis for your own software you may submit the package created by the command **make dist-bzip2**:

```
[nbatfai@desteny rcemu]$ make dist-bzip2
[nbatfai@desteny rcemu]$ ls -l *.bz2
-rw-rw-r--. 1 nbatfai nbatfai 526623 Dec 26 22:28 justine-0.0.17.tar.bz2
```

In this case, it is enough to edit the [myshmclient.cpp](https://github.com/nbatfai/robocar-emulator/blob/master/justine/rcemu/src/myshmclient.hpp). (But, of course, you may write your client agents from scratch using any programming languages such as C++11, Java, Python or R.)

# Accepted competitions #

## 2015 ##

### Europe/Hungary/Debrecen 3 ###

The purpose of this competition is to start building a community.

#### Organizers ####
Norbert Bátfai, PhD., University of Debrecen, IT Dept.
batfai.norbert@inf.unideb.hu
Competition announcement: https://www.facebook.com/events/709821489135286/

#### Results ####
  * TQPs: http://justine.inf.unideb.hu/2015/Europe/Hungary/Debrecen/3/TQPs/
  * Traffic logs: http://justine.inf.unideb.hu/2015/Europe/Hungary/Debrecen/3/TrafficLogs/
  * Competition report: http://justine.inf.unideb.hu/2015/Europe/Hungary/Debrecen/3/CompetitionReport/


### Europe/Hungary/Debrecen 2 ###

The purpose of this competition is to start building a community.

#### Organizers ####
Norbert Bátfai, PhD., University of Debrecen, IT Dept.
batfai.norbert@inf.unideb.hu
Competition announcement: https://www.facebook.com/events/755155657892240/

#### Competition Rules ####
  * There is no entry fee.
  * The competition do not require personal presence. (Moreover, competitors cannot be present in person.)
  * There is no reward or prize. (Play the game for the game's sake :)
  * Each team play each other team twice therefore there are N(N-1) rounds where N denotes the number of teams.
  * The smartcity server: Ubuntu 14.04.1 LTS (Trusty Tahr), gcc version 4.9.1, Boost 1.55

#### Schedules ####
  * Deadline for submission of Team Qualification Paper:  20 January.
  * Deadline for submission of the latest source code version:  24 January.
  * Competition rounds: 28 January.
  * Competition report: 30 January.

#### Results ####
  * TQPs: http://justine.inf.unideb.hu/2015/Europe/Hungary/Debrecen/2/TQPs/
  * Traffic logs: http://justine.inf.unideb.hu/2015/Europe/Hungary/Debrecen/2/TrafficLogs/
  * Competition report: http://justine.inf.unideb.hu/2015/Europe/Hungary/Debrecen/2/CompetitionReport/


---

If you wish to create your own competition please send us the following data:

### Continent/Country/City-number ###

The purpose of the competition: ...

#### Organizers ####
...
Competition announcement: ...

#### Competition Rules ####
  * ...
  * The smartcity server: ...

#### Schedules ####
  * Deadline for submission of Team Qualification Paper:  ...
  * Deadline for submission of the latest source code version:  ...
  * Competition rounds: ...
  * Competition report: ...

#### Results ####
  * TQPs:
  * Traffic logs:
  * Competition report:

---


## 2014 ##

### Europe/Hungary/Debrecen 1 ###

The purpose of this competition is to start gathering the first experiences and finding bugs in the source code.

#### Organizers ####
Norbert Bátfai, PhD., University of Debrecen, IT Dept.
batfai.norbert@inf.unideb.hu

#### Competition Rules ####
  * There is no entry fee.
  * The competition do not require personal presence. (Moreover, competitors cannot be present in person.)
  * There is no reward or prize. (Play the game for the game's sake :)
  * Each team play each other team twice therefore there are N(N-1) rounds where N denotes the number of teams.
  * The smartcity server: Ubuntu 14.04.1 LTS (Trusty Tahr), gcc version 4.9.1, Boost 1.55

#### Schedules ####
  * Deadline for submission of Team Qualification Paper: 16 December.
  * Deadline for submission of the latest source code version: 17 December.
  * Competition rounds: 18-19 December.
  * Competition report: 20 December.

#### Results ####
  * TQPs: http://robocar.inf.unideb.hu/~norbi/2014.12.18/TDPs/
  * Traffic logs: http://robocar.inf.unideb.hu/~norbi/2014.12.18/trafficlogs/
  * Competition report: http://robocar.inf.unideb.hu/~norbi/2014.12.18/TDPs/RobocarJegyzokonyvDebrecen2014_12_18_19.pdf (only Hungarian)

# Robocar World Championship Jargon #
  * **City Operating Area**: a city operating area is the rectangular part of the OSM map in which the competitions are played.

# World Ranking #

## World Ranking progression (Police Edition) ##

### 2015 ###

| **Team** | **Date** | **Points** |
|:---------|:---------|:-----------|
| RSM | 2015 | 68 |
| Kurea | 2015 | 64 |
| PoniPolice | 2015 | 50 |
| SampleClient | 2015 | 44 |
| FoTeL | 2015 | 42 |
| FTS | 2015 | 20 |

### 2014 ###

| **Team** | **Date** | **Points** |
|:---------|:---------|:-----------|
|FoTeL|2014| 37|
|PoniPolice|2014| 37|
|RoboSheriff|2014| 33|
|SampleClient|2014| 25|
|BDSRSTNT|2014| 7|
|B0LEX|2014| 7|

# World Records #

## World record progression (Police Edition) ##

| **Team** | **Date** | **Competition** | **Gangsters** |
|:---------|:---------|:----------------|:--------------|
|FoTeL|12/2014|Debrecen-1|100/53|
