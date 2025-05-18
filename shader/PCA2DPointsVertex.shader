#version 330 core
layout (location = 0) in vec3 pPos;
layout (location = 1) in vec3 inColor;
uniform mat4 model;
uniform mat4 view;
uniform float xMargin;
uniform float yMargin;
out vec3 pColor;
void main()
{
    vec4 tmpPos = view * model * vec4(pPos,1.0);
    if(tmpPos.x>xMargin-1&&
       tmpPos.x<1-xMargin&&
       tmpPos.y>yMargin-1&&
       tmpPos.y<1-yMargin){
        gl_Position = vec4(tmpPos.x,tmpPos.y,0.0,1.0);
    }
    else{
        gl_Position = vec4(100,100,0.0,1.0);
    }
    pColor = inColor;
}
