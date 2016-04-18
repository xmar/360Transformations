#pragma once
#include "layoutRhombicdodecaBased.hpp"
#include <stdexcept>

namespace IMT {

class LayoutRhombicdodeca: public LayoutRhombicdodecaBased
{
    public:
        static std::shared_ptr<LayoutRhombicdodeca> GenerateLayout(std::array<unsigned int,12> pixelEdges)
	    {
	        FaceResolutions fr(std::move(pixelEdges));
            return std::shared_ptr<LayoutRhombicdodeca>( new LayoutRhombicdodeca(std::move(fr)) );
	    }
        LayoutRhombicdodeca(unsigned int height): LayoutRhombicdodecaBased({{height,height,height,height,
            height,height,height,height,height,height,height,height}}), m_colsMaxOffset(), m_rowsMaxOffset() {}
        virtual ~LayoutRhombicdodeca(void) = default;

        virtual NormalizedFaceInfo From2dToNormalizedFaceInfo(const CoordI& pixel) const override;
        virtual CoordF FromNormalizedInfoTo2d(const NormalizedFaceInfo& ni) const override;

    protected:
        LayoutRhombicdodeca(FaceResolutions&& fr): LayoutRhombicdodecaBased(fr), m_colsMaxOffset(), m_rowsMaxOffset() {}

        Faces LayoutToFace(unsigned int i, unsigned int j) const;

        CoordF CanonicLayoutCoordToLayoutCoord(double i, double j, Faces f) const;

        virtual void InitImpl(void) override
        {
            m_colsMaxOffset[0] = MAX(GetRes(Faces::Face1), GetRes(Faces::Face2));
            m_colsMaxOffset[1] = MAX(GetRes(Faces::Face3), GetRes(Faces::Face4));
            m_colsMaxOffset[2] = MAX(GetRes(Faces::Face5), GetRes(Faces::Face6));
            m_colsMaxOffset[3] = MAX(GetRes(Faces::Face7), GetRes(Faces::Face8));
            m_colsMaxOffset[4] = MAX(GetRes(Faces::Face9), GetRes(Faces::Face10));
            m_colsMaxOffset[5] = MAX(GetRes(Faces::Face11), GetRes(Faces::Face12));

            m_rowsMaxOffset[0] = MAX(GetRes(Faces::Face1), MAX(GetRes(Faces::Face3), MAX(GetRes(Faces::Face5),
                                   MAX(GetRes(Faces::Face7), MAX(GetRes(Faces::Face9), GetRes(Faces::Face11))))));
            m_rowsMaxOffset[1] = MAX(GetRes(Faces::Face2), MAX(GetRes(Faces::Face4), MAX(GetRes(Faces::Face6),
                                   MAX(GetRes(Faces::Face8), MAX(GetRes(Faces::Face10), GetRes(Faces::Face12))))));

            unsigned int totalWidth(0), totalHeight(0);
            for (auto& w: m_colsMaxOffset) {totalWidth += w;}
            for (auto& h: m_rowsMaxOffset) {totalHeight += h;}
            SetWidth(totalWidth);
            SetHeight(totalHeight);
        }

        unsigned int IStartOffset(Faces f) const;
        unsigned int IEndOffset(Faces f) const;
        unsigned int JStartOffset(Faces f) const;
        unsigned int JEndOffset(Faces f) const;

        inline bool InFace(unsigned i, unsigned j, Faces f) const
        {
            return inInterval(i, IStartOffset(f), IEndOffset(f)) && inInterval(j, JStartOffset(f), JEndOffset(f));
        }
    private:
        typedef std::array<unsigned int, 6> ColsOffsetArray;
        typedef std::array<unsigned int, 2> RowsOffsetArray;

        ColsOffsetArray m_colsMaxOffset;
        RowsOffsetArray m_rowsMaxOffset;
};

}
