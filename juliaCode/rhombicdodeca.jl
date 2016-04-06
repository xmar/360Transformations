
module Rhom
include("rotation.jl")

Rhomb = Dict(
:A => [1, 1, 1],
:B => [1, -1, 1],
:C => [1, -1, -1],
:D => [1, 1, -1],
:E => [-1, 1, -1],
:F => [-1, 1, 1],
:G => [-1, -1, 1],
:H => [-1, -1, -1],

:I => [0, 0, 2],
:J => [2, 0 , 0],
:K => [0, 2, 0],
:L => [0, 0, -2],
:M => [-2, 0, 0],
:N => [0, -2, 0],
)

firstTransformation = sqrt(2)*Rot.RotMat([0,1,0], pi/4)/2

Rhomb2 = Dict()
for i in keys(Rhomb)
   Rhomb2[i] = firstTransformation*Rhomb[i]
end
Rhomb = Rhomb2

Face1 = Set([:A,:I,:B,:J])
Face2 = Set([:F,:I,:B,:K])

function FindTransformation(F1, F2)
end

end
