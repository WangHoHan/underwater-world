#version 410 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projectionViewMatrix;

void main()
{
    gl_Position = projectionViewMatrix * vec4(aPos, 1.0);
    TexCoords = vec3(aPos.x, aPos.y, -aPos.z);
}  