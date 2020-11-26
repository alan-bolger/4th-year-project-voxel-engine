#include "RaycasterTest.h"

/// <summary>
/// Constructor for the RaycasterTest class.
/// </summary>
/// <param name="t_renderer">- A pointer to a SDL_Renderer.</param>
Raycaster::Raycaster(SDL_Renderer *t_renderer) : m_renderer(t_renderer)
{

}

/// <summary>
/// Destructor for the RaycasterTest class.
/// </summary>
Raycaster::~Raycaster()
{

}

void Raycaster::update()
{
    for (int x = 0; x < w; x++)
    {
        // Calculate ray position and direction
        double cameraX = 2 * x / double(w) - 1; // x-coordinate in camera space
        double rayPosX = posX;
        double rayPosY = posY;
        double rayDirX = dirX + planeX * cameraX;
        double rayDirY = dirY + planeY * cameraX;

        // Which box of the map we're in
        int mapX = int(rayPosX);
        int mapY = int(rayPosY);

        // Length of ray from current position to next x or y-side
        double sideDistX;
        double sideDistY;

        // Length of ray from one x or y-side to next x or y-side
        double deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX));
        double deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY));
        double perpWallDist;

        // What direction to step in x or y-direction (either +1 or -1)
        int stepX;
        int stepY;

        int hit = 0; // Was there a wall hit?
        int side; // Was a NS or a EW wall hit?

        // Calculate step and initial sideDist
        if (rayDirX < 0)
        {
            stepX = -1;
            sideDistX = (rayPosX - mapX) * deltaDistX;
        }
        else
        {
            stepX = 1;
            sideDistX = (mapX + 1.0 - rayPosX) * deltaDistX;
        }

        if (rayDirY < 0)
        {
            stepY = -1;
            sideDistY = (rayPosY - mapY) * deltaDistY;
        }
        else
        {
            stepY = 1;
            sideDistY = (mapY + 1.0 - rayPosY) * deltaDistY;
        }

        // Perform DDA
        while (hit == 0)
        {
            // Jump to next map square, OR in x-direction, OR in y-direction
            if (sideDistX < sideDistY)
            {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            }
            else
            {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }

            // Check if ray has hit a wall
            if (worldMap[mapX][mapY] > 0)
            {
                hit = 1;
            }
        }

        // Calculate distance projected on camera direction (oblique distance will give fisheye effect!)
        if (side == 0)
        {
            perpWallDist = fabs((mapX - rayPosX + (1 - stepX) / 2) / rayDirX);
        }            
        else
        {
            perpWallDist = fabs((mapY - rayPosY + (1 - stepY) / 2) / rayDirY);
        }            

        // Calculate height of line to draw on screen
        int lineHeight = abs(int(h / perpWallDist));

        // Calculate lowest and highest pixel to fill in current stripe
        int drawStart = -lineHeight / 2 + h / 2;
        if (drawStart < 0)drawStart = 0;
        int drawEnd = lineHeight / 2 + h / 2;
        if (drawEnd >= h)drawEnd = h - 1;

        // Choose wall color
        SDL_Colour colour;

        switch (worldMap[mapX][mapY])
        {
        case 1:  
            colour.r = 255;
            colour.g = 0;
            colour.b = 0;
            break; // Red

        case 2:  
            colour.r = 0;
            colour.g = 255;
            colour.b = 0;
            break; // Green

        case 3:  
            colour.r = 0;
            colour.g = 0;
            colour.b = 255;
            break; // Blue

        case 4:  
            colour.r = 255;
            colour.g = 255;
            colour.b = 255;
            break; // White

        default: 
            colour.r = 0;
            colour.g = 255;
            colour.b = 255;
            break; // Yellow
        }

        // Give x and y sides different brightness
        if (side == 1) 
        {
            colour.r = colour.r / 2;
            colour.g = colour.g / 2;
            colour.b = colour.b / 2;
        }

        // Draw the pixels of the stripe as a vertical line
        // verLine(x, drawStart, drawEnd, color);
        if (m_renderer != nullptr)
        {
            SDL_SetRenderDrawColor(m_renderer, colour.r, colour.g, colour.b, SDL_ALPHA_OPAQUE);
            SDL_RenderDrawLine(m_renderer, x, drawStart, x, drawEnd);
        }
    }

    double moveSpeed = 0.1; // = fps.asSeconds() * 5.0; // The constant value is in squares/second
    double rotSpeed = 0.1; // = fps.asSeconds() * 3.0; // The constant value is in radians/second

    const Uint8 *f_state = SDL_GetKeyboardState(NULL);

    if (f_state[SDL_SCANCODE_LEFT])
    {
        // Both camera direction and camera plane must be rotated
        double oldDirX = dirX;
        dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
        dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
        double oldPlaneX = planeX;
        planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
        planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
    }
    else if (f_state[SDL_SCANCODE_RIGHT])
    {
        // Both camera direction and camera plane must be rotated
        double oldDirX = dirX;
        dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
        dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
        double oldPlaneX = planeX;
        planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
        planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
    }
    else if (f_state[SDL_SCANCODE_UP])
    {
        if (worldMap[int(posX + dirX * moveSpeed)][int(posY)] == false) posX += dirX * moveSpeed;
        if (worldMap[int(posX)][int(posY + dirY * moveSpeed)] == false) posY += dirY * moveSpeed;
    }
    else if (f_state[SDL_SCANCODE_DOWN])
    {
        if (worldMap[int(posX - dirX * moveSpeed)][int(posY)] == false) posX -= dirX * moveSpeed;
        if (worldMap[int(posX)][int(posY - dirY * moveSpeed)] == false) posY -= dirY * moveSpeed;
    }
}

void Raycaster::draw()
{

}

/// <summary>
/// Convert number to string.
/// </summary>
/// <param name="t_number">- Number to convert to string.</param>
std::string Raycaster::toString(int t_number)
{
    std::ostringstream buff;
    buff << t_number;
    return buff.str();
}

