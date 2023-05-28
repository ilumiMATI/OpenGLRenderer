#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
out vec4 frag_color;
uniform mat4 u_MVP;


void main()
{
    vec4 pos = u_MVP * position;

    gl_Position = pos;
    frag_color = color;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
in vec4 frag_color;

uniform vec4 u_Color;

void main()
{
   color = u_Color;
}