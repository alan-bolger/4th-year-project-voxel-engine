#version 330 core

in vec3 FragPos;
in vec3 Normal;
// in vec2 TexCoords;
in vec4 FragPosLightSpace;

out vec4 FragColor;

// --------------------------

struct Material
{
    //sampler2D diffuse;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    float alpha;
}; 
  
uniform Material material;

// --------------------------

struct DirLight 
{
    vec3 direction;  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

uniform DirLight dirLight;

// --------------------------

struct PointLight 
{    
    vec3 position;
    float constant;
    float linear;
    float quadratic;  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

uniform PointLight pointLight;

// --------------------------
  
uniform vec3 lightPosition;
uniform vec3 viewPosition;
uniform sampler2D shadowMap;
uniform bool showShadows;

// Functions
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewPos);
float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir, vec3 normal);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPosition - FragPos);
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    // result += CalcPointLight(pointLight, norm, FragPos, viewPosition); // Not implemented yet
    // result += CalcSpotLight(spotLight, norm, FragPos, viewDir); // Not implemented yet
    
    // FragColor = vec4(result, TexCoords); // For use with textures
    FragColor = vec4(result, material.alpha);
}

// Calculate directional lighting
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // Combine results
    vec3 ambient  = light.ambient * material.diffuse;
    vec3 diffuse  = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec;

    float shadow = ShadowCalculation(FragPosLightSpace, lightDir, normal);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));  

    // Return (ambient + diffuse + specular);
    return lighting;
}

// Calculate point lighting
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewPos)
{
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 viewDir    = normalize(viewPos - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    // Attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));  
    
    // Combine results
    vec3 ambient  = light.ambient  * material.diffuse;
    vec3 diffuse  = light.diffuse  * diff * material.diffuse;
    vec3 specular = light.diffuse * spec;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir, vec3 normal)
{
    // Perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // Transform to [0, 1] range
    projCoords = projCoords * 0.5 + 0.5;

    // Get closest depth value from the light's perspective
    float closestDepth = texture(shadowMap, projCoords.xy).r; 

    // Get depth of current fragment from the light's perspective
    float currentDepth = projCoords.z;
    
    // float bias = 0.01;

    float cosTheta = dot(normal, lightDir);
    cosTheta = clamp(cosTheta, 0, 1);
    float bias = 0.005*tan(acos(cosTheta)); // cosTheta is dot( n,l ), clamped between 0 and 1
    bias = clamp(bias, 0,0.01);
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }   
    
    shadow /= 9.0;

    if (projCoords.z > 1.0)
    {
        shadow = 0.0;
    }

    // Toggle shadows using uniform bool
    if (!showShadows)
    {
        shadow = 0;
    }

    return shadow;
}
