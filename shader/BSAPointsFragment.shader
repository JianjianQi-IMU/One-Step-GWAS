#version 330 core
out vec4 FragColor;
uniform vec3 pColor;
uniform float leftMargin;
uniform vec3 iResolution;
void main()
{
    float xpos = 2*gl_FragCoord.x/iResolution.x-1;
    if(xpos > leftMargin){
        FragColor = vec4(pColor,1.0);
    }
    else{
        FragColor = vec4(vec3(1.0),0.0);
    }
}
