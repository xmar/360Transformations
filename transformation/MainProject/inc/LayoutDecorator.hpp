#pragma once

#include "Layout.hpp"

#include <memory>

namespace IMT
{
class LayoutDecorator: public Layout
{
    public:
        explicit LayoutDecorator(std::shared_ptr<Layout> baseLayout):
            Layout(), m_baseLayout(baseLayout)
        {}
    protected:
        void InitImpl(void) override { m_baseLayout->InitImpl(); }
        std::shared_ptr<Picture> ReadNextPictureFromVideoImpl(void) override {return m_baseLayout->ReadNextPictureFromVideoImpl();}
        void WritePictureToVideoImpl(std::shared_ptr<Picture> pic) override { m_baseLayout->WritePictureToVideoImpl(pic);}
        std::shared_ptr<IMT::LibAv::VideoReader> InitInputVideoImpl(std::string pathToInputVideo, unsigned nbFrame) override {return m_baseLayout->InitInputVideoImpl(pathToInputVideo, nbFrame);}
        std::shared_ptr<IMT::LibAv::VideoWriter> InitOutputVideoImpl(std::string pathToOutputVideo, std::string codecId, unsigned fps, unsigned gop_size, std::vector<int> bit_rateVect) override {return m_baseLayout->InitOutputVideoImpl(pathToOutputVideo, codecId, fps, gop_size, bit_rateVect);}


        NormalizedFaceInfo From2dToNormalizedFaceInfo(const CoordI& pixel) const override { return m_baseLayout->From2dToNormalizedFaceInfo(pixel);}
        NormalizedFaceInfo From3dToNormalizedFaceInfo(const Coord3dSpherical& sphericalCoord) const  override {return m_baseLayout->From3dToNormalizedFaceInfo(sphericalCoord);}
        CoordF FromNormalizedInfoTo2d(const NormalizedFaceInfo& ni) const override {return m_baseLayout->FromNormalizedInfoTo2d(ni);}
        Coord3dCart FromNormalizedInfoTo3d(const NormalizedFaceInfo& ni) const override {return m_baseLayout->FromNormalizedInfoTo3d(ni);}
    private:
        std::shared_ptr<Layout> m_baseLayout;
};
}
