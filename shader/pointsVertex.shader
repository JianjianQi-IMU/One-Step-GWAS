#version 450 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in uint inColor;
uniform vec3 iResolution;
out vec2 xyPos;
flat out uint idColor;
void main()
{
    xyPos = vec2((aPos.x+1)*iResolution.x/2,(aPos.y+1)*iResolution.y/2);
    gl_Position = vec4(aPos,0.0,1.0);
    idColor = inColor;
}
