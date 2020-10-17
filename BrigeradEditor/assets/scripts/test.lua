Player = {}
local texture


function Player.OnCreate()
    print("OnCreate!")
    texture = Texture2D.Create("assets/textures/Rald.png")
end

function Player.OnUpdate(ts)
    Renderer2D:DrawQuad(0,-5,5,5,1.0,0.0,0.0,1.0)
end

function Player.OnDestroy()
    print("OnDestroy!")
end