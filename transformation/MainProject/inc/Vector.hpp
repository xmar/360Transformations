#pragma once

typedef  double SCALAR;

namespace IMT {

class Vector
{
public:
  constexpr Vector(void) {}
  ~Vector(void) = default;

  // virtual SCALAR Norm(void) const = 0;
  virtual std::ostream& operator<<(std::ostream& o) const = 0;
};

class VectorSpherical;

class VectorCartesian: public Vector
{
public:
  constexpr VectorCartesian(void): Vector(), m_x(0), m_y(0), m_z(0) {}
  constexpr VectorCartesian(SCALAR x, SCALAR y, SCALAR z): Vector(), m_x(x), m_y(y), m_z(z) {}
  ~VectorCartesian(void) = default;

  constexpr SCALAR DotProduct(const VectorCartesian& v) const {return m_x*v.m_x + m_y*v.m_y + m_z*v.m_z;}
  constexpr SCALAR Norm(void) const {return std::sqrt(DotProduct(*this));}

  constexpr VectorCartesian operator+(const VectorCartesian& v) const {return VectorCartesian(m_x+v.m_x, m_y+v.m_y, m_z+v.m_z);}
  constexpr VectorCartesian operator-(void) const {return VectorCartesian(-m_x, -m_y, -m_z);}
  constexpr VectorCartesian operator-(const VectorCartesian& v) const {return VectorCartesian(m_x-v.m_x, m_y-v.m_y, m_z-v.m_z);}
  constexpr VectorCartesian operator*(const SCALAR& s) const {return VectorCartesian(s*m_x, s*m_y, s*m_z);}
  constexpr VectorCartesian operator/(const SCALAR& s) const {return VectorCartesian(m_x/s, m_y/s, m_z/s);}
  //dot product
  constexpr SCALAR operator*(const VectorCartesian& v) const {return DotProduct(v);}
  //Vector product
  constexpr VectorCartesian operator^(const VectorCartesian& v) const {return VectorCartesian(m_y*v.m_z-m_z*v.m_y,
                    m_z*v.m_x-m_x*v.m_z,
                    m_x*v.m_y-m_y*v.m_x);}

  constexpr bool operator==(const VectorCartesian& v) const
  {
    return m_x == v.m_x && m_y == v.m_y &&  m_z == v.m_z;
  }
  constexpr bool operator!=(const VectorCartesian& v) const
  {
    return !(*this == v);
  }

  constexpr VectorCartesian VectorProduct(const VectorCartesian& v) const {return (*this)^v; }

  virtual std::ostream& operator<<(std::ostream& o) const override
  {
    o << "(" << m_x << ", "  << m_y << ", " <<  m_z << ")";
    return o;
  }

  static constexpr VectorCartesian FromSpherical(SCALAR theta, SCALAR phi)
  {
    return VectorCartesian(std::sin(phi)*std::cos(theta), std::sin(phi)*std::sin(theta), std::cos(phi));
  }

  // static VectorCartesian FromSpherical(const VectorSpherical& v)
  // {
  //   return v.ToCartesian();
  // }

  constexpr SCALAR GetX(void) const {return m_x;}
  constexpr SCALAR GetY(void) const {return m_y;}
  constexpr SCALAR GetZ(void) const {return m_z;}
  void SetX(const SCALAR& x) {m_x = x;}
  void SetY(const SCALAR& y) {m_y = y;}
  void SetZ(const SCALAR& z) {m_z = z;}
private:
  SCALAR m_x;
  SCALAR m_y;
  SCALAR m_z;
};


inline std::ostream& operator<<(std::ostream& o, const Vector& v) {return v.operator<<(o);}
inline constexpr VectorCartesian operator*(const SCALAR& s, const VectorCartesian& v) {return v * s;}

class VectorSpherical: public Vector
{
public:
  constexpr VectorSpherical(void): Vector(), m_r(0), m_theta(0), m_phi(0) {}
  constexpr VectorSpherical(SCALAR rho, SCALAR theta, SCALAR phi): Vector(), m_r(rho), m_theta(theta), m_phi(phi) {}
  constexpr VectorSpherical(const VectorCartesian& v): VectorSpherical(v.Norm(), std::atan2(v.GetY(), v.GetX()), std::acos(v.GetZ()/v.Norm())) {}
  ~VectorSpherical(void) = default;

  constexpr operator VectorCartesian() const { return ToCartesian();}

  constexpr SCALAR Norm(void) const {return GetRho();}

  constexpr SCALAR GetRho(void) const {return m_r;}
  constexpr SCALAR GetTheta(void) const {return m_theta;}
  constexpr SCALAR GetPhi(void) const {return m_phi;}
  void SetRho(const SCALAR& r) {m_r = r;}
  void SetTheta(const SCALAR& theta) {m_theta = theta;}
  void SetPhi(const SCALAR& phi) {m_phi = phi;}

  constexpr VectorCartesian ToCartesian(void) const
  {
    return VectorCartesian(std::sin(m_phi)*std::cos(m_theta), std::sin(m_phi)*std::sin(m_theta), std::cos(m_phi));
  }

  virtual std::ostream& operator<<(std::ostream& o) const override
  {
    o << "(" << m_r << ", "  << m_theta << ", " <<  m_phi << ")";
    return o;
  }
private:
  SCALAR m_r;
  SCALAR m_theta;
  SCALAR m_phi;
};


}
