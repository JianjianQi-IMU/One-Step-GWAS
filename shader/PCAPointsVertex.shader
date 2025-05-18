#version 330 core
layout (location = 0) in vec3 pPos;
layout (location = 1) in vec3 inColor;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec3 pColor;
void main()
{
    gl_Position = projection * view * model * vec4(pPos,1.0);
    pColor = inColor;
}
