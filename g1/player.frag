#version 330 core

out vec4 color;

uniform vec3 bcolor;
 
void main() 
{    
    color = vec4(bcolor, 0.5)  ;
}  