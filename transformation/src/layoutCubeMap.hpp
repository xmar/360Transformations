#pragma once

#include "layoutCubeMapBased.hpp"

namespace IMT {

class LayoutCubeMap: public LayoutCubeMapBased
{
    public:
        static std::shared_ptr<LayoutCubeMap> GenerateLayout(double yaw, double pitch, double roll, std::array<unsigned int,6> pixelEdges)
	    {
	        FaceResolutions fr(std::move(pixelEdges));
	        auto offsetArraysTuple = Init(fr);
            return std::shared_ptr<LayoutCubeMap>( new LayoutCubeMap(
                std::get<0>(offsetArraysTuple)[0] + std::get<0>(offsetArraysTuple)[1] + std::get<0>(offsetArraysTuple)[2],
                std::get<1>(offsetArraysTuple)[0] + std::get<1>(offsetArraysTuple)[1], yaw, pitch, roll,
                fr, offsetArraysTuple));
	    }
        LayoutCubeMap(unsigned int pixelEdge):
               LayoutCubeMapBased(3*pixelEdge, 2*pixelEdge, 0, 0, 0,
                           {{pixelEdge, pixelEdge, pixelEdge, pixelEdge, pixelEdge, pixelEdge}}),
               m_maxOffsetCols({{pixelEdge,pixelEdge,pixelEdge}}), m_maxOffsetRows({{pixelEdge,pixelEdge}})
               {}
        virtual ~LayoutCubeMap(void) = default;


        Faces From2dToFace(unsigned int i, unsigned int j) const;

        virtual NormalizedFaceInfo From2dToNormalizedFaceInfo(const CoordI& pixel) const override;
        virtual CoordF FromNormalizedInfoTo2d(const NormalizedFaceInfo& ni) const override;

    private:
        typedef std::array<unsigned int, 3> ColsOffsetArray;
        typedef std::array<unsigned int, 2> RowsOffsetArray;

        ColsOffsetArray m_maxOffsetCols;
        RowsOffsetArray m_maxOffsetRows;

        LayoutCubeMap(double yaw, double pitch, double roll, unsigned int width, unsigned int height, const FaceResolutions& fr, const std::tuple<ColsOffsetArray, RowsOffsetArray>& t):
            LayoutCubeMapBased(width, height, yaw, pitch, roll, fr), m_maxOffsetCols(std::get<0>(t)), m_maxOffsetRows(std::get<1>(t)) {}

        static std::tuple<ColsOffsetArray, RowsOffsetArray> Init(const FaceResolutions& fr)
        {
            ColsOffsetArray maxOffsetCols;
            RowsOffsetArray maxOffsetRows;
            maxOffsetCols[0] = MAX(fr.GetRes(Faces::Front), fr.GetRes(Faces::Back));
            maxOffsetCols[1] = MAX(fr.GetRes(Faces::Right), fr.GetRes(Faces::Left));
            maxOffsetCols[2] = MAX(fr.GetRes(Faces::Top), fr.GetRes(Faces::Bottom));

            maxOffsetRows[0] = MAX(fr.GetRes(Faces::Front), MAX(fr.GetRes(Faces::Right), fr.GetRes(Faces::Top)));
            maxOffsetRows[1] = MAX(fr.GetRes(Faces::Back), MAX(fr.GetRes(Faces::Left), fr.GetRes(Faces::Bottom)));
            return std::make_tuple(std::move(maxOffsetCols), std::move(maxOffsetRows));
        }

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
