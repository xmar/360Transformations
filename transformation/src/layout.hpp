#pragma once

#include <opencv2/opencv.hpp>
#include <cmath>
#include <boost/range/iterator_range.hpp>
#include "picture.hpp"

namespace IMT {
class Layout
{
    /* This virtual class is used to define any kind of output layout */
    public:
        Layout(unsigned int outWidth, unsigned int outHeight): m_outWidth(outWidth), m_outHeight(outHeight) {};

        /*Return the 3D coordinate cartesian of the point corresponding to the pixel with coordinate (i,j) on the 2d layout*/
        virtual cv::Point3f from2dTo3d(unsigned int i, unsigned int j) const = 0;        
        
        /* Same as from2dTo3d except it return spherical coordinate */
        cv::Point3f from2dTo3dSpherical(unsigned int i, unsigned int j)
        {
            auto cartP = from2dTo3d(i,j);
            float rho = cv::norm(cartP);
            float theta = std::atan2(cartP.y, cartP.x);
            float phi = std::acos(cartP.z / rho);
            return cv::Point3f(rho, theta, phi);
        }
   
        /*Return the coordinate of the 2d layout that correspond to the point on the sphere in shperical coordinate (1, theta, phi)*/
        virtual CoordF fromSphereTo2d(float theta, float phi) const = 0;

        unsigned int GetWidth(void) const {return m_outWidth;}
        unsigned int GetHeight(void) const {return m_outHeight;}
    protected:
        unsigned int m_outWidth;
        unsigned int m_outHeight;
    private:
};

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



}
