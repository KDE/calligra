
// these are the things horribly scattered around

#ifndef ENGINE2_H
#define ENGINE2_H

#define HIGHSET		0
#define LOWSET		1
#define CLOSESET	2

// scaled translation onto graph
#define PX( x )		(int)( xorig + (setno*xdepth_3D) + (x)*xscl )
#define PY( y )		(int)( yorig - (setno*ydepth_3D) + (y)*yscl )
#define PV( y )		(int)( vyorig - (setno*ydepth_3D) + (y)*vyscl )

/* --------------------------------------------------------------- */
/* vals in pixels */
/* ref is front plane */
/* allows for intersecting 3D lines      */
/*  (also used for single 3D lines >:-Q  */
struct YS {
  int y1;
  int y2;
  float slope;
  QColor lnclr;
  QColor shclr;
};


#define CX( i, d ) ( cx + ((d)? xdepth_3D:0)+ \
	(int)((double)(!params->explode.isNull()?params->explode[params->offsetCol+(i)]:0) * \
                   sin((double)(slice_angle[(i)][0])) ) )

#define CY( i, d ) ( cy - ((d)? ydepth_3D: 0) - \
            (int)( (double)(!params->explode.isNull()?params->explode[params->offsetCol+(i)]:0) * \
                   cos((double)(slice_angle[(i)][0])) ) )


#define IX( i, f, d ) ( CX( (i), (d) ) + (int)( (double)rad * sin((double)(slice_angle[(i)][(f)])) ) )

#define IY( i, f, d ) ( CY( (i), (d) ) - (int)( (double)rad * cos((double)(slice_angle[(i)][(f)])) ) )


/* same as above except o is angle */
#define OX( i, o, d ) ( CX( (i),(d)) + (int)( (double)rad * sin((double)((o))) ) )

#define OY( i, o, d ) ( CY( i,d) - (int)( (double)rad * cos((double)(o)) ) )

#define TO_INT_DEG( o ) (int)rint( (double)((o)/(2.0*M_PI)*360.0) )

#define TO_INT_DEG_FLOOR( o ) (int)floor( (double)((o)/(2.0*M_PI)*360.0) )

#define TO_INT_DEG_CEIL( o ) (int)ceil( (double)((o)/(2.0*M_PI)*360.0) )

#define TO_RAD( o ) ( (o)/360.0*(2.0*M_PI) )

/* assume !> 4*PI */
#define MOD_2PI( o ) ( (o)>=(2.0*M_PI)? ((o)-(2.0*M_PI)): (((o)<0)? ((o)+(2.0*M_PI)): (o)) )

#define MOD_360( o ) ( (o)>=360? (o)-360: (o) )								/* assume !> 720 */


struct tmp_slice_t 
{ 
  int	i;					// original index
  char	hidden;				// 'behind' top [3D] pie
  float	angle;				// radian
  float	slice; 
};			// radian
extern float pie_3D_rad; 	// user requested 3D angle in radians

// degrees (radians) between angle a, and depth angle
// 1&2, so comparisons can be done.
#define RAD_DIST1( a ) ( (dist_foo1=ABS(((a>-.00001&&a<.00001)?0.00001:a)-pie_3D_rad)), ((dist_foo1>M_PI)? ABS(dist_foo1-2.0*M_PI): dist_foo1) )

#define RAD_DIST2( a ) ( (dist_foo2=ABS(((a>-.00001&&a<.00001)?0.00001:a)-pie_3D_rad)), ((dist_foo2>M_PI)? ABS(dist_foo2-2.0*M_PI): dist_foo2) )


extern float	       dist_foo1, dist_foo2; // by Werner


#define	HYP_DEPTH	( (double)((imagewidth+imageheight)/2) * ((double)params->_3d_depth)/100.0 )
#define RAD_DEPTH	( (double)params->_3d_angle*2*M_PI/360 )


struct BS {
  float y1;
  float y2;
  QColor clr;
  QColor shclr;
};

#define	NUM_YPOINTS	(sizeof(ypoints) / sizeof(float))

/* ------------------------------------------------------------------------- */
// little/no error checking  0:    ok,
//							 -ret: error no graph output
//							 ret:  error graph out
// watch out for # params and array sizes==num_points

//typedef float* floatptr;

#define EPSILON		((1.0/256.0)/2.0)
#define GET_DEC(x)	( (x) - (float)(int)(x) )

#endif // ENGINE2_H
