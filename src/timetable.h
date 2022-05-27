#ifndef TRAINSTOP_H
#define TRAINSTOP_H
#include "station.h"

typedef struct {
	int trainId;
	int stationId;
	int arriveHour;
	int arriveMin;
	int departHour;
	int departMin;
} TrainStop;

typedef struct {
  TrainStop **trainstops;
  int count;
  int allocated;
} TimeTableList;

TimeTableList *newTimeTableList ();
void deleteTimeTableList (TimeTableList *ttl);
TrainStop *newTrainStop (TimeTableList *ttl);
void readTimeTableList (TimeTableList *ttl, char *fileName);
StationList *getfastestRoute (StationList *sl, TimeTableList *ttl, int fromStation, int toStation, int fromHour, int fromMin);
Station *getNextReachableStation (StationList *sl, TimeTableList *ttl, int fromHour, int fromMin);
int timestamp(int hour, int min);
int getDepartTime(TimeTableList *ttl, int fromTrain, int fromStation);
int getArriveTime(TimeTableList *ttl, int trainId, int stationId);
#endif
