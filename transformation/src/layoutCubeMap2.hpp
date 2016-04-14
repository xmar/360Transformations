#pragma once

#include "layoutCubeMapBased.hpp"
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
	    static std::shared_ptr<LayoutCubeMap2> GenerateLayout(std::array<unsigned int,6> pixelEdges)
	    {
	        FaceResolutions fr(std::move(pixelEdges));
            return std::make_shared<LayoutCubeMap2>(
                fr.GetRes(Faces::Left)+fr.GetRes(Faces::Front)+fr.GetRes(Faces::Right)+fr.GetRes(Faces::Back),
                fr.GetRes(Faces::Top)+fr.GetRes(Faces::Front)+fr.GetRes(Faces::Bottom),
                fr);
	    }

        /** \brief Layout with the same resolution for each faces
         *
         * \param pixelEdge unsigned int resolution of each squared face
         *
         */
		LayoutCubeMap2(unsigned int pixelEdge):
		            LayoutCubeMapBased(4*pixelEdge, 3*pixelEdge,
                                 {{pixelEdge, pixelEdge, pixelEdge, pixelEdge, pixelEdge, pixelEdge}}) {}
        LayoutCubeMap2(unsigned int width, unsigned int height, const FaceResolutions& fr):
                LayoutCubeMapBased(width, height, fr) {}

        virtual ~LayoutCubeMap2(void) = default;


		Faces From2dToFace(unsigned int i, unsigned int j) const;

        virtual NormalizedFaceInfo From2dToNormalizedFaceInfo(const CoordI& pixel) const override;
        virtual CoordF FromNormalizedInfoTo2d(const NormalizedFaceInfo& ni) const override;

        private:
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
