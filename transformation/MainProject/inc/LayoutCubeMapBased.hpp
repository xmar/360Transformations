#pragma once
#include "Layout.hpp"
#include <stdexcept>
#include <array>

namespace IMT {

class LayoutCubeMapBased : public Layout
{
    protected:
        struct FaceResolutions;//Forward declaration
    public:
        enum class Faces: int {Front, Back, Right, Left, Top, Bottom, Black, Last, First=Front};
        LayoutCubeMapBased(unsigned int outWidth, unsigned int outHeight, Quaternion rotationQuaternion, bool useTile,
                           double vectorOffsetRation,
                           FaceResolutions fr):
            Layout(outWidth, outHeight), m_fr(std::move(fr)), m_rotQuaternion(rotationQuaternion),
            m_faceRotations(), m_useTile(useTile), m_vectorOffsetRatio(vectorOffsetRation)
            {InitFaceRotations();};
        virtual ~LayoutCubeMapBased(void) = default;

        const bool& UseTile(void) const {return m_useTile;}
    protected:
        struct FaceResolutions
        {
            public:
                FaceResolutions(void) = delete;
                FaceResolutions(unsigned int front, unsigned int back, unsigned int right,
                                unsigned int left, unsigned int top, unsigned int bottom):
                         m_faces{{front, back, right, left, top, bottom}}   {}
                FaceResolutions(std::array<unsigned int, 6> faceResVect)://front, back, left, right, top, bottom
                    m_faces(std::move(faceResVect))
                    {}
                unsigned int GetRes(const Faces& f) const
                {
                    switch(f)
                    {
                        case Faces::Front:
                            return m_faces[0];
                        case Faces::Back:
                            return m_faces[1];
                        case Faces::Top:
                            return m_faces[4];
                        case Faces::Bottom:
                            return m_faces[5];
                        case Faces::Right:
                            return m_faces[3];
                        case Faces::Left:
                            return m_faces[2];
                        case Faces::Black:
                            throw std::invalid_argument("GetRes: Black is not a valid face");
                        case Faces::Last:
                            throw std::invalid_argument("GetRes: Last is not a valid face");
                    }
                }
            private:
                std::array<unsigned int, 6> m_faces;
        };

        virtual NormalizedFaceInfo From2dToNormalizedFaceInfo(const CoordI& pixel) const = 0;

        virtual CoordF FromNormalizedInfoTo2d(const NormalizedFaceInfo& ni) const = 0;

        virtual NormalizedFaceInfo From3dToNormalizedFaceInfo(const Coord3dSpherical& sphericalCoord) const final;

        virtual Coord3dCart FromNormalizedInfoTo3d(const NormalizedFaceInfo& ni) const final;

        Plan FromFaceToPlan(Faces f) const
        {
            switch(f)
            {
                case Faces::Front:
                    return Plan(-1,0,0,1);
                case Faces::Back:
                    return Plan(1,0,0,1);
                case Faces::Top:
                    return Plan(0,0,-1,1);
                case Faces::Bottom:
                    return Plan(0,0,1,1);
                case Faces::Right:
                    return Plan(0,-1,0,1);
                case Faces::Left:
                    return Plan(0,1,0,1);
                case Faces::Black:
                    return Plan(0,0,0,0);
                case Faces::Last:
                    throw std::invalid_argument("FaceToPlan: Last is not a valid face");

            }
        }

        const Quaternion& FaceToRotQuaternion(Faces f) const;

        unsigned int GetRes(const Faces& f) const {return m_fr.GetRes(f);}

    private:
        FaceResolutions m_fr;
        Quaternion m_rotQuaternion;//GlobalRotation
        std::array<Quaternion,6> m_faceRotations;
        bool m_useTile;
        double m_vectorOffsetRatio;

        void InitFaceRotations(void);
};

}
