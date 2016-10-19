#pragma once
#include "Layout.hpp"

#include <array>
#include <tuple>
#include <stdexcept>

namespace IMT {

template <unsigned int nbHTiles, unsigned int nbVTiles>
class LayoutEquirectangularTiles : public Layout
{
    public:
        typedef std::tuple<unsigned int, unsigned int> TileId;
        typedef std::array<std::array<std::tuple<unsigned int, unsigned int>, nbVTiles>,nbHTiles> TilesMap;
        typedef std::array<std::array<double, nbVTiles>,nbHTiles> ScaleTilesMap;
        virtual ~LayoutEquirectangularTiles() = default;

        virtual CoordI GetReferenceResolution(void) override
        {
            unsigned maxI(0), maxJ(0);
            for (unsigned i = 0; i < nbHTiles; ++i)
            {
                for (unsigned j = 0; j < nbVTiles; ++j)
                {
                    maxI = MAX(maxI, m_tr.GetResWidth(std::make_tuple(i,j)));
                    maxJ = MAX(maxJ, m_tr.GetResHeight(std::make_tuple(i,j)));
                }
            }
            std::cout << nbHTiles*maxI << " " << nbVTiles*maxJ << std::endl;
            return CoordI(nbHTiles*maxI, nbVTiles*maxJ);
        }

        static CoordI GetReferenceResolution(unsigned width, unsigned heigth, const ScaleTilesMap& scaleTm)
        {
            double sumRationCols = 0;
            double sumRationRows = 0;
            for (unsigned int i = 0; i < nbHTiles; ++i)
            {
                double maxCols(0), maxRows(0);
                for (unsigned int j = 0; j < nbVTiles; ++j)
                {
                    maxCols = std::max(maxCols, scaleTm[i][j]);
                    maxRows = std::max(maxRows, scaleTm[j][i]);
                }
                sumRationCols += maxCols;
                sumRationRows += maxRows;
            }
            std::cout << nbHTiles*width/sumRationCols << "; " << nbVTiles*heigth/sumRationRows << std::endl;
            std::cout << width << "; " << heigth << std::endl;
            std::cout << sumRationCols << "; " << sumRationRows << std::endl;
            return CoordI(nbHTiles*width/sumRationCols, nbVTiles*heigth/sumRationRows);
        }


    protected:
        virtual NormalizedFaceInfo From2dToNormalizedFaceInfo(const CoordI& pixel) const override
        {
            auto ti = From2dToTileId(pixel.x, pixel.y);
            if (ti == std::make_tuple(unsigned(-1), unsigned(-1)))
            {
                return NormalizedFaceInfo(CoordF(0,0), -1);
            }
            auto offset = TileIdTo2dOffset(ti);
            auto normPixel = pixel-offset;
            return NormalizedFaceInfo(CoordF(double(normPixel.x)/m_tr.GetResWidth(ti), double(normPixel.y)/m_tr.GetResHeight(ti)), FromTileId(ti));
        }

        virtual CoordF FromNormalizedInfoTo2d(const NormalizedFaceInfo& ni) const override
        {
            auto ti = ToTileId(ni.m_faceId);
            auto offset = TileIdTo2dOffset(ti);
            return CoordF(ni.m_normalizedFaceCoordinate.x*m_tr.GetResWidth(ti)+offset.x, ni.m_normalizedFaceCoordinate.y * m_tr.GetResHeight(ti)+offset.y);
        }

        virtual NormalizedFaceInfo From3dToNormalizedFaceInfo(const Coord3dSpherical& sphericalCoord) const override
        {
            //auto i = 8*std::fmod(PI()+sphericalCoord.y, 2.0*PI()/8)/ (2.0*PI());
            //auto j = 8*std::fmod(sphericalCoord.z, PI()/8) / PI();
            Coord3dSpherical rotCoord = Rotation(sphericalCoord , m_rotationMatrice.t());
            double i = 0.5+rotCoord.y/ (2.0*PI());
            double j = rotCoord.z / PI();
            auto ni =  unsigned(i*nbHTiles);
            auto nj =  unsigned(j*nbVTiles);
            if (ni == nbHTiles) {ni = nbHTiles-1;}
            if (nj == nbVTiles) {nj = nbVTiles-1;}
            return NormalizedFaceInfo(CoordF(nbHTiles*std::fmod(i,1.0/nbHTiles), nbVTiles*std::fmod(j,1.0/nbVTiles)), ni*nbVTiles+nj);
        }

        virtual Coord3dCart FromNormalizedInfoTo3d(const NormalizedFaceInfo& ni) const override
        {
            if (ni.m_faceId == -1) {return Coord3dCart(0,0,0);}
            auto ti = ToTileId(ni.m_faceId);
            double theta = 2.0*PI()*((std::get<0>(ti)+ni.m_normalizedFaceCoordinate.x)/nbHTiles-0.5);
            double phi = (std::get<1>(ti)+ni.m_normalizedFaceCoordinate.y)*PI()/nbVTiles;
            return Rotation(Coord3dSpherical(1, theta, phi), m_rotationMatrice);
        }

        virtual std::shared_ptr<Picture> ReadNextPictureFromVideoImpl(void) override
        {
            bool isInit = false;
            cv::Mat outputMat;
            if (m_useTile)
            {
              for (unsigned i = 0; i < nbHTiles; ++i)
              {
                  for (unsigned j = 0; j < nbVTiles; ++j)
                  {
                      TileId ti(i,j);
                      auto offset = TileIdTo2dOffset(ti);
                      cv::Rect roi( offset.x,  offset.y, m_tr.GetResWidth(ti), m_tr.GetResHeight(ti) );
                      auto facePictPtr = m_inputVideoPtr->GetNextPicture(FromTileId(ti));
                      //std::cout << "Expected Width: "<< m_tr.GetResWidth(ti) << "; Height " << m_tr.GetResHeight(ti) << "; received width "<< facePictPtr->cols << " height "<< facePictPtr->rows << std::endl;
                      if (!isInit)
                      {
                          outputMat = cv::Mat( GetHeight(), GetWidth(), facePictPtr->type());
                          isInit = true;
                      }
                      cv::Mat facePictMat ( outputMat, roi);
                      facePictPtr->copyTo(facePictMat);
                  }
              }
            }
            else
            {
              auto facePictPtr = m_inputVideoPtr->GetNextPicture(0);
              facePictPtr->copyTo(outputMat);
            }
            return std::make_shared<Picture>(outputMat);
        }

        virtual void WritePictureToVideoImpl(std::shared_ptr<Picture> pict) override
        {
            if (m_useTile)
            {
              for (unsigned i = 0; i < nbHTiles; ++i)
              {
                  for (unsigned j = 0; j < nbVTiles; ++j)
                  {
                      TileId ti(i,j);
                      auto offset = TileIdTo2dOffset(ti);
                      cv::Rect roi( offset.x,  offset.y, m_tr.GetResWidth(ti), m_tr.GetResHeight(ti) );
                      cv::Mat facePictMat ( pict->GetMat(), roi);
                      m_outputVideoPtr->Write( facePictMat, FromTileId(ti));
                  }
              }
            }
            else
            {
              m_outputVideoPtr->Write(pict->GetMat(), 0);
            }
        }

        virtual std::shared_ptr<IMT::LibAv::VideoReader> InitInputVideoImpl(std::string pathToInputVideo, unsigned nbFrame) override
        {
            std::shared_ptr<IMT::LibAv::VideoReader> vrPtr = std::make_shared<IMT::LibAv::VideoReader>(pathToInputVideo);
            vrPtr->Init(nbFrame);
            if ((m_useTile && vrPtr->GetNbStream() != nbHTiles*nbVTiles) || (vrPtr->GetNbStream() != 1))
            {
                std::cout << "Unsupported number of stream for EquirectangularTiles input video: "<<vrPtr->GetNbStream() <<" instead of "<< nbHTiles*nbVTiles << std::endl;
                return nullptr;
            }
            //we could add some other check for instance on the width, height of each stream
            return vrPtr;
        }

        virtual std::shared_ptr<IMT::LibAv::VideoWriter> InitOutputVideoImpl(std::string pathToOutputVideo, std::string codecId, unsigned fps, unsigned gop_size, std::vector<unsigned> bit_rateVect) override
        {
            std::shared_ptr<IMT::LibAv::VideoWriter> vwPtr = std::make_shared<IMT::LibAv::VideoWriter>(pathToOutputVideo);
            if (m_useTile)
            {
              std::array<unsigned, nbHTiles*nbVTiles> br;
              std::copy_n(std::make_move_iterator(bit_rateVect.begin()), nbHTiles*nbVTiles, br.begin());
              std::array<unsigned, nbHTiles*nbVTiles> widthArr;
              std::array<unsigned, nbHTiles*nbVTiles> heightArr;
              for (unsigned i = 0; i < nbHTiles; ++i)
              {
                  for (unsigned j = 0; j < nbVTiles; ++j)
                  {
                      TileId ti(i,j);
                      widthArr[FromTileId(ti)] = m_tr.GetResWidth(ti);
                      heightArr[FromTileId(ti)] = m_tr.GetResHeight(ti);
                  }
              }
              vwPtr->Init<nbHTiles*nbVTiles>(codecId, widthArr, heightArr, fps, gop_size, br);
            }
            else
            {
              std::array<unsigned, 1> br;
              std::copy_n(std::make_move_iterator(bit_rateVect.begin()), 1, br.begin());
              std::array<unsigned, 1> widthArr;
              std::array<unsigned, 1> heightArr;
              widthArr[0] = GetWidth();
              heightArr[0] = GetHeight();
              vwPtr->Init<1>(codecId, widthArr, heightArr, fps, gop_size, br);
            }
            return vwPtr;
        }

    private:
        struct TileResolutions
        {
            public:
                TileResolutions(void) = delete;
                TileResolutions(TilesMap tileResVect):
                    m_tiles(std::move(tileResVect))
                    {}
                const std::tuple<unsigned int, unsigned int>& GetRes(const TileId& t) const
                {
                    auto i = std::get<0>(t);
                    auto j = std::get<1>(t);
                    if (i >= nbHTiles || j >= nbVTiles)
                    {
                        throw std::invalid_argument("GetRes: invalide tile id -> ("+std::to_string(i)+","+std::to_string(j)+")");
                    }
                    return m_tiles[i][j];
                }
                unsigned int GetResWidth(const TileId& t) const
                {
                    return std::get<0>(GetRes(t));
                }
                unsigned int GetResHeight(const TileId& t) const
                {
                    return std::get<1>(GetRes(t));
                }

            private:
                TilesMap m_tiles;
        };


        void InitImpl(void) override
        {
            unsigned int sumWidth(0), sumHeight(0);
            for(unsigned int i = 0; i < nbHTiles; ++i)
            {
                unsigned int maxCol = 0;
                for (unsigned int j = 0; j < nbVTiles; ++j)
                {
                    maxCol = MAX(maxCol, m_tr.GetResWidth(std::make_tuple(i,j)));
                }
                sumWidth += maxCol;
                m_colsMaxSize[i] = maxCol;
            }
            for(unsigned int j = 0; j < nbVTiles; ++j)
            {
                unsigned int maxRow = 0;
                for (unsigned int i = 0; i < nbHTiles; ++i)
                {
                    maxRow = MAX(maxRow, m_tr.GetResHeight(std::make_tuple(i,j)));
                }
                sumHeight += maxRow;
                m_rowsMaxSize[j] = maxRow;
            }
            SetWidth(sumWidth);
            SetHeight(sumHeight);
            for (unsigned i = 0; i < nbHTiles; ++i)
            {
                for (unsigned j = 0; j < nbVTiles; ++j)
                {
                    unsigned int offI(0), offJ(0);
                    for (unsigned k = 0; k < i; ++k)
                    {
                        offI += m_colsMaxSize[k];
                    }
                    for (unsigned k = 0; k < j; ++k)
                    {
                        offJ += m_rowsMaxSize[k];
                    }
                    if (i != 0)
                    {
                        offI += (m_colsMaxSize[i]-m_tr.GetResWidth(std::make_tuple(i,j)))/2;
                    }
                    if (j != 0)
                    {
                        offJ += (m_rowsMaxSize[j]-m_tr.GetResHeight(std::make_tuple(i,j)))/2;
                    }
                    m_offsets[i][j] = CoordI(offI, offJ);
                }
            }
        }

        TileId From2dToTileId(unsigned int i, unsigned int j) const
        {
            for (unsigned k = 0; k < nbHTiles; ++k)
            {
                for (unsigned l = 0; l < nbVTiles; ++l)
                {
                    auto ti = std::make_tuple(k,l);
                    if (Inside(i,j, TileIdTo2dOffset(ti), TileIdTo2dEndOffset(ti)))
                    {
                        return ti;
                    }
                }
            }
            return std::make_tuple(unsigned(-1), unsigned(-1));
        }

        const CoordI& TileIdTo2dOffset(const TileId& ti) const {return TileIdTo2dOffset(std::get<0>(ti),std::get<1>(ti));}
        const CoordI& TileIdTo2dOffset(const unsigned int& i, const unsigned int& j) const
        {
            if (i >= nbHTiles || j >= nbVTiles)
            {
                throw std::invalid_argument("GetRes: invalide tile id -> ("+std::to_string(i)+","+std::to_string(j)+")");
            }
            return m_offsets[i][j];
        }

        CoordI TileIdTo2dEndOffset(const TileId& ti) const
        {
            const auto& coordStart = TileIdTo2dOffset(ti);
            return CoordI(coordStart.x+m_tr.GetResWidth(ti), coordStart.y+m_tr.GetResHeight(ti));
        }

        static TileId ToTileId(unsigned int i)
        {
            return std::make_tuple(i/nbVTiles,i%nbVTiles);
        }

        static unsigned int FromTileId(const LayoutEquirectangularTiles::TileId& ti)
        {
            return std::get<0>(ti)*nbVTiles+std::get<1>(ti);
        }

        static bool Inside(unsigned int i, unsigned int j, const CoordI& start, const CoordI& end)
        {
            return (start.x <= i && end.x >= i) && (start.y <= j && end.y >= j);
        }

        TileResolutions m_tr;
        std::array<unsigned int, nbVTiles> m_rowsMaxSize;
        std::array<unsigned int, nbHTiles> m_colsMaxSize;
        std::array<std::array<CoordI, nbVTiles>, nbHTiles> m_offsets;
        RotMat m_rotationMatrice;
        bool m_useTile;

    public:

        LayoutEquirectangularTiles(TilesMap tr, double yaw, double pitch, double roll, bool useTile): Layout(), m_tr(std::move(tr)),
          m_rowsMaxSize(), m_colsMaxSize(), m_offsets(),
          m_rotationMatrice(GetRotMatrice(yaw, pitch, roll)), m_useTile(useTile)
          {};


};
}
