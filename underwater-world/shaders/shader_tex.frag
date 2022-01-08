#version 410 core

uniform sampler2D textureSampler;
uniform vec3 lightDir;

in vec3 interpNormal;
in vec2 interpTexCoord;

float near = 0.02f;
float far = 100.0f;

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
	float depth = logisticDepth(gl_FragCoord.z, 0.1f, 3.0f);
	vec2 modifiedTexCoord = vec2(interpTexCoord.x, 1.0 - interpTexCoord.y); // Poprawka dla tekstur Ziemi, ktore bez tego wyswietlaja sie 'do gory nogami'
	vec3 color = texture2D(textureSampler, modifiedTexCoord).rgb;
	vec3 normal = normalize(interpNormal);
	float ambient = 0.2;
	float diffuse = max(dot(normal, -lightDir), 0.1);
	gl_FragColor = vec4(color * (ambient + (1-ambient) * diffuse * 0.6 ), 1.0) * (1.0f - depth) + vec4(depth * vec3(0.0f, 0.109f, 0.447f), 1.0f);;
}
