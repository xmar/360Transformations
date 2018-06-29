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
#include "LayoutViewport.hpp"
#include "LayoutPyramidal.hpp"
#include "LayoutPyramidal2.hpp"
#include "LayoutRhombicdodeca.hpp"
#include "OffsetTrans.hpp"
#include "HorizontalOffsetTrans.hpp"


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
  return std::make_shared<LayoutEquirectangularTiles<nbHTiles,nbVTiles>>(std::move(scaleRes), std::move(tilesRatios), rotationQuaternion, orignalRes, useTile, upscale, vectorialTrans);
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
std::vector<int> GetBitrateVector(std::string layoutSection, pt::ptree& ptree, int bitrateGoal)
{
    std::string layoutType;
    std::vector<int> bitrateVect;
    try {
        layoutType = ptree.get<std::string>(layoutSection+".type");
        if (layoutType == "equirectangular")
        {
            auto bitrate = ptree.get<double>(layoutSection+".bitrate");

            bitrateVect.push_back(bitrateGoal * bitrate);
            return bitrateVect;
        }
        if (layoutType == "cubeMap" || layoutType == "cubeMap2" || layoutType == "EAC")
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
        if (layoutType == "viewport")
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

Coord3dCart ParseVectorJSON(std::string s)
{
  std::stringstream ss(s);
  pt::ptree ptree_json;

  pt::json_parser::read_json(ss, ptree_json);

  auto& v = ptree_json.get_child("type");

  if (v.data() == "vectorCart")
  {
    auto vv = ptree_json.get_child("x");
    auto x = std::stod(vv.data());

    vv = ptree_json.get_child("y");
    auto y =  std::stod(vv.data());

    vv = ptree_json.get_child("z");
    auto z = std::stod(vv.data());
    return Coord3dCart(x, y, z);
  }
  else if (v.data() == "vectorSpherical")
  {
    auto vv = ptree_json.get_child("rho");
    auto rho = std::stod(vv.data());

    vv = ptree_json.get_child("theta");
    auto theta =  std::stod(vv.data())*PI()/180.f;

    vv = ptree_json.get_child("phi");
    auto phi = std::stod(vv.data())*PI()/180.f;
    return Coord3dSpherical(rho, theta, phi);
  }
  else if (v.data() == "rotation")
  {
    auto vv = ptree_json.get_child("rotation");
    std::ostringstream outSs;
    pt::json_parser::write_json(outSs, vv);
    auto q = ParseRotationJSON(outSs.str());
    return q.Rotation(Coord3dCart(1, 0, 0));
  }
  return Coord3dCart(0, 0, 0);
}

std::shared_ptr<VectorialTrans> GetVectorialTransformation(std::string transSection, pt::ptree& ptree, const Quaternion& layoutRotation)
{
    if (transSection.empty())
    {
        return std::make_shared<VectorialTrans>();
    }
    std::string transType;
    try
    {
        transType = ptree.get<std::string>(transSection+".vectorSpaceTransformationType");
        if (transType == "offsetTrans")
        {
            auto offsetRatio = ptree.get<double>(transSection+".offsetRatio");
            auto vector_b_conf = ptree.get_optional<std::string>(transSection+".emphDirection");
            Coord3dCart b(0, 0, 0);
            if (vector_b_conf)
            {
                b = ParseVectorJSON(vector_b_conf.get());
                b = b/b.Norm();
            }
            else
            {
                b = layoutRotation.Rotation(Coord3dCart(1, 0, 0));
            }
            std::cout << "offsetTrans -> alpha = " << offsetRatio << "; b = " << b << std::endl;
            return std::make_shared<OffsetTrans>(offsetRatio, std::move(b));
        }
        if (transType == "horizontalOffsetTrans")
        {
            auto offsetRatio = ptree.get<double>(transSection+".offsetRatio");
            auto orientation_conf = ptree.get_optional<std::string>(transSection+".orientation");
            Quaternion q;
            if (orientation_conf)
            {
                q = ParseRotationJSON(orientation_conf.get());
            }
            else
            {
                q = layoutRotation;
            }
            std::cout << "horizontalOffsetTrans -> alpha = " << offsetRatio << "; q = " << q << std::endl;
            return std::make_shared<HorizontalOffsetTrans>(offsetRatio, std::move(q));
        }

    }
    catch (std::exception &e)                                                    
    {                                                                            
        std::cout << "Error while parsing in configuration file the "<<transSection<<" Vectorial Transformation: " << e.what() << std::endl;
        throw e;                                                                 
    }                                                                            
    throw std::invalid_argument("Not supported type: "+transType);
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
        else
        {
            auto widthO = ptree.get_optional<double>(layoutSection+".refWidth");
            auto heightO = ptree.get_optional<double>(layoutSection+".refHeight");
            if (widthO && heightO)
            {
                inputWidth = widthO.get();
                inputHeight = heightO.get();
            } 
        }
        if (layoutType == "equirectangular")
        {
            auto rotationOpt = ptree.get_optional<std::string>(layoutSection+".rotation");
            Quaternion rotationQuaternion(1);
            if (rotationOpt && rotationOpt.get().size() > 0)
            {
                rotationQuaternion = ParseRotationJSON(rotationOpt.get());
            }
            std::string vectTrans;
            auto vectTransOptional = ptree.get_optional<std::string>(layoutSection+".vectorSpaceTransformation");
            if (vectTransOptional)
            {
                vectTrans = vectTransOptional.get();
            }
            auto vectorialTrans = GetVectorialTransformation(vectTrans, ptree, rotationQuaternion);
            if (isInput)
            {
                inputWidth = refRes.x;
                inputHeight = refRes.y;
                return std::shared_ptr<Layout>(std::make_shared<LayoutEquirectangular>(inputWidth, inputHeight, rotationQuaternion, vectorialTrans));
            }
            else
            {
                double relativeWidth = ptree.get<double>(layoutSection+".width");
                double relativeHeight = ptree.get<double>(layoutSection+".height");
                if (infer)
                {
                    return std::shared_ptr<Layout>(std::make_shared<LayoutEquirectangular>(relativeWidth*inputWidth, relativeHeight*inputHeight, rotationQuaternion, vectorialTrans));
                }
                else
                {
                    return std::shared_ptr<Layout>(std::make_shared<LayoutEquirectangular>(relativeWidth, relativeHeight, rotationQuaternion, vectorialTrans));
                }
            }

        }
        if (layoutType == "cubeMap" || layoutType == "EAC")
        {
            double edgeFront = ptree.get<double>(layoutSection+".cubeEdgeLengthFront");
            double edgeBack = ptree.get<double>(layoutSection+".cubeEdgeLengthBack");
            double edgeLeft = ptree.get<double>(layoutSection+".cubeEdgeLengthLeft");
            double edgeRight = ptree.get<double>(layoutSection+".cubeEdgeLengthRight");
            double edgeTop = ptree.get<double>(layoutSection+".cubeEdgeLengthTop");
            double edgeBottom = ptree.get<double>(layoutSection+".cubeEdgeLengthBottom");
            auto facesPositionStringOpt = ptree.get_optional<std::string>(layoutSection+".facesPosition");
            bool useEqualArea = layoutType == "EAC";
            std::string facesPositionString;
            if (facesPositionStringOpt)
            {
                facesPositionString = facesPositionStringOpt.get();
            }
            else
            {
                facesPositionString = "{\"face1\":\"right\", \"face1Rotation\":0, \"face2\":\"back\", \"face2Rotation\":0, \"face3\":\"left\", \"face3Rotation\":0, \"face4\":\"top\", \"face4Rotation\":-90, \"face5\":\"front\", \"face5Rotation\":-90, \"face6\":\"bottom\", \"face6Rotation\":-90}";
            }
            Quaternion rotationQuaternion = ParseRotationJSON(ptree.get<std::string>(layoutSection+".rotation"));
            std::string vectTrans;
            auto vectTransOptional = ptree.get_optional<std::string>(layoutSection+".vectorSpaceTransformation");
            if (vectTransOptional)
            {
                vectTrans = vectTransOptional.get();
            }
            auto vectorialTrans = GetVectorialTransformation(vectTrans, ptree, rotationQuaternion);
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
                return LayoutCubeMap::GenerateLayout(rotationQuaternion, useTile, vectorialTrans, facesPositionString, {{std::array<unsigned int,2>{unsigned(edgeFront*inputWidth/3), unsigned(edgeFront*inputHeight/2)}, std::array<unsigned int,2>{unsigned(edgeBack*inputWidth/3), unsigned(edgeBack*inputHeight/2)}, std::array<unsigned int,2>{unsigned(edgeLeft*inputWidth/3), unsigned(edgeLeft*inputHeight/2)}, std::array<unsigned int,2>{unsigned(edgeRight*inputWidth/3), unsigned(edgeRight*inputHeight/2)}, std::array<unsigned int,2>{unsigned(edgeTop*inputWidth/3),unsigned(edgeTop*inputHeight/2)}, std::array<unsigned int,2>{unsigned(edgeBottom*inputWidth/3),unsigned(edgeBottom*inputHeight/2)}}}, useEqualArea);
            }
            else
            {
                return LayoutCubeMap::GenerateLayout(rotationQuaternion, useTile, vectorialTrans, facesPositionString, {{std::array<unsigned int,2>{unsigned(edgeFront),unsigned(edgeFront)}, std::array<unsigned int,2>{unsigned(edgeBack), unsigned(edgeBack)}, std::array<unsigned int,2>{unsigned(edgeLeft), unsigned(edgeLeft)}, std::array<unsigned int,2>{unsigned(edgeRight), unsigned(edgeRight)}, std::array<unsigned int,2>{unsigned(edgeTop), unsigned(edgeTop)}, std::array<unsigned int,2>{unsigned(edgeBottom), unsigned(edgeBottom)}}}, useEqualArea);
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
            Quaternion rotationQuaternion = ParseRotationJSON(ptree.get<std::string>(layoutSection+".rotation"));
            std::string vectTrans;
            auto vectTransOptional = ptree.get_optional<std::string>(layoutSection+".vectorSpaceTransformation");
            if (vectTransOptional)
            {
                vectTrans = vectTransOptional.get();
            }
            auto vectorialTrans = GetVectorialTransformation(vectTrans, ptree, rotationQuaternion);
            bool useTile = (isInput || isOutput) ? ptree.get<bool>(layoutSection+".useTile"): false;
            if (isInput)
            {
                if (!infer)
                {
                    throw std::invalid_argument("Input with static resolution not supported yet");
                }
                //if (refRes == CoordI(0,0))
                //{
                //    refRes = LayoutCubeMap2::GetReferenceResolution(inputWidth, inputHeight, {{edgeFront, edgeBack, edgeLeft, edgeRight, edgeTop, edgeBottom}});
                //}
                //inputWidth = refRes.x;
                //inputHeight = refRes.y;
            }
            if (infer)
            {
                return LayoutCubeMap2::GenerateLayout(rotationQuaternion, useTile, vectorialTrans, {{std::array<unsigned int,2>{unsigned(edgeFront*inputWidth/4), unsigned(edgeFront*inputHeight/3)}, std::array<unsigned int,2>{unsigned(edgeBack*inputWidth/4), unsigned(edgeBack*inputHeight/3)}, std::array<unsigned int,2>{unsigned(edgeLeft*inputWidth/4), unsigned(edgeLeft*inputHeight/3)}, std::array<unsigned int,2>{unsigned(edgeRight*inputWidth/4), unsigned(edgeRight*inputHeight/3)}, std::array<unsigned int,2>{unsigned(edgeTop*inputWidth/4),unsigned(edgeTop*inputHeight/3)}, std::array<unsigned int,2>{unsigned(edgeBottom*inputWidth/4),unsigned(edgeBottom*inputHeight/3)}}});
            }
            else
            {
                return LayoutCubeMap2::GenerateLayout(rotationQuaternion, useTile, vectorialTrans, {{std::array<unsigned int,2>{unsigned(edgeFront),unsigned(edgeFront)}, std::array<unsigned int,2>{unsigned(edgeBack), unsigned(edgeBack)}, std::array<unsigned int,2>{unsigned(edgeLeft), unsigned(edgeLeft)}, std::array<unsigned int,2>{unsigned(edgeRight), unsigned(edgeRight)}, std::array<unsigned int,2>{unsigned(edgeTop), unsigned(edgeTop)}, std::array<unsigned int,2>{unsigned(edgeBottom), unsigned(edgeBottom)}}});
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
        if (layoutType == "viewport")
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
                throw std::invalid_argument("Viewport  layout cannot be the input of the transformation flow");
            }
            if (infer)
            {
                return std::make_shared<LayoutViewport>(std::move(dynamicPosition), width*inputWidth, height*inputHeight, horizontalAngleVision, verticalAngleOfVision);
            }
            else
            {
                return std::make_shared<LayoutViewport>(std::move(dynamicPosition), width, height, horizontalAngleVision, verticalAngleOfVision);
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
            std::string vectTrans;
            auto vectTransOptional = ptree.get_optional<std::string>(layoutSection+".vectorSpaceTransformation");
            if (vectTransOptional)
            {
                vectTrans = vectTransOptional.get();
            }
            auto vectorialTrans = GetVectorialTransformation(vectTrans, ptree, rotationQuaternion);
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
                return std::make_shared<LayoutPyramidal>(pyramidBaseEdge, rotationQuaternion, useTile, vectorialTrans, pyramidBaseEdgeLength*inputWidth/4);
            }
            else
            {
                return std::make_shared<LayoutPyramidal>(pyramidBaseEdge, rotationQuaternion, useTile, vectorialTrans, pyramidBaseEdgeLength);
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
            std::string vectTrans;
            auto vectTransOptional = ptree.get_optional<std::string>(layoutSection+".vectorSpaceTransformation");
            if (vectTransOptional)
            {
                vectTrans = vectTransOptional.get();
            }
            auto vectorialTrans = GetVectorialTransformation(vectTrans, ptree, rotationQuaternion);
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
                return LayoutPyramidal2::GenerateLayout(pyramidBaseEdge, rotationQuaternion, useTile, vectorialTrans, {{unsigned(pyramidBaseEdgeLength*inputWidth/4), unsigned(pyramidHeightLeft*inputWidth/4), unsigned(pyramidHeightRight*inputWidth/4), unsigned(pyramidHeightTop*inputWidth/4), unsigned(pyramidHeightBottom*inputWidth/4)}});
            }
            else
            {
                return LayoutPyramidal2::GenerateLayout(pyramidBaseEdge, rotationQuaternion, useTile, vectorialTrans, {{unsigned(pyramidBaseEdgeLength), unsigned(pyramidHeightLeft), unsigned(pyramidHeightRight), unsigned(pyramidHeightTop), unsigned(pyramidHeightBottom)}});
            }
        }
        if (layoutType == "rhombicDodeca")
        {
            Quaternion rotationQuaternion = ParseRotationJSON(ptree.get<std::string>(layoutSection+".rotation"));
            bool useTile = (isInput || isOutput) ? ptree.get<bool>(layoutSection+".useTile"): false;
            std::array<double, 12> faceResScale;
            std::array<unsigned int, 12> faceRes;
            std::string vectTrans;
            auto vectTransOptional = ptree.get_optional<std::string>(layoutSection+".vectorSpaceTransformation");
            if (vectTransOptional)
            {
                vectTrans = vectTransOptional.get();
            }
            auto vectorialTrans = GetVectorialTransformation(vectTrans, ptree, rotationQuaternion);

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

            return LayoutRhombicdodeca::GenerateLayout(rotationQuaternion, vectorialTrans, useTile, faceRes);
        }
        if (layoutType == "equirectangularTiled")
        {

            Quaternion rotationQuaternion = ParseRotationJSON(ptree.get<std::string>(layoutSection+".rotation"));
            bool useTile = (isInput || isOutput) ? ptree.get<bool>(layoutSection+".useTile"): false;
            bool upscale = ptree.get<bool>(layoutSection+".upscale");
            std::string vectTrans;
            auto vectTransOptional = ptree.get_optional<std::string>(layoutSection+".vectorSpaceTransformation");
            if (vectTransOptional)
            {
                vectTrans = vectTransOptional.get();
            }
            auto vectorialTrans = GetVectorialTransformation(vectTrans, ptree, rotationQuaternion);


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
