#ifndef _TKVECTOR_H
#define _TKVECTOR_H

#include <math.h>

class TKVec2
{
protected:
   float m_x, m_y;

public:
   TKVec2(const float &_x, const float &_y)
      { m_x = _x; m_y=_y; }

   inline float dotProduct( const TKVec2 &v2 ) const
      { return m_x*v2.m_x + m_y*v2.m_y; }

   inline float crossProduct( const TKVec2 &v2 ) const
      { return ( m_x*v2.m_y - m_y*v2.m_x ); }

   inline float magnitude() const
      { return sqrt(m_x*m_x + m_y*m_y); }

   inline void normalize()
      { float l=this->magnitude(); m_x/=l; m_y/=l; }

   inline void scale( const float &sx, const float &sy )
      { m_x *= sx; m_y *= sy; }

   inline void scale( const TKVec2 &v )
      { m_x *= v.m_x; m_y *= v.m_y; }

   inline void scale( const float &s )
      { m_x*=s; m_y*=s; }

   inline TKVec2 getPerpendicular() const
      { return TKVec2(-m_y, m_x); }

   inline void reverse()
      { m_x*=-1.0f; m_y*=-1.0f; }

   inline void rotate( const float &rad )
      { float newX, newY; newX= m_x*cos(rad) - m_y*sin(rad); newY=m_x*sin(rad)+m_y*cos(rad); m_x=newX; m_y=newY;}

   inline void rotateDeg( const float &deg )
      { float newX, newY, rad=deg*3.14159f/180.0f; 
      newX= m_x*cos(rad) - m_y*sin(rad); newY=m_x*sin(rad)+m_y*cos(rad); m_x=newX; m_y=newY;}

   inline void translate( const float &tx, const float &ty )
      { m_x += tx; m_y += ty; }

   inline float angle( const TKVec2 &v )
      { return acos( (this->dotProduct(v)) / (this->magnitude() * v.magnitude()) ); }

   inline float x() { return m_x; }
   inline float y() { return m_y; }

   inline void setX(const float &_x) { m_x=_x; }
   inline void setY(const float &_y) { m_y=_y; }

   inline void set(const float &_x, const float &_y) {m_x=_x; m_y=_y;}
};

inline float shortestDistance(float sx, float sy, float ex, float ey, float px, float py )
{
   TKVec2 u(sx-ex, sy-ey);
   TKVec2 pq(sx-px, sy-py);

   float magTop = fabs( pq.crossProduct(u) );
   float magU = u.magnitude();

   // Eeeewww
   if( magU==0.0f )
   {
      return -1.0f;
   }

   return magTop / magU;
}

inline bool collisionLine( float sx, float sy, float ex, float ey, float px, float py, float threshold )
{
   float minx, miny, maxx, maxy;
   
   if( sx < ex ) {
      minx = sx;
      maxx = ex;
   }
   else {
      minx = ex;
      maxx = sx;
   }

   if( sy < ey ) {
      miny = sy;
      maxy = ey;
   }
   else {
      miny = ey;
      maxy = sy;
   }
   
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
