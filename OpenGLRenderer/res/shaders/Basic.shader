#shader vertex
#version 330 core

layout(location = 0) in vec4 position;

uniform float u_Time;
uniform mat4 u_MVP;


void main()
{
    float multx = 0.25 * sin(u_Time) + 1.0;
    float multy = 0.25 * sin(u_Time*3) + 1.0;
    float posx = position.x * 640 *multx;
    float posy = position.y * 480 *multy;
    float alpha = atan(posy,posx);
    float mag = sqrt(pow(posy,2) + pow(posx,2));

    posx = mag * cos(u_Time + alpha) / 640.0;
    posy = mag * sin(u_Time + alpha) / 480.0;

    gl_Position = u_MVP * vec4(posx,posy, 0.0, 1.0);
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform vec4 u_Color;

void main()
{
   color = u_Color;
}