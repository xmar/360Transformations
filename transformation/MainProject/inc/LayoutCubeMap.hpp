#pragma once

#include "LayoutCubeMapBased.hpp"

#include <boost/property_tree/json_parser.hpp>
#include <sstream>


namespace IMT {

class LayoutCubeMap: public LayoutCubeMapBased
{
    struct FacePosition;
    public:
      static std::shared_ptr<LayoutCubeMap> GenerateLayout(Quaternion rotationQuaternion, bool useTile, std::shared_ptr<VectorialTrans> vectorialTrans, std::string facesPositionString, std::array<std::array<unsigned int, 2>,6> pixelEdges, bool useEqualArea)
	    {
	        FaceResolutions fr(std::move(pixelEdges));
            std::stringstream ss(facesPositionString);
            namespace pt = boost::property_tree;
            pt::ptree ptree_json;
            pt::json_parser::read_json(ss, ptree_json);
            std::array<Faces, 6> face;
            std::array<int, 6> rotationFace;
            for (int i = 0; i < 6; ++i)
            {
                auto v = ptree_json.get_child("face"+std::to_string(1+i));
                if (v.data() == "front")
                {
                    face[i] = Faces::Front;
                }
                else if (v.data() == "back")
                {
                    face[i] = Faces::Back;
                }
                else if (v.data() == "left")
                {
                    face[i] = Faces::Left;
                }
                else if (v.data() == "right")
                {
                    face[i] = Faces::Right;
                }
                else if (v.data() == "top")
                {
                    face[i] = Faces::Top;
                }
                else if (v.data() == "bottom")
                {
                    face[i] = Faces::Bottom;
                }
                else if (v.data() == "black")
                {
                    face[i] = Faces::Black;
                }
                else 
                {
                    throw std::invalid_argument(v.data() + " is not a supported face name.");
                }
                auto vv = ptree_json.get_child("face"+std::to_string(1+i)+"Rotation");
                rotationFace[i] = std::stoi(vv.data());
            }
            FacePosition fp(std::move(face), std::move(rotationFace));
	        auto offsetArraysTuple = Init(fr, fp);
            return std::shared_ptr<LayoutCubeMap>( new LayoutCubeMap( rotationQuaternion, useTile, vectorialTrans,
                std::get<0>(offsetArraysTuple)[0] + std::get<0>(offsetArraysTuple)[1] + std::get<0>(offsetArraysTuple)[2],
                std::get<1>(offsetArraysTuple)[0] + std::get<1>(offsetArraysTuple)[1],
                fr, offsetArraysTuple, std::move(fp), useEqualArea));
	    }
        LayoutCubeMap(unsigned int pixelEdge, bool useTile, std::shared_ptr<VectorialTrans> vectorialTrans):
               LayoutCubeMapBased(3*pixelEdge, 2*pixelEdge, Quaternion(1), useTile, vectorialTrans,
                           {{pixelEdge, pixelEdge, pixelEdge, pixelEdge, pixelEdge, pixelEdge}}),
               m_maxOffsetCols({{pixelEdge,pixelEdge,pixelEdge}}), m_maxOffsetRows({{pixelEdge,pixelEdge}}),
               m_fp({{Faces::Right, Faces::Back, Faces::Left, Faces::Top, Faces::Front, Faces::Bottom}}, {{0, 0, 0, -90, -90, -90}})
               {}
        virtual ~LayoutCubeMap(void) = default;


        virtual CoordI GetReferenceResolution(void) override
        {
            unsigned int maxCol = std::max(m_maxOffsetCols[0], std::max(m_maxOffsetCols[1], m_maxOffsetCols[2]));
            return CoordI(4*maxCol, 2*maxCol);
        }

        static CoordI GetReferenceResolution(unsigned width, unsigned heigth, const std::array<double,6>& scales)
        {
            const double w = width/(scales[0]+scales[2]+scales[4]);
             return CoordI(4*w, 2*w);
        }

    protected:
        Faces From2dToFace(unsigned int i, unsigned int j) const;

        virtual NormalizedFaceInfo From2dToNormalizedFaceInfo(const CoordI& pixel) const override;
        virtual CoordF FromNormalizedInfoTo2d(const NormalizedFaceInfo& ni) const override;

        virtual std::shared_ptr<Picture> ReadNextPictureFromVideoImpl(void) override;
        virtual void WritePictureToVideoImpl(std::shared_ptr<Picture>) override;
        virtual std::shared_ptr<IMT::LibAv::VideoReader> InitInputVideoImpl(std::string pathToInputVideo, unsigned nbFrame) override;
        virtual std::shared_ptr<IMT::LibAv::VideoWriter> InitOutputVideoImpl(std::string pathToOutputVideo, std::string codecId, unsigned fps, unsigned gop_size, std::vector<int> bit_rateVect) override;
    private:
        typedef std::array<unsigned int, 3> ColsOffsetArray;
        typedef std::array<unsigned int, 2> RowsOffsetArray;

        struct FacePosition
        {
            FacePosition(std::array<Faces, 6> faces, std::array<int, 6> rotationFace): m_face(std::move(faces)), m_rotationFace(std::move(rotationFace))
            {
                m_faceToId[Faces::Front]=-1; m_faceToId[Faces::Left]=-1; m_faceToId[Faces::Right]=-1; m_faceToId[Faces::Back]=-1; m_faceToId[Faces::Top]=-1; m_faceToId[Faces::Bottom]=-1;
                for(int i = 0; i < 6; ++i)
                {
                    m_faceToId[m_face[i]] = i;
                }
            };

            std::array<Faces, 6> m_face;
            std::array<int, 6> m_rotationFace;
            std::map<Faces, int> m_faceToId;
            void InitMaxOffset(ColsOffsetArray& maxOffsetCols, RowsOffsetArray& maxOffsetRows, const FaceResolutions& fr) const
            {
                maxOffsetCols[0] = MAX(fr.GetResH(m_face[0]), fr.GetResH(m_face[3]));
                maxOffsetCols[1] = MAX(fr.GetResH(m_face[1]), fr.GetResH(m_face[4]));
                maxOffsetCols[2] = MAX(fr.GetResH(m_face[2]), fr.GetResH(m_face[5]));
    
                maxOffsetRows[0] = MAX(fr.GetResV(m_face[0]), MAX(fr.GetResV(m_face[1]), fr.GetResV(m_face[2])));
                maxOffsetRows[1] = MAX(fr.GetResV(m_face[3]), MAX(fr.GetResV(m_face[4]), fr.GetResV(m_face[5])));
            }
            std::tuple<double, double> GetIJ2dToNorm(double u, double v, Faces f) const
            {
                int faceId = m_faceToId.at(f);
                if (m_rotationFace[faceId] == 0)
                {
                    return std::make_tuple(u, v);
                }
                else if (m_rotationFace[faceId] == -90)
                {
                    return std::make_tuple(1-v, u);
                }
                else if (m_rotationFace[faceId] == 90)
                {
                    return std::make_tuple(v, 1-u);
                }
                else if (m_rotationFace[faceId] == 180 || m_rotationFace[faceId] == -180)
                {
                    return std::make_tuple(1-u, 1-v);
                }
                else
                {
                    throw std::invalid_argument("GetIJ2dToNorm: Not supported rotation: "+std::to_string(m_rotationFace[faceId]));
                }
            }
            std::tuple<double, double> GetIJNormTo2d(double u, double v, Faces f) const
            {
                int faceId = m_faceToId.at(f);
                if (m_rotationFace[faceId] == 0)
                {
                    return std::make_tuple(u, v);
                }
                else if (m_rotationFace[faceId] == -90)
                {
                    return std::make_tuple(v, 1-u);
                }
                else if (m_rotationFace[faceId] == 90)
                {
                    return std::make_tuple(1-v, u);
                }
                else if (m_rotationFace[faceId] == 180 || m_rotationFace[faceId] == -180)
                {
                    return std::make_tuple(1-u, 1-v);
                }
                else
                {
                    throw std::invalid_argument("GetIJNormTo2d: Not supported rotation: "+std::to_string(m_rotationFace[faceId])+" for facePosition "+std::to_string(faceId) + " for faceId " +std::to_string(static_cast<int>(f)));
                }
            }
        };
        ColsOffsetArray m_maxOffsetCols;
        RowsOffsetArray m_maxOffsetRows;
        FacePosition m_fp;

        LayoutCubeMap(Quaternion rotationQuaternion, bool useTile, std::shared_ptr<VectorialTrans> vectorialTrans, unsigned int width, unsigned int height, const FaceResolutions& fr, const std::tuple<ColsOffsetArray, RowsOffsetArray>& t, FacePosition fp, bool useEqualArea):
            LayoutCubeMapBased(width, height, rotationQuaternion, useTile, vectorialTrans, fr, useEqualArea), m_maxOffsetCols(std::get<0>(t)), m_maxOffsetRows(std::get<1>(t)), m_fp(std::move(fp)) {}

        static std::tuple<ColsOffsetArray, RowsOffsetArray> Init(const FaceResolutions& fr, const FacePosition& fp)
        {
            ColsOffsetArray maxOffsetCols;
            RowsOffsetArray maxOffsetRows;
            fp.InitMaxOffset(maxOffsetCols, maxOffsetRows, fr);

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

class  LayoutConfigParserCubemap: public LayoutConfigParserCubemapBase
{
    public:
        LayoutConfigParserCubemap(std::string key, bool useEqualArea): LayoutConfigParserCubemapBase(key, useEqualArea),
            m_facesPositionString(this, "facesPosition", "JSON describing the face position and orientation. [default={\"face1\":\"right\", \"face1Rotation\":0, \"face2\":\"back\", \"face2Rotation\":0, \"face3\":\"left\", \"face3Rotation\":0, \"face4\":\"top\", \"face4Rotation\":-90, \"face5\":\"front\", \"face5Rotation\":-90, \"face6\":\"bottom\", \"face6Rotation\":-90}]", true, "{\"face1\":\"right\", \"face1Rotation\":0, \"face2\":\"back\", \"face2Rotation\":0, \"face3\":\"left\", \"face3Rotation\":0, \"face4\":\"top\", \"face4Rotation\":-90, \"face5\":\"front\", \"face5Rotation\":-90, \"face6\":\"bottom\", \"face6Rotation\":-90}")
        {}

        std::shared_ptr<Layout> Create(std::string layoutSection, pt::ptree& ptree) const override
        {
            Quaternion rot = m_rotationQuaternion.GetRotation(layoutSection, ptree);
            auto vectorialTrans = GetVectorialTransformation(m_vectTransOptional.GetValue(layoutSection, ptree), ptree, rot);
            auto inputWidth = m_width.GetValue(layoutSection, ptree);
            auto inputHeight = m_height.GetValue(layoutSection, ptree);
            auto edgeFront = m_edgeFront.GetValue(layoutSection, ptree);
            auto edgeRight = m_edgeRight.GetValue(layoutSection, ptree);
            auto edgeLeft = m_edgeLeft.GetValue(layoutSection, ptree);
            auto edgeBack = m_edgeBack.GetValue(layoutSection, ptree);
            auto edgeTop = m_edgeTop.GetValue(layoutSection, ptree);
            auto edgeBottom = m_edgeBottom.GetValue(layoutSection, ptree);
            return LayoutCubeMap::GenerateLayout(rot, false, std::move(vectorialTrans), m_facesPositionString.GetValue(layoutSection, ptree), {{std::array<unsigned int,2>{unsigned(edgeFront*inputWidth/3), unsigned(edgeFront*inputHeight/2)}, std::array<unsigned int,2>{unsigned(edgeBack*inputWidth/3), unsigned(edgeBack*inputHeight/2)}, std::array<unsigned int,2>{unsigned(edgeLeft*inputWidth/3), unsigned(edgeLeft*inputHeight/2)}, std::array<unsigned int,2>{unsigned(edgeRight*inputWidth/3), unsigned(edgeRight*inputHeight/2)}, std::array<unsigned int,2>{unsigned(edgeTop*inputWidth/3),unsigned(edgeTop*inputHeight/2)}, std::array<unsigned int,2>{unsigned(edgeBottom*inputWidth/3),unsigned(edgeBottom*inputHeight/2)}}}, m_useEqualArea);
        }
    private:
        KeyTypeDescription<std::string> m_facesPositionString;
};

}
