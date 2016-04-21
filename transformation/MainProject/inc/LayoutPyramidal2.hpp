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
	    static std::shared_ptr<LayoutPyramidal2> GenerateLayout(double baseEdge, double yaw, double pitch, double roll, std::array<unsigned int,5> pixelEdges)
	    {
	        FaceResolutions fr(std::move(pixelEdges));
            return std::shared_ptr<LayoutPyramidal2>(new LayoutPyramidal2(baseEdge,yaw, pitch, roll,
                fr.GetRes(Faces::Left)+fr.GetRes(Faces::Base)+fr.GetRes(Faces::Right),
                fr.GetRes(Faces::Top)+fr.GetRes(Faces::Base)+fr.GetRes(Faces::Bottom),
                fr));
	    }

        LayoutPyramidal2(double baseEdge, double yaw, double pitch, double roll, unsigned int pixelBaseEdge):
            LayoutPyramidalBased(baseEdge, yaw, pitch, roll, 3*pixelBaseEdge, 3*pixelBaseEdge,{{pixelBaseEdge,pixelBaseEdge,pixelBaseEdge,pixelBaseEdge,pixelBaseEdge}}) {};
        virtual ~LayoutPyramidal2(void) = default;

        Faces From2dToFace(unsigned int i, unsigned int j) const;

        virtual NormalizedFaceInfo From2dToNormalizedFaceInfo(const CoordI& pixel) const override;
        virtual CoordF FromNormalizedInfoTo2d(const NormalizedFaceInfo& ni) const override;


    protected:

    private:
        LayoutPyramidal2(double baseEdge, double yaw, double pitch, double roll, unsigned int width, unsigned int height, const FaceResolutions& fr):
                LayoutPyramidalBased(baseEdge, yaw, pitch, roll, width, height, fr) {}

        unsigned int IStartOffset(LayoutPyramidalBased::Faces f, unsigned int j) const;

        unsigned int IEndOffset(LayoutPyramidalBased::Faces f, unsigned int j) const;

        unsigned int JStartOffset(LayoutPyramidalBased::Faces f, unsigned int i) const;

        unsigned int JEndOffset(LayoutPyramidalBased::Faces f, unsigned int i) const;

        inline bool InFace(unsigned i, unsigned j, LayoutPyramidalBased::Faces f) const
        {
            return inInterval(i, IStartOffset(f, j), IEndOffset(f, j)) && inInterval(j, JStartOffset(f, i), JEndOffset(f, i));
        }
};
}
