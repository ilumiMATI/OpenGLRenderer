#shader vertex
#version 330 core

layout(location = 0) in vec4 position;

uniform float u_Time;
uniform mat4 u_MVP;


void main()
{
    vec4 pos = u_MVP * position;

    gl_Position = pos;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform vec4 u_Color;

void main()
{
   color = u_Color;
}