#pragma once

#include "Brigerad.h"
#include "panels/SceneHierarchyPanel.h"
#include "Brigerad/Renderer/Mesh.h"

namespace Brigerad
{
class EditorLayer : public Layer
{
public:
    EditorLayer();
    virtual ~EditorLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;

    void         OnUpdate(Timestep ts) override;
    virtual void OnImGuiRender() override;
    void         OnEvent(Event& e) override;

private:
    struct AlbedoInput
    {
        glm::vec3 Color = {
          0.972f,
          0.96f,
          0.915f};    // Silver, from
                      // https://docs.unrealengine.com/en-us/Engine/Rendering/Materials/PhysicallyBased
        Ref<Texture2D> TextureMap;
        bool           SRGB       = true;
        bool           UseTexture = false;
    };
    AlbedoInput m_albedoInput;

    struct NormalInput
    {
        Ref<Texture2D> TextureMap;
        bool           UseTexture = false;
    };
    NormalInput m_normalInput;

    struct MetalnessInput
    {
        float          Value = 1.0f;
        Ref<Texture2D> TextureMap;
        bool           UseTexture = false;
    };
    MetalnessInput m_metalnessInput;

    struct RoughnessInput
    {
        float          Value = 0.5f;
        Ref<Texture2D> TextureMap;
        bool           UseTexture = false;
    };
    RoughnessInput m_roughnessInput;

    struct Light
    {
        glm::vec3 Direction;
        glm::vec3 Radiance;
    };
    Light m_Light;
    float m_LightMultiplier = 0.3f;

    // PBR params
    float m_Exposure = 1.0f;

    bool m_RadiancePrefilter = false;

    float m_EnvMapRotation = 0.0f;

    glm::vec4        m_color = {1.0f, 0.0f, 0.0f, 1.0f};
    Ref<Texture2D>   m_texture;
    Ref<Framebuffer> m_fb;

    Ref<Scene> m_scene;
    Entity     m_squareEntity;
    Entity     m_textureEntity;
    Entity     m_cameraEntity;
    Entity     m_cameraEntity2;
    Entity     m_imguiWindowEntity;
    Entity     m_meshEntity;

    Ref<TextureCube> m_environmentCubeMap, m_environmentIrradiance;
    Ref<Texture2D>   m_brdfLut;
    // Ref<Mesh> m_mesh;

    SceneHierarchyPanel m_sceneHierarchyPanel;

    glm::vec2 m_viewportSize = glm::vec2 {0.0f};

    bool m_viewportFocused = false;
    bool m_viewportHovered = false;
};
}    // namespace Brigerad
