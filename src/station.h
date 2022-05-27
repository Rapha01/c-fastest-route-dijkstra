#ifndef STATION_H
#define STATION_H

typedef struct
{
  int id;
  char name[100];
  int fromStation;
  int arriveTime;
  int departTime;
  int fromTrain;
} Station;

typedef struct
{
  Station **stations;
  int count;
  int allocated;
} StationList;


StationList *newStationList ();
void deleteStationList (StationList *al);
Station *newStation (StationList *sl);
void readStationList (StationList *sl, char *fileName);
Station *findStation (StationList *sl, int id);


void stringSubst (char *s, char old, char new);  // gehört eigtl. nicht hierher

#endif