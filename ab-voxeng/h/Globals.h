// *********************************
// * Globals.h - Alan Bolger, 2020 *
// *********************************

#ifndef GLOBALS_H
#define GLOBALS_H

static unsigned int SCREEN_WIDTH;
static unsigned int SCREEN_HEIGHT;

static const int MAP_WIDTH = 1024;
static const int MAP_HEIGHT = 128;
static const int MAP_DEPTH = 1024;
static const int WATER_HEIGHT = 1;

struct Indices
{
	int x;
	int y;
	int z;
};

#endif // !GLOBALS_H

