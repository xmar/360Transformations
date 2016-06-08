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
	    static std::shared_ptr<LayoutCubeMap2> GenerateLayout(double yaw, double pitch, double roll, std::array<unsigned int,6> pixelEdges)
	    {
	        FaceResolutions fr(std::move(pixelEdges));
	        auto maxOffsetTFB = MAX(fr.GetRes(Faces::Front), MAX(fr.GetRes(Faces::Top), fr.GetRes(Faces::Bottom)));
	        auto maxOffsetLFRB = MAX(fr.GetRes(Faces::Front), MAX(fr.GetRes(Faces::Left), MAX(fr.GetRes(Faces::Right), fr.GetRes(Faces::Back))));
            return std::shared_ptr<LayoutCubeMap2>( new LayoutCubeMap2(yaw, pitch, roll,
                fr.GetRes(Faces::Left)+maxOffsetTFB+fr.GetRes(Faces::Right)+fr.GetRes(Faces::Back),
                fr.GetRes(Faces::Top)+maxOffsetLFRB+fr.GetRes(Faces::Bottom),
                fr, maxOffsetTFB, maxOffsetLFRB));
	    }

        /** \brief Layout with the same resolution for each faces
         *
         * \param pixelEdge unsigned int resolution of each squared face
         *
         */
		LayoutCubeMap2(unsigned int pixelEdge):
		            LayoutCubeMapBased(4*pixelEdge, 3*pixelEdge, 0, 0, 0,
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
        virtual std::shared_ptr<IMT::LibAv::VideoWriter> InitOutputVideoImpl(std::string pathToOutputVideo, std::string codecId, unsigned fps, unsigned gop_size, std::vector<unsigned> bit_rateVect) override;
    private:
        unsigned int m_maxOffsetTFB;
        unsigned int m_maxOffsetLFRB;

        LayoutCubeMap2(double yaw, double pitch, double roll, unsigned int width, unsigned int height, const FaceResolutions& fr,unsigned int maxOffsetTFB, unsigned int maxOffsetLFRB):
            LayoutCubeMapBased(width, height, yaw, pitch, roll, fr), m_maxOffsetTFB(maxOffsetTFB), m_maxOffsetLFRB(maxOffsetLFRB) {}

        unsigned int IStartOffset(LayoutCubeMapBased::Faces f) const;
        unsigned int IEndOffset(LayoutCubeMapBased::Faces f) const;
        unsigned int JStartOffset(LayoutCubeMapBased::Faces f) const;
        unsigned int JEndOffset(LayoutCubeMapBased::Faces f) const;

        inline bool InFace(unsigned i, unsigned j, LayoutCubeMapBased::Faces f) const
        {
            return inInterval(i, IStartOffset(f), IEndOffset(f)) && inInterval(j, JStartOffset(f), JEndOffset(f));
        }
};

}
