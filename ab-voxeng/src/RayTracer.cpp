#include "RayTracer.h"

/// <summary>
/// Constructor for the RayTracer class.
/// </summary>
ab::RayTracer::RayTracer()
{

}

/// <summary>
/// Destructor for the RayTracer class.
/// </summary>
ab::RayTracer::~RayTracer()
{

}

/// <summary>
/// Trace.
/// </summary>
/// <param name="t_rayorig">The ray origin.</param>
/// <param name="t_raydir">The ray direction.</param>
/// <param name="t_spheres">An array holding Sphere objects.</param>
/// <param name="t_depth">Used for recursion depth.</param>
glm::vec3 ab::RayTracer::trace(const glm::vec3 &t_rayorig, const glm::vec3 &t_raydir, const std::vector<Sphere> &t_spheres, const int &t_depth)
{ 
    //if (raydir.length() != 1) std::cerr << "Error " << raydir << std::endl;
    float tnear = INFINITY;
    const Sphere *sphere = NULL;
    // find intersection of this ray with the sphere in the scene
    for (unsigned i = 0; i < t_spheres.size(); ++i) 
    {
        float t0 = INFINITY, t1 = INFINITY;

        if (t_spheres[i].intersect(t_rayorig, t_raydir, t0, t1))
        {
            if (t0 < 0) t0 = t1;

            if (t0 < tnear) 
            {
                tnear = t0;
                sphere = &t_spheres[i];
            }
        }
    }
    // if there's no intersection return black or background color
    if (!sphere)
    {
        return glm::vec3(1.0, 0.0, 0.0);
    }

    glm::vec3 surfaceColor = glm::vec3(0, 0, 0); // color of the ray/surfaceof the object intersected by the ray 
    glm::vec3 phit = t_rayorig + t_raydir * tnear; // point of intersection 
    glm::vec3 nhit = phit - sphere->center; // normal at the intersection point 
    nhit = glm::normalize(nhit); // normalize normal direction 
    // If the normal and the view direction are not opposite to each other
    // reverse the normal direction. That also means we are inside the sphere so set
    // the inside bool to true. Finally reverse the sign of IdotN which we want
    // positive.
    float bias = 1e-4; // add some bias to the point from which we will be tracing 
    bool inside = false;

    if (glm::dot(t_raydir, nhit) > 0) nhit = -nhit, inside = true;

    if ((sphere->transparency > 0 || sphere->reflection > 0) && t_depth < MAX_RAY_DEPTH)
    {
        float facingratio = glm::dot(-t_raydir, nhit);
        // change the mix value to tweak the effect
        float fresneleffect = mix(pow(1 - facingratio, 3), 1, 0.1);
        // compute reflection direction (not need to normalize because all vectors
        // are already normalized)
        glm::vec3 refldir = t_raydir - nhit * 2.0f * glm::dot(t_raydir, nhit);
        refldir = glm::normalize(refldir);
        glm::vec3 reflection = trace(phit + nhit * bias, refldir, t_spheres, t_depth + 1);
        glm::vec3 refraction = glm::vec3(0, 0, 0);

        // if the sphere is also transparent compute refraction ray (transmission)
        if (sphere->transparency) 
        {
            float ior = 1.1, eta = (inside) ? ior : 1 / ior; // are we inside or outside the surface? 
            float cosi = glm::dot(-nhit, t_raydir);
            float k = 1 - eta * eta * (1 - cosi * cosi);
            glm::vec3 refrdir = t_raydir * eta + nhit * (eta * cosi - sqrt(k));
            refrdir = glm::normalize(refrdir);
            refraction = trace(phit - nhit * bias, refrdir, t_spheres, t_depth + 1);
        }
        // the result is a mix of reflection and refraction (if the sphere is transparent)
        surfaceColor = (
            reflection * fresneleffect +
            refraction * (1 - fresneleffect) * sphere->transparency) * sphere->surfaceColour;
    }
    else {
        // it's a diffuse object, no need to raytrace any further
        for (unsigned i = 0; i < t_spheres.size(); ++i) 
        {
            if (t_spheres[i].emissionColour.x > 0) 
            {
                // this is a light
                glm::vec3 transmission = glm::vec3(1, 1, 1);
                glm::vec3 lightDirection = t_spheres[i].center - phit;
                lightDirection = glm::normalize(lightDirection);

                for (unsigned j = 0; j < t_spheres.size(); ++j) 
                {
                    if (i != j) 
                    {
                        float t0, t1;

                        if (t_spheres[j].intersect(phit + nhit * bias, lightDirection, t0, t1)) 
                        {
                            transmission = glm::vec3(0, 0, 0);
                            break;
                        }
                    }
                }
                surfaceColor += sphere->surfaceColour * transmission * std::max(float(0), glm::dot(nhit, lightDirection)) * t_spheres[i].emissionColour;
            }
        }
    }

    return surfaceColor + sphere->emissionColour;
}

/// <summary>
/// Draw.
/// </summary>
/// <param name="t_spheres">An array holding Sphere objects.</param>
GLuint ab::RayTracer::draw(const std::vector<Sphere> &t_spheres, ab::Camera &t_camera)
{
    unsigned int width = 1280;
    unsigned int height = 720;

    std::vector<unsigned char> image;
    image.resize(width * height * 3); // RGB = 3
    std::vector<unsigned char>::iterator pixel = image.begin();
    float invWidth = 1 / float(width);
    float invHeight = 1 / float(height);
    float fov = 30;
    float aspectratio = width / float(height);
    float angle = std::tan(PI * 0.5 * fov / 180.0);

    int depth = 0;

    glm::vec3 f_ray_00;
    glm::vec3 f_ray_01;
    glm::vec3 f_ray_10;
    glm::vec3 f_ray_11;

    t_camera.getEyeRay(-1, -1, f_ray_00);
    t_camera.getEyeRay(-1, 1, f_ray_01);
    t_camera.getEyeRay(1, -1, f_ray_10);
    t_camera.getEyeRay(1, 1, f_ray_11);

    // Trace rays
    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x) 
        {
            float xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectratio;
            float yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
            glm::vec3 raydir = glm::vec3(xx, yy, -1);
            raydir = glm::normalize(raydir);
            glm::vec3 f_pixelColour = trace(t_camera.getEye(), raydir, t_spheres, 0);

            *pixel = std::min<float>(float(1), f_pixelColour.x) * 255; pixel++;
            *pixel = std::min<float>(float(1), f_pixelColour.y) * 255; pixel++;
            *pixel = std::min<float>(float(1), f_pixelColour.z) * 255; pixel++;
        
            //*pixel = unsigned char(f_pixelColour.x); pixel++; // R
            //*pixel = unsigned char(f_pixelColour.y); pixel++; // G
            //*pixel = unsigned char(f_pixelColour.z); pixel++; // B

            //glm::vec2 size = glm::vec2(1280, 720);
            //glm::vec2 pos = glm::vec2(x, y) / glm::vec2(size.x - 1, size.y - 1);
            //glm::vec3 dir = glm::mix(glm::mix(f_ray_00, f_ray_01, pos.y), glm::mix(f_ray_10, f_ray_11, pos.y), pos.x);
            //*pixel = trace(t_camera.getEye(), dir, t_spheres, 0);
        }
    }

    GLuint f_textureID = 0;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &f_textureID);
    glBindTexture(GL_TEXTURE_2D, f_textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1280, 720, 0, GL_RGB, GL_UNSIGNED_BYTE, &image[0]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);

    //// Save result to a PPM image (keep these flags if you compile under Windows)
    //std::ofstream ofs("./screenshot.ppm", std::ios::out | std::ios::binary);
    //ofs << "P6\n" << width << " " << height << "\n255\n";

    //for (unsigned int i = 0; i < width * height * 3; i += 3)
    //{
    //    ofs << image[i] << image[i + 1] << image[i + 2];
    //}

    //ofs.close();

    return f_textureID;
}

/// <summary>
/// Mix.
/// </summary>
/// <param name="t_a">Value A.</param>
/// <param name="t_b">Value B.</param>
/// <param name="t_mix">Mix value.</param>
/// <returns>The result.</returns>
float ab::RayTracer::mix(const float& t_a, const float& t_b, const float& t_mix)
{
    return t_b * t_mix + t_a * (1 - t_mix);
}
