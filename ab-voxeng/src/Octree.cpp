#include "Octree.h"

/// <summary>
/// Constructor for the Octree class.
/// </summary>
Octree::Octree()
{
    m_point = new Point(); // Empty node

    // If the point == NULL then the node is an internal node
    // If the point == (-1, -1, -1) then the node is empty
    // If the point > (1 x 1010^100, 1 x 1010^100, 1 x 1010^100) then the point is out in space somewhere
}

/// <summary>
/// Constructor for the Octree class.
/// </summary>
/// <param name="t_x">- Point X value.</param>
/// <param name="t_y">- Point Y value.</param>
/// <param name="t_z">- Point Z value.</param>
Octree::Octree(int t_x, int t_y, int t_z)
{
    m_point = new Point(t_x, t_y, t_z);
}

/// <summary>
/// Constructor for the Octree class.
/// Creates an Octree with defined boundaries.
/// </summary>
/// <param name="t_x_1">- Point X value for boundary start.</param>
/// <param name="t_y_1">- Point Y value for boundary start.</param>
/// <param name="t_z_1">- Point Z value for boundary start.</param>
/// <param name="t_x_2">- Point X value for boundary end.</param>
/// <param name="t_y_2">- Point Y value for boundary end.</param>
/// <param name="t_z_2">- Point Z value for boundary end.</param>
Octree::Octree(int t_x_1, int t_y_1, int t_z_1, int t_x_2, int t_y_2, int t_z_2)
{
    if (t_x_2 < t_x_1 || t_y_2 < t_y_1 || t_z_2 < t_z_1)
    {
        std::cout << "Boundary positions are not valid" << std::endl;
        return;
    }

    m_point = nullptr;
    m_topLeftFront = new Point(t_x_1, t_y_1, t_z_1);
    m_bottomRightBack = new Point(t_x_2, t_y_2, t_z_2);

    // Assign all children as nullptr 
    m_children.assign(8, nullptr);

    for (int i = TopLeftFront; i <= BottomLeftBack; ++i)
    {
        m_children[i] = new Octree();
    }        
}

/// <summary>
/// Destructor for the Octree class.
/// </summary>
Octree::~Octree()
{

}

/// <summary>
/// Insert point.
/// </summary>
/// <param name="t_x">- Point X value.</param>
/// <param name="t_y">- Point Y value.</param>
/// <param name="t_z">- Point Z value.</param>
void Octree::insert(int t_x, int t_y, int t_z)
{
    // Check if the point already exists in the octree 
    if (find(t_x, t_y, t_z))
    {
        std::cout << "Point already exists in the tree" << std::endl;
        return;
    }

    // Check if the point is out of bounds 
    if (t_x < m_topLeftFront->x || t_x > m_bottomRightBack->x || t_y < m_topLeftFront->y
        || t_y > m_bottomRightBack->y || t_z < m_topLeftFront->z || t_z > m_bottomRightBack->z)
    {
        std::cout << "Point is out of bounds" << std::endl;
        return;
    }

    // Binary search to insert the point 
    int midx = (m_topLeftFront->x + m_bottomRightBack->x) / 2;
    int midy = (m_topLeftFront->y + m_bottomRightBack->y) / 2;
    int midz = (m_topLeftFront->z + m_bottomRightBack->z) / 2;

    int pos = -1;

    // Checking the octant of the point
    if (t_x <= midx)
    {
        if (t_y <= midy)
        {
            if (t_z <= midz)
            {
                pos = TopLeftFront;
            }                
            else
            {
                pos = TopLeftBottom;
            }                
        }
        else 
        {
            if (t_z <= midz)
            {
                pos = BottomLeftFront;
            }                
            else
            {
                pos = BottomLeftBack;
            }                
        }
    }
    else 
    {
        if (t_y <= midy)
        {
            if (t_z <= midz)
            {
                pos = TopRightFront;
            }                
            else
            {
                pos = TopRightBottom;
            }                
        }
        else 
        {
            if (t_z <= midz)
            {
                pos = BottomRightFront;
            }                
            else
            {
                pos = BottomRightBack;
            }                
        }
    }

    // If an internal node is encountered 
    if (m_children[pos]->m_point == nullptr)
    {
        m_children[pos]->insert(t_x, t_y, t_z);
        return;
    }

    // If an empty node is encountered 
    else if (m_children[pos]->m_point->x == -1)
    {
        delete m_children[pos];
        m_children[pos] = new Octree(t_x, t_y, t_z);
        return;
    }
    else 
    {
        int x_ = m_children[pos]->m_point->x;
        int y_ = m_children[pos]->m_point->y;
        int z_ = m_children[pos]->m_point->z;

        delete m_children[pos];
        m_children[pos] = nullptr;

        if (pos == TopLeftFront) 
        {
            m_children[pos] = new Octree(m_topLeftFront->x, m_topLeftFront->y, m_topLeftFront->z, midx, midy, midz);
        }

        else if (pos == TopRightFront) 
        {
            m_children[pos] = new Octree(midx + 1, m_topLeftFront->y, m_topLeftFront->z, m_bottomRightBack->x, midy, midz);
        }
        else if (pos == BottomRightFront) 
        {
            m_children[pos] = new Octree(midx + 1, midy + 1, m_topLeftFront->z, m_bottomRightBack->x, m_bottomRightBack->y, midz);
        }
        else if (pos == BottomLeftFront) 
        {
            m_children[pos] = new Octree(m_topLeftFront->x, midy + 1, m_topLeftFront->z, midx, m_bottomRightBack->y, midz);
        }
        else if (pos == TopLeftBottom) 
        {
            m_children[pos] = new Octree(m_topLeftFront->x, m_topLeftFront->y, midz + 1, midx, midy, m_bottomRightBack->z);
        }
        else if (pos == TopRightBottom)
        {
            m_children[pos] = new Octree(midx + 1, m_topLeftFront->y, midz + 1, m_bottomRightBack->x, midy, m_bottomRightBack->z);
        }
        else if (pos == BottomRightBack) 
        {
            m_children[pos] = new Octree(midx + 1, midy + 1, midz + 1, m_bottomRightBack->x, m_bottomRightBack->y, m_bottomRightBack->z);
        }
        else if (pos == BottomLeftBack) 
        {
            m_children[pos] = new Octree(m_topLeftFront->x, midy + 1, midz + 1, midx, m_bottomRightBack->y, m_bottomRightBack->z);
        }

        m_children[pos]->insert(x_, y_, z_);
        m_children[pos]->insert(t_x, t_y, t_z);
    }
}

/// <summary>
/// Find point.
/// </summary>
/// <param name="t_x">- Point X value.</param>
/// <param name="t_y">- Point Y value.</param>
/// <param name="t_z">- Point Z value.</param>
bool Octree::find(int t_x, int t_y, int t_z)
{
    // Check if the point is out of bounds  
    if (t_x < m_topLeftFront->x || t_x > m_bottomRightBack->x || t_y < m_topLeftFront->y
        || t_y > m_bottomRightBack->y || t_z < m_topLeftFront->z || t_z > m_bottomRightBack->z)
    {
        return 0;
    }        

    // Otherwise perform binary search for each ordinate     
    int midx = (m_topLeftFront->x + m_bottomRightBack->x) / 2;
    int midy = (m_topLeftFront->y + m_bottomRightBack->y) / 2;
    int midz = (m_topLeftFront->z + m_bottomRightBack->z) / 2;

    int pos = -1;

    // Deciding the position where to move 
    if (t_x <= midx)
    {
        if (t_y <= midy)
        {
            if (t_z <= midz)
            {
                pos = TopLeftFront;
            }                
            else
            {
                pos = TopLeftBottom;
            }                
        }
        else
        {
            if (t_z <= midz)
            {
                pos = BottomLeftFront;
            }                
            else
            {
                pos = BottomLeftBack;
            }                
        }
    }
    else 
    {
        if (t_y <= midy)
        {
            if (t_z <= midz)
            {
                pos = TopRightFront;
            }                
            else
            {
                pos = TopRightBottom;
            }                
        }
        else 
        {
            if (t_z <= midz)
            {
                pos = BottomRightFront;
            }                
            else
            {
                pos = BottomRightBack;
            }                
        }
    }

    // If an internal node is encountered 
    if (m_children[pos]->m_point == nullptr)
    {
        return m_children[pos]->find(t_x, t_y, t_z);
    }

    // If an empty node is encountered 
    else if (m_children[pos]->m_point->x == -1)
    {
        return 0;
    }
    else 
    {
        // If node is found with the given value 
        if (t_x == m_children[pos]->m_point->x && t_y == m_children[pos]->m_point->y && t_z == m_children[pos]->m_point->z)
        {
            return 1;
        }            
    }

    return 0;
}
