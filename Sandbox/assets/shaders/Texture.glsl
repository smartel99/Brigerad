// Basic texture shader.

#type fragment
#version 330 core
        
layout(location = 0) out vec4 color;

in vec2 v_TextCoord;

uniform sampler2D u_Texture;
uniform vec2 u_TextureScale;
uniform vec4 u_Color;

void main()
{
    color = texture(u_Texture, v_TextCoord * u_TextureScale) * u_Color;
    //color = vec4(v_TextCoord, 0.0 , 1.0);
}


#type vertex
#version 330 core
        
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TextCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec2 v_TextCoord;

void main()
{
    v_TextCoord = a_TextCoord;
    // Set the position depending on the model and the camera.
    gl_Position = u_ViewProjection  * u_Transform * vec4(a_Position, 1.0);
}
