/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Background (header)                                    */
/******************************************************************/

#ifndef kpbackground_h
#define kpbackground_h

#include <qcolor.h>
#include <qstring.h>
#include <krect.h>
#include <ksize.h>

#include "global.h"
#include "kpclipart.h"

#include <koStream.h>
#include <iostream.h>

class KPresenterDoc;
class QPainter;
class QPixmap;
class KPPixmapCollection;
class KPGradientCollection;

/******************************************************************/
/* Class: KPBackGround                                            */
/******************************************************************/

class KPBackGround
{
public:
	KPBackGround( KPPixmapCollection *_pixmapCollection, KPGradientCollection *_gradientCollection,
				  KPresenterDoc *_doc );
	virtual ~KPBackGround()
    {; }

	virtual void setBackType( BackType _backType )
    { backType = _backType; }
	virtual void setBackView( BackView _backView )
    { backView = _backView; }
	virtual void setBackColor1( QColor _color )
    { removeGradient(); backColor1 = _color; }
	virtual void setBackColor2( QColor _color )
    { removeGradient(); backColor2 = _color; }
	virtual void setBackColorType( BCType _bcType )
    { removeGradient(); bcType = _bcType; }
	virtual void setBackPixFilename( QString _filename );
	virtual void setBackPix( QString _filename, QString _data );
	virtual void setBackClipFilename( QString _filename );
	virtual void setPageEffect( PageEffect _pageEffect )
    { pageEffect = _pageEffect; }

	virtual void setSize( KSize _size )
    { removeGradient(); ext = _size; footerHeight = 0; }
	virtual void setSize( int _width, int _height )
    { removeGradient(); ext = KSize( _width, _height ); footerHeight = 0; }

	virtual BackType getBackType()
    { return backType; }
	virtual BackView getBackView()
    { return backView; }
	virtual QColor getBackColor1()
    { return backColor1; }
	virtual QColor getBackColor2()
    { return backColor2; }
	virtual BCType getBackColorType()
    { return bcType; }
	virtual QString getBackPixFilename()
    { return backPixFilename; }
	virtual QString getBackClipFilename()
    { return backClipFilename; }
	virtual PageEffect getPageEffect()
    { return pageEffect; }

	virtual KSize getSize()
    { return ext; }

	virtual void draw( QPainter *_painter, KPoint _offset, bool _drawBorders );

	virtual void restore();

	virtual void save( ostream& out );
	virtual void load( KOMLParser& parser, vector<KOMLAttrib>& lst );

protected:
	virtual void drawBackColor( QPainter *_painter );
	virtual void drawBackPix( QPainter *_painter );
	virtual void drawBackClip( QPainter *_painter );
	virtual void drawBorders( QPainter *_painter );
	virtual void drawHeaderFooter( QPainter *_painter, const KPoint &_offset );
	virtual void removeGradient();

	BackType backType;
	BackView backView;
	QColor backColor1;
	QColor backColor2;
	BCType bcType;
	QString backPixFilename;
	QString backClipFilename;
	PageEffect pageEffect;

	QPixmap *backPix;
	KPPixmapCollection *pixmapCollection;
	KPGradientCollection *gradientCollection;
	QPixmap *gradient;
	KPClipart backClip;

	KSize ext;
	KSize pixSize;
	QString data;

	KPresenterDoc *doc;
	int footerHeight;

};

#endif
