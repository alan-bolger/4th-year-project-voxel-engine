// *********************************************
// * World.h and World.cpp - Alan Bolger, 2021 *
// *********************************************

#ifndef WORLD_H
#define WORLD_H

#include "Globals.h"
#include "Map.h"

#include <iostream>
#include <vector>
#include <functional>

class World
{
public:
	World();
	~World();
	void optimiseWorldStorage();
	void setVoxel(int x, int y, int z, char type);
	char getVoxel(int x, int y, int z);
	void populate(int heightMap[WORLD_WIDTH][WORLD_DEPTH], int treeMap[WORLD_WIDTH][WORLD_DEPTH], int waterMap[WORLD_WIDTH][WORLD_DEPTH]);
	void placeScenery(int treeMap[WORLD_WIDTH][WORLD_DEPTH]);

	std::vector<Map*> maps;
};

#endif // !WORLD_H

