#pragma once

#include "LayoutPyramidalBased.hpp"

namespace IMT
{

class LayoutPyramidal: public LayoutPyramidalBased
{
    public:
        /** \brief Constructor of a LayoutPyramidalBased (compact layout)
         *
         *          3*h
         *   +-----+-----+-----+
         *   |    /|     |\    |
         *   |  /  |     |  \  |
         *   |/  L |  Ba |  R \|  h
         *   |\    |     |    /|
         *   |  \  |     |  /  |
         *   | T  \|     |/  Bo|
         *   +-----+-----+-----+
         * \param baseEdge double
         * \param yaw double
         * \param pitch double
         * \param roll double
         * \param pixelBaseEdge unsigned int Number of pixel of the edge of the base
         *
         */
        LayoutPyramidal(double baseEdge,double yaw, double pitch, double roll, unsigned int pixelBaseEdge):
        LayoutPyramidalBased(baseEdge, yaw, pitch, roll, 3*pixelBaseEdge, pixelBaseEdge,{{pixelBaseEdge, pixelBaseEdge, pixelBaseEdge, pixelBaseEdge, pixelBaseEdge}}) {}
        virtual ~LayoutPyramidal(void) = default;

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
        virtual NormalizedFaceInfo From2dToNormalizedFaceInfo(const CoordI& pixel) const override;
        virtual CoordF FromNormalizedInfoTo2d(const NormalizedFaceInfo& ni) const override;


        //Face AngleToFace(double theta, double phi) const { AngleToFace(Coord3dSpherical(1,theta,phi));}
        //Face AngleToFace(const Coord3dSpherical& p) const {return AngleToFaceCart(SphericalToCart(p));}
        //Face AngleToFaceCart(const Coord3dCart& p) const;
        virtual std::shared_ptr<Picture> ReadNextPictureFromVideoImpl(void) override {std::cout << "Not implemented; Use pyramidal2 instead" << std::endl;}
        virtual void WritePictureToVideoImpl(std::shared_ptr<Picture>) override {std::cout << "Not implemented; Use pyramidal2 instead" << std::endl;}
        virtual std::shared_ptr<IMT::LibAv::VideoReader> InitInputVideoImpl(std::string pathToInputVideo, unsigned nbFrame) override {std::cout << "Not implemented; Use pyramidal2 instead" << std::endl;}
        virtual std::shared_ptr<IMT::LibAv::VideoWriter> InitOutputVideoImpl(std::string pathToOutputVideo, std::string codecId, unsigned fps, unsigned gop_size, std::vector<unsigned> bit_rateVect) override {std::cout << "Not implemented; Use pyramidal2 instead" << std::endl;}
    private:

};

}
