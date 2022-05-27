#include "station.h"
#include "timetable.h"
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{	
	if (argc != 8)
	{ 
		fputs ("usage: stationlister station.dat train.dat timetable.dat fromStation fromHour fromMin toStation \n", stderr);
		exit (1);
	}

	int i;
	StationList *sl = newStationList();
	readStationList (sl, argv[1]);
	TimeTableList *ttl = newTimeTableList();
	readTimeTableList (ttl, argv[3]);
	StationList *bestRoute;

	// Die Bahnhöfe sollte auf der Kommandozeile mit Namen angegeben werden, nicht mit Nummer.  Rade
	bestRoute = getfastestRoute(sl, ttl, atoi(argv[4]), atoi(argv[7]), atoi(argv[5]), atoi(argv[6]));
	
	printf("%14s%14s%14s%14s%14s\n","from","to","with Train","arriveTime","departTime");
	for (i = 0; i < bestRoute->count; i++){

		printf("%14d%14d%14d",bestRoute->stations[i]->fromStation,bestRoute->stations[i]->id,bestRoute->stations[i]->fromTrain);
		printf("%7d%7d",bestRoute->stations[i]->arriveTime/60/60,bestRoute->stations[i]->arriveTime/60%60);
		printf("%7d%7d\n",bestRoute->stations[i]->departTime/60/60,bestRoute->stations[i]->departTime/60%60);
		
	}

	free (sl); // Hier sollte man wohl deleteStationList und so machen.  Rade
	free (ttl);
	free (bestRoute);
	return 0;
}
