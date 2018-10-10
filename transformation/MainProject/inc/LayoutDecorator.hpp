#pragma once

#include "Layout.hpp"

#include <memory>

namespace IMT
{
class LayoutDecorator: public LayoutView
{
    public:
        explicit LayoutDecorator(std::shared_ptr<LayoutView> baseLayout):
            LayoutView(), m_baseLayout(baseLayout)
        {}


        //Use the decorated 
        //Coord3dCart From2dTo3d(const CoordI& pixelCoord) const final {return FromNormalizedInfoTo3d(From2dToNormalizedFaceInfo(pixelCoord));}
        //CoordF FromSphereTo2d(const Coord3dSpherical& sphericalCoord) const final {return FromNormalizedInfoTo2d(From3dToNormalizedFaceInfo(sphericalCoord));}
        //std::shared_ptr<Picture> ToLayout(const Picture& layoutPic, const LayoutView& destLayout) const final {return m_baseLayout->ToLayout(layoutPic,destLayout);}
        //std::shared_ptr<Picture> FromLayout(const Picture& picFromOtherLayout, const LayoutView& originalLayout) const final {return originalLayout.ToLayout(picFromOtherLayout, *this);} 

        //Forward all other function to the base layout
        void Init(void) final {return m_baseLayout->Init();}
        void NextStep(double relatifTimestamp) final {NextStepImpl(relatifTimestamp); return m_baseLayout->NextStep(relatifTimestamp);}
        unsigned int GetWidth(void) const final {return m_baseLayout->GetWidth();}
        unsigned int GetHeight(void) const final {return m_baseLayout->GetHeight();}
        CoordI GetReferenceResolution(void) final {return m_baseLayout->GetReferenceResolution();}
        double GetSurfacePixel(const CoordI& pixelCoord) final {return m_baseLayout->GetSurfacePixel(pixelCoord);}
        void InitInputVideo(std::string pathToInputVideo, unsigned nbFrame) final {return m_baseLayout->InitInputVideo(pathToInputVideo,nbFrame);}
        void InitOutputVideo(std::string pathToOutputVideo, std::string codecId, unsigned fps, unsigned gop_size, std::vector<int> bit_rateVect) final {return m_baseLayout->InitOutputVideo(pathToOutputVideo, codecId, fps, gop_size, bit_rateVect);}
        std::shared_ptr<Picture> ReadNextPictureFromVideo(void)  final {return m_baseLayout->ReadNextPictureFromVideo();}
        void WritePictureToVideo(std::shared_ptr<Picture> pic) final {return m_baseLayout->WritePictureToVideo(pic);}
        void SetInterpolationTech(Picture::InterpolationTech interpol) {return m_baseLayout->SetInterpolationTech(interpol);}
    protected:
        void InitImpl(void) override { m_baseLayout->InitImpl(); }

        NormalizedFaceInfo From2dToNormalizedFaceInfo(const CoordI& pixel) const override { return m_baseLayout->From2dToNormalizedFaceInfo(pixel);}
        NormalizedFaceInfo From3dToNormalizedFaceInfo(const Coord3dSpherical& sphericalCoord) const  override {return m_baseLayout->From3dToNormalizedFaceInfo(sphericalCoord);}
        CoordF FromNormalizedInfoTo2d(const NormalizedFaceInfo& ni) const override {return m_baseLayout->FromNormalizedInfoTo2d(ni);}
        Coord3dCart FromNormalizedInfoTo3d(const NormalizedFaceInfo& ni) const override {return m_baseLayout->FromNormalizedInfoTo3d(ni);}
        virtual void NextStepImpl(double relatifTimestamp) {};
    private:
        std::shared_ptr<LayoutView> m_baseLayout;
};
}
