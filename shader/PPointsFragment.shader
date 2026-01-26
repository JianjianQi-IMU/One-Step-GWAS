#version 330 core
in vec3 pColor;
in float pointSize;
out vec4 FragColor;
void main()
{
    /*
    vec2 coord = gl_PointCoord - vec2(0.5);
    float distance = length(coord);
    
    if (distance > 0.5) {
        discard;
    }
    
    float alpha = 1.0 - smoothstep(0.4, 0.5, distance);
    FragColor = vec4(pColor.rgb, alpha);
    */
    FragColor = vec4(0.0,0.0,1.0, 1.0);
}
