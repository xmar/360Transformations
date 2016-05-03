#pragma once
#include <string>
#include <vector>
#include <memory>

#include <boost/property_tree/ptree.hpp>

#include "Layout.hpp"
#include "LayoutCubeMap.hpp"
#include "LayoutCubeMap2.hpp"
#include "LayoutEquirectangular.hpp"
#include "LayoutEquirectangularTiles.hpp"
#include "LayoutFlatFixed.hpp"
#include "LayoutPyramidal.hpp"
#include "LayoutPyramidal2.hpp"
#include "LayoutRhombicdodeca.hpp"


namespace IMT {


namespace pt = boost::property_tree;
std::shared_ptr<Layout> InitialiseLayout(std::string layoutSection, pt::ptree& ptree, bool isInput, unsigned int inputWidth, unsigned int inputHeight)
{
    std::string layoutType;

    try {
        layoutType = ptree.get<std::string>(layoutSection+".type");
        bool infer = isInput ? true : ptree.get<bool>(layoutSection+".relativeResolution");
        if (layoutType == "equirectangular")
        {
            double yaw = ptree.get<double>(layoutSection+".yaw")*PI()/180;
            double pitch = ptree.get<double>(layoutSection+".pitch")*PI()/180;
            double roll = ptree.get<double>(layoutSection+".roll")*PI()/180;
            if (isInput)
            {
                return std::shared_ptr<Layout>(std::make_shared<LayoutEquirectangular>(inputWidth, inputHeight, yaw, pitch, roll));
            }
            else
            {
                double relativeWidth = ptree.get<double>(layoutSection+".width");
                double relativeHeight = ptree.get<double>(layoutSection+".height");
                if (infer)
                {
                    return std::shared_ptr<Layout>(std::make_shared<LayoutEquirectangular>(relativeWidth*inputWidth, relativeHeight*inputHeight, yaw, pitch, roll));
                }
                else
                {
                    return std::shared_ptr<Layout>(std::make_shared<LayoutEquirectangular>(relativeWidth, relativeHeight, yaw, pitch, roll));
                }
            }

        }
        CoordI refRes(0,0);
        if (isInput)
        {
            try
            {
                int width = ptree.get<double>(layoutSection+".refWidth");
                int height = ptree.get<double>(layoutSection+".refHeight");
                refRes = CoordI(width, height);
            }
            catch(...)
            {
                std::cout << "Could not find " << layoutSection+".refWidth" << " or " << layoutSection+".refHeight" <<
                    "for layoutSection=" << layoutSection << std::endl;
            }
        }
        if (layoutType == "cubeMap")
        {
            double edgeFront = ptree.get<double>(layoutSection+".cubeEdgeLengthFront");
            double edgeBack = ptree.get<double>(layoutSection+".cubeEdgeLengthBack");
            double edgeLeft = ptree.get<double>(layoutSection+".cubeEdgeLengthLeft");
            double edgeRight = ptree.get<double>(layoutSection+".cubeEdgeLengthRight");
            double edgeTop = ptree.get<double>(layoutSection+".cubeEdgeLengthTop");
            double edgeBottom = ptree.get<double>(layoutSection+".cubeEdgeLengthBottom");
            double yaw = ptree.get<double>(layoutSection+".yaw")*PI()/180;
            double pitch = ptree.get<double>(layoutSection+".pitch")*PI()/180;
            double roll = ptree.get<double>(layoutSection+".roll")*PI()/180;
            if (isInput)
            {
                if (!infer)
                {
                    throw std::invalid_argument("Input with static resolution not supported yet");
                }
                if (refRes == CoordI(0,0))
                {
                    refRes = LayoutCubeMap::GetReferenceResolution(inputWidth, inputHeight, {{edgeFront, edgeBack, edgeLeft, edgeRight, edgeTop, edgeBottom}});
                }
                inputWidth = refRes.x;
                inputHeight = refRes.y;
            }
            if (infer)
            {
                return LayoutCubeMap::GenerateLayout(yaw, pitch, roll, {{unsigned(edgeFront*inputWidth/4), unsigned(edgeBack*inputWidth/4), unsigned(edgeLeft*inputWidth/4), unsigned(edgeRight*inputWidth/4), unsigned(edgeTop*inputWidth/4), unsigned(edgeBottom*inputWidth/4)}});
            }
            else
            {
                return LayoutCubeMap::GenerateLayout(yaw, pitch, roll, {{unsigned(edgeFront), unsigned(edgeBack), unsigned(edgeLeft), unsigned(edgeRight), unsigned(edgeTop), unsigned(edgeBottom)}});
            }
        }
        if (layoutType == "cubeMap2")
        {
            double edgeFront = ptree.get<double>(layoutSection+".cubeEdgeLengthFront");
            double edgeBack = ptree.get<double>(layoutSection+".cubeEdgeLengthBack");
            double edgeLeft = ptree.get<double>(layoutSection+".cubeEdgeLengthLeft");
            double edgeRight = ptree.get<double>(layoutSection+".cubeEdgeLengthRight");
            double edgeTop = ptree.get<double>(layoutSection+".cubeEdgeLengthTop");
            double edgeBottom = ptree.get<double>(layoutSection+".cubeEdgeLengthBottom");
            double yaw = ptree.get<double>(layoutSection+".yaw")*PI()/180;
            double pitch = ptree.get<double>(layoutSection+".pitch")*PI()/180;
            double roll = ptree.get<double>(layoutSection+".roll")*PI()/180;
            if (isInput)
            {
                if (!infer)
                {
                    throw std::invalid_argument("Input with static resolution not supported yet");
                }
                if (refRes == CoordI(0,0))
                {
                    refRes = LayoutCubeMap2::GetReferenceResolution(inputWidth, inputHeight, {{edgeFront, edgeBack, edgeLeft, edgeRight, edgeTop, edgeBottom}});
                }
                inputWidth = refRes.x;
                inputHeight = refRes.y;
            }
            if (infer)
            {
                return LayoutCubeMap2::GenerateLayout(yaw, pitch, roll, {{unsigned(edgeFront*inputWidth/4), unsigned(edgeBack*inputWidth/4), unsigned(edgeLeft*inputWidth/4), unsigned(edgeRight*inputWidth/4), unsigned(edgeTop*inputWidth/4), unsigned(edgeBottom*inputWidth/4)}});
            }
            else
            {
                return LayoutCubeMap2::GenerateLayout(yaw, pitch, roll, {{unsigned(edgeFront), unsigned(edgeBack), unsigned(edgeLeft), unsigned(edgeRight), unsigned(edgeTop), unsigned(edgeBottom)}});
            }
        }
        if (layoutType == "flatFixed")
        {
            double yaw = ptree.get<double>(layoutSection+".yaw")*PI()/180;
            double pitch = ptree.get<double>(layoutSection+".pitch")*PI()/180;
            double roll = ptree.get<double>(layoutSection+".roll")*PI()/180;
            double width = ptree.get<double>(layoutSection+".width");
            double height = ptree.get<double>(layoutSection+".height");
            double horizontalAngleVision = ptree.get<double>(layoutSection+".horizontalAngleOfVision")*PI()/180;
            if (isInput)
            {
                throw std::invalid_argument("FlatFixed  layout cannot be the input of the transformation flow");
            }
            if (infer)
            {
                return std::make_shared<LayoutFlatFixed>(yaw, pitch, roll, width*inputWidth, height*inputHeight, horizontalAngleVision);
            }
            else
            {
                return std::make_shared<LayoutFlatFixed>(yaw, pitch, roll, width, height, horizontalAngleVision);
            }
        }
        if (layoutType == "pyramid")
        {
            double yaw = ptree.get<double>(layoutSection+".yaw")*PI()/180;
            double pitch = ptree.get<double>(layoutSection+".pitch")*PI()/180;
            double roll = ptree.get<double>(layoutSection+".roll")*PI()/180;
            double pyramidBaseEdge = ptree.get<double>(layoutSection+".pyramidBaseEdge");
            double pyramidBaseEdgeLength = ptree.get<double>(layoutSection+".pyramidBaseEdgeLength");
            double pyramidHeightTop = ptree.get<double>(layoutSection+".pyramidHeightTop");
            double pyramidHeightBottom = ptree.get<double>(layoutSection+".pyramidHeightBottom");
            double pyramidHeightLeft = ptree.get<double>(layoutSection+".pyramidHeightBottom");
            double pyramidHeightRight = ptree.get<double>(layoutSection+".pyramidHeightBottom");
            if (isInput)
            {
                if (!infer)
                {
                    throw std::invalid_argument("Input with static resolution not supported yet");
                }
                if (refRes == CoordI(0,0))
                {
                    refRes = LayoutPyramidal::GetReferenceResolution(inputWidth, inputHeight, {{pyramidBaseEdgeLength, pyramidHeightLeft, pyramidHeightRight, pyramidHeightTop, pyramidHeightBottom}});
                }
                inputWidth = refRes.x;
                inputHeight = refRes.y;
            }
            if (infer)
            {
                return std::make_shared<LayoutPyramidal>(pyramidBaseEdge, yaw, pitch, roll, pyramidBaseEdgeLength*inputWidth/4);
            }
            else
            {
                return std::make_shared<LayoutPyramidal>(pyramidBaseEdge, yaw, pitch, roll, pyramidBaseEdgeLength);
            }
        }
        if (layoutType == "pyramid2")
        {
            double yaw = ptree.get<double>(layoutSection+".yaw")*PI()/180;
            double pitch = ptree.get<double>(layoutSection+".pitch")*PI()/180;
            double roll = ptree.get<double>(layoutSection+".roll")*PI()/180;
            double pyramidBaseEdge = ptree.get<double>(layoutSection+".pyramidBaseEdge");
            double pyramidBaseEdgeLength = ptree.get<double>(layoutSection+".pyramidBaseEdgeLength");
            double pyramidHeightTop = ptree.get<double>(layoutSection+".pyramidHeightTop");
            double pyramidHeightBottom = ptree.get<double>(layoutSection+".pyramidHeightBottom");
            double pyramidHeightLeft = ptree.get<double>(layoutSection+".pyramidHeightLeft");
            double pyramidHeightRight = ptree.get<double>(layoutSection+".pyramidHeightRight");
            if (isInput)
            {
                if (!infer)
                {
                    throw std::invalid_argument("Input with static resolution not supported yet");
                }
                if (refRes == CoordI(0,0))
                {
                    refRes = LayoutPyramidal2::GetReferenceResolution(inputWidth, inputHeight, {{pyramidBaseEdgeLength, pyramidHeightLeft, pyramidHeightRight, pyramidHeightTop, pyramidHeightBottom}});
                }
                inputWidth = refRes.x;
                inputHeight = refRes.y;
            }
            if (infer)
            {
                return LayoutPyramidal2::GenerateLayout(pyramidBaseEdge, yaw, pitch, roll, {{unsigned(pyramidBaseEdgeLength*inputWidth/4), unsigned(pyramidHeightLeft*inputWidth/4), unsigned(pyramidHeightRight*inputWidth/4), unsigned(pyramidHeightTop*inputWidth/4), unsigned(pyramidHeightBottom*inputWidth/4)}});
            }
            else
            {
                return LayoutPyramidal2::GenerateLayout(pyramidBaseEdge, yaw, pitch, roll, {{unsigned(pyramidBaseEdgeLength), unsigned(pyramidHeightLeft), unsigned(pyramidHeightRight), unsigned(pyramidHeightTop), unsigned(pyramidHeightBottom)}});
            }
        }
        if (layoutType == "rhombicDodeca")
        {
            double yaw = ptree.get<double>(layoutSection+".yaw")*PI()/180;
            double pitch = ptree.get<double>(layoutSection+".pitch")*PI()/180;
            double roll = ptree.get<double>(layoutSection+".roll")*PI()/180;
            std::array<double, 12> faceResScale;
            std::array<unsigned int, 12> faceRes;
            for (unsigned int i = 0; i < 12; ++i)
            {
                faceResScale[i] = ptree.get<double>(layoutSection+".rhombEdgeLengthFace"+std::to_string(i+1));
            }
            if (isInput)
            {
                if (!infer)
                {
                    throw std::invalid_argument("Input with static resolution not supported yet");
                }
                if (refRes == CoordI(0,0))
                {
                    refRes = LayoutRhombicdodeca::GetReferenceResolution(inputWidth, inputHeight, faceResScale);
                }
                inputWidth = refRes.x;
                inputHeight = refRes.y;
            }

            for (unsigned int i = 0; i < 12; ++i)
            {
                faceRes[i] = (infer ? inputWidth/8.0 : 1)*faceResScale[i];
            }

            return LayoutRhombicdodeca::GenerateLayout(yaw, pitch, roll, faceRes);
        }
        if (layoutType == "equirectangularTiled")
        {
            LayoutEquirectangularTiles::ScaleTilesMap scaleRes;

            double yaw = ptree.get<double>(layoutSection+".yaw")*PI()/180;
            double pitch = ptree.get<double>(layoutSection+".pitch")*PI()/180;
            double roll = ptree.get<double>(layoutSection+".roll")*PI()/180;

            for (unsigned int i = 0; i < 8; ++i)
            {
                for (unsigned int j = 0; j < 8; ++j)
                {
                    auto scale = ptree.get<double>(layoutSection+".equirectangularTile_"+std::to_string(i)+"_"+std::to_string(j));
                    scaleRes[i][j] = scale;
                }
            }

            if (isInput)
            {
                if (!infer)
                {
                    throw std::invalid_argument("Input with static resolution not supported yet");
                }
                if (refRes == CoordI(0,0))
                {
                    refRes = LayoutEquirectangularTiles::GetReferenceResolution(inputWidth, inputHeight, scaleRes);
                }
                inputWidth = refRes.x;
                inputHeight = refRes.y;
            }

            LayoutEquirectangularTiles::TilesMap tileRes;
            for (unsigned int i = 0; i < 8; ++i)
            {
                for (unsigned int j = 0; j < 8; ++j)
                {
                    tileRes[i][j] = std::make_tuple(unsigned(scaleRes[i][j]*inputWidth/8),unsigned(scaleRes[i][j]*inputHeight/8));
                }
            }

            return std::make_shared<LayoutEquirectangularTiles>(tileRes, yaw, pitch, roll);
        }
    }
    catch (std::exception &e)
    {
        std::cout << "Error while parsing in configuration file the "<<layoutSection<<" layout: " << e.what() << std::endl;
        throw e;
    }
    throw std::invalid_argument("Not supported type: "+layoutType);
}

}
