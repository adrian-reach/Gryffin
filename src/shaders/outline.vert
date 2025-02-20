#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float scaleFactor;

void main()
{
    // Scale the vertex along its normal direction
    vec3 scaledPos = aPos + aNormal * (scaleFactor - 1.0);
    gl_Position = projection * view * model * vec4(scaledPos, 1.0);
}
