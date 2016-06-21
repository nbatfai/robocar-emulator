# Introduction #
Robocar World Championship (OOCWC = rObOCar World Championship) 
is intended to offer a common research platform for developing urban traffic control algorithms and for investigating the relationship between smart cities and robot cars with particular attention to spread of robot cars of the near future. At the heart of this initiative is the Robocar City Emulator. It will enable researchers to test and validate their theories and models.

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

## SamuSmartCity - Everybody has been travelling in a car since nobody got a car.

This is a new edition of Police edition of OOCWC which has been called "There is no own car"
edition, for detailed information see https://github.com/nbatfai/SamuSmartCity 