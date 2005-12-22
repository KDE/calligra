/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef _TKVECTOR_H
#define _TKVECTOR_H

#include <math.h>

class TKVec2
{
protected:
   double m_x, m_y;

public:
   TKVec2(const double &_x, const double &_y)
      { m_x = _x; m_y=_y; }

   inline double dotProduct( const TKVec2 &v2 ) const
      { return m_x*v2.m_x + m_y*v2.m_y; }

   inline double crossProduct( const TKVec2 &v2 ) const
      { return ( m_x*v2.m_y - m_y*v2.m_x ); }

   inline double magnitude() const
      { return sqrt(m_x*m_x + m_y*m_y); }

   inline void normalize()
      { double l=this->magnitude(); m_x/=l; m_y/=l; }

   inline void scale( const double &sx, const double &sy )
      { m_x *= sx; m_y *= sy; }

   inline void scale( const TKVec2 &v )
      { m_x *= v.m_x; m_y *= v.m_y; }

   inline void scale( const double &s )
      { m_x*=s; m_y*=s; }

   inline TKVec2 getPerpendicular() const
      { return TKVec2(-m_y, m_x); }

   inline void reverse()
      { m_x*=-1.0f; m_y*=-1.0f; }

   inline void rotate( const double &rad )
      { double newX, newY; newX= m_x*cos(rad) - m_y*sin(rad); newY=m_x*sin(rad)+m_y*cos(rad); m_x=newX; m_y=newY;}

   inline void rotateDeg( const double &deg )
      { double newX, newY, rad=deg*3.14159f/180.0f; 
      newX= m_x*cos(rad) - m_y*sin(rad); newY=m_x*sin(rad)+m_y*cos(rad); m_x=newX; m_y=newY;}

   inline void translate( const double &tx, const double &ty )
      { m_x += tx; m_y += ty; }

   inline double angle( const TKVec2 &v )
      { return acos( (this->dotProduct(v)) / (this->magnitude() * v.magnitude()) ); }

   inline double x() { return m_x; }
   inline double y() { return m_y; }

   inline void setX(const double &_x) { m_x=_x; }
   inline void setY(const double &_y) { m_y=_y; }

   inline void set(const double &_x, const double &_y) {m_x=_x; m_y=_y;}
};

inline double shortestDistance(double sx, double sy, double ex, double ey, double px, double py )
{
   TKVec2 u(sx-ex, sy-ey);
   TKVec2 pq(sx-px, sy-py);

   double magTop = fabs( pq.crossProduct(u) );
   double magU = u.magnitude();

   // Eeeewww
   if( magU==0.0f )
   {
      return -1.0f;
   }

   return magTop / magU;
}

inline bool collisionLine( double sx, double sy, double ex, double ey, double px, double py, double threshold )
{
   double minx, miny, maxx, maxy;
   
   if( sx < ex ) {
      minx = sx;
      maxx = ex;
   }
   else {
      minx = ex;
      maxx = sx;
   }
   
   minx -= threshold;
   maxx += threshold;

   if( sy < ey ) {
      miny = sy;
      maxy = ey;
   }
   else {
      miny = ey;
      maxy = sy;
   }

   miny -= threshold;
   maxy += threshold;

   if( !(px >= minx &&
	 px <= maxx &&
	 py >= miny &&
	 py <= maxy ) )
   {
      return false;
   }

   if( shortestDistance( sx, sy, ex, ey, px, py ) <= threshold )
      return true;

   return false;
}

#endif
