// Basic texture shader.

#type vertex
#version 330 core
        
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec2 a_TilingFactor;
layout(location = 4) in float a_TexIndex;
layout(location = 5) in float a_IsText;

uniform mat4 u_ViewProjection;

out vec4 v_Color;
out vec2 v_TexCoord;
out vec2 v_TilingFactor;
out float v_TexIndex;
out float v_IsText;

void main()
{
    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
    v_TilingFactor = a_TilingFactor;
    v_TexIndex = a_TexIndex;
    v_IsText = a_IsText;
    // Set the position depending on the model and the camera.
    gl_Position = u_ViewProjection* vec4(a_Position, 1.0);
}


#type fragment
#version 330 core
        
layout(location = 0) out vec4 color;

in vec4 v_Color;
in vec2 v_TexCoord;
in vec2 v_TilingFactor;
in float v_TexIndex;
in float v_IsText;

uniform sampler2D u_Textures[32];

void RenderTexture()
{
    vec4 texColor = v_Color;
    switch(int(v_TexIndex))
    {
        case 0: texColor *= texture(u_Textures[0], v_TexCoord * v_TilingFactor); break;
        case 1: texColor *= texture(u_Textures[1], v_TexCoord * v_TilingFactor); break;
        case 2: texColor *= texture(u_Textures[2], v_TexCoord * v_TilingFactor); break;
        case 3: texColor *= texture(u_Textures[3], v_TexCoord * v_TilingFactor); break;
        case 5: texColor *= texture(u_Textures[5], v_TexCoord * v_TilingFactor); break;
        case 6: texColor *= texture(u_Textures[6], v_TexCoord * v_TilingFactor); break;
        case 7: texColor *= texture(u_Textures[7], v_TexCoord * v_TilingFactor); break;
        case 8: texColor *= texture(u_Textures[8], v_TexCoord * v_TilingFactor); break;
        case 9: texColor *= texture(u_Textures[9], v_TexCoord * v_TilingFactor); break;
        case 10: texColor *= texture(u_Textures[10], v_TexCoord * v_TilingFactor); break;
        case 11: texColor *= texture(u_Textures[11], v_TexCoord * v_TilingFactor); break;
        case 12: texColor *= texture(u_Textures[12], v_TexCoord * v_TilingFactor); break;
        case 13: texColor *= texture(u_Textures[13], v_TexCoord * v_TilingFactor); break;
        case 14: texColor *= texture(u_Textures[14], v_TexCoord * v_TilingFactor); break;
        case 15: texColor *= texture(u_Textures[15], v_TexCoord * v_TilingFactor); break;
        case 16: texColor *= texture(u_Textures[16], v_TexCoord * v_TilingFactor); break;
        case 17: texColor *= texture(u_Textures[17], v_TexCoord * v_TilingFactor); break;
        case 18: texColor *= texture(u_Textures[18], v_TexCoord * v_TilingFactor); break;
        case 19: texColor *= texture(u_Textures[19], v_TexCoord * v_TilingFactor); break;
        case 20: texColor *= texture(u_Textures[20], v_TexCoord * v_TilingFactor); break;
        case 21: texColor *= texture(u_Textures[21], v_TexCoord * v_TilingFactor); break;
        case 22: texColor *= texture(u_Textures[22], v_TexCoord * v_TilingFactor); break;
        case 23: texColor *= texture(u_Textures[23], v_TexCoord * v_TilingFactor); break;
        case 24: texColor *= texture(u_Textures[24], v_TexCoord * v_TilingFactor); break;
        case 25: texColor *= texture(u_Textures[25], v_TexCoord * v_TilingFactor); break;
        case 26: texColor *= texture(u_Textures[26], v_TexCoord * v_TilingFactor); break;
        case 27: texColor *= texture(u_Textures[27], v_TexCoord * v_TilingFactor); break;
        case 28: texColor *= texture(u_Textures[28], v_TexCoord * v_TilingFactor); break;
        case 29: texColor *= texture(u_Textures[29], v_TexCoord * v_TilingFactor); break;
        case 30: texColor *= texture(u_Textures[30], v_TexCoord * v_TilingFactor); break;
        case 31: texColor *= texture(u_Textures[31], v_TexCoord * v_TilingFactor); break;
    }
    color = texColor;
}

void RenderText()
{
    vec4 texColor = v_Color;
    
    switch(int(v_TexIndex))
    {
        case 0: texColor *=  vec4(1.0, 1.0, 1.0, texture(u_Textures[0], v_TexCoord ).r); break;
        case 1: texColor *=  vec4(1.0, 1.0, 1.0, texture(u_Textures[1], v_TexCoord ).r); break;
        case 2: texColor *=  vec4(1.0, 1.0, 1.0, texture(u_Textures[2], v_TexCoord ).r); break;
        case 3: texColor *=  vec4(1.0, 1.0, 1.0, texture(u_Textures[3], v_TexCoord ).r); break;
        case 5: texColor *=  vec4(1.0, 1.0, 1.0, texture(u_Textures[5], v_TexCoord ).r); break;
        case 6: texColor *=  vec4(1.0, 1.0, 1.0, texture(u_Textures[6], v_TexCoord ).r); break;
        case 7: texColor *=  vec4(1.0, 1.0, 1.0, texture(u_Textures[7], v_TexCoord ).r); break;
        case 8: texColor *=  vec4(1.0, 1.0, 1.0, texture(u_Textures[8], v_TexCoord ).r); break;
        case 9: texColor *=  vec4(1.0, 1.0, 1.0, texture(u_Textures[9], v_TexCoord ).r); break;
        case 10: texColor *= vec4(1.0, 1.0, 1.0, texture(u_Textures[10], v_TexCoord).r); break;
        case 11: texColor *= vec4(1.0, 1.0, 1.0, texture(u_Textures[11], v_TexCoord).r); break;
        case 12: texColor *= vec4(1.0, 1.0, 1.0, texture(u_Textures[12], v_TexCoord).r); break;
        case 13: texColor *= vec4(1.0, 1.0, 1.0, texture(u_Textures[13], v_TexCoord).r); break;
        case 14: texColor *= vec4(1.0, 1.0, 1.0, texture(u_Textures[14], v_TexCoord).r); break;
        case 15: texColor *= vec4(1.0, 1.0, 1.0, texture(u_Textures[15], v_TexCoord).r); break;
        case 16: texColor *= vec4(1.0, 1.0, 1.0, texture(u_Textures[16], v_TexCoord).r); break;
        case 17: texColor *= vec4(1.0, 1.0, 1.0, texture(u_Textures[17], v_TexCoord).r); break;
        case 18: texColor *= vec4(1.0, 1.0, 1.0, texture(u_Textures[18], v_TexCoord).r); break;
        case 19: texColor *= vec4(1.0, 1.0, 1.0, texture(u_Textures[19], v_TexCoord).r); break;
        case 20: texColor *= vec4(1.0, 1.0, 1.0, texture(u_Textures[20], v_TexCoord).r); break;
        case 21: texColor *= vec4(1.0, 1.0, 1.0, texture(u_Textures[21], v_TexCoord).r); break;
        case 22: texColor *= vec4(1.0, 1.0, 1.0, texture(u_Textures[22], v_TexCoord).r); break;
        case 23: texColor *= vec4(1.0, 1.0, 1.0, texture(u_Textures[23], v_TexCoord).r); break;
        case 24: texColor *= vec4(1.0, 1.0, 1.0, texture(u_Textures[24], v_TexCoord).r); break;
        case 25: texColor *= vec4(1.0, 1.0, 1.0, texture(u_Textures[25], v_TexCoord).r); break;
        case 26: texColor *= vec4(1.0, 1.0, 1.0, texture(u_Textures[26], v_TexCoord).r); break;
        case 27: texColor *= vec4(1.0, 1.0, 1.0, texture(u_Textures[27], v_TexCoord).r); break;
        case 28: texColor *= vec4(1.0, 1.0, 1.0, texture(u_Textures[28], v_TexCoord).r); break;
        case 29: texColor *= vec4(1.0, 1.0, 1.0, texture(u_Textures[29], v_TexCoord).r); break;
        case 30: texColor *= vec4(1.0, 1.0, 1.0, texture(u_Textures[30], v_TexCoord).r); break;
        case 31: texColor *= vec4(1.0, 1.0, 1.0, texture(u_Textures[31], v_TexCoord).r); break;
        
    }
    color = texColor;
    //color = vec4(1.0, 0.0,0.0,1.0);
}

void main()
{
    // TODO: This apparently doesn't work on AMD GPUs, need to be tested.
    //color = texture(u_Textures[int(v_TexIndex)], v_TexCoord * v_TilingFactor) * v_Color;
    if(int(v_IsText) == 1)
    {
        RenderText();
    }
    else
    {
        RenderTexture();
    }
}
