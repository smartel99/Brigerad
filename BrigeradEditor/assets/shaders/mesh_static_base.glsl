// -----------------------------
// -- Brigerad Engine Mesh    --
// -----------------------------
// Note: this shader is still very much in progress. There are likely many bugs and future additions
// that will go in.
//       Currently heavily updated.
//
// References upon which this is based:
// - Unreal Engine 4 PBR notes
// (https://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf)
// - Frostbite's SIGGRAPH 2014 paper
// (https://seblagarde.wordpress.com/2015/07/14/siggraph-2014-moving-frostbite-to-physically-based-rendering/)
// - Michał Siejak's PBR project (https://github.com/Nadrin)
// - TheCherno's implementation from years ago in the Sparky engine
// (https://github.com/TheCherno/Sparky)
#type vertex
#version 430 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec3 a_tangent;
layout(location = 3) in vec3 a_binormal;
layout(location = 4) in vec2 a_texCoord;

uniform mat4 u_ViewProjectionMatrix;
uniform mat4 u_ModelMatrix;

out VertexOutput
{
    vec3  WorldPosition;
    vec3  Normal;
    vec2  TexCoord;
    mat3  WorldNormals;
    vec3  Binormal;
    float LightIntensity;
}
vs_Output;

void main()
{
    vs_Output.WorldPosition = vec3(u_ModelMatrix * vec4(a_position, 1.0));
    vs_Output.Normal        = a_normal;
    vs_Output.TexCoord      = vec2(a_texCoord.x, 1.0 - a_texCoord.y);
    vs_Output.WorldNormals  = mat3(u_ModelMatrix) * mat3(a_tangent, a_binormal, a_normal);
    vs_Output.Binormal      = a_binormal;

    vs_Output.LightIntensity = vs_Output.WorldPosition.z;
    gl_Position              = u_ViewProjectionMatrix * u_ModelMatrix * vec4(a_position, 1.0);
}



#type fragment
#version 430 core

layout(location = 0) out vec4 color;

in VertexOutput
{
    vec3  WorldPosition;
    vec3  Normal;
    vec2  TexCoord;
    mat3  WorldNormals;
    vec3  Binormal;
    float LightIntensity;
}
vs_Input;

void main()
{
    vec3 fragColor = vec3(1.0, 1.0, 1.0) * vs_Input.LightIntensity;
    color          = vec4(vs_Input.WorldPosition * 0.5 + 0.5, 1.0);
}
