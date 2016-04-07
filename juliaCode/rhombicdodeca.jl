
module Rhom
include("common.jl")

Rhomb = Dict(
:A => [-1, -1, -1],
:B => [1, -1, -1],
:C => [1, 1, -1],
:D => [-1, 1, -1],
:E => [-1, -1, 1],
:F => [1, -1, 1],
:G => [1, 1, 1],
:H => [-1, 1, 1],

:I => [-2, 0, 0],
:J => [2, 0 , 0],
:K => [0, -2, 0],
:L => [0, 2, 0],
:M => [0, 0, -2],
:N => [0, 0, 2],
)

firstTransformation = sqrt(2)/2*Common.RotMat([0,1,0], pi/4)
for i in 1:3
    for j in 1:3
        if firstTransformation[i,j] ≈ 0.5
            firstTransformation[i,j] = 0.5
        elseif firstTransformation[i,j] ≈ -0.5
            firstTransformation[i,j] = -0.5
        elseif firstTransformation[i,j] ≈ sqrt(2)/2
            firstTransformation[i,j] = sqrt(2)/2
        elseif firstTransformation[i,j] ≈  0
            firstTransformation[i,j] = 0
        end
    end
end
println(firstTransformation)

Rhomb2 = Dict()
for i in keys(Rhomb)
   Rhomb2[i] = firstTransformation*Rhomb[i]
end
Rhomb = Rhomb2

function PrintlnFace(F, text="")
    print(text)
    for s in F
        print("$s:$(Rhomb[s]) ")
    end
    println("")
end

Face1 = Set([:J,:F,:G,:N])
PrintlnFace(Face1, "Face1: ")
Face2 = Set([:G,:N,:L,:H])
PrintlnFace(Face2, "Face2: ")
Face3 = Set([:G,:J,:C,:L])
PrintlnFace(Face3, "Face3: ")
Face4 = Set([:N,:F,:E,:K])
PrintlnFace(Face4, "Face4: ")
Face5 = Set([:F, :J, :B, :K])
PrintlnFace(Face5, "Face5: ")
Face6 = Set([:J, :B, :M, :C])
PrintlnFace(Face6, "Face6: ")
Face7 = Set([:K, :B, :M, :A])
PrintlnFace(Face7, "Face7: ")
Face8 = Set([:L, :D, :C, :M])
PrintlnFace(Face8, "Face8: ")
Face9 = Set([:L, :D, :I, :H])
PrintlnFace(Face9, "Face9: ")
Face10 = Set([:K, :A, :I, :E])
PrintlnFace(Face10, "Face10: ")
Face11 = Set([:N, :E, :I, :H])
PrintlnFace(Face11, "Face11: ")
Face12 = Set([:M, :A, :I, :D])
PrintlnFace(Face12, "Face12: ")

RhombFace = Dict(:Face1=>Face1,
                  :Face2=>Face2,
                  :Face3=>Face3,
                  :Face4=>Face4,
                  :Face5=>Face5,
                  :Face6=>Face6,
                  :Face7=>Face7,
                  :Face8=>Face8,
                  :Face9=>Face9,
                  :Face10=>Face10,
                  :Face11=>Face11,
                  :Face12=>Face12,
                  )





function FindTransformation(F1, F2)
    f1a = [s for s in F1] 
    f2a = [s for s in F2] 
    for checkEigVal in [true, false]
        for i in 1:4
            s1 = f2a[i]
            f2a2 = [s for s in filter(s -> s != s1,f2a)]
            for j in 1:3
                s2 = f2a2[j]
                f2a3 = [s for s in filter(s -> s != s2,f2a2)]
                for k in 1:2
                    s3 = f2a3[k]
                    f2a4 = [s for s in filter(s -> s != s3,f2a3)]
                    R = Common.FindTransMat(Rhomb[f1a[1]],Rhomb[f1a[2]], Rhomb[f1a[3]], Rhomb[s1], Rhomb[s2], Rhomb[s3])
                    if R*Rhomb[f1a[4]] ≈ Rhomb[f2a4[1]]
                        #println("Found the transformation matrice for $(f1a[1])->$(s1); $(f1a[2])->$(s2); $(f1a[3])->$(s3); $(f1a[4])->$(f2a4[1])")
                        #println(det(R))
                        #println(R^-1)
                        #println(transpose(R))
                        #println(R)
                        #println(eigvals(R))
                        counterRealPositif = 0
                        counterRealNegatif = 0
                        if checkEigVal
                            for e in eigvals(R)
                                if angle(e) == 0
                                    counterRealPositif += 1
                                elseif angle(-e) == 0
                                    counterRealNegatif += 1
                                end
                            end
                        else
                            counterRealPositif = 1
                        end
                        if det(R) ≈ 1 && R^-1 ≈ transpose(R) && counterRealPositif == 1 && counterRealNegatif == 0
                            #println("############R is a rotation matrix")
                            return R
                        end
                    else
                        #println("Cannot tranform $(f1a[1])->$(s1); $(f1a[2])->$(s2); $(f1a[3])->$(s3); $(f1a[4])->$(f2a4[1])")
                    end
                end
            end
        end
    end
    return zeros(3,3)       
end
function FindTransformation(F1::Symbol, F2::Symbol)
    FindTransformation(RhombFace[F1], RhombFace[F2])
end

function GetAllFaceEquationAndRotation(Face)
    "Face is the reference face"
    println("Reference face $Face equation: $(Common.FindPlan(RhombFace[Face], Rhomb))")
    for f in RhombFace
        if f[1] != Face
            println("$(f[1]): plan equation: $(Common.FindPlan(f[2], Rhomb)); rotation matrix:\n $(FindTransformation(RhombFace[Face], f[2]))")
        end
end
end

end
