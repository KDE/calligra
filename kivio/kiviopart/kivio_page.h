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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef __kivio_page_h__
#define __kivio_page_h__

class KivioPage;
class KivioView;
class KivioMap;
class KivioCanvas;
class KivioDoc;
class KoDocumentEntry;

class QWidget;
class QPainter;
class QDomElement;

class KivioLayer;
class KoPoint;
class KivioGuideLines;
class DCOPObject;

#include <koDocument.h>

#include <qpen.h>
#include <qptrlist.h>
#include <qintdict.h>
#include <qmemarray.h>
#include <qrect.h>
#include <qpalette.h>
#include <qwidget.h>
#include <qvaluelist.h>
#include <koPoint.h>

#include "kivio_stencil.h"
#include <object.h>
#include <koffice_export.h>
class KivioPainter;
class KPrinter;
class KivioView;
class KoZoomHandler;
class KoRect;
class KoXmlWriter;
class KoStore;
class KoGenStyles;

#define BORDER_SPACE 1

struct AlignData
{
  enum Align { None, Left, Center, Right, Top, Bottom };
  Align v;
  Align h;
  bool centerOfPage;
};

struct DistributeData
{
  enum Distribute { None, Left, Center, Right, Top, Bottom, Spacing };
  enum Extent { Selection, Page };
  Distribute v;
  Distribute h;
  Extent extent;
};

class KIVIO_EXPORT KivioPage : public QObject
{
  Q_OBJECT
  public:
    KivioPage( KivioMap *_map, const QString &pageName,  const char *_name=0L );
    ~KivioPage();

    virtual DCOPObject* dcopObject();

    QString pageName()const { return m_strName; }
    bool setPageName( const QString& name, bool init = false );

    virtual QDomElement save( QDomDocument& );
    virtual void saveOasis(KoStore* store, KoXmlWriter* docWriter, KoGenStyles* styles);
    virtual bool loadXML( const QDomElement& );
    bool loadOasis(const QDomElement& page, KoOasisStyles& oasisStyles);
    bool isLoading();
    
    bool isHidden()const { return m_bPageHide; }
    void setHidden(bool hidden) { m_bPageHide=hidden; }

    void setMap( KivioMap* _map ) { m_pMap = _map; }

    KivioDoc* doc()const { return m_pDoc; }
    KivioMap* map()const { return m_pMap; }

    KivioPage *findPage( const QString & _name );

    void print( QPainter &painter, KPrinter *_printer );
    void update();
    int id()const { return m_id; }
    static KivioPage* find( int _id );

    KoPageLayout paperLayout()const { return m_pPageLayout; }
    void setPaperLayout(const KoPageLayout&);

    void paintContent( QPainter& painter, const QRect& rect, bool transparent, QPoint, KoZoomHandler*, bool );
    void printContent( QPainter& painter, int xdpi = 0, int ydpi = 0 );
    void printSelected( QPainter& painter, int xdpi = 0, int ydpi = 0 );

    bool isStencilSelected(Kivio::Object* object);
    void selectAllStencils();
    void unselectAllStencils();
    bool unselectStencil(Kivio::Object* object);
    void selectStencil(Kivio::Object* object);
    void selectStencils( double, double, double, double );
    QValueList<Kivio::Object*>* selectedStencils() { return &m_lstSelection; }
    bool checkForStencilTypeInSelection(Kivio::ShapeType type);
    bool checkForTextBoxesInSelection();

    KoRect getRectForAllSelectedStencils();
    KoRect getRectForAllStencils();

    int generateStencilIds( int );

    KivioStencil *checkForStencil( KoPoint *, int *, double, bool);
    Kivio::Object* checkForCollision(const KoPoint& point, int& collisionType);


    KivioLayer *curLayer()const { return m_pCurLayer; }
    void setCurLayer( KivioLayer *pLayer ) { m_pCurLayer=pLayer; }

    QPtrList<KivioLayer> *layers() { return &m_lstLayers; }
    KivioLayer *firstLayer();
    KivioLayer *nextLayer();
    KivioLayer *lastLayer();
    KivioLayer *prevLayer();
    bool removeCurrentLayer();
    void addLayer( KivioLayer * );
    void insertLayer( int, KivioLayer * );
    KivioLayer *layerAt( int );
    void takeLayer( KivioLayer *pLayer );
    KivioGuideLines* guideLines()const{ return gLines; }

    /*
    * Stencil routines
    */
    bool addStencil( Kivio::Object* );
    void alignStencils( AlignData );
    void distributeStencils( DistributeData );

    KivioConnectorTarget *connectPointToTarget( KivioConnectorPoint *, double );
    KoPoint snapToTarget( const KoPoint& p, double thresh, bool& hit );

    void setHidePage(bool _hide);
    
  public slots:
    void deleteSelectedStencils();
    void groupSelectedStencils();
    void ungroupSelectedStencils();

    void bringToFront();
    void sendToBack();

    void copy();
    void cut();
    void paste(KivioView* view);
    
    void setPaintSelected(bool paint = true);

  signals:
    void sig_updateView( KivioPage *_page );
    void sig_updateView( KivioPage *_page, const QRect& );
    void sig_nameChanged( KivioPage* page, const QString& old_name );

    void sig_PageHidden(KivioPage *_page);
    void sig_PageShown(KivioPage *_page);

    void sig_pageLayoutChanged(const KoPageLayout&);

  protected:
    void printPage( QPainter &_painter, const QRect& page_range, const QRect& view );

    QDomElement saveLayout( QDomDocument & );
    bool loadLayout( const QDomElement & );

    QString m_strName;

    KivioMap *m_pMap;
    KivioDoc *m_pDoc;

    QPtrList<KivioLayer> m_lstLayers;
    KivioLayer *m_pCurLayer;

    QValueList<Kivio::Object*> m_lstSelection;

    int m_id;
    bool m_bPageHide;
    static int s_id;
    static QIntDict<KivioPage>* s_mapPages;

    KoPageLayout m_pPageLayout;

    KivioGuideLines* gLines;
    DCOPObject* m_dcop;
};

#endif
