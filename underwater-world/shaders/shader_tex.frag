#version 410 core

uniform sampler2D textureSampler;
uniform sampler2D normalSampler;
uniform sampler2D depthMap;
//uniform vec3 lightDir;

in vec3 interpNormal;
in vec2 interpTexCoord;
in vec3 viewDir;
in vec3 viewDirTS;
in vec3 lightDirTS;
uniform int isParallax;

float near = 0.02f;
float far = 100.0f;

vec2 texCoords;
float height_scale = 0.5f;
  
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);

float linearizeDepth(float depth)
{
	return (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
}

float logisticDepth(float depth, float steepness, float offset)
{
	float zVal = linearizeDepth(depth);
	return (1 / (1 + exp(-steepness * (zVal - offset))));
}

void main()
{
	//vec3 lightDir = normalize(lightDirTS);
	//vec3 V = normalize(viewDirTS);
	//vec3 normal = normalize(vec3(texture2D(normalSampler, interpTexCoord)) * 2 - 1);
	//vec3 R = reflect(-normalize(lightDir),normal);

	//texCoords = ParallaxMapping(interpTexCoord,  viewDir);
	//if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
	//	discard;

	texCoords = interpTexCoord;
	vec3 lightDir = normalize(lightDirTS);
	vec3 V = normalize(viewDirTS);
	vec3 normal = normalize(vec3(texture2D(normalSampler, texCoords)) * 2 - 1);
	vec3 R = reflect(-normalize(lightDir),normal);

	float depth = logisticDepth(gl_FragCoord.z, 0.1f, 3.0f);
	vec2 modifiedTexCoord = vec2(texCoords.x, 1.0 - texCoords.y); // Poprawka dla tekstur Ziemi, ktore bez tego wyswietlaja sie 'do gory nogami'
	vec3 color = texture2D(textureSampler, modifiedTexCoord).rgb;
	//vec3 normal = normalize(interpNormal);
	float ambient = 0.2;
	float diffuse = max(dot(normal, -lightDir), 0.1);
	gl_FragColor = vec4(color * (ambient + (1-ambient) * diffuse * 0.6 ), 1.0) * (1.0f - depth) + vec4(depth * vec3(0.0f, 0.109f, 0.447f), 1.0f);;
	//gl_FragColor = vec4(color * (ambient + (1-ambient) * diffuse * 0.6 ), 1.0) * (1.0f - depth) + vec4(depth * vec3(0.219f, 0.407f, 0.705f), 1.0f);;
}

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
	const float numLayers = 10;
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;

    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy * height_scale; 
    vec2 deltaTexCoords = P / numLayers;

    vec2  currentTexCoords = texCoords;
	float currentDepthMapValue = texture(depthMap, currentTexCoords).r;
  
	while(currentLayerDepth < currentDepthMapValue)
	{
		// shift texture coordinates along direction of P
		currentTexCoords -= deltaTexCoords;
		// get depthmap value at current texture coordinates
		currentDepthMapValue = texture(depthMap, currentTexCoords).r;  
		// get depth of next layer
		currentLayerDepth += layerDepth;  
	}

	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

	// get depth after and before collision for linear interpolation
	float afterDepth  = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = texture(depthMap, prevTexCoords).r - currentLayerDepth + layerDepth;
 
	// interpolation of texture coordinates
	float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

	return finalTexCoords;  
} 