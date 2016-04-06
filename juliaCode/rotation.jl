module Rot

function RotMat(v, theta)
    "v direction vector, theta = rotation angle"
    u = v / norm(v)
    R = zeros(3,3)
    c = cos(theta)
    s = sin(theta)
    R[1,1] = u[1]*u[1] * (1-c)+c
    R[1,2] = u[1]*u[2] * (1-c) - u[3]*s
    R[1,3] = u[1]*u[3] * (1-c) + u[2]*s
    
    R[2,1] = u[2]*u[1] * (1-c) + u[3]*s
    R[2,2] = u[2]*u[2] * (1-c) + c
    R[2,3] = u[2]*u[3] * (1-c) - u[1]*s

    R[3,1] = u[3]*u[1] * (1-c) - u[2]*s
    R[3,2] = u[3]*u[2] * (1-c) + u[1]*s
    R[3,3] = u[3]*u[3] * (1-c) + c

    R
end


function FindTransMat(A,B,C,D,E,F)
"Find the rotation matrice R with R*A=D; R*B=E; R*C=F"
    M = zeros(9,9)
    M[1,1:3] = A
    M[2,4:6] = A
    M[3,7:9] = A
    M[4,1:3] = B
    M[5,4:6] = B
    M[6,7:9] = B
    M[7,1:3] = C
    M[8,4:6] = C
    M[9,7:9] = C

    X = [D;E;F]
    Y = X\M
    R = zeros(3,3)
    R[1,1:3] = Y[1:3]
    R[2,1:3] = Y[4:6]
    R[3,1:3] = Y[7:9]
    R
end

end
