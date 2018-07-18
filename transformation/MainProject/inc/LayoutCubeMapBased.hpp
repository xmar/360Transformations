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
                           std::shared_ptr<VectorialTrans> vectorialTrans,
                           FaceResolutions fr, bool useEqualArea = false):
            Layout(outWidth, outHeight, vectorialTrans), m_fr(std::move(fr)), m_rotQuaternion(rotationQuaternion),
            m_faceRotations(), m_useTile(useTile), m_equalArea(useEqualArea)
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
                         m_faces{{std::array<unsigned int,2>{front, front}, std::array<unsigned int,2>{back,back}, std::array<unsigned int,2>{right, right}, std::array<unsigned int,2>{left,left}, std::array<unsigned int,2>{top,top}, std::array<unsigned int,2>{bottom, bottom}}}   {}
                FaceResolutions(std::array<std::array<unsigned int,2>, 6> faceResVect)://front, back, left, right, top, bottom
                    m_faces(std::move(faceResVect))
                    {}
                unsigned int GetResH(const Faces& f) const
                {
                    switch(f)
                    {
                        case Faces::Front:
                            return m_faces[0][0];
                        case Faces::Back:
                            return m_faces[1][0];
                        case Faces::Top:
                            return m_faces[4][0];
                        case Faces::Bottom:
                            return m_faces[5][0];
                        case Faces::Right:
                            return m_faces[3][0];
                        case Faces::Left:
                            return m_faces[2][0];
                        case Faces::Black:
                            throw std::invalid_argument("GetRes: Black is not a valid face");
                        case Faces::Last:
                            throw std::invalid_argument("GetRes: Last is not a valid face");
                    }
                }
                unsigned int GetResV(const Faces& f) const
                {
                    switch(f)
                    {
                        case Faces::Front:
                            return m_faces[0][1];
                        case Faces::Back:
                            return m_faces[1][1];
                        case Faces::Top:
                            return m_faces[4][1];
                        case Faces::Bottom:
                            return m_faces[5][1];
                        case Faces::Right:
                            return m_faces[3][1];
                        case Faces::Left:
                            return m_faces[2][1];
                        case Faces::Black:
                            throw std::invalid_argument("GetRes: Black is not a valid face");
                        case Faces::Last:
                            throw std::invalid_argument("GetRes: Last is not a valid face");
                    }
                }
            private:
                std::array<std::array<unsigned int,2>, 6> m_faces;
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

        unsigned int GetResH(const Faces& f) const {return m_fr.GetResH(f);}
        unsigned int GetResV(const Faces& f) const {return m_fr.GetResV(f);}

    private:
        FaceResolutions m_fr;
        Quaternion m_rotQuaternion;//GlobalRotation
        std::array<Quaternion,6> m_faceRotations;
        bool m_useTile;
        bool m_equalArea;

        void InitFaceRotations(void);
};

class LayoutConfigParserCubemapBase: public LayoutConfigParser
{                                                                                
    public:                                                                      
        LayoutConfigParserCubemapBase(std::string key, bool useEqualArea): LayoutConfigParser(key),
            m_useEqualArea(useEqualArea),
            //m_useTile(this, "useTile", "Set to true to use track tiles [default = false]", true, false),
            m_edgeFront(this, "cubeEdgeLengthFront", "Relative size of the Front size (between 0 and 1) [default = 1]", true, 1),
            m_edgeRight(this, "cubeEdgeLengthRight", "Relative size of the Right size (between 0 and 1) [default = 1]", true, 1),
            m_edgeLeft(this, "cubeEdgeLengthLeft", "Relative size of the Left size (between 0 and 1) [default = 1]", true, 1),
            m_edgeBack(this, "cubeEdgeLengthBack", "Relative size of the Back size (between 0 and 1) [default = 1]", true, 1),
            m_edgeTop(this, "cubeEdgeLengthTop", "Relative size of the Top size (between 0 and 1) [default = 1]", true, 1),
            m_edgeBottom(this, "cubeEdgeLengthBottom", "Relative size of the Bottom size (between 0 and 1) [default = 1]", true, 1)
    {}                                                                           
                                                                                 
    protected:                                                                   
        bool m_useEqualArea;
        //KeyTypeDescription<bool> m_useTile;
        KeyTypeDescription<SCALAR> m_edgeFront;
        KeyTypeDescription<SCALAR> m_edgeRight;
        KeyTypeDescription<SCALAR> m_edgeLeft;
        KeyTypeDescription<SCALAR> m_edgeBack;
        KeyTypeDescription<SCALAR> m_edgeTop;
        KeyTypeDescription<SCALAR> m_edgeBottom;
};

}
