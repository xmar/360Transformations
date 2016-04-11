
module CubeMod
include("common.jl")

Cube = Dict(
:A => [1, -1, 1],
:B => [1, 1, 1],
:C => [1, 1, -1],
:D => [1, -1, -1],
:E => [-1, -1, -1],
:F => [-1, -1, 1],
:G => [-1, 1, 1],
:H => [-1, 1, -1]
)

function PrintlnFace(F, text="")
    print(text)
    for s in F
        print("$s:$(Cube[s]) ")
    end
    println("")
end

Face1 = Set([:A,:B,:C,:D])
PrintlnFace(Face1, "Face1: ")
Face2 = Set([:B,:G,:C,:H])
PrintlnFace(Face2, "Face2: ")
Face3 = Set([:G,:H,:F,:E])
PrintlnFace(Face3, "Face3: ")
Face4 = Set([:A,:F,:D,:E])
PrintlnFace(Face4, "Face4: ")
Face5 = Set([:F, :G, :A, :B])
PrintlnFace(Face5, "Face5: ")
Face6 = Set([:D, :E, :C, :H])
PrintlnFace(Face6, "Face6: ")

CubeFace = Dict(:Face1=>Face1,
                  :Face2=>Face2,
                  :Face3=>Face3,
                  :Face4=>Face4,
                  :Face5=>Face5,
                  :Face6=>Face6,
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
                    R = Common.FindTransMat(Cube[f1a[1]],Cube[f1a[2]], Cube[f1a[3]], Cube[s1], Cube[s2], Cube[s3])
                    if R*Cube[f1a[4]] ≈ Cube[f2a4[1]]
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
    FindTransformation(CubeFace[F1], CubeFace[F2])
end

function GetAllFaceEquationAndRotation(Face)
    "Face is the reference face"
    println("Reference face $Face equation: $(Common.FindPlan(CubeFace[Face], Cube))")
    for f in CubeFace
        if f[1] != Face
            println("$(f[1]): plan equation: $(Common.FindPlan(f[2], Cube)); rotation matrix:\n $(FindTransformation(CubeFace[Face], f[2]))")
        end
end
end

end
