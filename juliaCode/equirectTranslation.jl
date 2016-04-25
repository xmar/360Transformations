
module EquiMod
include("common.jl")

Version1_1=Dict(
:A => [1, -3pi/8, 5pi/16],
:B => [1, 3pi/8, 5pi/16],
:C => [1, 0, pi/2],
:D => [1, -pi, 0],
:E => [1, -pi/4, 0],
:F => [1, -5pi/8, 3pi/16]
)

Version1_2=Dict(
:A => [1, -3pi/8, 5pi/16],
:B => [1, 3pi/8, 5pi/16],
:C => [1, 0, pi/2],
:D => [1, -pi/2, 0],
:E => [1, pi/4, 0],
:F => [1, -pi/8, 3pi/16]
)

Version1_3=Dict(
:A => [1, -3pi/8, 5pi/16],
:B => [1, 3pi/8, 5pi/16],
:C => [1, 0, pi/2],
:D => [1, 0, 0],
:E => [1, 3pi/4, 0],
:F => [1, 3pi/8, 3pi/16]
)

Version1_4=Dict(
:A =>[1,-3pi/8,                                                                                                                                                                                                   5pi/16],
:B => [1, 3pi/8, 5pi/16],
:C => [1, 0, pi/2],
:D => [1, pi/2, 0],
:E => [1, -3pi/4, 0],
:F => [1, 7pi/8, 3pi/16]
)

Version2_1=Dict(
:A => [1, -3pi/8, 5pi/16],
:B => [1, -3pi/8, 11pi/16],
:C => [1, 0, pi/2],
:D => [1, 0, pi/4],
:E => [1, 0, 5pi/8],
:F => [1, -5pi/8, 7pi/16]
)

Version3_1=Dict(
:A => [1, -3pi/8, 5pi/16],
:B => [1, -3pi/8, 11pi/16],
:C => [1, 0, pi/2],
:D => [1, 0, 3pi/8],
:E => [1, 0, 3pi/4],
:F => [1, -5pi/8, 9pi/16]
)

Version4_1=Dict(
:A => [1, -3pi/8, 5pi/16],
:B => [1, -3pi/8, 11pi/16],
:C => [1, 0, pi/2],
:D => [1, 0, 5pi/8],
:E => [1, 0, pi],
:F => [1, -5pi/8, 13pi/16]
)

Version2_2=Dict(
:A => [1, -3pi/8, 5pi/16],
:B => [1, 3pi/8, 5pi/16],
:C => [1, 0, pi/2],
:D => [1, -pi/2, pi/4],
:E => [1, pi/4, pi/4],
:F => [1, -pi/8, 7pi/16]
)

Version2_3=Dict(
:A => [1, -3pi/8, 5pi/16],
:B => [1, 3pi/8, 5pi/16],
:C => [1, 0, pi/2],
:D => [1, 0, pi/4],
:E => [1, 3pi/4, pi/4],
:F => [1, 3pi/8, 7pi/16]
)

Version2_4=Dict(
:A => [1, -3pi/8, 5pi/16],
:B => [1, 3pi/8, 5pi/16],
:C => [1, 0, pi/2],
:D => [1, pi/2, pi/4],
:E => [1, -3pi/4, pi/4],
:F => [1, 7pi/8, 7pi/16]
)

Version3_2=Dict(
:A => [1, -3pi/8, 5pi/16],
:B => [1, 3pi/8, 5pi/16],
:C => [1, 0, pi/2],
:D => [1, -pi/2, 3pi/8],
:E => [1, pi/4, 3pi/8],
:F => [1, -pi/8, 9pi/16]
)

Version3_3=Dict(
:A => [1, -3pi/8, 5pi/16],
:B => [1, 3pi/8, 5pi/16],
:C => [1, 0, pi/2],
:D => [1, 0, 3pi/8],
:E => [1, 3pi/4, 3pi/8],
:F => [1, 3pi/8, 9pi/16]
)

Version3_4=Dict(
:A => [1, -3pi/8, 5pi/16],
:B => [1, 3pi/8, 5pi/16],
:C => [1, 0, pi/2],
:D => [1, pi/2, 3pi/8],
:E => [1, -3pi/4, 3pi/8],
:F => [1, 7pi/8, 9pi/16]
)

Version4_2=Dict(
:A => [1, -3pi/8, 5pi/16],
:B => [1, 3pi/8, 5pi/16],
:C => [1, 0, pi/2],
:D => [1, -pi/2, 5pi/8],
:E => [1, pi/4, 5pi/8],
:F => [1, -pi/8, 13pi/16]
)

Version4_3=Dict(
:A => [1, -3pi/8, 5pi/16],
:B => [1, 3pi/8, 5pi/16],
:C => [1, 0, pi/2],
:D => [1, 0, 5pi/8],
:E => [1, 3pi/4, 5pi/8],
:F => [1, 3pi/8, 13pi/16]
)

Version4_4=Dict(
:A => [1, -3pi/8, 5pi/16],
:B => [1, 3pi/8, 5pi/16],
:C => [1, 0, pi/2],
:D => [1, pi/2, 5pi/8],
:E => [1, -3pi/4, 5pi/8],
:F => [1, 7pi/8, 13pi/16]
)

function getTranslation(A,B,C,D,E,F)

   M=zeros(3,1)
   
   AT=Common.ConvertToCart(A)
   BT=Common.ConvertToCart(B)
   CT=Common.ConvertToCart(C)
   DT=Common.ConvertToCart(D)
   ET=Common.ConvertToCart(E)
   FT=Common.ConvertToCart(F)

   R=Common.FindTransMat(AT,BT,CT,DT,ET,FT)	
   println(R)

   yaw = atand(R[2,1]/R[1,1])
   if R[2,1]<0 && R[1,1]<0
	yaw = 180+yaw
   elseif R[2,1]>0 && R[1,1]<0
	yaw = 180-abs(yaw)
   end
   pitch = atand(-R[3,1]/sqrt(R[3,2]^2+R[3,3]^2))
   if R[3,1]>0
	pitch = -abs(pitch)
   end
   roll = atand(R[3,2]/R[3,3])

   M[1,1]=yaw
   M[2,1]=pitch
   M[3,1]=roll

   return M
end


M1_1=getTranslation(Version1_1[:A],Version1_1[:B],Version1_1[:C],Version1_1[:D],Version1_1[:E],Version1_1[:F])
println(M1_1[1], " ", M1_1[2], " ", M1_1[3])
M1_2=getTranslation(Version1_2[:A],Version1_2[:B],Version1_2[:C],Version1_2[:D],Version1_2[:E],Version1_2[:F])
println(M1_2[1], " ", M1_2[2], " ", M1_2[3])
M1_3=getTranslation(Version1_3[:A],Version1_3[:B],Version1_3[:C],Version1_3[:D],Version1_3[:E],Version1_3[:F])
println(M1_3[1], " ", M1_3[2], " ", M1_3[3])
M1_4=getTranslation(Version1_4[:A],Version1_4[:B],Version1_4[:C],Version1_4[:D],Version1_4[:E],Version1_4[:F])
println(M1_4[1], " ", M1_4[2], " ", M1_4[3])
M2_1=getTranslation(Version2_1[:A],Version2_1[:B],Version2_1[:C],Version2_1[:D],Version2_1[:E],Version2_1[:F])
println(M2_1[1], " ", M2_1[2], " ", M2_1[3])
M3_1=getTranslation(Version3_1[:A],Version3_1[:B],Version3_1[:C],Version3_1[:D],Version3_1[:E],Version3_1[:F])
println(M3_1[1], " ", M3_1[2], " ", M3_1[3])
M4_1=getTranslation(Version4_1[:A],Version4_1[:B],Version4_1[:C],Version4_1[:D],Version4_1[:E],Version4_1[:F])
println(M4_1[1], " ", M4_1[2], " ", M4_1[3])
M2_2=getTranslation(Version2_2[:A],Version2_2[:B],Version2_2[:C],Version2_2[:D],Version2_2[:E],Version2_2[:F])
println(M2_2[1], " ", M2_2[2], " ", M2_2[3])
M2_3=getTranslation(Version2_3[:A],Version2_3[:B],Version2_3[:C],Version2_3[:D],Version2_3[:E],Version2_3[:F])
println(M2_3[1], " ", M2_3[2], " ", M2_3[3])
M2_4=getTranslation(Version2_4[:A],Version2_4[:B],Version2_4[:C],Version2_4[:D],Version2_4[:E],Version2_4[:F])
println(M2_4[1], " ", M2_4[2], " ", M2_4[3])
M3_2=getTranslation(Version3_2[:A],Version3_2[:B],Version3_2[:C],Version3_2[:D],Version3_2[:E],Version3_2[:F])
println(M3_2[1], " ", M3_2[2], " ", M3_2[3])
M3_3=getTranslation(Version3_3[:A],Version3_3[:B],Version3_3[:C],Version3_3[:D],Version3_3[:E],Version3_3[:F])
println(M3_3[1], " ", M3_3[2], " ", M3_3[3])
M3_4=getTranslation(Version3_4[:A],Version3_4[:B],Version3_4[:C],Version3_4[:D],Version3_4[:E],Version3_4[:F])
println(M3_4[1], " ", M3_4[2], " ", M3_4[3])
M4_2=getTranslation(Version4_2[:A],Version4_2[:B],Version4_2[:C],Version4_2[:D],Version4_2[:E],Version4_2[:F])
println(M4_2[1], " ", M4_2[2], " ", M4_2[3])
M4_3=getTranslation(Version4_3[:A],Version4_3[:B],Version4_3[:C],Version4_3[:D],Version4_3[:E],Version4_3[:F])
println(M4_3[1], " ", M4_3[2], " ", M4_3[3])
M4_4=getTranslation(Version4_4[:A],Version4_4[:B],Version4_4[:C],Version4_4[:D],Version4_4[:E],Version4_4[:F])
println(M4_4[1], " ", M4_4[2], " ", M4_4[3])

end
