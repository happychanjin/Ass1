#version 410

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inUV;

uniform vec3 LightPos;
uniform vec3 Colour;

uniform int HasTexture;
uniform sampler2D texture1;
out vec4 frag_color;



void main() {
	// Lecture 5
	vec3  lightPos = vec3(1.0f, 0.0f, 2.0f);
	vec3  lightCol = vec3(0.3f, 0.2f, 0.5f);
	float lightAmbientPow = 0.08f;
	float lightSpecularPow = 2.0f;
	vec3  ambientCol = vec3(1.0f);
	float ambientPow = 0.2f;
	float shininess = 2000.0f;
	vec3 CamPos = vec3(0.0,0.0,3.0);

	vec3 ambient = ((lightAmbientPow * lightCol) + (ambientCol * ambientPow));

	// Diffuse
	vec3 N = normalize(inNormal);
	vec3 lightDir = normalize(lightPos - inPos);

	float dif = max(dot(N, lightDir), 0.0);
	vec3 diffuse = dif * lightCol;   // add diffuse intensity

	//Attenuation
	float dist = length(lightPos - inPos);
	if(dist!=0){
		diffuse = diffuse / dist;
	}

	// Specular
	vec3 viewDir = normalize(CamPos - inPos);
	vec3 h = normalize(lightDir + viewDir);
	float spec = pow(max(dot(N, h), 0.0), shininess); // Shininess coefficient (can be a uniform)

	vec3 specular = lightSpecularPow * spec * lightCol; // Can also use a specular color

	vec3 result;
	if(HasTexture==1){
		//result = texture(texture1,inUV);
		result = (ambient + diffuse + specular) * vec3(texture(texture1,inUV).xyz);
	}else{
		result = (ambient + diffuse + specular) * Colour;
	}
	frag_color = vec4(result,1.0);
}