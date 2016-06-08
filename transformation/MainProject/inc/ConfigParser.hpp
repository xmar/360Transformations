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
std::vector<unsigned> GetBitrateVector(std::string layoutSection, pt::ptree& ptree, unsigned bitrateGoal)
{
    std::string layoutType;
    std::vector<unsigned> bitrateVect;
    try {
        layoutType = ptree.get<std::string>(layoutSection+".type");
        if (layoutType == "equirectangular")
        {
            auto bitrate = ptree.get<double>(layoutSection+".bitrate");

            bitrateVect.push_back(bitrateGoal * bitrate);
            return bitrateVect;
        }
        if (layoutType == "cubeMap" || layoutType == "cubeMap2")
        {
            auto bitrateFront = ptree.get<double>(layoutSection+".bitrateFront");
            auto bitrateBack = ptree.get<double>(layoutSection+".bitrateBack");
            auto bitrateLeft = ptree.get<double>(layoutSection+".bitrateLeft");
            auto bitrateRight = ptree.get<double>(layoutSection+".bitrateRight");
            auto bitrateTop = ptree.get<double>(layoutSection+".bitrateTop");
            auto bitrateBottom = ptree.get<double>(layoutSection+".bitrateBottom");
            auto sum = bitrateFront+bitrateBack+bitrateLeft+bitrateRight+bitrateTop+bitrateBottom;
            bitrateVect.push_back(bitrateGoal*bitrateFront/sum);
            bitrateVect.push_back(bitrateGoal*bitrateBack/sum);
            bitrateVect.push_back(bitrateGoal*bitrateLeft/sum);
            bitrateVect.push_back(bitrateGoal*bitrateRight/sum);
            bitrateVect.push_back(bitrateGoal*bitrateTop/sum);
            bitrateVect.push_back(bitrateGoal*bitrateBottom/sum);
            return bitrateVect;
        }
        if (layoutType == "flatFixed")
        {
            bitrateVect.push_back(bitrateGoal);
            return bitrateVect;
        }
        if (layoutType == "pyramid" || layoutType == "pyramid2")
        {
            double pyramidBaseBitrate = ptree.get<double>(layoutSection+".pyramidBaseBitrate");
            double pyramidTopBitrate = ptree.get<double>(layoutSection+".pyramidTopBitrate");
            double pyramidBottomBitrate = ptree.get<double>(layoutSection+".pyramidBottomBitrate");
            double pyramidLeftBitrate = ptree.get<double>(layoutSection+".pyramidLeftBitrate");
            double pyramidRightBitrate = ptree.get<double>(layoutSection+".pyramidRightBitrate");
            auto sum = pyramidBaseBitrate+pyramidTopBitrate+pyramidBottomBitrate+pyramidLeftBitrate+pyramidRightBitrate;
            bitrateVect.push_back(bitrateGoal*pyramidBaseBitrate/sum);
            bitrateVect.push_back(bitrateGoal*pyramidTopBitrate/sum);
            bitrateVect.push_back(bitrateGoal*pyramidBottomBitrate/sum);
            bitrateVect.push_back(bitrateGoal*pyramidLeftBitrate/sum);
            bitrateVect.push_back(bitrateGoal*pyramidRightBitrate/sum);
            return bitrateVect;
        }
        if (layoutType == "rhombicDodeca")
        {
            std::array<double, 12> faceBitrate;
            for (unsigned int i = 0; i < 12; ++i)
            {
                faceBitrate[i] = ptree.get<double>(layoutSection+".rhombFace"+std::to_string(i+1)+"Bitrate");
            }
            unsigned sum = 0;
            for (auto b: faceBitrate) {sum += b;}
            for (auto b: faceBitrate) { bitrateVect.push_back(bitrateGoal*b/sum); }
            return bitrateVect;
        }
        if (layoutType == "equirectangularTiled")
        {
            std::array<std::array<unsigned, 8>, 8> faceBitrate;
            unsigned sum = 0;
            for (unsigned i = 0; i < 8; ++i)
            {
                for (unsigned j = 0; j < 8; ++j)
                {
                    faceBitrate[i][j] = ptree.get<double>(layoutSection+".equirectangularTileBitrate_"+std::to_string(i)+"_"+std::to_string(j));
                    sum += faceBitrate[i][j];
                }
            }
            for (unsigned i = 0; i < 8; ++i)
            {
                for (unsigned j = 0; j < 8; ++j)
                {
                    bitrateVect.push_back(bitrateGoal*faceBitrate[i][j]/sum);
                }
            }
            return bitrateVect;
        }
    }
    catch (std::exception &e)
    {
        std::cout << "Error while parsing in configuration file the "<<layoutSection<<" layout: " << e.what() << std::endl;
        throw e;
    }
    throw std::invalid_argument("Not supported type: "+layoutType);
}

std::shared_ptr<Layout> InitialiseLayout(std::string layoutSection, pt::ptree& ptree, bool isInput, unsigned int inputWidth, unsigned int inputHeight)
{
    std::string layoutType;

    try {
        layoutType = ptree.get<std::string>(layoutSection+".type");
        bool infer = isInput ? true : ptree.get<bool>(layoutSection+".relativeResolution");
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
        if (layoutType == "equirectangular")
        {
            double yaw = ptree.get<double>(layoutSection+".yaw")*PI()/180;
            double pitch = ptree.get<double>(layoutSection+".pitch")*PI()/180;
            double roll = ptree.get<double>(layoutSection+".roll")*PI()/180;
            if (isInput)
            {
                inputWidth = refRes.x;
                inputHeight = refRes.y;
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
