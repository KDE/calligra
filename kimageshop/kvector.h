/*
 *  kvector.h - part of KImageShop
 *
 *  Copyright (c) 1999 The KImageShop team (see file AUTHORS)
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
  KVector(float x, float y, float z = 0);
  KVector(int x, int y, int z = 0);
  KVector(long x, long y, long z = 0);

  bool isNull()	const;

  float length();

  float	 x() const;
  float	 y() const;
  float	 z() const;
  void   setX(float x);
  void   setY(float y);
  void   setZ(float Z);

  KVector &normalize();
  KVector &crossProduct(const KVector &v);
  float  dotProduct(const KVector &v);

  KVector &operator+=(const KVector &v);
  KVector &operator-=(const KVector &v);
  KVector &operator*=(int c);
  KVector &operator*=(long c);
  KVector &operator*=(float c);
  KVector &operator/=(int c);
  KVector &operator/=(long c);
  KVector &operator/=(float c);

  friend inline bool operator==(const KVector &, const KVector &);
  friend inline bool operator!=(const KVector &, const KVector &);
  friend inline KVector operator+(const KVector &, const KVector &);
  friend inline KVector operator-(const KVector &, const KVector &);
  friend inline KVector operator*(const KVector &, int);
  friend inline KVector operator*(int, const KVector &);
  friend inline KVector operator*(const KVector &, long);
  friend inline KVector operator*(long, const KVector &);
  friend inline KVector operator*(const KVector &, float);
  friend inline KVector operator*(float, const KVector &);
  friend inline KVector operator-(const KVector &);
  friend inline KVector operator/(const KVector &, int);
  friend inline KVector operator/(const KVector &, long);
  friend inline KVector operator/(const KVector &, float);
   
 private:
  float m_x;
  float m_y;
  float m_z;
};

inline KVector::KVector()
{ m_x=0; m_y=0; m_z=0; }

inline KVector::KVector(float x, float y, float z)
{ m_x=x; m_y=y; m_z=z; }

inline KVector::KVector(int x, int y, int z)
{ m_x=(float)x; m_y=(float)y; m_z=(float)z; }

inline KVector::KVector(long x, long y, long z)
{ m_x=(long)x; m_y=(long)y; m_z=(long)z; }

inline bool KVector::isNull() const
{ return m_x == 0 && m_y == 0 && m_z == 0; }

inline float KVector::length()
{  return (sqrt(m_x*m_x + m_y*m_y + m_z*m_z)); }

inline float KVector::dotProduct(const KVector &v)
{ return m_x*v.m_x + m_y*v.m_y + m_z*v.m_z; }

inline float KVector::x() const
{ return m_x; }

inline float KVector::y() const
{ return m_y; }

inline float KVector::z() const
{ return m_z; }

inline void KVector::setX(float x)
{ m_x=x; }

inline void KVector::setY(float y)
{ m_y=y; }

inline void KVector::setZ(float z)
{ m_z=z; }

inline KVector &KVector::operator+=(const KVector &v)
{ m_x+=v.m_x; m_y+=v.m_y; m_z+=v.m_z; return *this; }

inline KVector &KVector::operator-=(const KVector &v)
{ m_x-=v.m_x; m_y-=v.m_y; m_z-=v.m_z; return *this; }

inline KVector &KVector::operator*=(int c)
{ m_x*=(float)c; m_y*=(float)c; m_z*=(float)c; return *this; }

inline KVector &KVector::operator*=(long c)
{ m_x*=(float)c; m_y*=(float)c; m_z*=(float)c; return *this; }

inline KVector &KVector::operator*=(float c)
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
{ return KVector((float)(v.m_x*c), (float)(v.m_y*c), (float)(v.m_z*c)); }

inline KVector operator*(int c, const KVector &v)
{ return KVector((float)(v.m_x*c), (float)(v.m_y*c), (float)(v.m_z*c)); }

inline KVector operator*(const KVector &v, long c)
{ return KVector((float)(v.m_x*c), (float)(v.m_y*c), (float)(v.m_z*c)); }

inline KVector operator*(long c, const KVector &v)
{ return KVector((float)(v.m_x*c), (float)(v.m_y*c), (float)(v.m_z*c)); }

inline KVector operator*(const KVector &v, float c)
{ return KVector(v.m_x*c, v.m_y*c, v.m_z*c); }

inline KVector operator*(float c, const KVector &v)
{ return KVector(v.m_x*c, v.m_y*c, v.m_z*c); }

inline KVector operator-(const KVector &v)
{ return KVector(-v.m_x, -v.m_y, -v.m_z); }

inline KVector &KVector::operator/=(int c)
{
  if (!c == 0)
    {
      m_x/=(float)c;
      m_y/=(float)c;
      m_z/=(float)c;
    }
    return *this;
}

inline KVector &KVector::operator/=(long c)
{
  if (!c == 0)
    {
      m_x/=(float)c;
      m_y/=(float)c;
      m_z/=(float)c;
    }
    return *this;
}

inline KVector &KVector::operator/=(float c)
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

