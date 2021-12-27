# version 330 core

out vec4 FragColor;

in vec3 crntPos;
in vec3 normal;
in vec3 color;
in vec2 texCoord;

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;


vec4 pointLight() {
	vec3 lightVec = lightPos - crntPos;
	float dist = length(lightVec);
	float a = 3.0;
	float b = 0.7;
	float inten = 1.0f / (a * dist * dist + b * dist + 1.0f);

	float ambient = 0.20f;
	vec3 normalized = normalize(normal);
	vec3 lightDirection = normalize(lightVec);
	float diffuse = max(dot(normalized, lightDirection), 0.0f);
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normalized);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	return (texture(tex0, texCoord) * lightColor * (diffuse * inten + ambient) + texture(tex1, texCoord).r * specular * inten) * lightColor;
}


vec4 directLight() {
	float ambient = 0.20f;
	vec3 normalized = normalize(normal);
	vec3 lightDirection = normalize(vec3(1.0f, 1.0f, 0.0f));
	float diffuse = max(dot(normalized, lightDirection), 0.0f);
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normalized);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	return (texture(tex0, texCoord) * lightColor * (diffuse + ambient) + texture(tex1, texCoord).r * specular) * lightColor;
}



vec4 spotLight() {
	float outerCone = 0.9f;
	float innerCone = 0.95f;
	
	float ambient = 0.20f;
	vec3 normalized = normalize(normal);
	vec3 lightDirection = normalize(lightPos - crntPos);
	float diffuse = max(dot(normalized, lightDirection), 0.0f);
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normalized);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	float angle = dot(vec3(0.0f, -1.0f, 0.0f), lightDirection);
	float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);

	return (texture(tex0, texCoord) * lightColor * (diffuse * inten + ambient) + texture(tex1, texCoord).r * specular * inten) * lightColor;
}



void main() {
	FragColor = pointLight() + directLight() + spotLight();
}
