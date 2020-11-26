#ifndef OCTREE_H
#define OCTREE_H

#include <iostream> 
#include <vector>

#include "glm/glm.hpp"

#define TopLeftFront 0 
#define TopRightFront 1 
#define BottomRightFront 2 
#define BottomLeftFront 3 
#define TopLeftBottom 4 
#define TopRightBottom 5 
#define BottomRightBack 6 
#define BottomLeftBack 7 

struct Point 
{
	Point() : x(-1), y(-1), z(-1) {}
	Point(int t_x, int t_y, int t_z) : x(t_x), y(t_y), z(t_z) {}

	int x;
	int y;
	int z;
};

class Octree
{
public:
	Octree();
	Octree(int t_x, int t_y, int t_z);
	Octree(int t_x_1, int t_y_1, int t_z_1, int t_x_2, int t_y_2, int t_z_2);
	~Octree();
	void insert(int x, int y, int z);
	bool find(int x, int y, int z);

private:
	Point *m_point;
	Point *m_topLeftFront;
	Point *m_bottomRightBack;
	std::vector<Octree*> m_children;
};

#endif // !OCTREE_H