local LuaActor ={}

-- override event from blueprint
function LuaActor:ReceiveBeginPlay()
    self.bCanEverTick = true
    -- set bCanBeDamaged property in parent
    self.bCanBeDamaged = false
    print("actor:ReceiveBeginPlay")
end

-- override event from blueprint
function LuaActor:ReceiveEndPlay(reason)
    print("actor:ReceiveEndPlay")
end

function LuaActor:Tick(dt)
    print("actor:Tick",self,dt)
    -- call actor function
    local pos = self:K2_GetActorLocation()
    -- can pass self as Actor*
    local dist = self:GetHorizontalDistanceTo(self)
    print("actor pos",pos,dist)
end

return Class(nil, nil, LuaActor)