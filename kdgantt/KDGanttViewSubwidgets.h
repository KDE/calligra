/* -*- Mode: C++ -*-
   $Id$
   KDGantt - a multi-platform charting engine
*/

/****************************************************************************
 ** Copyright (C)  2002-2004 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KDGantt library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KDGantt licenses may use this file in
 ** accordance with the KDGantt Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.klaralvdalens-datakonsult.se/Public/products/ for
 **   information about KDGantt Commercial License Agreements.
 **
 ** Contact info@klaralvdalens-datakonsult.se if any conditions of this
 ** licensing are not clear to you.
 **
 ** As a special exception, permission is given to link this program
 ** with any edition of Qt, and distribute the resulting executable,
 ** without including the source code for Qt in the source distribution.
 **
 **********************************************************************/


#ifndef KDGANTTVIEWSUBWIDGETS_H
#define KDGANTTVIEWSUBWIDGETS_H


#include <qwidget.h>
#include <qlistview.h>
#include <qsplitter.h>
#include <qevent.h>
#include <qvaluelist.h>
#include <qcanvas.h>
#include <qwhatsthis.h>
#include <qpopupmenu.h>
#include <qtooltip.h>
#include <qtimer.h>
#include <qgroupbox.h>
#include <qvgroupbox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qbrush.h>
#include <qvbox.h>
#include <qdockwindow.h>
#include <qtimer.h>

#include "KDGanttView.h"
#include "KDGanttViewTaskLink.h"
#include "KDGanttViewTaskLinkGroup.h"
#include "KDGanttViewSummaryItem.h"
#include "KDGanttSemiSizingControl.h"
#include "KDGanttViewItemDrag.h"

#define Type_is_KDGanttGridItem 1
#define Type_is_KDGanttViewItem 2
#define Type_is_KDGanttTaskLink 3

class KDCanvasWhatsThis;
class KDToolTip;
class KDCanvasRectangle;
class KDTimeHeaderToolTip;

class KDTimeHeaderWidget : public QWidget
{
   Q_OBJECT

public:
   typedef KDGanttView::Scale Scale;
   typedef KDGanttView::YearFormat YearFormat;
   typedef KDGanttView::HourFormat HourFormat;
   struct DateTimeColor {
     QDateTime datetime;
     QDateTime end;
     QColor color;
     Scale minScaleView;
     Scale maxScaleView;
     //KDCanvasLine* canvasLine;
     KDCanvasRectangle* canvasRect;
   };
   typedef QValueList<DateTimeColor> ColumnColorList;
  typedef QValueList<DateTimeColor> IntervalColorList;
   /*
     enum Scale { Minute, Hour, Day, Week, Month, Auto };
     enum YearFormat { FourDigit, TwoDigit, TwoDigitApostrophe };
     enum HourFormat { Hour_24, Hour_12 };
   */

   KDTimeHeaderWidget (QWidget* parent,KDGanttView* gant);
  ~KDTimeHeaderWidget();

   QString getToolTipText(QPoint p);
   void zoomToFit();
   void zoom(double, bool absolute = true);
   void zoomToSelection( QDateTime startTime, QDateTime endTime);
   double zoomFactor();
   void setAutoScaleMinorTickCount( int count );
   int autoScaleMinorTickCount();
   void setHorizonStart( const QDateTime& start );
   QDateTime horizonStart() const;
   void setHorizonEnd( const QDateTime& start );
   QDateTime horizonEnd() const;

   void setMaximumScale( Scale );
   KDTimeHeaderWidget::Scale maximumScale() const;
   void setMinimumScale( Scale );
   KDTimeHeaderWidget::Scale minimumScale() const;
   KDTimeHeaderWidget::Scale scale() const;
   void setMajorScaleCount( int count );
   int majorScaleCount() const;
   void setMinorScaleCount( int count );
   int minorScaleCount() const;
   void setMinimumColumnWidth( int width );
   int minimumColumnWidth() const;
   void setYearFormat( YearFormat format );
   KDTimeHeaderWidget::YearFormat yearFormat() const;
   void setHourFormat( HourFormat format );
   KDTimeHeaderWidget::HourFormat hourFormat() const;
   void setShowMajorTicks( bool );
   bool showMajorTicks() const;
   void setShowMinorTicks( bool );
   void setScale( Scale unit);
   bool showMinorTicks() const;
   void setColumnBackgroundColor( const QDateTime& column,
				  const QColor& color,
				  Scale mini =  KDGanttView::Minute ,
				  Scale maxi =  KDGanttView::Month);
   void setIntervalBackgroundColor( const QDateTime& start,
				    const QDateTime& end,
				  const QColor& color,
				  Scale mini =  KDGanttView::Minute ,
				  Scale maxi =  KDGanttView::Month);
   bool changeBackgroundInterval( const QDateTime& oldstart,
				  const QDateTime& oldend,
				  const QDateTime& newstart,
				  const QDateTime& newend );
   bool deleteBackgroundInterval( const QDateTime& start,
				  const QDateTime& end );
   void clearBackgroundColor();
   QColor columnBackgroundColor( const QDateTime& column ) const;
   void setWeekendBackgroundColor( const QColor& color );
   QColor weekendBackgroundColor() const;
   void setWeekdayBackgroundColor( const QColor& color, int weekday );
   QColor weekdayBackgroundColor(int weekday) const;
   void setWeekendDays( int start, int end );
   void weekendDays( int& start, int& end ) const;
   void computeTicks(bool doNotComputeRealScale = false);
   void paintEvent(QPaintEvent *);
   int getCoordX(QDate);
   int getCoordX(QDateTime);
   QDateTime getDateTimeForIndex(int coordX, bool local = true );
   void setShowPopupMenu( bool show, bool showZoom, bool showScale,bool showTime,
                          bool showYear,bool showGrid, bool showPrint);
   bool registerStartTime();
   bool registerEndTime();
   bool showPopupMenu() const;
   ColumnColorList columnBackgroundColorList() const {
      return ccList;
    }
   QColor weekdayColor[8];
   void repaintMe(int left, int wid, QPainter *p = 0);

   void centerDateTime( const QDateTime& center );

public slots:
    void setSettings(int);
    void checkWidth( int );
    void addTickRight( int num = 1 );
    void addTickLeft( int num = 1 );
    void preparePopupMenu();
signals:
    void sizeChanged( int );

private:
    friend class KDTimeTableWidget;
    friend class KDGanttViewItem;
    friend class KDGanttView;
    virtual void mousePressEvent ( QMouseEvent * e );
    virtual void mouseReleaseEvent ( QMouseEvent * e );
    virtual void mouseDoubleClickEvent ( QMouseEvent * e );
    virtual void mouseMoveEvent ( QMouseEvent * e );
    double secsFromTo( QDateTime begin, QDateTime end );
    void updateTimeTable();
    void computeIntervals( int height );
    bool getColumnColor(QColor& col,int coordLow, int coordHigh);
    void moveTimeLineTo(int x);
    //void  mousePressEvent ( QMouseEvent * ) ;
    void resizeEvent ( QResizeEvent * ) ;
    QValueList<int> majorTicks;
    QValueList<QString> minorText;
    QValueList<QString> majorText;
    QDateTime myHorizonStart, myHorizonEnd, myRealEnd,myRealStart;
    QDateTime myCenterDateTime;
    void saveCenterDateTime();
    Scale myScale,myRealScale,myMaxScale,myMinScale;
    YearFormat myYearFormat;
    HourFormat myHourFormat;
    int myMinimumColumWidth;
    bool flagShowMajorTicks, flagShowMinorTicks, flagShowPopupMenu;
    bool flagShowZoom, flagShowScale ,flagShowTime ,flagShowYear;
    bool flagShowGrid ,flagShowPrint;
    bool flagStartTimeSet,flagEndTimeSet;
    QColor myWeekendBackgroundColor;
    int myWeekendDaysStart, myWeekendDaysEnd;
    ColumnColorList ccList;
    IntervalColorList icList;
    int myMinorScaleCount,myMajorScaleCount;
    int myRealMinorScaleCount,myRealMajorScaleCount;
    bool flagDoNotRecomputeAfterChange,flagDoNotRepaintAfterChange;
    QString getYear(QDate);
    QString getHour(QTime);
    QDateTime getEvenTimeDate(QDateTime ,Scale);
    void computeRealScale(QDateTime start);
    int myGridMinorWidth;
    int myMajorGridHeight;
    QPopupMenu * myPopupMenu, *scalePopupMenu, *timePopupMenu;
    QPopupMenu * yearPopupMenu, *gridPopupMenu;
    KDGanttView* myGanttView;
    double myZoomFactor;
    int myAutoScaleMinorTickcount;
    bool flagZoomToFit;
    int mySizeHint;
    int myMinimumWidth;
    int getTickTime();
    KDTimeHeaderToolTip* myToolTip;
    bool mouseDown;
    int beginMouseDown;
    int endMouseDown;
    bool autoComputeTimeLine;
    QPixmap paintPix;
};

/* KDTimeTableWidget */
class KDListView ;

class KDTimeTableWidget : public QCanvas
{
   Q_OBJECT

public:
   KDTimeTableWidget (QWidget* parent,KDGanttView* my);

    void setBlockUpdating( bool block = true );
    bool blockUpdating();
    void inc_blockUpdating();
    void dec_blockUpdating();
    void setShowTaskLinks( bool show );
    bool showTaskLinks();
    QPtrList<KDGanttViewTaskLink>taskLinks();
    void clearTaskLinks();
    void updateMyContent();
    void removeItemFromTasklinks( KDGanttViewItem * );
    void setHorBackgroundLines( int count, QBrush brush );
    int horBackgroundLines( QBrush& brush );

    void setNoInformationBrush( const QBrush& brush );
    QBrush noInformationBrush() const;

    int getCoordX( QDateTime dt );
    
signals:
   void   heightComputed( int );

public slots:
  void expandItem(QListViewItem * );
  void collapseItem(QListViewItem * );
  void highlightItem(QListViewItem * );
  void resetWidth( int );
  void checkHeight( int );
private:
   friend class KDGanttViewTaskLink;
   friend class KDTimeHeaderWidget;
   friend class KDGanttView;
   friend class KDGanttViewTaskItem;
   KDGanttView* myGanttView;

   bool taskLinksVisible;

   QPtrList<KDGanttViewTaskLink> myTaskLinkList;

   QPtrList<KDCanvasLine> verGridList;
   QPtrList<KDCanvasLine> horGridList;
   QPtrList<KDCanvasRectangle> horDenseList;
   QPtrList<KDCanvasRectangle> showNoInfoList;
   int denseLineCount;
   QBrush denseLineBrush, noInfoLineBrush;
   QPtrList<KDCanvasRectangle> columnColorList;

  int computeHeight();
  void computeVerticalGrid();
  void computeHorizontalGrid();
  void computeDenseLines();
  void computeShowNoInformation();
  void computeTaskLinks();
  void computeMinorGrid();
  void computeMajorGrid();

   void showMajorGrid();
   void showMinorGrid();
   void hideGrid();

   QPen gridPen;
   int maximumComputedGridHeight;
  int minimumHeight;
  int int_blockUpdating;
  bool flag_blockUpdating;
  int pendingHeight;
  int pendingWidth;

};

class KDLegendWidget : public KDGanttSemiSizingControl
{
   Q_OBJECT

public:
  KDLegendWidget ( QWidget* parent, KDGanttMinimizeSplitter* legendParent );
  void showMe(bool);
  bool isShown();
  void addLegendItem( KDGanttViewItem::Shape shape, const QColor& shapeColor, const QString& text );
  void clearLegend();
  void setFont( QFont );
  void drawToPainter( QPainter *p );
  void setAsDockwindow( bool dockwin );
  bool asDockwindow();
  QDockWindow* dockwindow();
  QSize legendSize();
  QSize legendSizeHint();
 private:
  QGroupBox * myLegend;
  QLabel* myLabel;
  QScrollView * scroll;
  QDockWindow* dock;
  KDGanttMinimizeSplitter* myLegendParent;
};

class KDGanttView;
class KDListView : public QListView
{
   Q_OBJECT

public:
   KDListView (QWidget* parent,KDGanttView* gv );
   KDGanttView* myGanttView;
   void drawToPainter( QPainter *p );
   void setCalendarMode( bool mode );
  bool calendarMode() { return _calendarMode; };
  QString getWhatsThisText(QPoint p);
  void setOpen ( QListViewItem * item, bool open );
  void dragEnterEvent ( QDragEnterEvent * );
  void dragMoveEvent ( QDragMoveEvent * );
  void dragLeaveEvent ( QDragLeaveEvent * );
  void dropEvent ( QDropEvent * );
  QDragObject * dragObject ();
  void startDrag ();
  void paintemptyarea ( QPainter * p, const QRect & rect ){ QListView::paintEmptyArea( p, rect );};
private slots:
  void dragItem( QListViewItem * );
 private:
   void resizeEvent ( QResizeEvent * ) ;
  void contentsMouseDoubleClickEvent ( QMouseEvent * e );
  bool _calendarMode;



};


class KDCanvasText : public QCanvasText
{
public:
    KDCanvasText( KDTimeTableWidget* canvas, void* parentItem, int type );
    int myParentType;
    void* myParentItem;
};


class KDCanvasLine : public QCanvasLine
{
public:
    KDCanvasLine( KDTimeTableWidget* canvas, void* parentItem, int type );
    int myParentType;
    void* myParentItem;
};


class KDCanvasPolygonItem: public QCanvasPolygonalItem
{
public:
    KDCanvasPolygonItem( KDTimeTableWidget* canvas, void* parentItem,
                         int type );
    int myParentType;
    void* myParentItem;
};


class KDCanvasPolygon: public QCanvasPolygon
{
public:
    KDCanvasPolygon( KDTimeTableWidget* canvas, void* parentItem, int type );
    int myParentType;
    void* myParentItem;
};


class KDCanvasEllipse: public QCanvasEllipse
{
public:
    KDCanvasEllipse( KDTimeTableWidget* canvas, void* parentItem, int type );
    int myParentType;
    void* myParentItem;
};


class KDCanvasRectangle: public QCanvasRectangle
{
public:
    KDCanvasRectangle( KDTimeTableWidget* canvas, void* parentItem, int type );
    int myParentType;
    void* myParentItem;
};


class KDCanvasToolTip;

class KDGanttCanvasView : public QCanvasView
{
    Q_OBJECT

public:
    KDGanttCanvasView(KDGanttView* sender, QCanvas* canvas = 0, QWidget* parent = 0, const char* name = 0 );
    ~KDGanttCanvasView();
    QString getToolTipText(QPoint p);
    QString getWhatsThisText(QPoint p);
    void drawToPainter ( QPainter * p );
    void resetCutPaste( KDGanttViewItem* );
    void setShowPopupMenu( bool show );
    bool showPopupMenu();
    void cutItem (  KDGanttViewItem* );
    void insertItemAsRoot( KDGanttViewItem* );
    void insertItemAsChild( KDGanttViewItem* , KDGanttViewItem* );
    void insertItemAfter( KDGanttViewItem* , KDGanttViewItem* );
protected:
    friend class KDGanttView;
    friend class KDListView;
    virtual void contentsMousePressEvent ( QMouseEvent * ) ;
    virtual void contentsMouseReleaseEvent ( QMouseEvent * );
    virtual void contentsMouseDoubleClickEvent ( QMouseEvent * );
    virtual void contentsMouseMoveEvent ( QMouseEvent * ) ;
    virtual void viewportPaintEvent ( QPaintEvent * pe );
    void resizeEvent ( QResizeEvent * ) ;
    void set_MouseTracking(bool on);
    KDGanttView* mySignalSender;
    KDGanttViewItem* currentItem, *lastClickedItem, *cuttedItem;
    KDGanttViewTaskLink* currentLink;
    int getType(QCanvasItem*);
    KDGanttViewItem* getItem(QCanvasItem*);
    KDGanttViewTaskLink* getLink(QCanvasItem*);
    KDCanvasWhatsThis* myWhatsThis;
    QPopupMenu* onItem;
    bool _showItemAddPopupMenu;
    int myMyContentsHeight;
    KDGanttViewItem *fromItem;
    bool linkItemsEnabled;
    QCanvasLine *linkLine;
    int fromArea;
    bool autoScrollEnabled;
    int getItemArea(KDGanttViewItem *item, int x);
    int getLinkType(int from, int to);

signals:
  void heightResized( int );
  void widthResized( int );
public slots:
  void set_Mouse_Tracking(bool on);
  void moveMyContent( int, int );
  void setMyContentsHeight( int );
private slots:
  void cutItem();
  void pasteItem( int );
  void newRootItem( int );
  void newChildItem( int );
  void slotScrollTimer();

private:
  KDCanvasToolTip* myToolTip;
  QTimer *myScrollTimer;
  QPoint mousePos;
};

class KDTimeHeaderToolTip :public QToolTip
{

public:
  KDTimeHeaderToolTip( QWidget *wid, KDTimeHeaderWidget* header ) : QToolTip( wid ), _wid(wid),_header (header) {

};

protected:
  virtual void maybeTip( const QPoint& p)
    {
      static bool ishidden = true;
      if (QToolTip::isGloballyEnabled () ) {
	if (ishidden) {
	  tip( QRect( p.x(),p.y(),5,5), _header->getToolTipText(p));
	}
	else
	  hide();
	ishidden = !ishidden;
      }
  }
private:
  QWidget* _wid;
  KDTimeHeaderWidget * _header;
};

class KDCanvasToolTip :public QToolTip
{

public:
  KDCanvasToolTip( QWidget *wid, KDGanttCanvasView* canview ) : QToolTip( wid ), _wid(wid),_canview (canview) {

};

protected:
  virtual void maybeTip( const QPoint& p)
    {
      static bool ishidden = true;
      if (QToolTip::isGloballyEnabled () ) {
	if (ishidden) {
	  tip( QRect( p.x()-2,p.y()-2,5,5), _canview->getToolTipText(p));
	}
	else
	  hide();
	ishidden = !ishidden;
      }
  }
private:
  QWidget* _wid;
  KDGanttCanvasView * _canview;
};

class KDCanvasWhatsThis :public QWhatsThis
{
public:
  KDCanvasWhatsThis( QWidget *wid, KDGanttCanvasView* canview ) : QWhatsThis( wid ), _wid(wid),_canview (canview) { };

protected:
  virtual QString text( const QPoint& p)
  {
    return _canview->getWhatsThisText(p) ;
  }
private:
  QWidget* _wid;
  KDGanttCanvasView * _canview;
};

class KDListViewWhatsThis :public QWhatsThis
{
public:
  KDListViewWhatsThis( QWidget *wid, KDListView* view ) : QWhatsThis( wid ), _wid(wid),_view (view) { };

protected:
  virtual QString text( const QPoint& p)
  {
    return _view->getWhatsThisText(p) ;
  }
private:
  QWidget* _wid;
  KDListView * _view;
};



#endif
