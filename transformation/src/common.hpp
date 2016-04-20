#pragma once

#include <cmath>
#include <boost/range/iterator_range.hpp>
#include <stdexcept>


namespace IMT {

typedef cv::Vec3b Pixel;
typedef cv::Point2d CoordF;
typedef cv::Point2i CoordI;
template <int i>
struct SpacePoint {
    template <class ... Args> explicit SpacePoint(Args&&... args): d(std::forward<Args>(args)...), x(d.x), y(d.y), z(d.z) {}
    template <int j> explicit SpacePoint(const SpacePoint<j>& sp) = delete;
    template <int j> explicit SpacePoint(SpacePoint<j>&& sp) = delete;
    SpacePoint(const SpacePoint& sp): d(sp.d), x(d.x), y(d.y), z(d.z) {}
    SpacePoint(SpacePoint&& sp): d(std::move(sp.d)), x(d.x), y(d.y), z(d.z) {}
    SpacePoint<i> operator+(const SpacePoint<i>& sp) const {return SpacePoint<i>(d+sp.d);}
    SpacePoint<i> operator-(const SpacePoint<i>& sp) const {return SpacePoint<i>(d-sp.d);}
    SpacePoint<i> operator+(SpacePoint<i>&& sp) const {return SpacePoint(sp.d+=d);}
    SpacePoint<i> operator-(SpacePoint<i>&& sp) const {return SpacePoint(-(sp.d-=d));}
    SpacePoint<i> operator*(const double& s) const {return SpacePoint<i>(d*s);}
    SpacePoint<i> operator/(const double& s) const {return SpacePoint<i>(d/s);}
    SpacePoint<i>& operator+=(const SpacePoint<i>& sp) { d+=sp.d; return SpacePoint(d);}
    SpacePoint<i>& operator-=(const SpacePoint<i>& sp) { d+=sp.d; return SpacePoint(d);}
    operator cv::Point3d() const { return d;}
    operator cv::Point3d&&() { return std::move(d);}
    SpacePoint<i>& operator=(const SpacePoint<i>& sp) { this->d = sp.d; return *this;}
    SpacePoint<i>& operator=(SpacePoint<i>&& sp) { std::swap(this->d, sp.d); return *this;}
    template <int j> operator SpacePoint<j>(void) const;
    cv::Point3d d;
    double& x;
    double& y;
    double& z;
};
typedef SpacePoint<0> Coord3dCart;
typedef SpacePoint<1> Coord3dSpherical;
typedef cv::Vec4d Plan;// (a,b,c,d) a.x+b.y+c.z+d=0
typedef cv::Mat_<double> RotMat;
template <int i> constexpr double norm(const SpacePoint<i>& sp) {return cv::norm(sp.d);}
template <> constexpr double norm(const Coord3dSpherical& sp) {return sp.x;}

constexpr double PI() {return std::atan(1)*4.0;}

constexpr bool inIntervalStrict(double x, double a, double b)
{//true if a < x < b
    return (a <= b) ? (x > a) && (x < b) : inIntervalStrict(x, b, a);
}

constexpr bool inInterval(double x, double a, double b)
{//true if a < x < b
    return (x == a) || (x == b) || inIntervalStrict(x ,a ,b);
}

inline Coord3dSpherical CartToSpherical(const Coord3dCart& coordCart)
{
    double rho = norm(coordCart);
    if (rho > 0)
    {
        double theta = std::atan2(coordCart.y, coordCart.x); //between (-PI;PI)
        double phi = std::acos(coordCart.z / rho); //between (0, PI)
        return Coord3dSpherical(rho, theta, phi);
    }
    else
    {
        return Coord3dSpherical(0, 0, 0);
    }
}

inline Coord3dCart SphericalToCart(const Coord3dSpherical& coordSphe)
{
    double x = coordSphe.x*std::cos(coordSphe.y)*std::sin(coordSphe.z);
    double y = coordSphe.x*std::sin(coordSphe.y)*std::sin(coordSphe.z);
    double z = coordSphe.x*std::cos(coordSphe.z);
    return Coord3dCart(x,y,z);
}

inline Coord3dCart ConvertCoord(const Coord3dSpherical& coordSphe) {return SphericalToCart(coordSphe);}
inline Coord3dSpherical ConvertCoord(const Coord3dCart& coordCart) {return CartToSpherical(coordCart);}
template <int i> template <int j> SpacePoint<i>::operator SpacePoint<j>(void) const{return ConvertCoord(*this);}

inline Coord3dCart Rotation(const Coord3dCart& coordBefRot, const RotMat& rotationMat)
{//hypothesis rotationMat is a 3x3 rotation matrix
    double rotX = rotationMat.at<double>(0,0)*coordBefRot.x + rotationMat.at<double>(0,1)*coordBefRot.y + rotationMat.at<double>(0,2)*coordBefRot.z;
    double rotY = rotationMat.at<double>(1,0)*coordBefRot.x + rotationMat.at<double>(1,1)*coordBefRot.y + rotationMat.at<double>(1,2)*coordBefRot.z;
    double rotZ = rotationMat.at<double>(2,0)*coordBefRot.x + rotationMat.at<double>(2,1)*coordBefRot.y + rotationMat.at<double>(2,2)*coordBefRot.z;
    return Coord3dCart(rotX,rotY,rotZ);
}

inline RotMat GetRotMatrice(double yaw, double pitch, double roll)
{
    //Z*Y*X
    double cosY = std::cos(yaw);
    double sinY = std::sin(yaw);
    double cosP = std::cos(pitch);
    double sinP = std::sin(pitch);
    double cosR = std::cos(roll);
    double sinR = std::sin(roll);
    RotMat m(3,3);
    m(0,0) = cosP * cosY;
    m(0,1) = cosY*sinP*sinR -sinY*cosR;
    m(0,2) = cosY*sinP*cosR + sinY * sinR;
    m(1,0) = sinY*cosP;
    m(1,1) = cosY * cosR ;
    m(1,2) = sinY*sinP*cosR - sinR * cosY;
    m(2,0) = -sinP;
    m(2,1) = cosP * sinR;
    m(2,2) = cosP * cosR;
    return m;
}

inline Coord3dCart Rotation(const Coord3dCart& coordBefRot, double yaw, double pitch, double roll)
{
    return Rotation(coordBefRot, GetRotMatrice(yaw,pitch,roll));
}

inline Coord3dSpherical Rotation(const Coord3dSpherical& coordBefRot, double yaw, double pitch, double roll)
{ return CartToSpherical(Rotation(SphericalToCart(coordBefRot), yaw, pitch, roll));}

inline Plan Rotation(const Plan& p, double theta, double phi, double roll)
{
    Coord3dCart r = Rotation(Coord3dCart(p[0],p[1], p[2]), theta, phi, roll);
    return Plan(r.x, r.y, r.z, p[3]);
}

inline Coord3dCart IntersectionPlan(const Plan& p, double theta, double phi)
{//Return the cartesian coordinate of the intersaction of the line v(1,theta, phi) in spherical coordinate as the dirtion vector and the plan a.x+b.y+c.z+d = 0
    //if no solution return (0,0,0)
    const double& a = p[0];
    const double& b = p[1];
    const double& c = p[2];
    const double& d = p[3];
    double cosT(std::cos(theta)), sinT(std::sin(theta)), cosP(std::cos(phi)), sinP(std::sin(phi));
    double u = a*cosT*sinP + b*sinT*sinP + c*cosP;
    if (u != 0)
    {
        double x = -d* cosT*sinP / u;
        double y = -d* sinT*sinP / u;
        double z = -d* cosP / u;
        return Coord3dCart(x, y, z);
    }
    else
    {
        throw std::logic_error("No intersection");
    }
}

inline Coord3dSpherical IntersectionPlanSpherical(const Plan& p, double theta, double phi)
{
    return CartToSpherical(IntersectionPlan(p, theta, phi));
}

inline Coord3dSpherical IntersectionPlanSpherical (const Plan& p, const Coord3dSpherical& secondPoint)
{ //intersection between the plan p and the line from O to secondPoint;
    return IntersectionPlanSpherical(p, secondPoint.y, secondPoint.z);
}

inline Coord3dSpherical IntersectionPlanSpherical (const Plan& p, const Coord3dCart& secondPoint)
{ //intersection between the plan p and the line from O to secondPoint;
    return IntersectionPlanSpherical(p, CartToSpherical(secondPoint));
}


inline constexpr bool AlmostEqual(double a, double b)
{
   return inIntervalStrict(a, b-0.0005f, b+0.0005f);
}

template<class enum_type>
class enum_iterator {
private:
    enum_type value;
    //typedef typename std::underlying_type<enum_type>::type under;
    typedef int under;
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
    enum_iterator& operator=(const enum_iterator& rhs) noexcept(true) {value=rhs.value; return *this;}
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




}
