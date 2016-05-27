module Rhombic
include("common.jl")


function getAngles()
    M1 = [1, 0, 0]
    F = [1, -1, 1]
    J = [2, 0 , 0]
    E = [-1, -1, 1]
    G = [1, 1, 1]
    N = [0, 0, 2]

    println("M1=", M1)
    R = zeros(5,3)
    R[1,1:3] = M1
    println(R[1,1:3])

    R[2,1:3] = F
    R[3,1:3] = J
    R[4,1:3] = F+(F-J)/2
    M2 = R[4,1:3]/norm(R[4,1:3])
    
    println(R[4,1:3])
    println("M2=",M2)

    R[5,1:3] = E+(E-N)/2
    M3 = R[5,1:3]/norm(R[5,1:3])

    T = Common.FindTransMat(M2,M1,M3,M1,G,N)
    println(T)

    theta = rad2deg(atan2(T[3,3],T[3,2]))
    println("theta=", theta)
    phi = rad2deg(atan2(sqrt(T[3,2]^2+T[3,3]^2),-T[3,1]))
    println("phi=", phi)
end

getAngles()

end
