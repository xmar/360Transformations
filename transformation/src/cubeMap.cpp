#include "cubeMap.hpp"
#include "equirectangular.hpp"
#include <math.h> 
#include <iterator>
#include <boost/range/iterator_range.hpp>

using namespace IMT;

template<class enum_type>
class enum_iterator {
private:
    enum_type value;
    typedef typename std::underlying_type<enum_type>::type under;
public:   
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef enum_type value_type;
    typedef enum_type reference;
    typedef enum_type* pointer;
    typedef std::random_access_iterator_tag iterator_category;
    
    constexpr enum_iterator() :value() {}
    constexpr enum_iterator(const enum_iterator& rhs) noexcept(true) :value(rhs.value) {}
    constexpr explicit enum_iterator(enum_type value_) noexcept(true) :value(value_) {}
    ~enum_iterator() noexcept(true) {}
    enum_iterator& operator=(const enum_iterator& rhs) noexcept(true) {value=rhs.valud; return *this;}
    enum_iterator& operator++() noexcept(true) {value = (enum_type)(under(value) + 1); return *this;}
    enum_iterator operator++(int) noexcept(true) {enum_iterator r(*this); ++*this; return r;}
    enum_iterator& operator+=(size_type o) noexcept(true) {value = (enum_type)(under(value) + o); return *this;}
    friend constexpr enum_iterator operator+(const enum_iterator& it, size_type o) noexcept(true) {return enum_iterator((enum_type)(under(it)+o));}
    friend constexpr enum_iterator operator+(size_type o, const enum_iterator& it) noexcept(true) {return enum_iterator((enum_type)(under(it)+o));}
    enum_iterator& operator--() noexcept(true) {value = (enum_type)(under(value) - 1); return *this;}
    enum_iterator operator--(int) noexcept(true) {enum_iterator r(*this); --*this; return r;}
    enum_iterator& operator-=(size_type o) noexcept(true) {value = (enum_type)(under(value) + o); return *this;}
    friend constexpr enum_iterator operator-(const enum_iterator& it, size_type o) noexcept(true) {return enum_iterator((enum_type)(under(it)-o));}
    friend constexpr difference_type operator-(enum_iterator lhs, enum_iterator rhs) noexcept(true) {return under(lhs.value)-under(rhs.value);}
    constexpr reference operator*() const noexcept(true) {return value;}
    constexpr reference operator[](size_type o) const noexcept(true) {return (enum_type)(under(value) + o);}
    constexpr const enum_type* operator->() const noexcept(true) {return &value;}
    constexpr friend bool operator==(const enum_iterator& lhs, const enum_iterator& rhs) noexcept(true) {return lhs.value==rhs.value;}
    constexpr friend bool operator!=(const enum_iterator& lhs, const enum_iterator& rhs) noexcept(true) {return lhs.value!=rhs.value;} 
    constexpr friend bool operator<(const enum_iterator& lhs, const enum_iterator& rhs) noexcept(true) {return lhs.value<rhs.value;}
    constexpr friend bool operator>(const enum_iterator& lhs, const enum_iterator& rhs) noexcept(true) {return lhs.value>rhs.value;}
    constexpr friend bool operator<=(const enum_iterator& lhs, const enum_iterator& rhs) noexcept(true) {return lhs.value<=rhs.value;}
    constexpr friend bool operator>=(const enum_iterator& lhs, const enum_iterator& rhs) noexcept(true) {return lhs.value>=rhs.value;}
    friend void swap(const enum_iterator& lhs, const enum_iterator& rhs) noexcept(true) {std::swap(lhs.value, rhs.value);}
};
template<class enum_type> constexpr boost::iterator_range<enum_iterator<enum_type>> get_range() noexcept(true) 
{return boost::make_iterator_range(enum_iterator<enum_type>(enum_type::First), enum_iterator<enum_type>(enum_type::Last));}


enum class Face {FRONT, BACK, TOP, BOTTOM, LEFT, RIGHT, Last, First=FRONT};

static cv::Point3f Get3dPoint(int i, int j, float cubeEdge, int nbRowPixels, int nbColPixels, Face f)
{
    float normalizedI = ((2.f*float(i)/nbColPixels)-1.f)*cubeEdge;
    float normalizedJ = ((2.f*float(j)/nbRowPixels)-1.f)*cubeEdge;
    switch (f)
    {
        case Face::FRONT:
            return cv::Point3f(cubeEdge, normalizedI, normalizedJ);
        case Face::BACK:
            return cv::Point3f(-cubeEdge, -normalizedI, normalizedJ);
        case Face::BOTTOM:
            return cv::Point3f(-normalizedJ, normalizedI, cubeEdge);
        case Face::TOP:
            return cv::Point3f(normalizedJ, normalizedI, -cubeEdge);
        case Face::LEFT:
            return cv::Point3f(-normalizedI, cubeEdge, normalizedJ);
        case Face::RIGHT:
            return cv::Point3f(normalizedI, -cubeEdge, normalizedJ);
    }
}

static CoordI GetPixelCoordInOut(int i, int j, int nbRowPixels, int nbColPixels, Face f)
{
    switch (f)
    {
        case Face::FRONT:
            return CoordI(i, j);
        case Face::BACK:
            return CoordI(i, j+nbRowPixels);
        case Face::LEFT:
            return CoordI(i+nbColPixels, j);
        case Face::RIGHT:
            return CoordI(i+nbColPixels, j+nbRowPixels);
        case Face::TOP:
            return CoordI(i+2*nbColPixels, j);
        case Face::BOTTOM:
            return CoordI(i+2*nbColPixels, j+nbRowPixels);
    }
}

CubeMap CubeMap::FromEquirectangular(const Equirectangular& er)
{
    int cubeSize = 0.5*er.GetMat().cols/3;
    CubeMap cm(cv::Mat::zeros( cubeSize*2, cubeSize*3, er.GetMat().type()));

    int cubeRows = cubeSize;
    int cubeCols = cubeSize;

    for (auto i = 0; i < cubeCols; ++i)
    {
        for (auto j = 0; j < cubeRows; ++j)
        {
            for (auto f: get_range<Face>())
            {
                auto p = Get3dPoint(i, j, 2.f, cubeRows, cubeCols, f);
                float rho = std::sqrt(p.x*p.x + p.y*p.y + p.z*p.z);
                float theta = std::atan2(p.y, p.x);
                float phi = std::acos(p.z/rho);
                cm.SetValue(GetPixelCoordInOut(i,j, cubeRows, cubeCols, f), er.GetPixelFromAngle(CoordF(theta,phi)));
            }
        }
    }
    return cm;
}
