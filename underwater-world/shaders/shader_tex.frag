#version 410 core

uniform sampler2D textureSampler;
uniform vec3 lightDir;

in vec3 interpNormal;
in vec2 interpTexCoord;

void main()
{
	vec2 modifiedTexCoord = vec2(interpTexCoord.x, 1.0 - interpTexCoord.y); // Poprawka dla tekstur Ziemi, ktore bez tego wyswietlaja sie 'do gory nogami'
	vec3 color = texture2D(textureSampler, modifiedTexCoord).rgb;
	vec3 normal = normalize(interpNormal);
	float ambient = 0.2;
	float diffuse = max(dot(normal, -lightDir), 0.1);
	gl_FragColor = vec4(color * (ambient + (1-ambient) * diffuse * 0.6 ), 1.0);
}
