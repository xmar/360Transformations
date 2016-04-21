#include "LayoutEquirectangularTiles.hpp"

using namespace IMT;

static LayoutEquirectangularTiles::TileId ToTileId(unsigned int i)
{
    return std::make_tuple(i/8,i%8);
}

static unsigned int FromTileId(const LayoutEquirectangularTiles::TileId& ti)
{
    return std::get<0>(ti)*8+std::get<1>(ti);
}

Layout::NormalizedFaceInfo LayoutEquirectangularTiles::From2dToNormalizedFaceInfo(const CoordI& pixel) const
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

CoordF LayoutEquirectangularTiles::FromNormalizedInfoTo2d(const Layout::NormalizedFaceInfo& ni) const
{
    auto ti = ToTileId(ni.m_faceId);
    auto offset = TileIdTo2dOffset(ti);
    return CoordF(ni.m_normalizedFaceCoordinate.x*m_tr.GetResWidth(ti)+offset.x, ni.m_normalizedFaceCoordinate.y * m_tr.GetResHeight(ti)+offset.y);
}

Layout::NormalizedFaceInfo LayoutEquirectangularTiles::From3dToNormalizedFaceInfo(const Coord3dSpherical& sphericalCoord) const
{
    //auto i = 8*std::fmod(PI()+sphericalCoord.y, 2.0*PI()/8)/ (2.0*PI());
    //auto j = 8*std::fmod(sphericalCoord.z, PI()/8) / PI();
    double i = 0.5+sphericalCoord.y/ (2.0*PI());
    double j = sphericalCoord.z / PI();
    auto ni =  unsigned(i*8);
    auto nj =  unsigned(j*8);
    if (ni == 8) {ni = 7;}
    if (nj == 8) {nj = 7;}
    return NormalizedFaceInfo(CoordF(8*std::fmod(i,1.0/8), 8*std::fmod(j,1.0/8)), ni*8+nj);
}

Coord3dCart LayoutEquirectangularTiles::FromNormalizedInfoTo3d(const Layout::NormalizedFaceInfo& ni) const
{
    if (ni.m_faceId == -1) {return Coord3dCart(0,0,0);}
    auto ti = ToTileId(ni.m_faceId);
    double theta = (std::get<0>(ti)+ni.m_normalizedFaceCoordinate.x-4)*2.0*PI()/8;
    double phi = (std::get<1>(ti)+ni.m_normalizedFaceCoordinate.y)*PI()/8;
    return Coord3dSpherical(1, theta, phi);
}

static bool Inside(unsigned int i, unsigned int j, const CoordI& start, const CoordI& end)
{
    return (start.x <= i && end.x >= i) && (start.y <= j && end.y >= j);
}

LayoutEquirectangularTiles::TileId LayoutEquirectangularTiles::From2dToTileId(unsigned int i, unsigned int j) const
{
    for (unsigned k = 0; k < 8; ++k)
    {
        for (unsigned l = 0; l < 8; ++l)
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

CoordI LayoutEquirectangularTiles::TileIdTo2dOffset(const TileId& ti) const
{
    auto i = std::get<0>(ti);
    auto j = std::get<1>(ti);
    if (i >= 8 || j >= 8)
    {
        throw std::invalid_argument("GetRes: invalide tile id -> ("+std::to_string(i)+","+std::to_string(j)+")");
    }
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
        offI += (m_colsMaxSize[i]-m_tr.GetResWidth(ti))/2;
    }
    if (j != 0)
    {
        offJ += (m_rowsMaxSize[j]-m_tr.GetResHeight(ti))/2;
    }
    return CoordI(offI, offJ);
}

CoordI LayoutEquirectangularTiles::TileIdTo2dEndOffset(const TileId& ti) const
{
    auto coordStart = TileIdTo2dOffset(ti);
    return CoordI(coordStart.x+m_tr.GetResWidth(ti), coordStart.y+m_tr.GetResHeight(ti));
}
