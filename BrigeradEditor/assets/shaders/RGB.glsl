#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;

uniform mat4 u_vp;
uniform mat4 u_transform;

out vec3 v_Position;
out vec4 v_Color;

void main()
{
    v_Position = a_Position;
    v_Color = a_Color;
    // Set the position depending on the model and the camera.
    gl_Position = u_vp * u_transform * vec4(a_Position, 1.0);
}


#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 v_Position;
in vec4 v_Color;

void main()
{
    //color = vec4(v_Position * 0.5 + 0.5, 1.0);
    color = v_Color;
}
