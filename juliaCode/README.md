===== How To Used =====

This folder contains modules for julia: http://julialang.org/

A module Common is described in the common.jl file and a modul Rhomb
in the file rhombicdodeca.jl

The module Rhomb define a rhombicdodeca center on O with its all 12
faces tangent to the unit shpere. The Face1 is centered with the X axis.

To load the module use the command ``include("rhombicdodeca.jl")`` inside
julia command line.

Then, you can call the function ``GetAllFaceEquationAndRotation(:Face1)``
to get the equation of all faces of the rhombicdodeca + the matrice of
rotation to get each face from the :Face1. (:Face1 can be replace by the
name of any of the 12 faces).
