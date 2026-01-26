#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in uint inColor;

out vec3 pColor;
out float pointSize;

uniform float pSize;
uniform vec3 iResolution;
void main()
{
    // xyPos = vec2((aPos.x+1)*iResolution.x/2,(aPos.y+1)*iResolution.y/2);
    gl_Position = vec4(aPos,0.0,1.0);
    pColor = vec3(1.0,0.0,1.0);
    pointSize = 5.0;
    gl_PointSize = 5.0;
}
