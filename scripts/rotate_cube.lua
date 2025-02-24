-- rotate_cube.lua
function Start()
    print("Rotate Cube Script Started!")
    local rotation = transform:getEulerAngles()
    print("Start: Current Rotation: " .. rotation.x .. ", " .. rotation.y .. ", " .. rotation.z)
end

function Update(deltaTime)
    local rotation = transform:getEulerAngles()
    print("Current Rotation: " .. rotation.x .. ", " .. rotation.y .. ", " .. rotation.z)
    rotation.y = rotation.y + 45.0 * deltaTime
    print("New Rotation: " .. rotation.x .. ", " .. rotation.y .. ", " .. rotation.z)
    transform:setEulerAngles(rotation)
end