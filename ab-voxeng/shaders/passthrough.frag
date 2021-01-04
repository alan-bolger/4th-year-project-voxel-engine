#version 330 core

uniform sampler2D diffuseTexture;
uniform vec3 viewPosition;

in vec3 fragPos;
in vec2 texCoords;
in vec3 normal;

out vec4 fragColour;

struct directionalLight 
{
    vec3 direction;  
    vec3 ambient;
    vec3 diffuse;
};  

uniform directionalLight dirLight;

vec3 calculateDirectionalLight(directionalLight light, vec3 t_normal, vec3 viewDir, vec3 texColour)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(t_normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, t_normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 0.3);

    vec3 ambient  = light.ambient * texColour;
    vec3 diffuse  = light.diffuse * diff * texColour;
    vec3 lighting = ambient + diffuse;

    return lighting;
}

void main()
{
    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(viewPosition - fragPos);
    vec3 texColour = texture(diffuseTexture, texCoords).rgb;
    vec3 result = calculateDirectionalLight(dirLight, norm, viewDir, texColour);    	

    fragColour = vec4(result, 1.0);
}