module Common

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

function ConvertToCart(A)
"Convert the spherical to cartezian coordinates"
    println(A[1], " ", A[2], " ", A[3])
    M = zeros(3)
    M[1] = sin(A[3])*cos(A[2])
    M[2] = sin(A[3])*sin(A[2])
    M[3] = cos(A[3])

    return M
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
    Y = M^-1*X
    R = zeros(3,3)
    R[1,1:3] = Y[1:3]
    R[2,1:3] = Y[4:6]
    R[3,1:3] = Y[7:9]
    return R
end

function FindPlan(A,B,C)
    "Find the equation of the plan going through the dot A, B and C"
    M = zeros(4,4)

    M[1,1:3] = A
    M[1,4] = 1
    M[2,1:3] = B
    M[2,4] = 1
    M[3,1:3] = C
    M[3,4] = 1
    M[4,4] = 1

    #|println(M)|#

    X = [0, 0, 0, 1]
    Y = M^-1*X
    return Y
end

function FindPlan(Face, object)
    counter = 0
    A = []
    B = []
    C = []
    for s in Face
        if counter == 0
            A = object[s]
        elseif counter == 1
            B = object[s]
        elseif counter == 2
            C = object[s]
        else
            continue
        end
        counter += 1
    end
    return FindPlan(A,B,C)
end
end
