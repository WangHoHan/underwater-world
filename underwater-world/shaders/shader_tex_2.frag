#version 410 core

//uniform vec3 objectColor;
uniform vec3 lightDir;
//uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform sampler2D color_texture;


in vec3 interpNormal;
in vec3 fragPos;
in vec2 uvCoord;

void main()
{
	//float falloff = pow(length(cameraPos-fragPos),2)*0.01;
	vec3 color = texture(color_texture,uvCoord).rgb;
	vec3 spec = vec3(0.7);
	//vec3 lightDir = normalize(lightPos-fragPos);
	vec3 V = normalize(cameraPos-fragPos);
	vec3 normal = normalize(interpNormal);
	vec3 R = reflect(-normalize(lightDir),normal);
	
	float specular = pow(max(0,dot(R,V)),10);
	float diffuse = max(0,dot(normal,normalize(lightDir)));
	gl_FragColor = vec4(mix(color,color*diffuse+spec*specular,0.7), 1.0);
	//gl_FragColor = vec4(mix(vec3(0.1,0.1,0.1),mix(color,color*diffuse+spec*specular,0.7),min(1,1/falloff)), 1.0);
}
