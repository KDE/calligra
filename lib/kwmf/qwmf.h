/* Windows Meta File Loader
 *
 * Copyright ( C ) 1998 Stefan Taferner
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or ( at your
 * option ) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABLILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details. You should have received a copy
 * of the GNU General Public License along with this program; if not, write
 * to the Free Software Foundation, Inc, 675 Mass Ave, Cambridge, MA 02139,
 * USA.
 */
#ifndef qwmf_h
#define qwmf_h

#include <qstring.h>
#include <qpainter.h>
#include <qpointarray.h>
#include <qbrush.h>
#include <qpen.h>
#include <qcolor.h>
#include <qpicture.h>
#include <qrect.h>

class QPainter;
class WmfCmd;
class WinObjHandle;
class WinObjPenHandle;
class WinObjBrushHandle;
struct WmfPlaceableHeader;
class QPaintDevice;

class QWinMetaFile
{
public:
    QWinMetaFile();
    virtual ~QWinMetaFile();

    /** Load WMF file. Returns true on success. */
    virtual bool load( const QString &fileName );
    virtual bool load( QBuffer &buffer );

    /** Paint metafile to given paint-device. Returns true on success. */
    virtual bool paint( const QPaintDevice* target );

    /** Returns true if the metafile is placeable. */
    bool isPlaceable( void ) const { return mIsPlaceable; }

    /** Returns true if the metafile is enhanced. */
    bool isEnhanced( void ) const { return mIsEnhanced; }

    /** Set single-step mode. */
    virtual void singleStep( bool ss );

    /** Returns bounding rectangle if isPlaceable()==true,
        otherwise unspecified result. */
    QRect bbox( void ) const { return mBBox; }

public: // should be protected but cannot
    /** Metafile painter methods */

    // set window origin
    void setWindowOrg( short num, short* parms );
    // set window extents
    void setWindowExt( short num, short* parms );
    // draw polygon
    void polygon( short num, short* parms );
    // draw series of lines
    void polyline( short num, short* parms );
    // set polygon fill mode
    void setPolyFillMode( short num, short* parms );
    // create a logical brush
    void createBrushIndirect( short num, short* parms );
    // create a logical pen
    void createPenIndirect( short num, short* parms );
    // set background pen color
    void setBkColor( short num, short* parms );
    // set background pen mode
    void setBkMode( short num, short* parms );
    // draw line to coord
    void lineTo( short num, short* parms );
    // move pen to coord
    void moveTo( short num, short* parms );
    // draw ellipse
    void ellipse( short num, short* parms );
    // Activate object handle
    void selectObject( short num, short* parms );
    // Free object handle
    void deleteObject( short num, short* parms );
    // Set raster operation mode
    void setRop( short num, short* parms );
    // Escape ( enhanced command set )
    void escape( short num, short* parms );

    // do nothing
    void noop( short /*num*/, short* /*parms*/ ) { }

    // Resolution of the image in dots per inch
    int dpi( void ) const { return mDpi; }

protected:
    /** Calculate header checksum */
    unsigned short calcCheckSum( WmfPlaceableHeader* );

    /** Find function in metafunc table by metafile-function.
        Returns index or -1 if not found. */
    virtual int findFunc( unsigned short aFunc ) const;

    /** Fills given parms into mPoints. */
    QPointArray* pointArray( short num, short* parms );

    /** Returns color given by the two parameters */
    QColor color( short* parm );

    /** Converts two parameters to long */
    unsigned int toDWord( short* parm );

    /** Handle win-object-handles */
    int handleIndex( void ) const;
    WinObjPenHandle* createPen( void );
    WinObjBrushHandle* createBrush( void );
    void deleteHandle( int );

protected:
    QPainter mPainter;
    QPointArray mPoints;
    bool mIsPlaceable, mIsEnhanced;
    WmfCmd* mFirstCmd;
    bool mWinding;
    QBrush mBrush;
    QPen mPen;
    QRect mBBox;
    bool mSingleStep;
    WinObjHandle** mObjHandleTab;
    int mDpi;
};

#endif /*qwmf_h*/
