/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000 theKompany.com
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
class KivioPoint;
class KivioGuideLines;

#include <koDocument.h>
#include <koPageLayoutDia.h>

#include <qpen.h>
#include <qlist.h>
#include <qintdict.h>
#include <qarray.h>
#include <qrect.h>
#include <qpalette.h>
#include <qwidget.h>

#include "kivio_rect.h"
#include "kivio_stencil.h"

class KivioPainter;

#define BORDER_SPACE 1

#include "tkunits.h"


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

class KivioPSPrinter;

class KivioPage : public QObject
{ Q_OBJECT
public:
  KivioPage( KivioMap *_map, const char *_name );
  ~KivioPage();

  QString pageName() { return m_strName; }
  bool setPageName( const QString& name, bool init = FALSE );

  virtual QDomElement save( QDomDocument& );
  virtual bool loadXML( const QDomElement& );
  bool isLoading();

  bool isHidden() { return m_bPageHide; }
  void setHidden(bool hidden) { m_bPageHide=hidden; }

  void setMap( KivioMap* _map ) { m_pMap = _map; }

  KivioDoc* doc() { return m_pDoc; }
  KivioMap* map() { return m_pMap; }

  KivioPage *findPage( const QString & _name );

  void print( KivioPSPrinter * );
  void print( QPainter &painter, QPrinter *_printer );
  void update();
  int id() { return m_id; }
  static KivioPage* find( int _id );

  KoPageLayout paperLayout() { return m_pPageLayout; }

  void paintContent( KivioPainter& painter, const QRect& rect, bool transparent, QPoint, float );
  void printContent( KivioPainter& painter );

  bool isStencilSelected( KivioStencil * );
  void selectAllStencils();
  void unselectAllStencils();
  bool unselectStencil( KivioStencil * );
  void selectStencil( KivioStencil * );
  void selectStencils( float, float, float, float );
  bool stencilInRect( float, float, float, float, KivioStencil * );
  QList<KivioStencil> *selectedStencils() { return &m_lstSelection; }

  KivioRect getRectForAllSelectedStencils();
  KivioRect getRectForAllStencils();
  
  int generateStencilIds( int );


  KivioStencil *checkForStencil( KivioPoint *, int *, float);


  KivioLayer *curLayer() { return m_pCurLayer; }
  void setCurLayer( KivioLayer *pLayer ) { m_pCurLayer=pLayer; }

  QList<KivioLayer> *layers() { return &m_lstLayers; }
  KivioLayer *firstLayer();
  KivioLayer *nextLayer();
  KivioLayer *lastLayer();
  KivioLayer *prevLayer();
  bool removeCurrentLayer();
  void addLayer( KivioLayer * );
  void insertLayer( int, KivioLayer * );
  KivioLayer *layerAt( int );

  bool pagePropertiesDlg();

  KivioGuideLines* guideLines(){ return gLines; }

  /*
   * Stencil routines
   */
  bool addStencil( KivioStencil * );
  void alignStencils( AlignData );
  void distributeStencils( DistributeData );

public slots:
  void paperLayoutDlg();
  void deleteSelectedStencils();
  void groupSelectedStencils();
  void ungroupSelectedStencils();

  void bringToFront();
  void sendToBack();

  void copy();
  void cut();
  void paste();

signals:
  void sig_updateView( KivioPage *_page );
  void sig_updateView( KivioPage *_page, const QRect& );
  void sig_nameChanged( KivioPage* page, const QString& old_name );

protected:
  void printPage( QPainter &_painter, const QRect& page_range, const QRect& view );

  QDomElement saveLayout( QDomDocument & );
  bool loadLayout( const QDomElement & );

  QString m_strName;

  KivioMap *m_pMap;
  KivioDoc *m_pDoc;

  QList<KivioLayer> m_lstLayers;
  KivioLayer *m_pCurLayer;

  QList<KivioStencil> m_lstSelection;

  int m_id;
  bool m_bPageHide;
  static int s_id;
  static QIntDict<KivioPage>* s_mapPages;

  KoPageLayout m_pPageLayout;
  KoHeadFoot m_pHeadFoot;

  KivioGuideLines* gLines;
};

#endif
