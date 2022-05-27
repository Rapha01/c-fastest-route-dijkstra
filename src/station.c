#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "station.h"

StationList *newStationList (){
	StationList *sl = (StationList *) malloc (sizeof (StationList));
	sl->count = 0;
	sl->allocated = 10;
	sl->stations = (Station **) calloc (sl->allocated, sizeof (Station *));
	return sl;
}

void deleteStationList (StationList *sl){
	int i;
	for (i = 0; i < sl->count; i ++)  free(sl->stations[i]);
	free (sl->stations);
	free (sl);
}

Station *newStation (StationList *sl){
	if (sl->count >= sl->allocated)
	{ 
		sl->allocated *= 2;
		sl->stations = (Station **) realloc (sl->stations, sl->allocated * sizeof (Station *));
	}
	Station *a = (Station *) malloc (sizeof (Station));
	sl->stations[sl->count++] = a;
	return a;
}


void readStationList (StationList *sl, char *fileName){
	FILE *file = fopen (fileName, "r");
	if (file == NULL)  { perror (fileName);  exit (1); }
	int r, id;
	char name[100];
	
	while (r != EOF){
		Station *a = newStation (sl);
		r = fscanf(file, "%d %s", &id,name);
		
		if(r != EOF){
			a->id = id;
			strcpy(a->name,name);
			stringSubst (a->name, '_', ' ');
			a->fromStation = 0;
		}
		//printf("%d, %s, %d\n",a->id,a->name,a->fromStation);
	}
	fclose (file);
}

Station *findStation (StationList *sl, int id)
{
  int i;
  for (i = 0; i < sl->count; i ++)
    if (sl->stations[i]->id == id)
      return (sl->stations[i]);
  //fprintf (stderr, "Station Id %d not found\n", id);
  exit (1);
}

void stringSubst (char *s, char old, char new)
{
  while (*s) { if (*s == old) *s = new; s ++; }
}
