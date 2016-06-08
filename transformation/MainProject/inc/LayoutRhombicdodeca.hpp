#pragma once
#include "LayoutRhombicdodecaBased.hpp"
#include <stdexcept>
#include <algorithm>

namespace IMT {

class LayoutRhombicdodeca: public LayoutRhombicdodecaBased
{
    public:
        static std::shared_ptr<LayoutRhombicdodeca> GenerateLayout(double yaw, double pitch, double roll, std::array<unsigned int,12> pixelEdges)
	    {
	        FaceResolutions fr(std::move(pixelEdges));
            return std::shared_ptr<LayoutRhombicdodeca>( new LayoutRhombicdodeca(yaw, pitch, roll, std::move(fr)) );
	    }
        LayoutRhombicdodeca(unsigned int height): LayoutRhombicdodecaBased(0,0,0,{{height,height,height,height,
            height,height,height,height,height,height,height,height}}), m_colsMaxOffset(), m_rowsMaxOffset() {}
        virtual ~LayoutRhombicdodeca(void) = default;

        virtual CoordI GetReferenceResolution(void) override
        {
            auto maxWidthRes = *std::max_element(m_colsMaxOffset.begin(), m_colsMaxOffset.end());
            return CoordI(maxWidthRes*8, maxWidthRes*4);
        }

        static CoordI GetReferenceResolution(unsigned width, unsigned heigth, const std::array<double,12>& scales)
        {
            const double w = width/(std::max(scales[0],scales[1])+std::max(scales[2],scales[3])+std::max(scales[4],scales[5])+std::max(scales[6],scales[7])+std::max(scales[8],scales[9])+std::max(scales[10],scales[11]));
             return CoordI(8*w, 4*w);
        }
    protected:
        virtual NormalizedFaceInfo From2dToNormalizedFaceInfo(const CoordI& pixel) const override;
        virtual CoordF FromNormalizedInfoTo2d(const NormalizedFaceInfo& ni) const override;


        LayoutRhombicdodeca(double yaw, double pitch, double roll, FaceResolutions&& fr): LayoutRhombicdodecaBased(yaw, pitch, roll, fr), m_colsMaxOffset(), m_rowsMaxOffset() {}

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
        virtual std::shared_ptr<Picture> ReadNextPictureFromVideoImpl(void) override;
        virtual void WritePictureToVideoImpl(std::shared_ptr<Picture>) override;
        virtual std::shared_ptr<IMT::LibAv::VideoReader> InitInputVideoImpl(std::string pathToInputVideo, unsigned nbFrame) override;
        virtual std::shared_ptr<IMT::LibAv::VideoWriter> InitOutputVideoImpl(std::string pathToOutputVideo, std::string codecId, unsigned fps, unsigned gop_size, std::vector<unsigned> bit_rateVect) override;
    private:
        typedef std::array<unsigned int, 6> ColsOffsetArray;
        typedef std::array<unsigned int, 2> RowsOffsetArray;

        ColsOffsetArray m_colsMaxOffset;
        RowsOffsetArray m_rowsMaxOffset;
};

}
