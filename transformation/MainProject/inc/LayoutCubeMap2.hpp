#pragma once

#include "LayoutCubeMapBased.hpp"
#include <memory>

namespace IMT {

class LayoutCubeMap2: public LayoutCubeMapBased
{
	public:
        /** \brief Convenient way to build a new LayoutCubeMap2 with different resolution for each faces
         *
         * \param pixelEdges std::array<unsigned int,6> The array of resolution Front,Back,Left,Right,Top,Bottom
         * \return LayoutCubeMap2 The LayoutCubeMap2 object generated
         *
         */
	    static std::shared_ptr<LayoutCubeMap2> GenerateLayout(bool useTile, std::array<std::array<unsigned int, 2>,6> pixelEdges, bool useEqualArea)
	    {
	        FaceResolutions fr(std::move(pixelEdges));
	        auto maxOffsetTFB = MAX(fr.GetResV(Faces::Front), MAX(fr.GetResV(Faces::Top), fr.GetResV(Faces::Bottom)));
	        auto maxOffsetLFRB = MAX(fr.GetResH(Faces::Front), MAX(fr.GetResH(Faces::Left), MAX(fr.GetResH(Faces::Right), fr.GetResH(Faces::Back))));
            return std::shared_ptr<LayoutCubeMap2>( new LayoutCubeMap2(useTile, 
                fr.GetResH(Faces::Left)+maxOffsetTFB+fr.GetResH(Faces::Right)+fr.GetResH(Faces::Back),
                fr.GetResV(Faces::Top)+maxOffsetLFRB+fr.GetResV(Faces::Bottom),
                fr, maxOffsetTFB, maxOffsetLFRB, useEqualArea));
	    }

        /** \brief Layout with the same resolution for each faces
         *
         * \param pixelEdge unsigned int resolution of each squared face
         *
         */
		LayoutCubeMap2(unsigned int pixelEdge, bool useTile):
		            LayoutCubeMapBased(4*pixelEdge, 3*pixelEdge, useTile,
                                 {{pixelEdge, pixelEdge, pixelEdge, pixelEdge, pixelEdge, pixelEdge}})
                    , m_maxOffsetTFB(pixelEdge), m_maxOffsetLFRB(pixelEdge) {}

        virtual ~LayoutCubeMap2(void) = default;

        virtual CoordI GetReferenceResolution(void) override
        {
            return CoordI(4*m_maxOffsetLFRB, 2*m_maxOffsetLFRB);
        }

        static CoordI GetReferenceResolution(unsigned width, unsigned heigth, const std::array<double,6>& scales)
        {
            const double w = width/(scales[0]+scales[2]+scales[3]+scales[1]);
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
        unsigned int m_maxOffsetTFB;
        unsigned int m_maxOffsetLFRB;

        LayoutCubeMap2(double useTile, unsigned int width, unsigned int height, const FaceResolutions& fr,unsigned int maxOffsetTFB, unsigned int maxOffsetLFRB, bool useEqualArea):
            LayoutCubeMapBased(width, height, useTile, fr, useEqualArea), m_maxOffsetTFB(maxOffsetTFB), m_maxOffsetLFRB(maxOffsetLFRB) {}

        unsigned int IStartOffset(LayoutCubeMapBased::Faces f) const;
        unsigned int IEndOffset(LayoutCubeMapBased::Faces f) const;
        unsigned int JStartOffset(LayoutCubeMapBased::Faces f) const;
        unsigned int JEndOffset(LayoutCubeMapBased::Faces f) const;

        inline bool InFace(unsigned i, unsigned j, LayoutCubeMapBased::Faces f) const
        {
            return inInterval(i, IStartOffset(f), IEndOffset(f)) && inInterval(j, JStartOffset(f), JEndOffset(f));
        }
};

class  LayoutConfigParserCubemap2: public LayoutConfigParserCubemapBase
{
    public:
        LayoutConfigParserCubemap2(std::string key, bool useEqualArea): LayoutConfigParserCubemapBase(key, useEqualArea)
        {}

    protected:
        std::shared_ptr<LayoutView> CreateImpl(std::string layoutSection, pt::ptree& ptree) const override
        {
            auto inputWidth = m_width.GetValue(layoutSection, ptree);
            auto inputHeight = m_height.GetValue(layoutSection, ptree);
            auto edgeFront = m_edgeFront.GetValue(layoutSection, ptree);
            auto edgeRight = m_edgeRight.GetValue(layoutSection, ptree);
            auto edgeLeft = m_edgeLeft.GetValue(layoutSection, ptree);
            auto edgeBack = m_edgeBack.GetValue(layoutSection, ptree);
            auto edgeTop = m_edgeTop.GetValue(layoutSection, ptree);
            auto edgeBottom = m_edgeBottom.GetValue(layoutSection, ptree);
            return LayoutCubeMap2::GenerateLayout(false, {{std::array<unsigned int,2>{unsigned(edgeFront*inputWidth/3), unsigned(edgeFront*inputHeight/2)}, std::array<unsigned int,2>{unsigned(edgeBack*inputWidth/3), unsigned(edgeBack*inputHeight/2)}, std::array<unsigned int,2>{unsigned(edgeLeft*inputWidth/3), unsigned(edgeLeft*inputHeight/2)}, std::array<unsigned int,2>{unsigned(edgeRight*inputWidth/3), unsigned(edgeRight*inputHeight/2)}, std::array<unsigned int,2>{unsigned(edgeTop*inputWidth/3),unsigned(edgeTop*inputHeight/2)}, std::array<unsigned int,2>{unsigned(edgeBottom*inputWidth/3),unsigned(edgeBottom*inputHeight/2)}}}, m_useEqualArea);
        }
    private:
};


}
