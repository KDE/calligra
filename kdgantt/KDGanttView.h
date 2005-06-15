/* -*- Mode: C++ -*-
   $Id$
   KDChart - a multi-platform charting engine
*/

/****************************************************************************
 ** Copyright (C)  2001-2004 Klarälvdalens Datakonsult AB.  All rights reserved.
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

#ifndef KDGANTTVIEW_H
#define KDGANTTVIEW_H

#include <qptrlist.h>
#include <qwidget.h>
#include <qlistview.h>
#include <qsplitter.h>
#include <qlayout.h>
#include <qfont.h>
#include <qdom.h>
#include <qvbox.h>


#include "KDGanttViewItem.h"
#include "KDGanttViewTaskLinkGroup.h"
#include "KDGanttMinimizeSplitter.h"
#include "KDGanttViewItemDrag.h"

class KDGanttViewTaskLink;
class QPrinter;
class QIODevice;
class itemAttributeDialog;
class KDListView;
class KDGanttViewItem;
class KDGanttViewEventItem;
class KDGanttViewTaskItem;
class KDGanttViewSummaryItem;
class KDTimeTableWidget;
class KDTimeHeaderWidget;
class KDLegendWidget;
class KDGanttCanvasView;
class KDGanttViewTaskLink;
class KDGanttMinimizeSplitter;

class KDGanttView : public KDGanttMinimizeSplitter
{
    Q_OBJECT

    Q_PROPERTY( bool showLegend READ showLegend WRITE setShowLegend )
    Q_PROPERTY( bool showListView READ showListView WRITE setShowListView )
    Q_PROPERTY( bool showTaskLinks READ showTaskLinks WRITE setShowTaskLinks )
    Q_PROPERTY( bool editorEnabled READ editorEnabled WRITE setEditorEnabled )
    Q_PROPERTY( QDateTime horizonStart READ horizonStart WRITE setHorizonStart )
    Q_PROPERTY( QDateTime horizonEnd READ horizonEnd WRITE setHorizonEnd )
    Q_PROPERTY( Scale scale READ scale WRITE setScale )
    Q_PROPERTY( YearFormat yearFormat READ yearFormat WRITE setYearFormat )
    Q_PROPERTY( HourFormat hourFormat READ hourFormat WRITE setHourFormat )
    Q_PROPERTY( bool showMinorTicks READ showMinorTicks WRITE setShowMinorTicks )
    Q_PROPERTY( bool showMajorTicks READ showMajorTicks WRITE setShowMajorTicks )
    Q_PROPERTY( bool editable READ editable WRITE setEditable )
    Q_PROPERTY( QColor textColor READ textColor WRITE setTextColor )
    Q_PROPERTY( int majorScaleCount READ majorScaleCount WRITE setMajorScaleCount )
    Q_PROPERTY( int minorScaleCount READ minorScaleCount WRITE setMinorScaleCount )
    Q_PROPERTY( int autoScaleMinorTickCount READ autoScaleMinorTickCount WRITE setAutoScaleMinorTickCount )
    Q_PROPERTY( Scale maximumScale READ maximumScale WRITE setMaximumScale )
    Q_PROPERTY( Scale minimumScale READ minimumScale WRITE setMinimumScale )
    Q_PROPERTY( int minimumColumnWidth READ minimumColumnWidth WRITE setMinimumColumnWidth )
    Q_PROPERTY( int ganttMaximumWidth READ ganttMaximumWidth WRITE setGanttMaximumWidth )
    Q_PROPERTY( QColor weekendBackgroundColor READ weekendBackgroundColor WRITE setWeekendBackgroundColor )
    Q_PROPERTY( QColor ganttViewBackgroundColor READ gvBackgroundColor WRITE setGvBackgroundColor )
    Q_PROPERTY( QColor listViewBackgroundColor READ lvBackgroundColor WRITE setLvBackgroundColor )
    Q_PROPERTY( QColor timeHeaderBackgroundColor READ timeHeaderBackgroundColor WRITE setTimeHeaderBackgroundColor )
    Q_PROPERTY( QColor legendHeaderBackgroundColor READ legendHeaderBackgroundColor WRITE setLegendHeaderBackgroundColor )
    Q_PROPERTY( double zoomFactor READ zoomFactor )
    Q_PROPERTY( bool showHeaderPopupMenu READ showHeaderPopupMenu WRITE setShowHeaderPopupMenu )
    Q_PROPERTY( bool showTimeTablePopupMenu READ showTimeTablePopupMenu WRITE setShowTimeTablePopupMenu )
    Q_PROPERTY( bool headerVisible READ headerVisible WRITE setHeaderVisible )
    Q_PROPERTY( bool showLegendButton READ showLegendButton	WRITE setShowLegendButton )
    Q_PROPERTY( bool legendIsDockwindow READ legendIsDockwindow WRITE setLegendIsDockwindow )
    Q_PROPERTY( bool displayEmptyTasksAsLine READ displayEmptyTasksAsLine WRITE setDisplayEmptyTasksAsLine )
    Q_PROPERTY( QBrush noInformationBrush READ noInformationBrush WRITE setNoInformationBrush )
    Q_PROPERTY( bool dragEnabled READ dragEnabled WRITE setDragEnabled )
    Q_PROPERTY( bool dropEnabled READ dropEnabled WRITE setDropEnabled )
    Q_PROPERTY( bool calendarMode READ calendarMode WRITE setCalendarMode )

    Q_ENUMS( Scale )
    Q_ENUMS( YearFormat )
    Q_ENUMS( HourFormat )

public:
    enum Scale { Minute, Hour, Day, Week, Month, Auto };
    enum YearFormat { FourDigit, TwoDigit, TwoDigitApostrophe, NoDate };
    enum HourFormat { Hour_24, Hour_12, Hour_24_FourDigit };
    enum RepaintMode { No, Medium, Always };

    KDGanttView( QWidget* parent = 0, const char* name = 0 );
    ~KDGanttView();

    virtual void show();
    virtual bool close ( bool alsoDelete );
    void setRepaintMode( RepaintMode mode );
    void setUpdateEnabled( bool enable);
    bool getUpdateEnabled( )const;

    void setGanttMaximumWidth( int w );
    int ganttMaximumWidth() const;
    void setShowLegend( bool show );
    bool showLegend() const;
    void setLegendIsDockwindow( bool dock );
    bool legendIsDockwindow( ) const;
    QDockWindow* legendDockwindow( ) const;
    void setShowListView( bool show );
    bool showListView() const;
    void setEditorEnabled( bool enable );
    bool editorEnabled() const;
    void setListViewWidth( int );
    int listViewWidth();
    void setEditable( bool editable );
    bool editable() const;
    void setCalendarMode( bool mode );
    bool calendarMode() const;
    void setDisplaySubitemsAsGroup( bool show );
    bool displaySubitemsAsGroup() const;
    void setDisplayEmptyTasksAsLine( bool show );
    bool displayEmptyTasksAsLine() const;

    void setHorBackgroundLines( int count = 2,
				QBrush brush =
				QBrush( QColor ( 200,200,200 ),
					Qt::Dense6Pattern ));
    int horBackgroundLines( QBrush& brush );
    bool saveProject( QIODevice* );
    bool loadProject( QIODevice* );
    void print( QPrinter* printer = 0 ,
		bool printListView = true, bool printTimeLine = true,
		bool printLegend = false );
    QSize drawContents( QPainter* p = 0,
		      bool drawListView = true, bool drawTimeLine = true,
		      bool drawLegend = false );
    void setZoomFactor( double factor, bool absolute );
    double zoomFactor() const;
    void zoomToFit();
    void ensureVisible( KDGanttViewItem* );
    void center( KDGanttViewItem* );
    void centerTimeline( const QDateTime& center );
    void centerTimelineAfterShow( const QDateTime& center );
    void setTimelineToStart();
    void setTimelineToEnd();
    void addTicksLeft( int num = 1 );
    void addTicksRight( int num = 1 );
    void setShowTaskLinks( bool show );
    bool showTaskLinks() const;

    void setFont(const QFont& f);
    void setShowHeaderPopupMenu( bool show = true,
                                 bool showZoom = true,
                                 bool showScale = true,
                                 bool showTime = true,
                                 bool showYear = true,
                                 bool showGrid = true,
                                 bool showPrint = false);
    bool showHeaderPopupMenu() const;
    void setShowTimeTablePopupMenu( bool );
    bool showTimeTablePopupMenu() const;
    void setShapes( KDGanttViewItem::Type type,
                    KDGanttViewItem::Shape start,
                    KDGanttViewItem::Shape middle,
                    KDGanttViewItem::Shape end,
		    bool overwriteExisting = true  );
    bool shapes( KDGanttViewItem::Type type,
                 KDGanttViewItem::Shape& start,
                 KDGanttViewItem::Shape& middle,
                 KDGanttViewItem::Shape& end ) const;
    void setColors( KDGanttViewItem::Type type,
                    const QColor& start, const QColor& middle,
                    const QColor& end,
		    bool overwriteExisting = true );
    bool colors( KDGanttViewItem::Type type,
                 QColor& start, QColor& middle, QColor& end ) const;
    void setDefaultColor( KDGanttViewItem::Type type,
			  const QColor&,
			  bool overwriteExisting = true );
    QColor defaultColor( KDGanttViewItem::Type type ) const;
    void setHighlightColors( KDGanttViewItem::Type type,
                             const QColor& start, const QColor& middle,
                             const QColor& end,
			     bool overwriteExisting = true );
    bool highlightColors( KDGanttViewItem::Type type,
                          QColor& start, QColor& middle, QColor& end ) const;
    void setDefaultHighlightColor( KDGanttViewItem::Type type,
				   const QColor&,
				   bool overwriteExisting = true );
    QColor defaultHighlightColor( KDGanttViewItem::Type type ) const;
    void setTextColor( const QColor& color );
    QColor textColor() const;

    void setNoInformationBrush( const QBrush& brush );
    QBrush noInformationBrush() const;

    // Link-related stuff
    QPtrList<KDGanttViewTaskLink> taskLinks() const;
    QPtrList<KDGanttViewTaskLinkGroup> taskLinkGroups() const;

    // Legend-related stuff
    void addLegendItem( KDGanttViewItem::Shape shape, const QColor& shapeColor, const QString& text );
    void clearLegend();
    // Header-related stuff
    void setHorizonStart( const QDateTime& start );
    QDateTime horizonStart() const;
    void setHorizonEnd( const QDateTime& start );
    QDateTime horizonEnd() const;
    void setScale( Scale );
    Scale scale() const;
    void setMaximumScale( Scale );
    Scale maximumScale() const;
    void setMinimumScale( Scale );
    Scale minimumScale() const;
    void setAutoScaleMinorTickCount( int count );
    int autoScaleMinorTickCount() const;
    void setMajorScaleCount( int count );
    int majorScaleCount() const;
    void setMinorScaleCount( int count );
    int minorScaleCount() const;
    void setMinimumColumnWidth( int width );
    int minimumColumnWidth() const;
    void setYearFormat( YearFormat format );
    YearFormat yearFormat() const;
    void setHourFormat( HourFormat format );
    HourFormat hourFormat() const;
    void setShowMajorTicks( bool );
    bool showMajorTicks() const;
    void setShowMinorTicks( bool );
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


    void setPaletteBackgroundColor(const QColor& col);
    void setGvBackgroundColor ( const QColor & );
    void setLvBackgroundColor ( const QColor & );
    void setTimeHeaderBackgroundColor ( const QColor & );
    void setLegendHeaderBackgroundColor ( const QColor & );
    QColor gvBackgroundColor () const ;
    QColor lvBackgroundColor () const ;
    QColor timeHeaderBackgroundColor () const ;
    QColor legendHeaderBackgroundColor () const ;
    void addUserdefinedLegendHeaderWidget( QWidget * w );

    void setWeekendDays( int start, int end );
    void weekendDays( int& start, int& end ) const;

    static QPixmap getPixmap( KDGanttViewItem::Shape shape, const QColor& shapeColor,const QColor& backgroundColor, int itemSize);

    void setHeaderVisible( bool );
    bool headerVisible() const;

    void setShowLegendButton( bool show );
    bool showLegendButton() const;

    // Pass-through methods from QListView
    virtual int addColumn( const QString& label, int width = -1 );
    virtual int addColumn( const QIconSet& iconset, const QString& label,
                           int width = -1 );
    virtual void removeColumn( int index );
    KDGanttViewItem* selectedItem() const;
    void setSelected( KDGanttViewItem*, bool );
    KDGanttViewItem* firstChild() const;
    KDGanttViewItem* lastItem() const;
    int childCount() const;
    void clear();

    void setDragEnabled( bool b );
    void setDropEnabled( bool b );
    void setDragDropEnabled( bool b );
    bool dragEnabled() const;
    bool dropEnabled() const;
    bool isDragEnabled() const;
    bool isDropEnabled() const;

    virtual bool lvDropEvent ( QDropEvent *e, KDGanttViewItem*, KDGanttViewItem*);
    virtual void lvStartDrag (KDGanttViewItem*);
    virtual bool lvDragMoveEvent (QDragMoveEvent * e,KDGanttViewItem*, KDGanttViewItem*);
    virtual void lvDragEnterEvent (QDragEnterEvent * e );
    QSize sizeHint();
    KDGanttViewItem* getItemByName( const QString& name ) const;
    QDateTime getDateTimeForCoordX(int coordX, bool global = true ) const;
    KDGanttViewItem* getItemByListViewPos( const QPoint& pos ) const;
    KDGanttViewItem* getItemByGanttViewPos( const QPoint& pos ) const;
    KDGanttViewItem* getItemAt( const QPoint& pos , bool global = true ) const;

    // setting the vertical scrollbars of the listview and the timetable
    // default values: always off for the listview, always on for the timetable
    void setLvVScrollBarMode( QScrollView::ScrollBarMode );
    void setGvVScrollBarMode( QScrollView::ScrollBarMode );

    void setLinkItemsEnabled(bool on);
    bool isLinkItemsEnabled() const;
    
public slots:
    void editItem( KDGanttViewItem* );
    void zoomToSelection( const QDateTime& start,  const QDateTime&  end);

signals:
    void timeIntervallSelected( const QDateTime& start,  const QDateTime&  end);
    void timeIntervalSelected( const QDateTime& start,  const QDateTime&  end);
    void rescaling( Scale );

    // the following signals are emitted if an item is clicked in the
    // listview (inclusive) or in the ganttview
    void itemLeftClicked( KDGanttViewItem* );
    void itemMidClicked( KDGanttViewItem* );
    void itemRightClicked( KDGanttViewItem* );
    void itemDoubleClicked( KDGanttViewItem* );

    // The following signal is emitted when two items shall be linked
    void linkItems( KDGanttViewItem* from, KDGanttViewItem* to, int linkType );
    
    // the following signals are emitted if an item is clicked in the
    // listview (exlusive) or in the ganttview
    // gv... means item in ganttview clicked

    void gvCurrentChanged( KDGanttViewItem* );
    void gvItemLeftClicked( KDGanttViewItem* );
    void gvItemMidClicked( KDGanttViewItem* );
    void gvItemRightClicked( KDGanttViewItem* );
  // the point is the global position!!
    void gvMouseButtonClicked ( int button, KDGanttViewItem* item, const QPoint & pos);
    void gvItemDoubleClicked( KDGanttViewItem* );
    // the point is the global position!!
    void gvContextMenuRequested ( KDGanttViewItem * item, const QPoint & pos );

    // lv... means item in listview clicked
    void lvCurrentChanged( KDGanttViewItem* );
    void lvItemRenamed( KDGanttViewItem* , int col, const QString & text  );
    void lvMouseButtonPressed(  int button, KDGanttViewItem * item, const QPoint & pos, int c );
    void lvItemLeftClicked( KDGanttViewItem* );
    void lvItemMidClicked( KDGanttViewItem* );
    void lvItemRightClicked( KDGanttViewItem* );
    void lvContextMenuRequested ( KDGanttViewItem * item, const QPoint & pos, int col );
    void lvMouseButtonClicked ( int button, KDGanttViewItem* item, const QPoint & pos, int c );
    void lvItemDoubleClicked( KDGanttViewItem* );
    void lvSelectionChanged( KDGanttViewItem* );

    void itemConfigured( KDGanttViewItem* );

    void taskLinkLeftClicked( KDGanttViewTaskLink* );
    void taskLinkMidClicked( KDGanttViewTaskLink* );
    void taskLinkRightClicked( KDGanttViewTaskLink* );
    void taskLinkDoubleClicked( KDGanttViewTaskLink* );

    void dropped ( QDropEvent * e, KDGanttViewItem* droppedItem, KDGanttViewItem* itemBelowMouse);
private slots:
    void forceRepaint( int val = 0 );
    void slotSelectionChanged( QListViewItem* item );
    void slotCurrentChanged ( QListViewItem * item );
    void slotItemRenamed ( QListViewItem * item, int col, const QString & text  );
  void slotMouseButtonPressed (  int button, QListViewItem * item, const QPoint & pos, int c );
    void slotmouseButtonClicked ( int button, QListViewItem * item, const QPoint & pos, int c );
    void slotcontextMenuRequested ( QListViewItem * item, const QPoint & pos, int col );
    void slotdoubleClicked ( QListViewItem * item );
    void slotHeaderSizeChanged();
    void addTickRight();
    void addTickLeft();
    void enableAdding( int );
    void slot_lvDropped(QDropEvent* e, KDGanttViewItem* droppedItem, KDGanttViewItem* itemBelowMouse );
private:
    struct legendItem {
      KDGanttViewItem::Shape shape;
      QColor color;
      QString text;
    };
    bool loadXML( const QDomDocument& doc );
    QDomDocument saveXML( bool withPI = true ) const;

    static QString scaleToString( Scale scale );
    static QString yearFormatToString( YearFormat format );
    static QString hourFormatToString( HourFormat format );
    static Scale stringToScale( const QString& string );
    static YearFormat stringToYearFormat( const QString& string );
    static HourFormat stringToHourFormat( const QString& string );

    // PENDING(lutz) Review these
    friend class KDGanttCanvasView;
    friend class KDGanttViewEventItem;
    friend class KDGanttViewItem;
    friend class KDGanttViewTaskItem;
    friend class KDGanttViewSummaryItem;
    friend class KDGanttViewTaskLink;
    friend class KDGanttViewCalendarItem;
    friend class KDTimeTableWidget;
    friend class KDTimeHeaderWidget;
    friend class KDListView;
    friend class KDGanttViewTaskLinkGroup;
    friend class KDLegendWidget;

    KDListView * myListView;
    KDGanttCanvasView *myCanvasView;
    KDTimeHeaderWidget * myTimeHeader;
    KDTimeTableWidget * myTimeTable;
    KDLegendWidget * myLegend;
    itemAttributeDialog* myItemAttributeDialog;
    QVBox * leftWidget,  * rightWidget;
    QHBox * spacerLeft;
    QScrollView* myTimeHeaderScroll;
    QHBox* myTimeHeaderContainer ;
    QWidget* timeHeaderSpacerWidget;
    QWidget *spacerRight;

    bool listViewIsVisible;
    bool chartIsEditable;
    bool editorIsEnabled;
    bool _displaySubitemsAsGroup;
    bool _displayEmptyTasksAsLine;
    bool _showLegendButton;
    bool _showHeader;
    bool _enableAdding;
    bool fCenterTimeLineAfterShow;
    bool fDragEnabled;
    bool fDropEnabled;
    bool closingBlocked;
    QDateTime dtCenterTimeLineAfterShow;
    KDGanttViewItem::Shape myDefaultShape [9];
    QColor myColor[9],myColorHL[9];
    bool undefinedShape[3],undefinedColor[3],undefinedColorHL[3];
    QColor myTextColor;
    QColor myDefaultColor[3],myDefaultColorHL[3];
    QPtrList<KDGanttViewTaskLinkGroup> myTaskLinkGroupList;
    QPtrList<legendItem> *myLegendItems;
    void addTaskLinkGroup(KDGanttViewTaskLinkGroup*);
    void removeTaskLinkGroup(KDGanttViewTaskLinkGroup*);
    int getIndex( KDGanttViewItem::Type ) const;
    void notifyEditdialog( KDGanttViewItem * );
    void initDefaults();
    KDGanttViewItem* myCurrentItem;
    KDGanttMinimizeSplitter *mySplitter;
protected:
  virtual QDragObject * dragObject ();
  virtual void startDrag ();
};



#endif
