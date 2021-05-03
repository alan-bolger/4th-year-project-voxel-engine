// *****************************************
// * Map.h and Map.cpp - Alan Bolger, 2021 *
// *****************************************

#ifndef MAP_H
#define MAP_H

#include "Globals.h"
#include "Chunk.h"

#include <iostream>
#include <vector>
#include <functional>

class Map
{
public:
	Map();
	~Map();
	bool checkIsEmpty();
	void setVoxel(int x, int y, int z, char type);
	char getVoxel(int x, int y, int z);
	void populate(int heightMap[MAP_WIDTH][MAP_DEPTH], int treeMap[MAP_WIDTH][MAP_DEPTH], int waterMap[MAP_WIDTH][MAP_DEPTH]);
	void placeScenery(int treeMap[MAP_WIDTH][MAP_DEPTH], Indices worldSize);
	void checkAllChunks();

	std::vector<Chunk*> chunks;
};

#endif // !MAP_H