/*
 *  kvector.h - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter <me@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __kvector_h__
#define __kvector_h__

/*
 * 2D/3D vector class
 */

class KVector
{
 public:
  KVector();
  KVector(double x, double y, double z = 0);
  KVector(int x, int y, int z = 0);
  KVector(long x, long y, long z = 0);

  bool isNull()	const;

  double length();

  double	 x() const;
  double	 y() const;
  double	 z() const;
  void   setX(double x);
  void   setY(double y);
  void   setZ(double Z);

  KVector &normalize();
  KVector &crossProduct(const KVector &v);
  double  dotProduct(const KVector &v);

  KVector &operator+=(const KVector &v);
  KVector &operator-=(const KVector &v);
  KVector &operator*=(int c);
  KVector &operator*=(long c);
  KVector &operator*=(double c);
  KVector &operator/=(int c);
  KVector &operator/=(long c);
  KVector &operator/=(double c);

  friend inline bool operator==(const KVector &, const KVector &);
  friend inline bool operator!=(const KVector &, const KVector &);
  friend inline KVector operator+(const KVector &, const KVector &);
  friend inline KVector operator-(const KVector &, const KVector &);
  friend inline KVector operator*(const KVector &, int);
  friend inline KVector operator*(int, const KVector &);
  friend inline KVector operator*(const KVector &, long);
  friend inline KVector operator*(long, const KVector &);
  friend inline KVector operator*(const KVector &, double);
  friend inline KVector operator*(double, const KVector &);
  friend inline KVector operator-(const KVector &);
  friend inline KVector operator/(const KVector &, int);
  friend inline KVector operator/(const KVector &, long);
  friend inline KVector operator/(const KVector &, double);
   
 private:
  double m_x;
  double m_y;
  double m_z;
};

inline KVector::KVector()
{ m_x=0; m_y=0; m_z=0; }

inline KVector::KVector(double x, double y, double z)
{ m_x=x; m_y=y; m_z=z; }

inline KVector::KVector(int x, int y, int z)
{ m_x=static_cast<double>(x); m_y=static_cast<double>(y); m_z=static_cast<double>(z); }

inline KVector::KVector(long x, long y, long z)
{ m_x=static_cast<long>(x); m_y=static_cast<long>(y); m_z=static_cast<long>(z); }

inline bool KVector::isNull() const
{ return m_x == 0 && m_y == 0 && m_z == 0; }

inline double KVector::length()
{  return (sqrt(m_x*m_x + m_y*m_y + m_z*m_z)); }

inline double KVector::dotProduct(const KVector &v)
{ return m_x*v.m_x + m_y*v.m_y + m_z*v.m_z; }

inline double KVector::x() const
{ return m_x; }

inline double KVector::y() const
{ return m_y; }

inline double KVector::z() const
{ return m_z; }

inline void KVector::setX(double x)
{ m_x=x; }

inline void KVector::setY(double y)
{ m_y=y; }

inline void KVector::setZ(double z)
{ m_z=z; }

inline KVector &KVector::operator+=(const KVector &v)
{ m_x+=v.m_x; m_y+=v.m_y; m_z+=v.m_z; return *this; }

inline KVector &KVector::operator-=(const KVector &v)
{ m_x-=v.m_x; m_y-=v.m_y; m_z-=v.m_z; return *this; }

inline KVector &KVector::operator*=(int c)
{ m_x*=static_cast<double>(c); m_y*=static_cast<double>(c); m_z*=static_cast<double>(c); return *this; }

inline KVector &KVector::operator*=(long c)
{ m_x*=static_cast<double>(c); m_y*=static_cast<double>(c); m_z*=static_cast<double>(c); return *this; }

inline KVector &KVector::operator*=(double c)
{ m_x*=c; m_y*=c; m_z*=c; return *this; }

inline bool operator==(const KVector &v1, const KVector &v2)
{ return v1.m_x == v2.m_x && v1.m_y == v2.m_y && v1.m_z == v2.m_z; }

inline bool operator!=(const KVector &v1, const KVector &v2)
{ return v1.m_x != v2.m_x || v1.m_y != v2.m_y || v1.m_z != v2.m_z; }

inline KVector operator+(const KVector &v1, const KVector &v2)
{ return KVector(v1.m_x+v2.m_x, v1.m_y+v2.m_y, v1.m_z+v2.m_z); }

inline KVector operator-(const KVector &v1, const KVector &v2)
{ return KVector(v1.m_x-v2.m_x, v1.m_y-v2.m_y, v1.m_z-v2.m_z); }

inline KVector operator*(const KVector &v, int c)
{ return KVector(static_cast<double>(v.m_x*c), static_cast<double>(v.m_y*c), static_cast<double>(v.m_z*c)); }

inline KVector operator*(int c, const KVector &v)
{ return KVector(static_cast<double>(v.m_x*c), static_cast<double>(v.m_y*c), static_cast<double>(v.m_z*c)); }

inline KVector operator*(const KVector &v, long c)
{ return KVector(static_cast<double>(v.m_x*c), static_cast<double>(v.m_y*c), static_cast<double>(v.m_z*c)); }

inline KVector operator*(long c, const KVector &v)
{ return KVector(static_cast<double>(v.m_x*c), static_cast<double>(v.m_y*c), static_cast<double>(v.m_z*c)); }

inline KVector operator*(const KVector &v, double c)
{ return KVector(v.m_x*c, v.m_y*c, v.m_z*c); }

inline KVector operator*(double c, const KVector &v)
{ return KVector(v.m_x*c, v.m_y*c, v.m_z*c); }

inline KVector operator-(const KVector &v)
{ return KVector(-v.m_x, -v.m_y, -v.m_z); }

inline KVector &KVector::operator/=(int c)
{
  if (!c == 0)
    {
      m_x/=static_cast<double>(c);
      m_y/=static_cast<double>(c);
      m_z/=static_cast<double>(c);
    }
    return *this;
}

inline KVector &KVector::operator/=(long c)
{
  if (!c == 0)
    {
      m_x/=static_cast<double>(c);
      m_y/=static_cast<double>(c);
      m_z/=static_cast<double>(c);
    }
    return *this;
}

inline KVector &KVector::operator/=(double c)
{
  if (!c == 0)
    {
      m_x/=c;
      m_y/=c;
      m_z/=c;
    }
    return *this;
}

#endif

