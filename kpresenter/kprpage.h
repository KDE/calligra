// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2002 Laurent MONTEL <lmontel@mandrakesoft.com>
   Copyright (C) 2004-2005 Thorsten Zachmann <zachmann@kde.org>

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
#include "global.h"
#include <koPoint.h>
#include <koRect.h>
#include <kostyle.h> // for KoStyleChangeDefMap
#include <qvariant.h>
#include <koDocumentChild.h>
#include <koPictureCollection.h>

class KMacroCommand;
class KoSavingContext;
class KURL;
class KPTextView;
class KPObject;
class KPresenterDoc;
class KPresenterView;
class KoDocumentEntry;
class KoPageLayout;
class KCommand;
class KoPointArray;
class DCOPObject;
class KPTextObject;
class KoTextObject;
class KPPixmapObject;
class KPPartObject;
class KPBackGround;

class KoXmlWriter;
class KoGenStyles;


class KPrPage
{
public:
    KPrPage(KPresenterDoc *_doc, KPrPage *masterPage = 0 );
    virtual ~KPrPage();

    virtual DCOPObject* dcopObject();

    KPresenterDoc * kPresenterDoc() const {return m_doc; }

    QString manualTitle()const;
    void insertManualTitle(const QString & title);
    QString pageTitle( const QString &_title = QString() ) const;

    void setNoteText( const QString &_text );
    QString noteText( )const;

    /**
     * set the master page to masterPage
     */
    void setMasterPage( KPrPage *_masterPage ) { m_masterPage = _masterPage; }
    /**
     * return the master page
     */
    KPrPage * masterPage() const { return m_masterPage; }


    bool isMasterPage() const;

    /**
     * @return the object of the page
     */
    const QPtrList<KPObject> & objectList() const { return m_objectList; }

    /**
     * @return object at position num
     */
    KPObject *getObject( int num );

    /**
     * appends object _obj to m_objectList
     * the name of the object is unified
     */
    void appendObject( KPObject *_obj );
    /**
     * replaces object oldObject by newObject
     */
    void replaceObject( KPObject *oldObject, KPObject *newObject );
    /**
     * take object object from m_objectList
     */
    int takeObject( KPObject *object );
    /**
     * delete selected objects from m_objectList
     */
    KCommand * deleteSelectedObjects();

    void insertObject( KPObject *object, int pos );
    void completeLoading( bool _clean, int lastObj );
    /**
     * Create a uniq name for a object.
     * if the name already exists append ' (x)'. // FIXME: not allowed by I18N
     */
    void unifyObjectName( KPObject *object );

    KoRect getPageRect() const;
    QRect getZoomPageRect()const;

    /**
     * set m_objectList to objectLlist
     */
    void setObjectList( QPtrList<KPObject> objectLlist ) {
        m_objectList.setAutoDelete( false ); m_objectList = objectLlist; m_objectList.setAutoDelete( false );
    }

    /**
     * return the number of objects in m_objectList
     */
    unsigned int objNums() const { return m_objectList.count(); }

    /**
     * The following two functions are only used in KPresenterPageIface.
     */
    int numTextObject() const;
    KPTextObject *textFrameSet( unsigned int _num ) const;

    int numSelected() const;
    void pasteObjs( const QByteArray & data, int nbCopy = 1, double angle = 0.0 ,
                    double _increaseX=0.0, double increaseY = 0.0, double moveX=0.0, double moveY=0.0);
    KCommand * replaceObjs( bool createUndoRedo, double _orastX, double _orastY,
                            const QColor & _txtBackCol, const QColor & _otxtBackCol);

    void copyObjs(QDomDocument &doc, QDomElement &presenter, QValueList<KoPictureKey> & lst) const;

    KPObject* getSelectedObj() const;
    /**
     * @return All selected objets.
     * If withoutHeaderFooter is set to true a selected header
     * or footer are not returned.
     */
    QPtrList<KPObject> getSelectedObjects( bool withoutHeaderFooter = false ) const;
    KPPixmapObject* getSelectedImage() const;

    ImageEffect getImageEffect(ImageEffect eff) const;
    KCommand * setImageEffect(ImageEffect eff, QVariant param1, QVariant param2, QVariant param3);

    /**
     * group selected objects.
     */
    void groupObjects();
    void ungroupObjects( KMacroCommand ** macro );
    void raiseObjs( bool forward );
    void lowerObjs( bool backward );
    bool getCheckConcavePolygon( bool check ) const;
    int getCornersValue( int corners ) const;
    int getSharpnessValue( int sharpness ) const;
    PictureMirrorType getPictureMirrorType( PictureMirrorType type ) const;
    int getPictureDepth( int depth ) const;
    bool getPictureSwapRGB( bool swapRGB ) const;
    bool getPictureGrayscal( bool grayscal ) const;
    int getPictureBright( int bright ) const;
    QPixmap getPicturePixmap() const;
    int getRndY( int _ry ) const;
    int getRndX( int _rx ) const;
    int getPieAngle( int pieAngle ) const;
    int getPieLength( int pieLength ) const;
    bool getSticky( bool s ) const;
    PieType getPieType( PieType pieType ) const;
    int getGYFactor( int yfactor )const;
    int getGXFactor( int xfactor )const;
    bool getGUnbalanced( bool  unbalanced ) const;
    bool getBackUnbalanced( unsigned int ) const;
    BCType getGType( BCType gt )const;
    QColor getGColor2( const QColor &g2 ) const;
    QColor getGColor1( const QColor & g1)const;
    FillType getFillType( FillType ft ) const;
    QBrush getBrush( const QBrush &brush )const;
    LineEnd getLineEnd( LineEnd le ) const;
    LineEnd getLineBegin( LineEnd lb ) const;

    bool getProtect( bool p ) const;
    bool differentProtect( bool p) const;

    bool getKeepRatio( bool p ) const;
    bool differentKeepRatio( bool p) const;

    bool getProtectContent(bool prot) const;

    KCommand* setPen( const QPen &pen, LineEnd lb, LineEnd le, int flags );
    KCommand* setBrush( const QBrush &brush, FillType ft, const  QColor& g1, const QColor &g2,
                        BCType gt, bool unbalanced, int xfactor, int yfactor, int flags );

    QPen getPen( const QPen & pen ) const;

    /**
     * Insert an object into the page
     */
    KCommand * insertObject( const QString &name, KPObject * object, const KoRect &r, bool addCommand = true );

    virtual KPPartObject* insertObject( const KoRect&, KoDocumentEntry& );

    void insertRectangle( const KoRect &r, const QPen & pen, const QBrush &brush, FillType ft,
                          const QColor &g1, const QColor & g2,BCType gt, int rndX, int rndY,
                          bool unbalanced, int xfactor, int yfactor );

    void insertCircleOrEllipse( const KoRect &r, const QPen &pen, const QBrush &brush, FillType ft,
                                const QColor &g1, const QColor &g2, BCType gt, bool unbalanced, int xfactor, int yfactor );

    void insertPie( const KoRect &r, const QPen &pen, const QBrush &brush, FillType ft,
                    const QColor &g1, const QColor &g2,BCType gt, PieType pt, int _angle, int _len,
                    LineEnd lb,LineEnd le,bool unbalanced, int xfactor, int yfactor );

    KPTextObject*  insertTextObject( const KoRect& r, const QString& text = QString::null, KPresenterView *_view = 0L );
    void insertLine( const KoRect &r, const QPen &pen, LineEnd lb, LineEnd le, LineType lt );

    void insertAutoform( const KoRect &r, const QPen &pen, const QBrush &brush, LineEnd lb, LineEnd le,
                         FillType ft,const QColor &g1, const QColor &g2, BCType gt, const QString &fileName,
                         bool unbalanced,int xfactor, int yfactor );

    void insertFreehand( const KoPointArray &points, const KoRect &r, const QPen &pen,LineEnd lb, LineEnd le );
    void insertPolyline( const KoPointArray &points, const KoRect &r, const QPen &pen,LineEnd lb, LineEnd le );
    void insertQuadricBezierCurve( const KoPointArray &points, const KoPointArray &allPoints, const KoRect &r,
                                   const QPen &pen,LineEnd lb, LineEnd le );
    void insertCubicBezierCurve( const KoPointArray &points, const KoPointArray &allPoints, const KoRect &r,
                                 const QPen &pen,LineEnd lb, LineEnd le );

    void insertPolygon( const KoPointArray &points, const KoRect &r, const QPen &pen, const QBrush &brush,
                        FillType ft,const QColor &g1, const QColor &g2, BCType gt, bool unbalanced,
                        int xfactor, int yfactor, bool _checkConcavePolygon, int _cornersValue, int _sharpnessValue );

    void insertClosedLine( const KoPointArray &points, const KoRect &r, const QPen &pen, const QBrush &brush,
                           FillType ft,const QColor &g1, const QColor &g2,
                           BCType gt, bool unbalanced, int xfactor, int yfactor, ToolEditMode _mode );

    KCommand* setPieSettings( PieType pieType, int angle, int len, int flags );
    KCommand* setRectSettings( int _rx, int _ry, int flags );
    KCommand* setPolygonSettings( bool _checkConcavePolygon, int _cornersValue, int _sharpnessValue, int flags );
    KCommand* setPictureSettings( PictureMirrorType _mirrorType, int _depth, bool _swapRGB, bool _grayscal,
                                  int _bright );

    void slotRepaintVariable();
    void recalcPageNum();
    void changePicture( const KURL & url, QWidget *parent );
    void insertPicture( const QString &, int _x = 10, int _y = 10 );
    void insertPicture( const QString &_file, const KoRect &_rect );

    void enableEmbeddedParts( bool f );

    KPBackGround *background() { return m_kpbackground; }

    void makeUsedPixmapList();

    void setBackColor( const QColor &backColor1, const QColor &backColor2, BCType bcType,
                       bool unbalanced, int xfactor, int yfactor );
    void setBackPicture( const KoPictureKey & key );
    bool getBackUnbalanced(  )const;
    void setBackView( BackView backView );
    void setBackType( BackType backType );

    void setPageEffect(  PageEffect pageEffect );
    void setPageEffectSpeed( EffectSpeed pageEffectSpeed );
    void setPageTimer(  int pageTimer );
    void setPageSoundEffect(  bool soundEffect );
    void setPageSoundFileName(  const QString &fileName );
    BackType getBackType(  ) const ;
    BackView getBackView( )const ;
    KoPictureKey getBackPictureKey( )const ;
    KoPicture getBackPicture( )const ;
    QColor getBackColor1( )const ;
    QColor getBackColor2()const ;
    int getBackXFactor()const ;
    int getBackYFactor( )const;
    BCType getBackColorType( )const;
    PageEffect getPageEffect( )const;
    EffectSpeed getPageEffectSpeed() const;
    int getPageTimer(  )const;
    bool getPageSoundEffect( )const;
    QString getPageSoundFileName()const;


    void setUseMasterBackground( bool useMasterBackground );
    bool useMasterBackground() const;

    /**
     * return the list of steps where objects appear/disappear.
     */
    QValueList<int> getEffectSteps() const;

    bool isSlideSelected() const {return  m_selectedSlides;}
    void slideSelected(bool _b){m_selectedSlides=_b;}

    void setInsPictureFile( const QString &_file ) { m_pictureFile = _file; }

    QString insPictureFile() const { return m_pictureFile; }

    void deSelectAllObj();
    void deSelectObj( KPObject *kpobject );
    QDomElement saveObjects( QDomDocument &doc, QDomElement &objects, double yoffset, KoZoomHandler* zoomHandler,
                             int saveOnlyPage ) const;

    bool oneObjectTextExist(bool forceAllTextObject = true) ;
    bool oneObjectTextSelected();
    bool isOneObjectSelected();
    bool haveASelectedPartObj();
    bool haveASelectedGroupObj();
    bool haveASelectedPixmapObj();

    KoRect getBoundingRect(const KoRect &rect) const;
    KoRect getBoundingAllObjectRect(const KoRect &rect) const;
    bool chPic( KPresenterView *_view);

    //return command when we move object
    KCommand *moveObject(KPresenterView *_view, double diffx, double diffy);
    KCommand *moveObject(KPresenterView *m_view,const KoPoint &_move,bool key);

    KCommand *rotateSelectedObjects(float _newAngle, bool addAngle=false);
    KCommand *shadowObj(ShadowDirection dir,int dist, const QColor &col);
    KCommand *stickyObj(bool _sticky, KPrPage * currentPage);

    QPtrList<KoTextObject> allTextObjects() const;
    void addTextObjects(QPtrList<KoTextObject>& lst) const;

    void repaintObj();

    KPObject * getCursor(const QPoint &pos );
    KPObject * getCursor(const KoPoint &pos );

    /**
     * Returns the object at position pos
     * @param pos the position in the document on which to look
     * @param withoutProtected if set to true only unprotecred objects are returned
     * @return the object at position pos
     */
    KPObject* getObjectAt( const KoPoint &pos, bool withoutProtected = false ) const;
    KPPixmapObject * picViewOrigHelper() const;
    void applyStyleChange( KoStyleChangeDefMap changed );

    void reactivateBgSpellChecking(bool refreshTextObj);

    bool canMoveOneObject() const;
    KCommand *alignVertical( VerticalAlignmentType _type );
    void changeTabStopValue ( double _tabStop );
    bool savePicture( KPresenterView *_view ) const;
    bool findTextObject( KPObject *obj );
    KPObject *nextTextObject(KPTextObject *obj);

    void getAllObjectSelectedList(QPtrList<KPObject> &lst,bool force = false );
    void getAllEmbeddedObjectSelected(QPtrList<KoDocumentChild> &embeddedObjects );

    void load( const QDomElement &element );
    void loadOasis( KoOasisContext & context );

    QDomElement save( QDomDocument &doc, const bool saveAsKOffice1Dot1 );

    bool saveOasisPage( KoStore *store, KoXmlWriter &xmlWriter, int posPage, KoSavingContext& context, int & indexObj, int &partIndexObj, KoXmlWriter* manifestWriter ) const;
    QString saveOasisPageStyle( KoStore *store, KoGenStyles& mainStyles ) const;
    QString saveOasisPageEffect() const;
    bool saveOasisNote( KoXmlWriter &xmlWriter ) const;
    bool saveOasisStickyPage( KoStore *store, KoXmlWriter &xmlWriter, KoSavingContext& context, int & indexObj, int &partIndexObj, KoXmlWriter* manifestWriter ) const;

    bool hasHeader() const { return m_bHasHeader; }
    bool hasFooter() const { return m_bHasFooter; }
    void setHeader( bool b );
    void setFooter( bool b );

    QString oasisNamePage( int posPage ) const;

private:
    void makeUsedPixmapListForGroupObject( KPObject *_obj );
    void completeLoadingForGroupObject( KPObject *_obj );
    bool objectNameExists( KPObject *object, QPtrList<KPObject> &list );
    void saveOasisObject( KoStore *store, KoXmlWriter &xmlWriter, KoSavingContext& context, int & indexObj, int &partIndexObj,  KoXmlWriter* manifestWriter, bool stickyObj=false ) const;


    // list of objects
    QPtrList<KPObject> m_objectList;
    KPresenterDoc *m_doc;
    KPrPage *m_masterPage;
    KPBackGround *m_kpbackground;
    QString m_manualTitle;
    QString m_noteText;
    DCOPObject *m_dcop;
    bool m_selectedSlides;

    QString m_pictureFile;
    bool m_bHasHeader, m_bHasFooter;
    bool m_useMasterBackground;

    /**
     * The page effect
     */
    PageEffect m_pageEffect;
    /**
     * The speed of the page effect.
     */
    EffectSpeed m_pageEffectSpeed;

    /**
     * true when a sound effect will be played when going to this page
     */
    // ### This could be simply !soundFileName.isEmpty()...
    bool m_soundEffect;
    /**
     * Sound played when showing this page
     */
    QString m_soundFileName;
    /**
     * TODO
     */
    int m_pageTimer;
};
#endif //KPRPAGE_H
