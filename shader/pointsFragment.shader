#version 450 core
in vec2 xyPos;
flat in uint idColor;
out vec4 FragColor;
uniform float pSize;
uniform vec3 colorList[8];


float isPaint(vec2 inPoint,vec2 cenPoint,float inSize)
{
    float size2 = inSize * inSize;
    float dx = inPoint.x - cenPoint.x;
    float dy = inPoint.y - cenPoint.y;
    return step(size2,dx*dx+dy*dy);
};

float paintVal(vec2 inPoint,vec2 cenPoint,float x1,float x2)
{
    float size1 = x1*x1;
    float size2 = x2*x2;
    float dx = inPoint.x - cenPoint.x;
    float dy = inPoint.y - cenPoint.y;
    float sizev = dx*dx+dy*dy;
    return smoothstep(size1,size2,sizev);
};

void main()
{
    /*float id = paintVal(gl_FragCoord.xy,xyPos,pSize,pSize+1);*/
    float id = isPaint(gl_FragCoord.xy,xyPos,pSize);
    if(1.0-id<0.01) discard;
    /*FragColor = vec4(pColor,1.0)*(1-id)+vec4(vec3(1.0),1.0)*id;*/
    if(idColor==0) FragColor = vec4(colorList[1],1.0);
    else FragColor = vec4(colorList[0],1.0);
    //FragColor = vec4(vec3(idColor/8),1.0);

}
