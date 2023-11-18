#version 430

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 color;

uniform sampler2D ourTexture;

uniform vec3 viewPos;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

struct DirectionLight
{
    int enable;
    vec3 direction;
    vec3 lightColor;
};

struct PointLight {
    int enable;
    vec3 position;  
    vec3 lightColor;

    float constant;
    float linear;
    float quadratic;
};

struct Spotlight {
    int enable;
    vec3 position;
    vec3 direction;
    vec3 lightColor;
    float cutOff;

    // Paramters for attenuation formula
    float constant;
    float linear;
    float quadratic;      
}; 

uniform Material material;
uniform DirectionLight dl;
uniform PointLight pl;
uniform Spotlight sl;

void main() {
  color = vec4(0.0, 0.0, 0.0, 0.0) ;
  if ( dl.enable == 1 ) {
      // 環境光
    vec3 ambient = dl.lightColor * material.ambient;

    // 漫反射 
    vec3 norm = normalize(Normal);
    vec3 lightPos = -1*dl.direction ; 
    vec3 lightDir = normalize(lightPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = dl.lightColor * (diff * material.diffuse);

    // 鏡面光
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(dl.direction, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = dl.lightColor * (spec * material.specular);  

    vec3 result = ambient + diffuse + specular;
    color = color + vec4(result, 1.0);
  } // if 
  if ( pl.enable == 1 ) {
     // 環境光
    vec3 ambient = pl.lightColor * material.ambient;

    // 漫反射 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(pl.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = pl.lightColor * (diff * material.diffuse);

   // 鏡面光
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = pl.lightColor * (spec * material.specular);  

    float distance    = length(pl.position - FragPos);
    float attenuation = 1.0 / (pl.constant + pl.linear * distance + pl.quadratic * (distance * distance));

    ambient  *= attenuation; 
    diffuse  *= attenuation;
    specular *= attenuation;
    vec3 result =  ambient + diffuse + specular ;
    color = color + vec4(result, 1.0);
  } // if 
  if ( sl.enable == 1  ) {
     // 環境光
    vec3 ambient = sl.lightColor * material.ambient;

    // 漫反射 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(sl.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = sl.lightColor * (diff * material.diffuse);

    // 鏡面光
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = sl.lightColor * (spec * material.specular);  

    float distance    = length(sl.position - FragPos);
    float attenuation = 1.0 / (sl.constant + sl.linear * distance + sl.quadratic * (distance * distance));

    ambient  *= attenuation; 
    diffuse  *= attenuation;
    specular *= attenuation;
    vec3 result =  ambient + diffuse + specular ;
    float theta = dot(lightDir, normalize(-sl.direction));
    if(theta > sl.cutOff)
      color = color + vec4(result, 1.0);
    else 
      color = color + vec4(ambient, 1.0) ;
  } // if 

  color = color * texture(ourTexture, TexCoord)  ;
}
