/* This file is part of the KDE project
   Copyright (C) 2002 Laurent MONTEL <lmontel@mandrakesoft.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KPRPAGE_H
#define KPRPAGE_H

#include <qwidget.h>
#include <qptrlist.h>
#include <global.h>
#include "kpbackground.h"
class KPTextView;
class KPObject;
class KPresenterDoc;
class KPBackGround;
class KPresenterView;
class KoDocumentEntry;
class KoRect;
class KoPageLayout;
class KCommand;

class KPrPage
{
public:

    // constructor - destructor
    KPrPage(KPresenterDoc *_doc);
    virtual ~KPrPage();

    KPresenterDoc * kPresenterDoc() const {return m_doc; }

    void setPageLayout(KoPageLayout pgLayout);

    QString getManualTitle();
    void insertManualTitle(const QString & title);
    QString pageTitle( const QString &_title ) const;

    void setNoteText( const QString &_text );
    QString getNoteText( );

    const QPtrList<KPObject> & objectList() const { return m_objectList;}
    void appendObject(KPObject *);
    void insertObject(KPObject *_oldObj, KPObject *_newObject);
    void takeObject(KPObject *_obj);
    void removeObject( int pos);
    void insertObject(KPObject *_obj,int pos);
    void completeLoading( bool _clean, int lastObj );



    KoRect getPageRect() const;

    QRect getZoomPageRect()const;

    void setObjectList( QPtrList<KPObject> _list ) {
        m_objectList.setAutoDelete( false ); m_objectList = _list; m_objectList.setAutoDelete( false );
    }

    unsigned int objNums() const { return m_objectList.count(); }

    void deleteObjs( bool _add=true );
    int numSelected() const;
    void pasteObjs( const QByteArray & data, int currPage );
    KCommand * replaceObjs( bool createUndoRedo, unsigned int _orastX,unsigned int _orastY,const QColor & _txtBackCol, const QColor & _otxtBackCol);

    void copyObjs();
    KPObject* getSelectedObj();
    void groupObjects();
    void ungroupObjects();
    void raiseObjs();
    void lowerObjs();
    bool getPolygonSettings( bool *_checkConcavePolygon, int *_cornersValue, int *_sharpnessValue );
    int getRndY( int _ry );
    int getRndX( int _rx );
    int getPieAngle( int pieAngle );
    int getPieLength( int pieLength );
    bool getSticky( bool s );
    PieType getPieType( PieType pieType );
    int getGYFactor( int yfactor );
    int getGXFactor( int xfactor );
    bool getGUnbalanced( bool  unbalanced );
    bool getBackUnbalanced( unsigned int );
    BCType getGType( BCType gt );
    QColor getGColor2( const QColor &g2 );
    QColor getGColor1( const QColor & g1 );
    FillType getFillType( FillType ft );
    QBrush getBrush( const QBrush &brush );
    LineEnd getLineEnd( LineEnd le );
    LineEnd getLineBegin( LineEnd lb );
    bool setLineEnd( LineEnd le );
    bool setLineBegin( LineEnd lb );

    bool setPenBrush( QPen pen, QBrush brush, LineEnd lb, LineEnd le, FillType ft, QColor g1, QColor g2,
			   BCType gt, bool unbalanced, int xfactor, int yfactor, bool sticky );

    QPen getPen( const QPen & pen );

    // insert an object
    virtual void insertObject( const QRect&, KoDocumentEntry& );

    void insertRectangle( QRect r, QPen pen, QBrush brush, FillType ft, QColor g1, QColor g2,BCType gt, int rndX, int rndY, bool unbalanced, int xfactor, int yfactor );

    void insertCircleOrEllipse( QRect r, QPen pen, QBrush brush, FillType ft, QColor g1, QColor g2, BCType gt, bool unbalanced, int xfactor, int yfactor );

    void insertPie( QRect r, QPen pen, QBrush brush, FillType ft, QColor g1, QColor g2,BCType gt, PieType pt, int _angle, int _len, LineEnd lb,LineEnd le,bool unbalanced, int xfactor, int yfactor );

    void insertTextObject( const QRect& r, const QString& text = QString::null, KPresenterView *_view = 0L );
    void insertLine( QRect r, QPen pen, LineEnd lb, LineEnd le, LineType lt );

    void insertAutoform( QRect r, QPen pen, QBrush brush, LineEnd lb, LineEnd le, FillType ft,QColor g1, QColor g2, BCType gt, const QString &fileName, bool unbalanced,int xfactor, int yfactor );

    void insertFreehand( const QPointArray &points, QRect r, QPen pen,LineEnd lb, LineEnd le );
    void insertPolyline( const QPointArray &points, QRect r, QPen pen,LineEnd lb, LineEnd le );
    void insertQuadricBezierCurve( const QPointArray &points, const QPointArray &allPoints, QRect r, QPen pen,LineEnd lb, LineEnd le );
    void insertCubicBezierCurve( const QPointArray &points, const QPointArray &allPoints, QRect r, QPen pen,LineEnd lb, LineEnd le );

    void insertPolygon( const QPointArray &points, QRect r, QPen pen, QBrush brush, FillType ft,QColor g1, QColor g2, BCType gt, bool unbalanced, int xfactor, int yfactor, bool _checkConcavePolygon, int _cornersValue, int _sharpnessValue );


    void alignObjsLeft();
    void alignObjsCenterH();
    void alignObjsRight();
    void alignObjsTop();
    void alignObjsCenterV();
    void alignObjsBottom();

    int getPenBrushFlags();
    bool setPieSettings( PieType pieType, int angle, int len );
    bool setRectSettings( int _rx, int _ry );
    bool setPolygonSettings( bool _checkConcavePolygon, int _cornersValue, int _sharpnessValue );
    bool setPenColor( QColor c, bool fill );
    bool setBrushColor( QColor c, bool fill );

    void slotRepaintVariable();
    void recalcPageNum();
    void changePicture( const QString & filename );
    void changeClipart( const QString & filename );
    void insertPicture( const QString &, int _x = 10, int _y = 10 );
    void insertClipart( const QString & );

    void enableEmbeddedParts( bool f );
    void deletePage( );

    KPBackGround *background(){return kpbackground;}

    void makeUsedPixmapList();

    void setBackColor( QColor backColor1, QColor backColor2, BCType bcType,
			    bool unbalanced, int xfactor, int yfactor );
    void setBackPixmap( const KPImageKey & key );
    bool getBackUnbalanced(  );
    void setBackClipart(  const KPClipartKey & key );
    void setBackView( BackView backView );
    void setBackType( BackType backType );

    void setPageEffect(  PageEffect pageEffect );
    void setPageTimer(  int pageTimer );
    void setPageSoundEffect(  bool soundEffect );
    void setPageSoundFileName(  const QString &fileName );
    BackType getBackType(  );
    BackView getBackView( );
    KoImageKey getBackPixKey( );
    KPClipartKey getBackClipKey(  );
    QColor getBackColor1( );
    QColor getBackColor2();
    int getBackXFactor();
    int getBackYFactor(  );
    BCType getBackColorType( );
    PageEffect getPageEffect( );
    int getPageTimer(  );
    bool getPageSoundEffect( );
    QString getPageSoundFileName();

protected:

private:
    // list of objects
    QPtrList<KPObject> m_objectList;
    KPresenterDoc *m_doc;
    KPBackGround *kpbackground;
    QString manualTitle;
    QString noteText;
};
#endif //KPRPAGE_H
