#version 330 core
layout (location = 0) in vec2 pPos;
uniform vec3 pointsColor;
out vec3 pColor;
void main()
{
    gl_Position = vec4(pPos,0.0f,1.0f);
    pColor = pointsColor;
}
