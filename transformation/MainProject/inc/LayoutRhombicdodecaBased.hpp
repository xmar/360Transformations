#pragma once
#include "Layout.hpp"
#include <stdexcept>

namespace IMT {

class LayoutRhombicdodecaBased: public Layout
{
    public:
        virtual ~LayoutRhombicdodecaBased(void) = default;

        enum class Faces{ Face1, Face2, Face3, Face4, Face5, Face6, Face7, Face8, Face9, Face10, Face11, Face12, Black, Last, First=Face1 };

        virtual NormalizedFaceInfo From2dToNormalizedFaceInfo(const CoordI& pixel) const = 0;
        virtual CoordF FromNormalizedInfoTo2d(const NormalizedFaceInfo& ni) const = 0;
        virtual NormalizedFaceInfo From3dToNormalizedFaceInfo(const Coord3dSpherical& sphericalCoord) const final;
        virtual Coord3dCart FromNormalizedInfoTo3d(const NormalizedFaceInfo& ni) const final;

    protected:
        struct FaceResolutions
        {
            public:
                FaceResolutions(void) = delete;
                FaceResolutions(std::array<unsigned int, 12> faceResVect):
                    m_faces(std::move(faceResVect))
                    {}
                unsigned int GetRes(const Faces& f) const
                {
                    switch(f)
                    {
                        case Faces::Face1:
                            return m_faces[0];
                        case Faces::Face2:
                            return m_faces[1];
                        case Faces::Face3:
                            return m_faces[2];
                        case Faces::Face4:
                            return m_faces[3];
                        case Faces::Face5:
                            return m_faces[4];
                        case Faces::Face6:
                            return m_faces[5];
                        case Faces::Face7:
                            return m_faces[6];
                        case Faces::Face8:
                            return m_faces[7];
                        case Faces::Face9:
                            return m_faces[8];
                        case Faces::Face10:
                            return m_faces[9];
                        case Faces::Face11:
                            return m_faces[10];
                        case Faces::Face12:
                            return m_faces[11];
                        case Faces::Black:
                            throw std::invalid_argument("GetRes: Black is not a valid face");
                        case Faces::Last:
                            throw std::invalid_argument("GetRes: Last is not a valid face");
                    }
                }
            private:
                std::array<unsigned int, 12> m_faces;
        };

        LayoutRhombicdodecaBased(double yaw, double pitch, double roll, FaceResolutions fr): Layout(), m_fr(std::move(fr)),
        m_rotMat(GetRotMatrice(yaw, pitch, roll)) {}
        Plan FaceToPlan(Faces f) const
        {
            switch(f)
            {
                case Faces::Face1:
                    return Plan(-1,0,0,1);
                case Faces::Face2:
                    return Plan(-1,-std::sqrt(2),-1,2);
                case Faces::Face3:
                    return Plan(-1,-std::sqrt(2),1,2);
                case Faces::Face4:
                    return Plan(-1,std::sqrt(2),-1,2);
                case Faces::Face5:
                    return Plan(-1,std::sqrt(2),1,2);
                case Faces::Face6:
                    return Plan(0,0,1,1);
                case Faces::Face7:
                    return Plan(1,std::sqrt(2),1,2);
                case Faces::Face8:
                    return Plan(1,-std::sqrt(2),1,2);
                case Faces::Face9:
                    return Plan(1,-std::sqrt(2),-1,2);
                case Faces::Face10:
                    return Plan(1,std::sqrt(2),-1,2);
                case Faces::Face11:
                    return Plan(0,0,-1,1);
                case Faces::Face12:
                    return Plan(1,0,0,1);
                case Faces::Black:
                    return Plan(0,0,0,0);
                case Faces::Last:
                    throw std::invalid_argument("FaceToPlan: Last is not a valid face");
            }
        }

        RotMat FaceToRotMat(Faces f) const; //Rot matrix to transform Face1 into f

        unsigned int GetRes(Faces f) const {return m_fr.GetRes(f);}
    private:
        FaceResolutions m_fr;
        RotMat m_rotMat;
};

}
