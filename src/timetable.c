#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timetable.h"

TimeTableList *newTimeTableList () {
  TimeTableList *ttl = (TimeTableList *) malloc (sizeof (TimeTableList));
  ttl->count = 0;
  ttl->allocated = 10;
  ttl->trainstops = (TrainStop **) calloc (ttl->allocated, sizeof (TrainStop *));
  return ttl;
}

void deleteTimeTableList (TimeTableList *ttl) {
  int i;
  for (i = 0; i < ttl->count; i ++)  free (ttl->trainstops[i]);
  free (ttl->trainstops);
  free (ttl);
}

TrainStop *newTrainStop (TimeTableList *ttl) {
  if (ttl->count >= ttl->allocated)
  { 
	ttl->allocated *= 2;
    ttl->trainstops = (TrainStop **) realloc (ttl->trainstops, ttl->allocated * sizeof (TrainStop *));
  }
  TrainStop *a = (TrainStop *) malloc (sizeof (TrainStop));
  ttl->trainstops[ttl->count++] = a;
  return a;
}

void readTimeTableList (TimeTableList *ttl, char *fileName){
	FILE *file = fopen (fileName, "r");
	if (file == NULL)  { perror (fileName);  exit (1); }
	int r, trainId, stationId, arriveHour, arriveMin, departHour, departMin, leaver, newcomer;
	
	while (r != EOF){
		TrainStop *a = newTrainStop (ttl);
		r = fscanf(file, "%d %d %d %d %d %d %d %d", &trainId,&stationId,&arriveHour,&arriveMin,&departHour, &departMin, &leaver, &newcomer);
		
		if(r != EOF){
			a->trainId = trainId;
			a->stationId = stationId;
			a->arriveHour = arriveHour;
			a->arriveMin = arriveMin;
			a->departHour = departHour;
			a->departMin = departMin;	
		}
	}
	fclose (file);
}

StationList *getfastestRoute (StationList *sl, TimeTableList *ttl, int fromStation, int toStation, int fromHour, int fromMin) {
	int i;
	int debug = 0;
	Station *nextStation,*station,*lastStation;
	nextStation = sl->stations[0];	
	StationList *bestRoute = newStationList();
	StationList *bestRouteReversed = newStationList();
	
	for (i = 0; i < sl->count; i ++){
		station = sl->stations[i];
		station->arriveTime = 0;
		station->fromStation = 0;
	}
	
	findStation(sl, fromStation)->fromStation = -1;
	findStation(sl, fromStation)->arriveTime = timestamp(fromHour,fromMin);
	
	/*
	*	Get next rechable station until destination is reached and save fromStations and fromTrains, so the Path can be recreated
	*/
	i=0;
	while(nextStation->id != toStation){
		nextStation = getNextReachableStation(sl,ttl,fromHour,fromMin);
		i++;
		if(debug)printf("\n%s","Next Reachable Station: Id, fromStation, fromTrain, arriveTime, departTime");
		if(debug)printf("\n%d, %d, %d, %d",nextStation->id,nextStation->fromStation,nextStation->fromTrain,nextStation->arriveTime);
		if(debug)printf("\n%s\n","____________");
	}
	
	/*
	*	Go back the Route - from Destination to Start - and save the stations to bestRouteReversed
	*/
	station = nextStation;
	lastStation = nextStation;
	i=0;
	do{
		if(debug)printf("\n%s","Way back: Id, fromStation, fromTrain, arriveTime ");
		if(debug)printf("\n%d, %d, %d, %d",station->id,station->fromStation,station->fromTrain,station->arriveTime);
		Station *a = newStation(bestRouteReversed);
		
		a->id = station->id;
		*a->name = *station->name;
		a->fromStation = station->fromStation;
		a->fromTrain = station->fromTrain;
		
		
		a->arriveTime = getArriveTime(ttl,station->fromTrain,station->fromStation);
		//a->arriveTime = findStation(sl,station->fromStation)->arriveTime;
		
		
		if(lastStation != nextStation){ 
			a->departTime = getDepartTime(ttl, station->fromTrain, station->fromStation);
		}
		
		lastStation = a;
		station = findStation(sl, station->fromStation);	
		i++;
	}while(station->id != fromStation && i<sl->count);
	
	/*
	*	Reverse bestRouteReversed to bestRoute and return it
	*/
	for (i = bestRouteReversed->count-1; i >= 0; i= i-1){
		Station *a = newStation(bestRoute);
		// *a = *bestRouteReversed->stations[i]; wär hier einfacher.  Rade
		a->id = bestRouteReversed->stations[i]->id;
		*a->name = *bestRouteReversed->stations[i]->name;
		a->fromStation = bestRouteReversed->stations[i]->fromStation;
		a->fromTrain = bestRouteReversed->stations[i]->fromTrain;		
		a->arriveTime = bestRouteReversed->stations[i]->arriveTime;
		a->departTime = bestRouteReversed->stations[i]->departTime;
	}
	deleteStationList(bestRouteReversed);
	
	return bestRoute;
}

Station *getNextReachableStation (StationList *sl, TimeTableList *ttl, int fromHour, int fromMin) {
	int i,j;
	int debug=0;
	Station *station;
	TimeTableList *departures = newTimeTableList();
	StationList *possibleDestinations = newStationList();
	// also da jedesmal eine ganze neue Liste aufzubauen, ist irgendwie ein Overkill.  Man könnte für die Departures die Destinations ja gleich finden, indem man einfach in der Liste den nächsten Eintrag nimmt.  Und die früheste Destination könnte man auch gleich on-the-fly herausfinden.  Rade
	
	/*
	*	Get the Departures from all marked stations and store to departures(ttl)
	*/
	// Das könnte man sich überhaupt sparen, wenn die TrainStops einen Pointer zur Station hätten.  Dann könnte man in der Schleife unten in einer Zeile feststellen, ob der TrainStop zu einer markierten Station gehört.  Rade
	if(debug)printf("\n%s","All possible trains from station, trainid, depart");
	for (i = 0; i < sl->count; i ++){
		station = sl->stations[i];
		if (station->fromStation != 0){	
			for (j = 0; j < ttl->count; j ++){
				if (ttl->trainstops[j]->stationId == station->id && 
					timestamp(ttl->trainstops[j]->departHour,ttl->trainstops[j]->departMin) >= findStation(sl, station->id)->arriveTime){
					
					TrainStop *a = newTrainStop(departures);
					a->trainId = ttl->trainstops[j]->trainId;
					a->stationId = ttl->trainstops[j]->stationId;
					a->arriveHour = ttl->trainstops[j]->arriveHour;
					a->arriveMin = ttl->trainstops[j]->arriveMin;
					a->departHour = ttl->trainstops[j]->departHour;
					a->departMin = ttl->trainstops[j]->departMin;					
					if(debug)printf("\n %d, %d, %d, %d",a->stationId,a->trainId,a->departHour,a->departMin);
				}
			}		
		}
		
	}
	
	
	/*
	*	With all possible departures from marked stations, find all reachable, unmarked 
	*	destinations and add them to possible destinations (with arriveTime).
	*/
	TrainStop *departStop;
	TrainStop *nextdest = (TrainStop *) malloc (sizeof (TrainStop));
	TrainStop *arriveStop;
	
	for (i = 0; i < departures->count; i ++){
	
		departStop = departures->trainstops[i];
		nextdest->arriveHour = 100;
		nextdest->arriveMin = 100;
		
		for (j = 0; j < ttl->count; j ++){
			arriveStop = ttl->trainstops[j];
			if(departStop->trainId == arriveStop->trainId){			
				if(timestamp(arriveStop->arriveHour,arriveStop->arriveMin) < timestamp(nextdest->arriveHour,nextdest->arriveMin) && 
							timestamp(arriveStop->arriveHour,arriveStop->arriveMin) > timestamp(departStop->departHour,departStop->departMin)){
					
					//nextdest = ttl->trainstops[j];				
					nextdest->trainId = arriveStop->trainId;
					nextdest->stationId = arriveStop->stationId;
					nextdest->arriveHour = arriveStop->arriveHour;
					nextdest->arriveMin = arriveStop->arriveMin;
					nextdest->departHour = arriveStop->departHour;
					nextdest->departMin = arriveStop->departMin;
				}
				
			}
		}
		
		if(findStation(sl,nextdest->stationId)->fromStation == 0){
			Station *a = newStation (possibleDestinations);
			a->id = nextdest->stationId;
			if(findStation(sl,departStop->stationId)->fromTrain == nextdest->trainId){
				a->fromStation = findStation(sl,departStop->stationId)->fromStation;
			}
			else{
				a->fromStation = departStop->stationId;
			}
			a->fromTrain = departStop->trainId;		
			a->arriveTime = timestamp(nextdest->arriveHour,nextdest->arriveMin);
		}
		
	}
	free(nextdest);
	deleteTimeTableList(departures);
	
	if(possibleDestinations->count == 0){exit(1);}
	
	/*
	*	Using possibleDestinations, find the Destination, that is the shortest afar from a not marked 
	*	station (has the lowest arriveTime) and return it.
	*/
	if(debug)printf("\n\n%s", "Possible Destinations - station, fromstation, fromtrain, arrivetime");
	if(debug)printf("\n%d,%d, %d, %d",possibleDestinations->stations[0]->id,possibleDestinations->stations[0]->fromStation,
					possibleDestinations->stations[0]->fromTrain,possibleDestinations->stations[0]->arriveTime);	
	int lowest = 0;
	
	for (i = 0; i < possibleDestinations->count; i ++){
		if(possibleDestinations->stations[i]->arriveTime < possibleDestinations->stations[lowest]->arriveTime){
			lowest = i;
		}
		if(debug)printf("\n%d,%d, %d, %d",possibleDestinations->stations[i]->id,possibleDestinations->stations[i]->fromStation,
						possibleDestinations->stations[i]->fromTrain,possibleDestinations->stations[i]->arriveTime);
	}
	if(debug)printf("\nlowest %d,%d, %d,\n",possibleDestinations->stations[lowest]->id,possibleDestinations->stations[lowest]->fromTrain,
					possibleDestinations->stations[lowest]->arriveTime);
	
	
	Station *finalStation = findStation(sl,possibleDestinations->stations[lowest]->id);	
	finalStation->arriveTime = possibleDestinations->stations[lowest]->arriveTime;
	finalStation->fromStation = possibleDestinations->stations[lowest]->fromStation;
	finalStation->fromTrain = possibleDestinations->stations[lowest]->fromTrain;
	deleteStationList(possibleDestinations);
	
	return finalStation;
}

int getArriveTime(TimeTableList *ttl, int trainId, int stationId){
	int i;
	int lowestTime = 100000;
	for (i = 0; i < ttl->count; i ++){
		if(ttl->trainstops[i]->trainId == trainId && ttl->trainstops[i]->stationId == stationId){
			lowestTime = timestamp(ttl->trainstops[i]->arriveHour,ttl->trainstops[i]->arriveMin);
		}
	}	
	return lowestTime;
}


int getDepartTime(TimeTableList *ttl, int trainId, int stationId){
	int i;
	int lowestTime = 100000;
	for (i = 0; i < ttl->count; i ++){
		if(ttl->trainstops[i]->trainId == trainId && ttl->trainstops[i]->stationId == stationId){
			return timestamp(ttl->trainstops[i]->departHour,ttl->trainstops[i]->departMin);
		}
	}
	
	return lowestTime;
}

// Wenn man hier z.B. ein "int timestamp(TranStop *a)" hätte, würde das Programm kürzer und leichter lesbar werden.  Rade
int timestamp(int hour, int min){
	return hour*60*60+min*60;
}
