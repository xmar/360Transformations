#pragma once
#include <memory>
#include "LayoutPyramidalBased.hpp"

namespace IMT {
class LayoutPyramidal2 : public LayoutPyramidalBased
{
    public:
        /** \brief Contructor of a LayoutPyramidal2 (with black pixels)
         *
         *            h
         *   +-----+-----+-----+
         *   |        .        |
         *   |       / \       |
         *   |      / T \      |
         *   |------------------
         *   |    /|     |\    |
         *   |  /  |     |  \  |
         *   |/  L |  Ba |  R \|  h
         *   |\    |     |    /|
         *   |  \  |     |  /  |
         *   |    \|     |/    |
         *   |-----------------|
         *   |      \ Bo/      |
         *   |       \ /       |
         *   |        .        |
         *   +-----+-----+-----+
         * \param baseEdge double  size of the edge of the base in the 3D space
         * \param yaw double
         * \param pitch double
         * \param roll double
         * \param pixelBaseEdge unsigned int Number of pixel of the edge of the base
         *
         */

        /** \brief Convenient way to build a new LayoutPyramidal2 with different resolution for each faces
         *
         * \param pixelEdges std::array<unsigned int,5> The array of resolution Base,Left,Right,Top,Bottom
         * \return LayoutPyramidal2 The LayoutPyramidal2 object generated
         *
         */
	    static std::shared_ptr<LayoutPyramidal2> GenerateLayout(double baseEdge, Quaternion rotationQuaternion, bool useTile, std::shared_ptr<VectorialTrans> vectorialTrans, std::array<unsigned int,5> pixelEdges)
	    {
	        FaceResolutions fr(std::move(pixelEdges));
            return std::shared_ptr<LayoutPyramidal2>(new LayoutPyramidal2(baseEdge, rotationQuaternion, useTile,
                fr.GetRes(Faces::Left)+fr.GetRes(Faces::Base)+fr.GetRes(Faces::Right),
                fr.GetRes(Faces::Top)+fr.GetRes(Faces::Base)+fr.GetRes(Faces::Bottom), vectorialTrans,
                fr));
	    }

        LayoutPyramidal2(double baseEdge, Quaternion rotationQuaternion, bool useTile, std::shared_ptr<VectorialTrans> vectorialTrans, unsigned int pixelBaseEdge):
            LayoutPyramidalBased(baseEdge, rotationQuaternion, 3*pixelBaseEdge, 3*pixelBaseEdge, useTile, vectorialTrans, {{pixelBaseEdge,pixelBaseEdge,pixelBaseEdge,pixelBaseEdge,pixelBaseEdge}}) {std::cout << 3*pixelBaseEdge<<std::endl;};
        virtual ~LayoutPyramidal2(void) = default;

        virtual CoordI GetReferenceResolution(void) override
        {
            return CoordI(GetRes(Faces::Base)*4, GetRes(Faces::Base)*2);
        }

        static CoordI GetReferenceResolution(unsigned width, unsigned heigth, const std::array<double,5>& scales)
        {
            const double w = width/(scales[0]+scales[1]+scales[2]);
             return CoordI(4*w, 2*w);
        }

    protected:
        Faces From2dToFace(unsigned int i, unsigned int j) const;

        virtual NormalizedFaceInfo From2dToNormalizedFaceInfo(const CoordI& pixel) const override;
        virtual CoordF FromNormalizedInfoTo2d(const NormalizedFaceInfo& ni) const override;

        virtual std::shared_ptr<Picture> ReadNextPictureFromVideoImpl(void) override;
        virtual void WritePictureToVideoImpl(std::shared_ptr<Picture>) override;
        virtual std::shared_ptr<IMT::LibAv::VideoReader> InitInputVideoImpl(std::string pathToInputVideo, unsigned nbFrame) override;
        virtual std::shared_ptr<IMT::LibAv::VideoWriter> InitOutputVideoImpl(std::string pathToOutputVideo, std::string codecId, unsigned fps, unsigned gop_size, std::vector<int> bit_rateVect) override;
    private:
        LayoutPyramidal2(double baseEdge, Quaternion rotationQuaternion, bool useTile, unsigned int width, unsigned int height, std::shared_ptr<VectorialTrans> vectorialTrans, const FaceResolutions& fr):
                LayoutPyramidalBased(baseEdge, rotationQuaternion, width, height, useTile, vectorialTrans, fr) {}

        unsigned int IStartOffset(LayoutPyramidalBased::Faces f, unsigned int j) const;

        unsigned int IEndOffset(LayoutPyramidalBased::Faces f, unsigned int j) const;

        unsigned int JStartOffset(LayoutPyramidalBased::Faces f, unsigned int i) const;

        unsigned int JEndOffset(LayoutPyramidalBased::Faces f, unsigned int i) const;

        inline bool InFace(unsigned i, unsigned j, LayoutPyramidalBased::Faces f) const
        {
            return inInterval(i, IStartOffset(f, j), IEndOffset(f, j)) && inInterval(j, JStartOffset(f, i), JEndOffset(f, i));
        }
};

class LayoutConfigParserPyramid2: public LayoutConfigParserPyramidBase
{
    public:
        LayoutConfigParserPyramid2(std::string key): LayoutConfigParserPyramidBase(key) {}

        std::shared_ptr<Layout> Create(std::string layoutSection, pt::ptree& ptree) const override
        {
            Quaternion rot = m_rotationQuaternion.GetRotation(layoutSection, ptree);
            auto vectorialTrans = GetVectorialTransformation(m_vectTransOptional.GetValue(layoutSection, ptree), ptree, rot);
            auto inputWidth = m_width.GetValue(layoutSection, ptree);
            auto pyramidBaseEdge = m_pyramidBaseEdge.GetValue(layoutSection, ptree);
            auto pyramidBaseEdgeLength = m_pyramidBaseEdgeLength.GetValue(layoutSection, ptree);
            auto pyramidHeightLeft = m_pyramidHeightLeft.GetValue(layoutSection, ptree);
            auto pyramidHeightRight = m_pyramidHeightRight.GetValue(layoutSection, ptree);
            auto pyramidHeightTop = m_pyramidHeightTop.GetValue(layoutSection, ptree);
            auto pyramidHeightBottom = m_pyramidHeightBottom.GetValue(layoutSection, ptree);
            return LayoutPyramidal2::GenerateLayout(pyramidBaseEdge, rot, false, vectorialTrans, {{unsigned(pyramidBaseEdgeLength*inputWidth/4), unsigned(pyramidHeightLeft*inputWidth/4), unsigned(pyramidHeightRight*inputWidth/4), unsigned(pyramidHeightTop*inputWidth/4), unsigned(pyramidHeightBottom*inputWidth/4)}});
        }
};

}
