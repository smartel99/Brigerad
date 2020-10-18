Player = {}
local texture
local subTexture
local time = 0.0

Player.col = vec4.new(0.0, 1.0, 0.0, 1.0)

function Player.OnCreate()
    texture = Texture2D.Create("assets/textures/Rald.png")
    subTexture = SubTexture2D.CreateFromCoords(texture, vec2.new(0.0, 0.0),
                                               vec2.new(250.0, 250.0),
                                               vec2.new(1.0, 1.0))
    local tag = this:GetTagComponent().tag
    local pos = this:GetTransformComponent():GetPosition()
    print(pos.x)
    print(tag .. " pos: x: " .. pos.x .. "\ty: " .. pos.y .. "\tz: " .. pos.z)
end

function Player.OnUpdate(ts) 
    time = time+ts
    local y = (6.283 * math.sin(time))
    this:GetTransformComponent():SetPosition(vec3.new(0.0, y, 0.0))
end

function Player.OnRender()
    Renderer2D.DrawQuad(vec2.new(1.0, 0.0), vec2.new(1.0, 1.0), Player.col)
    Renderer2D.DrawQuad(vec2.new(2.0, 0.0), vec2.new(1.0, 1.0), texture,
                        vec2.new(1, 1), vec4.new(1.0, 0.0, 0.0, 1.0))
    Renderer2D.DrawQuad(vec2.new(3.0, 0.0), vec2.new(1.0, 1.0), subTexture)
end

function Player.OnDestroy() print("OnDestroy!") end
