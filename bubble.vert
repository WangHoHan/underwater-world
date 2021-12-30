# version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex;


out vec3 crntPos;
out vec3 normal;
out vec3 color;
out vec2 texCoord;


uniform mat4 camMatrix;
uniform mat4 model;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;
uniform vec3 move;


void main() {
	crntPos = vec3(model * translation * -rotation * scale * vec4(aPos, 1.0f));
	normal = aNormal;
	color = aColor;
	texCoord = mat2(0.0, -1.0, 1.0, 0.0) * aTex;
	gl_Position = camMatrix * vec4(crntPos.x + move.x, crntPos.y + move.y, crntPos.z + move.z, 1.0);
}
