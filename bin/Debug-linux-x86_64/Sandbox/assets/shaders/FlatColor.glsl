#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;

uniform mat4 u_vp;
uniform mat4 u_transform;

out vec3 v_Position;

void main()
{
    v_Position = a_Position;
    // Set the position depending on the model and the camera.
    gl_Position = u_vp * u_transform * vec4(a_Position, 1.0);
}


#type fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform vec3 u_Color;

void main()
{
    color = vec4(u_Color, 1.0);
}