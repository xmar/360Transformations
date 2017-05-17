#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <memory>

#include <boost/property_tree/ptree.hpp>
#include <boost/preprocessor.hpp>
#include <boost/property_tree/json_parser.hpp>

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

#define RANGE_NB_H_TILES ((1) (2) (3) (4) (5) (8) (16))
#define RANGE_NB_V_TILES ((1) (2) (3) (4) (5) (8) (16))
//#define RANGE_NB_H_TILES ((3))
//#define RANGE_NB_V_TILES ((3))

#define GENERATE_EQUI_TILED_LAYOUT(nbHTiles,nbVTiles) \
  LayoutEquirectangularTiles<nbHTiles,nbVTiles>::ScaleTilesMap scaleRes;\
  for (unsigned int i = 0; i < nbHTiles; ++i)\
  {\
      for (unsigned int j = 0; j < nbVTiles; ++j)\
      {\
          auto scale = ptree.get<double>(layoutSection+".equirectangularTile_"+std::to_string(i)+"_"+std::to_string(j));\
          scaleRes[i][j] = scale;\
      }\
  }\
  if (isInput)\
  {\
      if (!infer)\
      {\
          throw std::invalid_argument("Input with static resolution not supported yet");\
      }\
      if (refRes == CoordI(0,0))\
      {\
          refRes = LayoutEquirectangularTiles<nbHTiles,nbVTiles>::GetReferenceResolution(inputWidth, inputHeight, scaleRes);\
      }\
      inputWidth = refRes.x;\
      inputHeight = refRes.y;\
  }\
  /*LayoutEquirectangularTiles<nbHTiles,nbVTiles>::TilesMap tileRes;\
  for (unsigned int i = 0; i < nbHTiles; ++i)\
  {\
      for (unsigned int j = 0; j < nbVTiles; ++j)\
      {\
          tileRes[i][j] = std::make_tuple(unsigned(scaleRes[i][j]*inputWidth/nbHTiles),unsigned(scaleRes[i][j]*inputHeight/nbVTiles));\
      }\
  }*/\
  std::array<double, nbHTiles> hRatios;\
  std::array<double, nbVTiles> vRatios;\
  double sumH = 0;\
  double sumV = 0;\
  for (unsigned i = 0; i < nbHTiles; ++i)\
  {\
    hRatios[i] =  ptree.get<double>(layoutSection+".hTileRation_"+std::to_string(i));\
    sumH += hRatios[i];\
  }\
  for (unsigned j = 0; j < nbVTiles; ++j)\
  {\
    vRatios[j] =  ptree.get<double>(layoutSection+".vTileRation_"+std::to_string(j));\
    sumV += vRatios[j];\
  }\
  for(auto& hr: hRatios) {hr /= sumH;}\
  for(auto& vr: vRatios) {vr /= sumV;}\
  auto tilesRatios = std::make_tuple(std::move(hRatios),std::move(vRatios));\
  auto orignalRes = std::make_tuple(inputWidth, inputHeight);\
  return std::make_shared<LayoutEquirectangularTiles<nbHTiles,nbVTiles>>(std::move(scaleRes), std::move(tilesRatios), rotationQuaternion, orignalRes, useTile, upscale);
///END MACRO GENERATE_EQUI_TILED_LAYOUT

#define TEST_AND_GENERATE_EQUI_TILED_LAYOUT(r, p)\
  if (nbHTiles == BOOST_PP_SEQ_ELEM(0,p) && nbVTiles == BOOST_PP_SEQ_ELEM(1,p))\
  {\
    GENERATE_EQUI_TILED_LAYOUT(BOOST_PP_SEQ_ELEM(0,p),BOOST_PP_SEQ_ELEM(1,p))\
  }
///END MACRO TEST_AND_GENERATE_EQUI_TILED_LAYOUT

#define GET_BITRATE_VECT(nbHTiles, nbVTiles) \
  std::array<std::array<double, nbVTiles>, nbHTiles> faceBitrate;\
  double sum = 0;\
  for (unsigned i = 0; i < nbHTiles; ++i)\
  {\
      for (unsigned j = 0; j < nbVTiles; ++j)\
      {\
          faceBitrate[i][j] = ptree.get<double>(layoutSection+".equirectangularTileBitrate_"+std::to_string(i)+"_"+std::to_string(j));\
          sum += faceBitrate[i][j];\
      }\
  }\
  for (unsigned i = 0; i < nbHTiles; ++i)\
  {\
      for (unsigned j = 0; j < nbVTiles; ++j)\
      {\
          bitrateVect.push_back(bitrateGoal*faceBitrate[i][j]/sum);\
      }\
  }
///END MACRO GET_BITRATE_VECT

#define TEST_AND_GET_BITRATE_VECT(r,p)\
  if (nbHTiles == BOOST_PP_SEQ_ELEM(0,p) && nbVTiles == BOOST_PP_SEQ_ELEM(1,p))\
  {\
    GET_BITRATE_VECT(BOOST_PP_SEQ_ELEM(0,p),BOOST_PP_SEQ_ELEM(1,p))\
  }
///END MACRO TEST_AND_GET_BITRATE_VECT


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
            auto useTile = ptree.get<bool>(layoutSection+".useTile");
            if (useTile)
            {
              auto bitrateFront = ptree.get<double>(layoutSection+".bitrateFront");
              auto bitrateBack = ptree.get<double>(layoutSection+".bitrateBack");
              auto bitrateLeft = ptree.get<double>(layoutSection+".bitrateLeft");
              auto bitrateRight = ptree.get<double>(layoutSection+".bitrateRight");
              auto bitrateTop = ptree.get<double>(layoutSection+".bitrateTop");
              auto bitrateBottom = ptree.get<double>(layoutSection+".bitrateBottom");
              double sum = bitrateFront+bitrateBack+bitrateLeft+bitrateRight+bitrateTop+bitrateBottom;
              bitrateVect.push_back(bitrateGoal*bitrateFront/sum);
              bitrateVect.push_back(bitrateGoal*bitrateBack/sum);
              bitrateVect.push_back(bitrateGoal*bitrateLeft/sum);
              bitrateVect.push_back(bitrateGoal*bitrateRight/sum);
              bitrateVect.push_back(bitrateGoal*bitrateTop/sum);
              bitrateVect.push_back(bitrateGoal*bitrateBottom/sum);
              return bitrateVect;
            }
            else
            {
              auto bitrate = ptree.get<double>(layoutSection+".bitrate");
              bitrateVect.push_back(bitrateGoal * bitrate);
              return bitrateVect;
            }
        }
        if (layoutType == "flatFixed")
        {
            bitrateVect.push_back(bitrateGoal);
            return bitrateVect;
        }
        if (layoutType == "pyramid" || layoutType == "pyramid2")
        {
          auto useTile = ptree.get<bool>(layoutSection+".useTile");
          if (useTile)
          {
            double pyramidBaseBitrate = ptree.get<double>(layoutSection+".pyramidBaseBitrate");
            double pyramidTopBitrate = ptree.get<double>(layoutSection+".pyramidTopBitrate");
            double pyramidBottomBitrate = ptree.get<double>(layoutSection+".pyramidBottomBitrate");
            double pyramidLeftBitrate = ptree.get<double>(layoutSection+".pyramidLeftBitrate");
            double pyramidRightBitrate = ptree.get<double>(layoutSection+".pyramidRightBitrate");
            double sum = pyramidBaseBitrate+pyramidTopBitrate+pyramidBottomBitrate+pyramidLeftBitrate+pyramidRightBitrate;
            bitrateVect.push_back(bitrateGoal*pyramidBaseBitrate/sum);
            bitrateVect.push_back(bitrateGoal*pyramidTopBitrate/sum);
            bitrateVect.push_back(bitrateGoal*pyramidBottomBitrate/sum);
            bitrateVect.push_back(bitrateGoal*pyramidLeftBitrate/sum);
            bitrateVect.push_back(bitrateGoal*pyramidRightBitrate/sum);
            return bitrateVect;
          }
          else
          {
            auto bitrate = ptree.get<double>(layoutSection+".bitrate");
            bitrateVect.push_back(bitrateGoal * bitrate);
            return bitrateVect;
          }
        }
        if (layoutType == "rhombicDodeca")
        {
          auto useTile = ptree.get<bool>(layoutSection+".useTile");
          if (useTile)
          {
            std::array<double, 12> faceBitrate;
            for (unsigned int i = 0; i < 12; ++i)
            {
                faceBitrate[i] = ptree.get<double>(layoutSection+".rhombFace"+std::to_string(i+1)+"Bitrate");
            }
            double sum = 0;
            for (auto b: faceBitrate) {sum += b;}
            for (auto b: faceBitrate) { bitrateVect.push_back(bitrateGoal*b/sum); }
            return bitrateVect;
          }
          else
          {
            auto bitrate = ptree.get<double>(layoutSection+".bitrate");
            bitrateVect.push_back(bitrateGoal * bitrate);
            return bitrateVect;
          }
        }
        if (layoutType == "equirectangularTiled")
        {
          auto useTile = ptree.get<bool>(layoutSection+".useTile");
          if (useTile)
          {
            unsigned int nbHTiles = ptree.get<unsigned int>(layoutSection+".nbHTiles");
            unsigned int nbVTiles = ptree.get<unsigned int>(layoutSection+".nbVTiles");

            BOOST_PP_SEQ_FOR_EACH_PRODUCT(TEST_AND_GET_BITRATE_VECT, RANGE_NB_H_TILES RANGE_NB_V_TILES)

            return bitrateVect;
          }
          else
          {
            auto bitrate = ptree.get<double>(layoutSection+".bitrate");
            bitrateVect.push_back(bitrateGoal * bitrate);
            return bitrateVect;
          }
        }
    }
    catch (std::exception &e)
    {
        std::cout << "Error while parsing in configuration file the "<<layoutSection<<" layout: " << e.what() << std::endl;
        throw e;
    }
    throw std::invalid_argument("Not supported type: "+layoutType);
}

Quaternion ParseRotationJSON(std::string s)
{
  std::stringstream ss(s);
  pt::ptree ptree_json;

  pt::json_parser::read_json(ss, ptree_json);

  auto& v = ptree_json.get_child("type");

  if (v.data() == "euler")
  {
    auto vv = ptree_json.get_child("yaw");
    auto y = std::stod(vv.data())*PI()/180;

    vv = ptree_json.get_child("pitch");
    auto p =  std::stod(vv.data())*PI()/180;

    vv = ptree_json.get_child("roll");
    auto r = std::stod(vv.data())*PI()/180;
    return Quaternion::FromEuler(y, p, r);
  }
  else if (v.data() == "quaternion")
  {
    auto vv = ptree_json.get_child("w");
    auto w = std::stod(vv.data());

    vv = ptree_json.get_child("x");
    auto x =  std::stod(vv.data());

    vv = ptree_json.get_child("y");
    auto y = std::stod(vv.data());

    vv = ptree_json.get_child("z");
    auto z = std::stod(vv.data());
    auto q = Quaternion(w, VectorCartesian(x, y, z));
    q.Normalize();
    return q;
  }
  else if (v.data() == "angleAxis")
  {
    auto vv = ptree_json.get_child("angle");
    auto theta = std::stod(vv.data())*PI()/180;

    vv = ptree_json.get_child("x");
    auto x =  std::stod(vv.data());

    vv = ptree_json.get_child("y");
    auto y = std::stod(vv.data());

    vv = ptree_json.get_child("z");
    auto z = std::stod(vv.data());

    VectorCartesian v(x, y, z);
    auto q = Quaternion(std::cos(theta/2.0), std::cos(theta/2.0)*v/v.Norm());
    q.Normalize();
    return q;
  }

  // BOOST_FOREACH(boost::property_tree::ptree::value_type &v, ptree_json.get_child("particles.electron"))
  // {
  //     assert(v.first.empty()); // array elements have no names
  //     std::cout << v.second.data() << std::endl;
  //     // etc
  // }
  return Quaternion(0);
}

enum class LayoutStatus
{
  Input, /// indicates that the layout is an input layout and so will have an input video
  Intermediate, /// indicates that the layout is neither an input layout nor an output layout
  Output /// indicates that the layout is an output layout and so may have an output video
};

std::shared_ptr<Layout> InitialiseLayout(std::string layoutSection, pt::ptree& ptree, LayoutStatus layoutStatus, unsigned int inputWidth, unsigned int inputHeight)
{
    std::string layoutType;

    bool isInput = layoutStatus == LayoutStatus::Input;
    bool isOutput = layoutStatus == LayoutStatus::Output;
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
            catch(std::exception &e)
            {
                std::cout << "Could not find " << layoutSection+".refWidth" << " or " << layoutSection+".refHeight" <<
                    "for layoutSection=" << layoutSection << std::endl;
                throw e;
            }
        }
        if (layoutType == "equirectangular")
        {
            Quaternion rotationQuaternion = ParseRotationJSON(ptree.get<std::string>(layoutSection+".rotation"));
            double vectorOffsetRation = ptree.get<double>(layoutSection+".offsetRatio");
            if (isInput)
            {
                inputWidth = refRes.x;
                inputHeight = refRes.y;
                return std::shared_ptr<Layout>(std::make_shared<LayoutEquirectangular>(inputWidth, inputHeight, rotationQuaternion, vectorOffsetRation));
            }
            else
            {
                double relativeWidth = ptree.get<double>(layoutSection+".width");
                double relativeHeight = ptree.get<double>(layoutSection+".height");
                if (infer)
                {
                    return std::shared_ptr<Layout>(std::make_shared<LayoutEquirectangular>(relativeWidth*inputWidth, relativeHeight*inputHeight, rotationQuaternion, vectorOffsetRation));
                }
                else
                {
                    return std::shared_ptr<Layout>(std::make_shared<LayoutEquirectangular>(relativeWidth, relativeHeight, rotationQuaternion, vectorOffsetRation));
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
            double vectorOffsetRation = ptree.get<double>(layoutSection+".offsetRatio");
            Quaternion rotationQuaternion = ParseRotationJSON(ptree.get<std::string>(layoutSection+".rotation"));
            bool useTile = (isInput || isOutput) ? ptree.get<bool>(layoutSection+".useTile"): false;
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
                return LayoutCubeMap::GenerateLayout(rotationQuaternion, useTile, vectorOffsetRation, {{unsigned(edgeFront*inputWidth/4), unsigned(edgeBack*inputWidth/4), unsigned(edgeLeft*inputWidth/4), unsigned(edgeRight*inputWidth/4), unsigned(edgeTop*inputWidth/4), unsigned(edgeBottom*inputWidth/4)}});
            }
            else
            {
                return LayoutCubeMap::GenerateLayout(rotationQuaternion, useTile, vectorOffsetRation, {{unsigned(edgeFront), unsigned(edgeBack), unsigned(edgeLeft), unsigned(edgeRight), unsigned(edgeTop), unsigned(edgeBottom)}});
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
            double vectorOffsetRation = ptree.get<double>(layoutSection+".offsetRatio");
            Quaternion rotationQuaternion = ParseRotationJSON(ptree.get<std::string>(layoutSection+".rotation"));
            bool useTile = (isInput || isOutput) ? ptree.get<bool>(layoutSection+".useTile"): false;
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
                return LayoutCubeMap2::GenerateLayout(rotationQuaternion, useTile, vectorOffsetRation, {{unsigned(edgeFront*inputWidth/4), unsigned(edgeBack*inputWidth/4), unsigned(edgeLeft*inputWidth/4), unsigned(edgeRight*inputWidth/4), unsigned(edgeTop*inputWidth/4), unsigned(edgeBottom*inputWidth/4)}});
            }
            else
            {
                return LayoutCubeMap2::GenerateLayout(rotationQuaternion, useTile, vectorOffsetRation, {{unsigned(edgeFront), unsigned(edgeBack), unsigned(edgeLeft), unsigned(edgeRight), unsigned(edgeTop), unsigned(edgeBottom)}});
            }
        }
        if (layoutType == "flatFixed")
        {
            bool dynamicPositions = ptree.get<bool>(layoutSection+".dynamicPositions");
            Quaternion rotationQuaternion;
            std::string pathToPositionTrace;
            if (!dynamicPositions) {
              rotationQuaternion = ParseRotationJSON(ptree.get<std::string>(layoutSection+".rotation"));
            }
            else
            {
              std::cout << "DEBUG dynamic position set" << std::endl;
              pathToPositionTrace = ptree.get<std::string>(layoutSection+".positionTrace");
            }
            DynamicPosition dynamicPosition = dynamicPositions ? DynamicPosition(pathToPositionTrace)  :DynamicPosition(rotationQuaternion);
            double width = ptree.get<double>(layoutSection+".width");
            double height = ptree.get<double>(layoutSection+".height");
            double horizontalAngleVision = ptree.get<double>(layoutSection+".horizontalAngleOfVision")*PI()/180;
            double verticalAngleOfVision = ptree.get<double>(layoutSection+".verticalAngleOfVision")*PI()/180;
            if (isInput)
            {
                throw std::invalid_argument("FlatFixed  layout cannot be the input of the transformation flow");
            }
            if (infer)
            {
                return std::make_shared<LayoutFlatFixed>(std::move(dynamicPosition), width*inputWidth, height*inputHeight, horizontalAngleVision, verticalAngleOfVision);
            }
            else
            {
                return std::make_shared<LayoutFlatFixed>(std::move(dynamicPosition), width, height, horizontalAngleVision, verticalAngleOfVision);
            }
        }
        if (layoutType == "pyramid")
        {
            Quaternion rotationQuaternion = ParseRotationJSON(ptree.get<std::string>(layoutSection+".rotation"));
            double pyramidBaseEdge = ptree.get<double>(layoutSection+".pyramidBaseEdge");
            double pyramidBaseEdgeLength = ptree.get<double>(layoutSection+".pyramidBaseEdgeLength");
            double pyramidHeightTop = ptree.get<double>(layoutSection+".pyramidHeightTop");
            double pyramidHeightBottom = ptree.get<double>(layoutSection+".pyramidHeightBottom");
            double pyramidHeightLeft = ptree.get<double>(layoutSection+".pyramidHeightLeft");
            double pyramidHeightRight = ptree.get<double>(layoutSection+".pyramidHeightRight");
            bool useTile = (isInput || isOutput) ? ptree.get<bool>(layoutSection+".useTile"): false;
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
                return std::make_shared<LayoutPyramidal>(pyramidBaseEdge, rotationQuaternion, useTile, pyramidBaseEdgeLength*inputWidth/4);
            }
            else
            {
                return std::make_shared<LayoutPyramidal>(pyramidBaseEdge, rotationQuaternion, useTile, pyramidBaseEdgeLength);
            }
        }
        if (layoutType == "pyramid2")
        {
            Quaternion rotationQuaternion = ParseRotationJSON(ptree.get<std::string>(layoutSection+".rotation"));
            double pyramidBaseEdge = ptree.get<double>(layoutSection+".pyramidBaseEdge");
            double pyramidBaseEdgeLength = ptree.get<double>(layoutSection+".pyramidBaseEdgeLength");
            double pyramidHeightTop = ptree.get<double>(layoutSection+".pyramidHeightTop");
            double pyramidHeightBottom = ptree.get<double>(layoutSection+".pyramidHeightBottom");
            double pyramidHeightLeft = ptree.get<double>(layoutSection+".pyramidHeightLeft");
            double pyramidHeightRight = ptree.get<double>(layoutSection+".pyramidHeightRight");
            bool useTile = (isInput || isOutput) ? ptree.get<bool>(layoutSection+".useTile"): false;
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
                return LayoutPyramidal2::GenerateLayout(pyramidBaseEdge, rotationQuaternion, useTile, {{unsigned(pyramidBaseEdgeLength*inputWidth/4), unsigned(pyramidHeightLeft*inputWidth/4), unsigned(pyramidHeightRight*inputWidth/4), unsigned(pyramidHeightTop*inputWidth/4), unsigned(pyramidHeightBottom*inputWidth/4)}});
            }
            else
            {
                return LayoutPyramidal2::GenerateLayout(pyramidBaseEdge, rotationQuaternion, useTile, {{unsigned(pyramidBaseEdgeLength), unsigned(pyramidHeightLeft), unsigned(pyramidHeightRight), unsigned(pyramidHeightTop), unsigned(pyramidHeightBottom)}});
            }
        }
        if (layoutType == "rhombicDodeca")
        {
            Quaternion rotationQuaternion = ParseRotationJSON(ptree.get<std::string>(layoutSection+".rotation"));
            bool useTile = (isInput || isOutput) ? ptree.get<bool>(layoutSection+".useTile"): false;
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

            return LayoutRhombicdodeca::GenerateLayout(rotationQuaternion, useTile, faceRes);
        }
        if (layoutType == "equirectangularTiled")
        {

            Quaternion rotationQuaternion = ParseRotationJSON(ptree.get<std::string>(layoutSection+".rotation"));
            bool useTile = (isInput || isOutput) ? ptree.get<bool>(layoutSection+".useTile"): false;
            bool upscale = ptree.get<bool>(layoutSection+".upscale");

            unsigned int nbHTiles = ptree.get<unsigned int>(layoutSection+".nbHTiles");
            unsigned int nbVTiles = ptree.get<unsigned int>(layoutSection+".nbVTiles");

            BOOST_PP_SEQ_FOR_EACH_PRODUCT(TEST_AND_GENERATE_EQUI_TILED_LAYOUT, RANGE_NB_H_TILES RANGE_NB_V_TILES)

            throw std::invalid_argument("Not supported type: equirectangularTiled with nbHTiles = "+std::to_string(nbHTiles)+" and nbVTiles = "+std::to_string(nbVTiles));
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
