#version 330 core

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

out vec4 FragColor;
  
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace; // For shadows
  
uniform vec3 viewPos;
uniform Material material;
uniform PointLight pointLights[2];
uniform bool pointLightsTurned[2];

uniform sampler2D shadowMap;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcDirLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

float ShadowCalculation()
{
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5; 
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    vec3 lightDir = pointLights[0].position;
    float bias = max(0.05 * (1.0 - dot(normalize(Normal), normalize(lightDir))), 0.001);
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    return shadow;
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0f, 0.0f, 0.0f);
    // for(int i = 0; i < 2; i++) {
    //     if (pointLightsTurned[i])
    //         result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    // }

    if (pointLightsTurned[0])
        result += CalcDirLight(pointLights[0], norm, FragPos, viewDir);
    if (pointLightsTurned[1])
        result += CalcPointLight(pointLights[1], norm, FragPos, viewDir);

    FragColor = vec4(result, 1.0);
    // FragColor = vec4(vec3(1.0f - ShadowCalculation()), 1.0f);
} 

vec3 CalcDirLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position);
    // диффузное освещение
    float diff = max(dot(normal, lightDir), 0.0);
    // освещение зеркальных бликов
    vec3 reflectDir = reflect(lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // комбинируем результаты
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

    return (ambient + (1.0f - ShadowCalculation()) * (diffuse + specular));
} 

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // диффузное освещение
    float diff = max(dot(normal, lightDir), 0.0);
    // освещение зеркальных бликов
    vec3 reflectDir = reflect(lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // затухание
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // комбинируем результаты
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    
    return (ambient + diffuse + specular);
} 