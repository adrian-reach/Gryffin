-- rotate_cube.lua
function Start()
    print("Rotate Cube Script Started!")
end

function Update(deltaTime)
    local rotation = transform:getEulerAngles()
    rotation.y = rotation.y + 45.0 * deltaTime
    transform:setEulerAngles(rotation)
end