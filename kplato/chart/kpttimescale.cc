/* This file is part of the KDE project
   Copyright (C) 2005 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; 
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kpttimescale.h"

#include <kglobal.h>
#include <klocale.h>
#include <kcalendarsystem.h>
#include <kdebug.h>

#include <qcanvas.h>
#include <qpopupmenu.h>
#include <qpainter.h>
#include <qdrawutil.h>
#include <qsize.h>

namespace KPlato
{

KPTTimeScale::KPTTimeScale(QWidget *parent, bool bottom, bool enableScrollbar)
    : QScrollView(parent),
      m_bottom(bottom),
      m_canvasWidth(0)
{
    setResizePolicy(QScrollView::Manual);
    setVScrollBarMode(QScrollView::AlwaysOff);
    setHScrollBarMode(enableScrollbar ? QScrollView::AlwaysOn : QScrollView::AlwaysOff);
    
    m_header = new KPTTimeHeaderWidget(viewport(), this, bottom);
    m_header->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    addChild(m_header);
    m_header->setShowMajorTicks(true);
    m_header->setScale(KPTTimeHeaderWidget::Day);
    m_header->checkWidth(m_canvasWidth);

    int hei = m_header->height();
    resizeContents(m_header->width(), hei);
    resize(width(), hei);
    if (hScrollBarMode() !=  QScrollView::AlwaysOff) {
        hei += horizontalScrollBar()->height();
    }
    setMaximumHeight(hei+2); //hmmm
    
    connect(m_header, SIGNAL(sizeChanged(int)), SLOT(slotHeaderWidthChanged(int)));
    connect(m_header, SIGNAL(scaleChanged(int)), SLOT(slotUnitChanged(int)));
    connect(m_header, SIGNAL(timeFormatChanged(int)), SLOT(slotTimeFormatChanged(int)));
    
    //kdDebug()<<k_funcinfo<<"size: "<<width()<<","<<height()<<" frame: "<<frameSize().height()<<endl;

}
KPTTimeScale::~KPTTimeScale()
{
}

int KPTTimeScale::posX(const QDateTime &dt) {
    return m_header->getCoordX(dt);
}

int KPTTimeScale::fullHeight() {
    return height() + (horizontalScrollBar()->isEnabled() ? horizontalScrollBar()->height() : 0); 
}

void KPTTimeScale::setScale(KPTTimeHeaderWidget::Scale unit) {
    if (unit != m_header->scale())
        m_header->setScale(unit);
}

void KPTTimeScale::setRange(const QDateTime& start, const QDateTime& end) {
    //kdDebug()<<k_funcinfo<<endl;
    if (start == m_startTime && end == m_endTime)
        return;
    m_startTime = start;
    m_endTime = end;
    QDateTime s = start;
    QDateTime e = end;
    switch (m_header->realScale()) {
    case KPTTimeHeaderWidget::Hour:
        // Major scale is Day
        if (start.daysTo(end) < 1) {
            s = start;
            e = end.addDays(1);
        }
        break;
    case KPTTimeHeaderWidget::Day: {
        // Major scale is Week
        int startday = KGlobal::locale()->weekStartDay();
        int day = s.date().dayOfWeek();
        s = s.addDays(startday < day ? startday - day : startday - 7 - day);
        if (s.daysTo(end) < 7) {
            e = end.addDays(7);
        }
        break;
    }
    case KPTTimeHeaderWidget::Week:
        // Major scale is Month
        s = QDateTime(QDate(s.date().year(), s.date().month(), 1), QTime());
        if (s.date().month() == e.date().month() && s.date().year() == e.date().year()) {
            e = QDateTime(QDate(e.date().year(), e.date().month(),e.date().daysInMonth()), QTime());
        }
        break;
    case KPTTimeHeaderWidget::Month:
        // Major scale is Year
        if (start.date().year() == end.date().year()) {
            s = QDateTime(QDate(start.date().year(), 1, 1),QTime());
            e = end;
        }
        break;
    case KPTTimeHeaderWidget::Auto:
    case Default:
        break;
    }
    setHorizonStart(s);
    setHorizonEnd(e);
    //kdDebug()<<k_funcinfo<<m_header->realScale()<<": "<<horizonStart().toString()<<" - "<<horizonEnd().toString()<<endl;
}

void KPTTimeScale::setHorizonStart(const QDateTime& start) {
    m_header->setHorizonStart(start);
    resizeContents(m_header->width(), m_header->height());
}

QDateTime KPTTimeScale::horizonStart() const {
    return m_header->horizonStart();
}

void KPTTimeScale::setHorizonEnd(const QDateTime& end) {
    m_header->setHorizonEnd(end);
    resizeContents(m_header->width(), m_header->height());
}

QDateTime KPTTimeScale::horizonEnd() const {
    return m_header->horizonEnd();
}

void KPTTimeScale::setHourFormat(KPTTimeHeaderWidget::HourFormat format) {
    m_header->setHourFormat(format);
}

void KPTTimeScale::setShowPopupMenu(bool show, bool showZoom, bool showScale, bool showTime, bool showYear,bool showGrid, bool showPrint) {
    m_header->setShowPopupMenu(show, showZoom, showScale, showTime, showYear, showGrid, showPrint);
}

void KPTTimeScale::slotUnitChanged(int unit) {
    //kdDebug()<<k_funcinfo<<endl;
    emit unitChanged(unit);
}

void KPTTimeScale::slotTimeFormatChanged(int format) {
    //kdDebug()<<k_funcinfo<<endl;
    emit timeFormatChanged(format);
}

void KPTTimeScale::slotHeaderWidthChanged(int w) {
    //kdDebug()<<k_funcinfo<<"w="<<w<<endl;
    resizeContents(w, m_header->height());
    horizontalScrollBar()->setValue(0);
    emit headerWidthChanged(w);
}

const QValueList<int> &KPTTimeScale::majorGridValues() const {
    return m_header->majorTicks;
}

/*const QValueList<int> &KPTTimeScale::minorGridValues() const {
    return m_header->majorTicks; //FIXME
}
*/

/* ***************************************************************************
   KPTTimeHeaderWidget:: KPTTimeHeaderWidget
   This is a (modified) copy of the KDTimeHeaderWidget class from KDGanttView,
   to keep the look and feel of the KDGanttView timescale.
   The main changes are:
   - The widget can be used both at the top of the chart and at the bottom,
     the major- and minor scale is swapped accordingly.
   - It's KDE'ified, using KGlobal & friends, kdDebug, etc
   ************************************************************************* */
KPTTimeHeaderWidget::KPTTimeHeaderWidget(QWidget* parent, KPTTimeScale *view, bool minorOnTop)
    : QWidget (parent),
      m_minorOnTop(minorOnTop)
{
    m_minimumSizeHint = QSize(0,0);    
    myToolTip = new KPTTimeHeaderToolTip(this,this);
    mySizeHint = 0;
    myView = view;
    flagDoNotRecomputeAfterChange = true;
    QDateTime start = (QDateTime::currentDateTime ()).addSecs(-3600);
    setHorizonStart(start);
    setHorizonEnd( start.addSecs(3600*2));
    flagStartTimeSet = false;
    flagEndTimeSet = false;
    myCenterDateTime = QDateTime::currentDateTime ();
    setScale(KPTTimeHeaderWidget::Day);
    //setScale(KPTTimeHeaderWidget::Hour);
    myMaxScale = KPTTimeHeaderWidget::Month;
    myMinScale = KPTTimeHeaderWidget::Minute;
    myAutoScaleMinorTickcount = 100;
    setMajorScaleCount( 1 );
    setMinorScaleCount( 1);
    setMinimumColumnWidth( 5 );
    setYearFormat(KPTTimeHeaderWidget::FourDigit );
    setHourFormat( KPTTimeHeaderWidget::Hour_12 );
    myZoomFactor = 1.0;
    setWeekendBackgroundColor(QColor(220,220,220) );
    setWeekendDays( 6, 7 );
    myGridMinorWidth = 0;
    myPopupMenu = new QPopupMenu(this);
    QPopupMenu * zoomPopupMenu = new QPopupMenu(this);
    myPopupMenu->insertItem (i18n("Zoom"),zoomPopupMenu, 1);
    zoomPopupMenu->insertItem( i18n("Zoom to 100%"),this, SLOT(setSettings(int)),0 ,21,21 );
    zoomPopupMenu->insertItem( i18n("Zoom to Fit"),this, SLOT(setSettings(int)),0 ,20,20 );
    zoomPopupMenu->insertItem( i18n("Zoom In (x 2)"),this, SLOT(setSettings(int)),0 ,22,22 );
    zoomPopupMenu->insertItem( i18n("Zoom In (x 6)"),this, SLOT(setSettings(int)),0 ,24,24 );
    zoomPopupMenu->insertItem( i18n("Zoom In (x 12)"),this, SLOT(setSettings(int)),0 ,26,26 );
    zoomPopupMenu->insertItem( i18n("Zoom Out (x 1/2)"),this, SLOT(setSettings(int)),0 ,23,23 );
    zoomPopupMenu->insertItem( i18n("Zoom Out (x 1/6)"),this, SLOT(setSettings(int)),0 ,25,25 );
    zoomPopupMenu->insertItem( i18n("Zoom Out (x 1/12)"),this, SLOT(setSettings(int)),0 ,27,27 );
    scalePopupMenu = new QPopupMenu(this);
    myPopupMenu->insertItem (i18n("Scale"),scalePopupMenu, 2);
    scalePopupMenu->insertItem( i18n("Minute"),this, SLOT(slotScaleChanged(int)),0 ,1,1 );
    scalePopupMenu->insertItem( i18n("Hour"),this, SLOT(slotScaleChanged(int)),0 ,2,2 );
    scalePopupMenu->insertItem( i18n("Day"),this, SLOT(slotScaleChanged(int)),0 ,3,3 );
    scalePopupMenu->insertItem( i18n("Week"),this, SLOT(slotScaleChanged(int)),0 ,4,4 );
    scalePopupMenu->insertItem( i18n("Month"),this, SLOT(slotScaleChanged(int)),0 ,5,5 );
    //scalePopupMenu->insertItem( i18n("Auto"),this, SLOT(slotScaleChanged(int)),0 ,6,6 );
    scalePopupMenu->setCheckable ( true );
    timePopupMenu = new QPopupMenu(this);
    myPopupMenu->insertItem (i18n("Time Format"),timePopupMenu, 3);
    timePopupMenu->insertItem( i18n("24 Hour"),this, SLOT(slotTimeFormatChanged(int)),0 ,40,40 );
    timePopupMenu->insertItem( i18n("12 PM Hour"),this, SLOT(slotTimeFormatChanged(int)),0 ,41,41 );
    timePopupMenu->insertItem( i18n("24:00 Hour"),this, SLOT(slotTimeFormatChanged(int)),0 ,42,42 );
    yearPopupMenu = new QPopupMenu(this);
    myPopupMenu->insertItem (i18n("Year Format"),yearPopupMenu, 4);
    yearPopupMenu->insertItem( i18n("Four Digit"),this, SLOT(setSettings(int)),0 ,50,50 );
    yearPopupMenu->insertItem( i18n("Two Digit"),this, SLOT(setSettings(int)),0 ,51,51 );
    yearPopupMenu->insertItem( i18n("Two Digit Apostrophe"),this, SLOT(setSettings(int)),0 ,52,52 );
    yearPopupMenu->insertItem( i18n("No Date on Minute/Hour Scale"),this, SLOT(setSettings(int)),0 ,53,53 );

    gridPopupMenu = new QPopupMenu(this);
    myPopupMenu->insertItem (i18n("Grid"),gridPopupMenu,5);
    gridPopupMenu->insertItem( i18n("Show Minor Grid"),this, SLOT(setSettings(int)),0 ,10,10 );
    gridPopupMenu->insertItem( i18n("Show Major Grid"),this, SLOT(setSettings(int)),0 ,11,11 );
    gridPopupMenu->insertItem( i18n("Show No Grid"),this, SLOT(setSettings(int)),0 ,12,12 );
    myPopupMenu->insertItem( i18n("Print"),this, SLOT(setSettings(int)),0 ,30,30 );
    connect(myPopupMenu, SIGNAL (  aboutToShow () ) , this, SLOT( preparePopupMenu() )) ;
    flagZoomToFit = false;
    setShowMinorTicks( true );
    myRealEnd =  myHorizonEnd;
    myRealStart = myHorizonStart;
    autoComputeTimeLine = true;
    flagDoNotRecomputeAfterChange = false;
    flagDoNotRepaintAfterChange = false;
    setShowPopupMenu(false,false,false,false,false,false,false);
    for (int j =1;j<8;++j)
        weekdayColor[j] = Qt::white;
    myMinimumWidth = 0;
    mouseDown = false;
    beginMouseDown = 0;
    endMouseDown = 0;

    //kdDebug()<<k_funcinfo<<"horizon: "<<horizonStart().toString()<<" - "<<horizonEnd().toString()<<endl;
}

KPTTimeHeaderWidget::~KPTTimeHeaderWidget()
{
    delete myToolTip;
}
void  KPTTimeHeaderWidget::preparePopupMenu()
{
    myPopupMenu->setItemVisible ( 1, flagShowZoom  );
    myPopupMenu->setItemVisible ( 2, flagShowScale );
    myPopupMenu->setItemVisible ( 3, flagShowTime );
    myPopupMenu->setItemVisible ( 4, flagShowYear );
    myPopupMenu->setItemVisible ( 5, flagShowGrid);
    myPopupMenu->setItemVisible ( 30, flagShowPrint );
    myPopupMenu->changeItem( 1, i18n("Zoom ") + "(" +QString::number( zoomFactor(), 'f',3) +")" );
    int i = 0;
    int id;
    while ( ( id = scalePopupMenu->idAt( i++ )) >= 0 ) {
        scalePopupMenu->setItemChecked ( id, false );
    }
    scalePopupMenu->setItemChecked ( scalePopupMenu->idAt ( (int)( scale()) ), true );
    i = 0;
    while ( ( id = timePopupMenu->idAt( i++ )) >= 0 ) {
        timePopupMenu->setItemChecked ( id, false );
    }
    timePopupMenu->setItemChecked ( timePopupMenu->idAt ( (int)( hourFormat()) ), true );
    i = 0;
    while ( ( id = yearPopupMenu->idAt( i++ )) >= 0 ) {
        yearPopupMenu->setItemChecked ( id, false );
    }
    yearPopupMenu->setItemChecked ( yearPopupMenu->idAt ( (int)( yearFormat()) ), true );
    i = 0;
    while ( ( id = gridPopupMenu->idAt( i++ )) >= 0 ) {
        gridPopupMenu->setItemChecked ( id, false );
    }

    gridPopupMenu->setItemChecked ( gridPopupMenu->idAt ( 0 ), showMinorTicks() );
    gridPopupMenu->setItemChecked ( gridPopupMenu->idAt ( 1 ), showMajorTicks() );
    gridPopupMenu->setItemChecked ( gridPopupMenu->idAt ( 2 ),
                                    !(showMajorTicks() || showMinorTicks()) );


}

QString  KPTTimeHeaderWidget::getToolTipText(QPoint p)
{
    QString s = getDateTimeForIndex( p.x()).toString();
    return s;
}
void KPTTimeHeaderWidget::addTickRight( int num )
{
    int secs = ((num*getTickTime())-30);
    setHorizonEnd(getDateTimeForIndex(width()).addSecs(secs));
    //qApp->processEvents();
}

void KPTTimeHeaderWidget::addTickLeft( int num )
{
    int secs = ((num*getTickTime())-30);
    setHorizonStart(getDateTimeForIndex(0).addSecs(-secs));
    //qApp->processEvents();
}
// the time in secs of one minor grid tick
int KPTTimeHeaderWidget::getTickTime()
{
    return getDateTimeForIndex(0).secsTo(getDateTimeForIndex(myGridMinorWidth));
}


void KPTTimeHeaderWidget::checkWidth( int wid )
{
    //kdDebug()<<k_funcinfo<<"wid="<<wid<<endl;
    // we have to set the minimum width one pixel higher than the
    // viewport width of the canvas view in  order to
    // avoid that the horiz. scrollbar of the canvasview is hidden
    myMinimumWidth = wid + 1;
    if ( myMinimumWidth  > width() ||
         ( myMinimumWidth > mySizeHint &&
           myMinimumWidth < (width() - myGridMinorWidth  )) )
        computeTicks();
}

bool KPTTimeHeaderWidget::registerStartTime()
{

/*    QListViewItemIterator it( myGanttView->myListView );
    if (!flagStartTimeSet) {
        QDateTime temp , time;
        KDGanttViewItem* item;
        bool setNewTime = false;
        item = (KDGanttViewItem*)myGanttView->myListView->firstChild();
        if ( item ) {
            temp = item->startTime();
            time = temp;
            //  while ( item != 0)
            for ( ; it.current(); ++it ) {
                item = ( KDGanttViewItem* )it.current();
                if (item->isVisibleInGanttView) {
                    if ( !setNewTime )
                        temp = item->startTime();
                    switch( item->type() ) {
                    case KDGanttViewItem::Event:
                        time = ((KDGanttViewEventItem*)item)->leadTime();
                        setNewTime = true;
                        break;
                    case KDGanttViewItem::Summary:
                    case KDGanttViewItem::Task:
                        time = item->startTime();
                        setNewTime = true;
                        break;
                    default:
                        time = temp;
                    }
                    if ( time < temp) {
                        temp = time ;
                    }
                }
            }
            if ( setNewTime )
                if ( myHorizonStart != temp) {
                    myHorizonStart = temp;
                    computeTicks();
                    return true;
                }
        }
    }*/
    return false;
}


bool KPTTimeHeaderWidget::registerEndTime()
{
/*    if (!flagEndTimeSet) {
        QDateTime temp , time;
        KDGanttViewItem* item;
        bool setNewTime = false;
        item = (KDGanttViewItem*)myGanttView->myListView->firstChild();
        if ( item ) {
            temp = item->startTime();
            time = temp;
            QListViewItemIterator it( myGanttView->myListView );
            for ( ; it.current(); ++it ) {
                item = ( KDGanttViewItem* )it.current();
                if (item->isVisibleInGanttView) {
                    if ( !setNewTime )
                        temp = item->startTime();
                    switch( item->type() ) {
                    case KDGanttViewItem::Event:
                        time = ((KDGanttViewEventItem*)item)->startTime();
                        setNewTime = true;
                        break;
                    case KDGanttViewItem::Summary:
                        time = item->endTime();
                        if ( time < ((KDGanttViewSummaryItem*)item)->actualEndTime())
                            time = ((KDGanttViewSummaryItem*)item)->actualEndTime();
                        setNewTime = true;
                        break;
                    case KDGanttViewItem::Task:
                        time = item->endTime();
                        setNewTime = true;
                        break;
                    default:
                        time = temp;
                    }
                    if ( time > temp)
                        temp = time ;
                }
            }

            if ( setNewTime )
                if (myHorizonEnd != temp ) {
                    myHorizonEnd = temp;
                    computeTicks();
                    return true;
                }
        }
    }*/
    return false;
}


void KPTTimeHeaderWidget::setShowPopupMenu( bool show,
                                           bool showZoom,
                                           bool showScale,
                                           bool showTime,
                                           bool showYear,
                                           bool showGrid,
                                           bool showPrint)
{
    flagShowPopupMenu = show;
    flagShowZoom = showZoom;
    flagShowScale  = showScale;
    flagShowTime  = showTime;
    flagShowYear = showYear;
    flagShowGrid  = showGrid;
    flagShowPrint = showPrint;
}


bool KPTTimeHeaderWidget::showPopupMenu() const
{
    return flagShowPopupMenu;
}

void KPTTimeHeaderWidget::setSettings(int i)
{

    switch (i) {
    case 1:
        setScale(KPTTimeHeaderWidget::Minute );
        break;
    case 2:
        setScale(KPTTimeHeaderWidget::Hour );
        break;
    case 3:
        setScale(KPTTimeHeaderWidget::Day );
        break;
    case 4:
        setScale(KPTTimeHeaderWidget::Week );
        break;
    case 5:
        setScale(KPTTimeHeaderWidget::Month );
        break;
    case 6:
        setScale(KPTTimeHeaderWidget::Auto );
        break;
    case 10:
        setShowMinorTicks( true );
        break;
    case 11:
        setShowMajorTicks( true );{

        }
        break;
    case 12:
        setShowMajorTicks( false );
        setShowMinorTicks( false);
        break;
    case 20:
        zoomToFit();
        break;
    case 21:
        zoom(1.0);
        break;
    case 22:
        zoom(2.0,false);
        break;
    case 23:
        zoom(0.5,false);
        break;
    case 24:
        zoom(6.0,false);
        break;
    case 25:
        zoom(0.16666,false);
        break;
    case 26:
        zoom(12.0,false);
        break;
    case 27:
        zoom(0.08333,false);
        break;
    case 30:
        //myChartView->print();
        break;
    case 40:
    case 41:
    case 42:
        setHourFormat( (KPTTimeHeaderWidget::HourFormat) (i - 40) );
        break;
    case 50:
    case 51:
    case 52:
    case 53:
        setYearFormat( (KPTTimeHeaderWidget::YearFormat) ( i - 50) );
        break;

    case 60:

        break;

    case 61:

        break;

    case 62:

        break;

    case 63:

        break;

    case 64:

        break;
    }
}
void KPTTimeHeaderWidget::zoomToFit()
{
    flagZoomToFit = true;
    computeTicks();
}
double KPTTimeHeaderWidget::zoomFactor()
{
    return myZoomFactor;
}
double KPTTimeHeaderWidget::secsFromTo(const QDateTime &begin, const QDateTime &end )
{
    QDateTime temp;
    double secs, days;
    days = begin.daysTo(end);
    temp = begin.addDays((int) days);
    secs = temp.secsTo(end);
    secs += days * 86400.0;
    return secs;
}


void KPTTimeHeaderWidget::zoomToSelection( QDateTime start, QDateTime end)
{
    if (start < myHorizonStart) {
        myHorizonStart = start;
        flagStartTimeSet = true;
        //qDebug("myHorizonStart reset");
    }
    if (end > myHorizonEnd) {
        myHorizonEnd = end;
        flagEndTimeSet = true;
        //qDebug("myHorizonEnd reset ");
    }
    flagDoNotRepaintAfterChange = true;//avoid flicker
    zoom(1.0); // set to 100%
    int viewWid = myView->canvasWidth();
    int timeWid =  getCoordX(end)-getCoordX(start);
    double fac;
    fac  = ( (double)viewWid)/((double) timeWid  );
    zoom (fac);
    timeWid =  getCoordX(end)-getCoordX(start);
    int count = 0;
    int lastScaleCount = 0;
    while (timeWid >viewWid || ( ( myRealMinorScaleCount != lastScaleCount)  && timeWid*2 < viewWid ) ) {
        lastScaleCount = myRealMinorScaleCount;
        fac = (fac * (double)viewWid)/(double)timeWid;
        zoom (fac);
        timeWid =  getCoordX(end)-getCoordX(start);
        if ( count++ > 10 ) {
            //qDebug("Exiting while loop in zoomToSelection ");
            break;
        }
    }
    flagDoNotRepaintAfterChange = false;
    updateTimeTable();
    repaint();
    moveTimeLineTo((getCoordX(start)-(viewWid-timeWid)/2));
}
void KPTTimeHeaderWidget::moveTimeLineTo(int X)
{
/*    int Y = myChartView->myCanvasView->contentsY ();
    myChartView->myCanvasView->setContentsPos (X, Y );*/
}

void KPTTimeHeaderWidget::zoom(double factor, bool absolute)
{
    if ( factor < 0.000001 ) {
        //qDebug("KPTTimeHeaderWidget::zoom() : Zoom factor to low. Nothing zoomed. ");
        return;
    }
    double newZoom;
    if (absolute)
        newZoom = factor;
    else
        newZoom = myZoomFactor * factor;
    double relativeZoom;
    relativeZoom = newZoom / myZoomFactor;

    //qDebug("zooming relative %f ", relativeZoom);
    //qDebug("zooming absolute %f ", newZoom);
    int viewWid = myView->canvasWidth();
    if ( width() * relativeZoom < viewWid && ( newZoom > 1.01 || newZoom < 0.99 ) ) {
        //qDebug("KPTTimeHeaderWidget::zoom() : Zoom factor to low for current horizon. ");
        //qDebug("zooming relative %f, zooming absolute %f, viewWidth %d width %d ", relativeZoom,  newZoom, viewWid, width() );
        return;
    }
    myZoomFactor = newZoom;
    computeTicks();
}

/*!
  Sets the start of the horizon of the Gantt chart. If \a start is
  null, the horizon start is computed automatically.

  \param start the start of the horizon
  \sa horizonStart()
*/
void KPTTimeHeaderWidget::setHorizonStart( const QDateTime& start )
{
    myHorizonStart = start;
    flagStartTimeSet = true;
    computeTicks();
}


/*!
  Returns the start of the horizon of the Gantt chart.

  \return the start of the horizon of the Gantt chart
  \sa setHorizonStart()
*/
QDateTime KPTTimeHeaderWidget::horizonStart() const
{
    return myHorizonStart;
}


/*!
  Sets the end of the horizon of the Gantt chart. If \a end is
  null, the horizon end is computed automatically.

  \param end the end of the horizon
  \sa setHorizonEnd()
*/
void KPTTimeHeaderWidget::setHorizonEnd( const QDateTime& start )
{
    myHorizonEnd = start;
    flagEndTimeSet = true;
    computeTicks();

}


/*!
  Returns the end of the horizon of the Gantt chart.

  \return the end of the horizon of the Gantt chart
  \sa setHorizonEnd()
*/
QDateTime KPTTimeHeaderWidget::horizonEnd() const
{
    return myHorizonEnd;
}


/*!
  Configures the unit of the minor scale of the header. The major
  unit is computed automatically.
  Resets the zoomng factor to 1 (i.e. 100%).

  \param unit the unit of the minor scale of the header.
  \sa scale()
*/
void KPTTimeHeaderWidget::setScale(Scale unit )
{
    myScale = unit;
    myZoomFactor = 1.0;
    computeTicks();


}


/*!
  Returns the unit of the minor scale of the header.

  \return the unit of the minor scale of the header.
  \sa setScale()
*/
KPTTimeHeaderWidget::Scale KPTTimeHeaderWidget::scale() const
{
    return myScale;
}


/*!
  Sets the maximal allowed time scale of the minor scale of the header.

  \param unit the unit of the minor scale of the header.
  \sa scale()
*/
void KPTTimeHeaderWidget::setMaximumScale( Scale unit )
{
    myMaxScale = unit;
    computeTicks();
}


/*!
  Returns the maximal allowed time scale of the minor scale of the header.

  \return the unit of the minor scale of the header.
  \sa setScale()
*/
KPTTimeHeaderWidget::Scale  KPTTimeHeaderWidget::maximumScale() const
{
    return myMaxScale;
}


/*!
  Sets the minimal allowed time scale of the minor scale of the header.

  \param unit the unit of the minor scale of the header.
  \sa scale()
*/
void  KPTTimeHeaderWidget::setMinimumScale( Scale unit )
{
    myMinScale = unit;
    computeTicks();
}


/*!
  Returns the minimal allowed time scale of the minor scale of the header.

  \return the unit of the minor scale of the header.
  \sa setScale()
*/
KPTTimeHeaderWidget::Scale  KPTTimeHeaderWidget::minimumScale() const
{
    return myMinScale;
}


/*!
  Sets the minimum width a column needs to have. If the size of the
  Gantt chart and the scale would make it necessary to go below this
  limit otherwise, the chart will automatically be made less exact.

  \param width the minimum column width
  \sa minimumColumnWidth()
*/
void KPTTimeHeaderWidget::setMinimumColumnWidth( int width )
{
    myMinimumColumWidth =  width;
    computeTicks();
}


/*!
  Returns the minimum width a column needs to have.

  \return the column minimum width
  \sa setMinimumColumnWidth()
*/
int KPTTimeHeaderWidget::minimumColumnWidth() const
{
    return myMinimumColumWidth;
}


/*!
  Specifies the format in which to display years. If no years are
  shown, this method has no effect.

  \param format the year format
  \sa yearFormat(), setHourFormat(), hourFormat()
*/
void KPTTimeHeaderWidget::setYearFormat( YearFormat format )
{
    myYearFormat =  format;
    computeTicks();
}


/*!
  Returns the format in which to display years.

  \return the year format
  \sa setYearFormat(), setHourFormat(), hourFormat()
*/
KPTTimeHeaderWidget::YearFormat KPTTimeHeaderWidget::yearFormat() const
{
    return  myYearFormat;
}


/*!
  Specifies the format in which to display hours. If no hours are
  shown, this method has no effect.

  \param format the hour format
  \sa hourFormat(), setYearFormat(), yearFormat()
*/
void KPTTimeHeaderWidget::setHourFormat( HourFormat format )
{
    myHourFormat = format;
    computeTicks();
}


/*!
  Returns the format in which to display hours.

  \return the hour format
  \sa setHourFormat(), setYearFormat(), yearFormat()
*/
KPTTimeHeaderWidget::HourFormat KPTTimeHeaderWidget::hourFormat() const
{
    return myHourFormat;
}


/*!
  Specifies whether ticks should be shown on the major scale.

  \param show true in order to show ticks, false in order to hide them
  \sa showMajorTicks(), setShowMinorTicks(), showMinorTicks()
*/
void KPTTimeHeaderWidget::setShowMajorTicks( bool show )
{
    flagShowMajorTicks = show;
    if (show) {
        setShowMinorTicks(false);
    }
    updateTimeTable();
}


/*!
  Returns whether ticks are shown on the major scale.

  \return true if ticks are shown on the major scale
  \sa setShowMajorTicks(), setShowMinorTicks(), showMinorTicks()
*/
bool KPTTimeHeaderWidget::showMajorTicks() const
{
    return flagShowMajorTicks;
}


/*!
  Specifies whether ticks should be shown on the minor scale.

  \param show true in order to show ticks, false in order to hide them
  \sa showMinorTicks(), setShowMajorTicks(), showMajorTicks()
*/
void KPTTimeHeaderWidget::setShowMinorTicks( bool show )
{
    flagShowMinorTicks = show;
    if (show)
        setShowMajorTicks(false );
    //repaintMe();
    updateTimeTable();
}


/*!
  Returns whether ticks are shown on the minor scale.

  \return true if ticks are shown on the minor scale
  \sa setShowMinorTicks(), setShowMajorTicks(), showMajorTicks()
*/
bool KPTTimeHeaderWidget::showMinorTicks() const
{
    return flagShowMinorTicks;
}


/*!
  Sets the background color for the column closest to \a column.

  \param column the column to set the background color for
  \param color the background color
  \sa columnBackgroundColor(), setWeekendBackgroundColor(),
  weekendBackgroundColor()
*/
void KPTTimeHeaderWidget::setColumnBackgroundColor( const QDateTime& column,
                                                   const QColor& color,
                                                   Scale mini, Scale maxi )
{
    ColumnColorList::iterator it;
    for ( it = ccList.begin(); it != ccList.end(); ++it ) {
        if ((*it).datetime == column) {
            (*it).color = color;
            (*it).minScaleView = mini;
            (*it).maxScaleView = maxi;
            return;
        }
    }
    DateTimeColor newItem;
    newItem.datetime = column;
    newItem.color = color;
    newItem.minScaleView = mini;
    newItem.maxScaleView = maxi;
    ccList.append(newItem);
    updateTimeTable();
}

void KPTTimeHeaderWidget::computeIntervals( int height )
{

    IntervalColorList::iterator it;
    int left, right;
    for ( it = icList.begin(); it != icList.end(); ++it ) {
        if ( (*it).minScaleView <= myRealScale && (*it).maxScaleView >= myRealScale ) {
            left = getCoordX((*it).datetime);
            right = getCoordX((*it).end);
            if ( right == left )
                ++right;
            (*it).canvasRect->setPen( QPen::NoPen );
            (*it).canvasRect->setBrush( QBrush( (*it).color, SolidPattern) );
            (*it).canvasRect->setSize( right - left ,height );
            (*it).canvasRect->move( left,0 );
            (*it).canvasRect->show();
        } else {
            (*it).canvasRect->hide();
            /*
              (*it).canvasLine->setPen( QPen( (*it).color, right - left ) );
              (*it).canvasLine->setPoints( mid ,0 ,mid ,height );
              (*it).canvasLine->show();
              } else {
              (*it).canvasLine->hide();
            */
        }
    }
}
bool KPTTimeHeaderWidget::changeBackgroundInterval( const QDateTime& oldstart,
                                                   const QDateTime& oldend,
                                                   const QDateTime& newstart,
                                                   const QDateTime& newend )
{
    IntervalColorList::iterator it;
    for ( it = icList.begin(); it != icList.end(); ++it ) {
        if ((*it).datetime == oldstart && (*it).end == oldend ) {
            IntervalColorList::iterator it2;
            for ( it2 = icList.begin(); it2 != icList.end(); ++it2 ) {
                if ((*it2).datetime == newstart && (*it2).end == newend )
                    return false;
            }
            (*it).datetime = newstart;
            (*it).end = newend;
            updateTimeTable();
            return true;
        }
    }
    return false;
}
bool KPTTimeHeaderWidget::deleteBackgroundInterval( const QDateTime& start,
                                                   const QDateTime& end)
{
    IntervalColorList::iterator it;
    for ( it = icList.begin(); it != icList.end(); ++it ) {
        if ((*it).datetime == start && (*it).end == end ) {
            //delete  (*it).canvasLine;
            delete  (*it).canvasRect;
            icList.remove(it);
            updateTimeTable();
            return true;
        }
    }
    return false;
}

void KPTTimeHeaderWidget::setIntervalBackgroundColor( const QDateTime& start,
                                                     const QDateTime& end,
                                                     const QColor& color,
                                                     Scale mini ,
                                                     Scale maxi )
{
/*
    IntervalColorList::iterator it;
    for ( it = icList.begin(); it != icList.end(); ++it ) {
        if ((*it).datetime == start && (*it).end == end ) {
            (*it).color = color;
            (*it).minScaleView = mini;
            (*it).maxScaleView = maxi;
            return;
        }
    }
    DateTimeColor newItem;
    if ( start <= end ) {
        newItem.datetime = start;
        newItem.end = end;
    } else {
        newItem.datetime = end;
        newItem.end = start;
    }
    newItem.color = color;
    newItem.minScaleView = mini;
    newItem.maxScaleView = maxi;
    newItem.canvasRect = new KDCanvasRectangle(myGanttView->myTimeTable,0,Type_is_KDGanttGridItem);
    newItem.canvasRect->setZ(-19);
    icList.append(newItem);
    updateTimeTable();*/

}

void KPTTimeHeaderWidget::clearBackgroundColor()
{
/*
    IntervalColorList::iterator itic;
    for ( itic = icList.begin(); itic != icList.end(); ++itic ) {
        delete  (*itic).canvasRect;
    }
    ccList.clear();
    icList.clear();
    updateTimeTable();*/
}

QDateTime KPTTimeHeaderWidget::getDateTimeForIndex(int X, bool local )
{
/*    int coordX = X;
    if ( !local ) {
        QPoint p = QPoint ( X, 1 );
        coordX = myChartView->myTimeHeaderScroll->viewportToContents(myChartView->myTimeHeaderScroll->mapFromGlobal( p )).x();

    }
    double secs = (secsFromTo( myRealStart, myRealEnd ) * ((double)coordX))/(double)width();
    double days = secs/86400.0;
    secs = secs - ( ((int) days) *86400.0 );
    return (myRealStart.addDays ( (int) days )).addSecs( (int) secs);*/
    return QDateTime();
}

bool KPTTimeHeaderWidget::getColumnColor(QColor& col,int coordLow, int coordHigh)
{
    if (!flagShowMajorTicks && !flagShowMinorTicks)
        return false;
    QDateTime start,end;
    start = getDateTimeForIndex(coordLow);
    end = getDateTimeForIndex(coordHigh).addSecs(-1);
    Scale tempScale = myRealScale;
    if (flagShowMajorTicks)
        switch (myRealScale)
            {
            case KPTTimeHeaderWidget::Minute: 
                tempScale = KPTTimeHeaderWidget::Hour;
                break;
            case KPTTimeHeaderWidget::Hour: 
                tempScale = KPTTimeHeaderWidget::Day;
                break;
            case KPTTimeHeaderWidget::Day: 
                tempScale = KPTTimeHeaderWidget::Week;
                break;
            case KPTTimeHeaderWidget::Week: 
                tempScale = KPTTimeHeaderWidget::Month;
                break;
            case KPTTimeHeaderWidget::Month: 
                return false;  
                break;
            case KPTTimeHeaderWidget::Auto: 
                return false;  
                break;
            }
    //check defined column color
    ColumnColorList::iterator it;
    for ( it = ccList.begin(); it != ccList.end(); ++it ) {
        if ((*it).datetime  >= start && (*it).datetime  <= end) {
            if (tempScale >= (*it).minScaleView &&   tempScale <= (*it).maxScaleView    ) {
                col = (*it).color;
                return true;
            }
        }
    }

    if (tempScale > KPTTimeHeaderWidget::Day) return false;

    start = getDateTimeForIndex((coordLow+coordHigh)/2);
    int day = start.date().dayOfWeek ();
    //checkweekdaycolor
    if (weekdayColor[day] != Qt::white) {
        col = weekdayColor[day];
        return true;
    }
    //checkweekendcolor
    int endday = myWeekendDaysEnd;
    col = myWeekendBackgroundColor;
    if (myWeekendDaysStart > myWeekendDaysEnd)
        endday +=7;
    if (day >= myWeekendDaysStart && day <= endday) {
        return true;
    } else {
        if (day+7 >= myWeekendDaysStart && day+7 <= endday) {
            return true;
        }
    }
    return false;
}

/*!
  Returns the background color for the column closes to \a column.

  \param column the column to query the background color for
  \return the background color of the specified column
  \sa setColumnBackgroundColor(), setWeekendBackgroundColor(),
  weekendBackgroundColor()
*/
QColor KPTTimeHeaderWidget::columnBackgroundColor( const QDateTime& column ) const
{
    QColor c;
    c = white;
    ColumnColorList::const_iterator ite;
    for ( ite = ccList.begin(); ite != ccList.end(); ++ite ) {
        if ((*ite).datetime == column) {
            c = (*ite).color;
        }
    }
    return c;
}


/*!
  Specifies the background color for weekend days. If no individual
  days are visible on the Gantt chart, this method has no visible
  effect.

  \param color the background color to use for weekend days.
  \sa weekendBackgroundColor(), setWeekendDays(), weekendDays()
*/
void KPTTimeHeaderWidget::setWeekendBackgroundColor( const QColor& color )
{
    myWeekendBackgroundColor = color ;
    updateTimeTable();
}


/*!
  Returns the background color for weekend days.

  \return the background color for weekend days
  \sa setWeekendBackgroundColor(), setWeekendDays(), weekendDays()
*/
QColor KPTTimeHeaderWidget::weekendBackgroundColor() const
{
    return myWeekendBackgroundColor;
}

/*!
  Specifies the background color for week days. If no individual
  days are visible on the Gantt chart, this method has no visible
  effect. The days are specified as an interval of integer values
  where 1 means Monday and 7 means Sunday.

  \param color the background color to use for weekend days.
  \param weekday the day of the week (Monday = 1, Sunday = 7)
  \sa weekendBackgroundColor(), setWeekendDays(), weekendDays()
*/
void KPTTimeHeaderWidget::setWeekdayBackgroundColor( const QColor& color, int  weekday )
{
    weekdayColor[weekday] = color;
    updateTimeTable();
}


/*!
  Returns the background color for weekday days.

  \param the day of the week (Monday = 1, Sunday = 7)
  \return the background color for weekend days
  \sa setWeekendBackgroundColor(), setWeekendDays(), weekendDays()
*/
QColor KPTTimeHeaderWidget::weekdayBackgroundColor(int weekday) const
{
    return weekdayColor[weekday];
}


/*!
  Defines which days are considered weekends. The days are specified
  as an interval of integer values where 1 means Monday and 7 means
  Sunday. In order to define a weekend from Sunday to Monday, specify
  (7,1).

  \param start the first day of the weekend
  \param end the last day of the weekend
  \sa weekendDays(), setWeekendBackgroundColor(), weekendBackgroundColor()
*/
void KPTTimeHeaderWidget::setWeekendDays( int start, int end )
{
    myWeekendDaysStart = start;
    myWeekendDaysEnd = end;
    updateTimeTable();
}


/*!
  Returns which days are considered weekends.

  \param start in this parameter, the first day of the weekend is returned
  \param end in this parameter, the end day of the weekend is returned
  \sa setWeekendDays(), setWeekendBackgroundColor(), weekendBackgroundColor()
*/
void KPTTimeHeaderWidget::weekendDays( int& start, int& end ) const
{
    start = myWeekendDaysStart;
    end = myWeekendDaysEnd ;
}



/*!
  Sets the number of ticks in the major scale.

  \param count the number of ticks in the major scale
  \sa majorScaleCount(), setMinorScaleCount(), minorScaleCount()
*/
void KPTTimeHeaderWidget::setMajorScaleCount( int count )
{
    myMajorScaleCount=count;
    computeTicks();
}


/*!
  Returns the number of ticks per unit in the major scale.

  \return the number of ticks in the major scale
  \sa setMajorScaleCount(), setMinorScaleCount(), minorScaleCount()
*/
int KPTTimeHeaderWidget::majorScaleCount() const
{
    return myMajorScaleCount;
}


/*!
  Sets the number of ticks in the minor scale.

  \param count the number of ticks in the minor scale
  \sa minorScaleCount, setMajorScaleCount, majorScaleCount()
*/
void KPTTimeHeaderWidget::setMinorScaleCount( int count )
{
    myMinorScaleCount = count;
    computeTicks();
}


/*!
  Returns the number of ticks per unit in the minor scale.

  \return the number of ticks in the minor scale
  \sa setMinorScaleCount(), setMajorScaleCount(), majorScaleCount()
*/
int KPTTimeHeaderWidget::minorScaleCount() const
{
    return myMinorScaleCount ;

}


void KPTTimeHeaderWidget::resizeEvent ( QResizeEvent *e)
{
    //kdDebug()<<k_funcinfo<<"Size: "<<e->size().width()<<"x"<<e->size().height()<<endl;
    paintPix.resize(800, QMAX(height(), m_minimumSizeHint.height()));
    setMinimumSize(m_minimumSizeHint);
}


void KPTTimeHeaderWidget::updateTimeTable()
{
    //qDebug("KPTTimeHeaderWidget::updateTimeTable() ");
    if (flagDoNotRecomputeAfterChange) return;
    // setting the scrolling steps
    int scrollLineStep = myGridMinorWidth;
    if (showMajorTicks()) {
        QValueList<int>::iterator intIt = majorTicks.begin();
        scrollLineStep = 5 * myGridMinorWidth;
        if (intIt != majorTicks.end()) {
            int left = *intIt;
            ++intIt;
            if (intIt != majorTicks.end()) {
                scrollLineStep = *intIt-left;
            }
        }
    }
//    myChartView->myCanvasView->horizontalScrollBar()->setLineStep(scrollLineStep);
//    myChartView->myTimeTable->maximumComputedGridHeight = 0;
//    myChartView->myTimeTable->updateMyContent();
}


void KPTTimeHeaderWidget::setAutoScaleMinorTickCount( int count )
{
    myAutoScaleMinorTickcount = count;
    computeTicks();

}


int KPTTimeHeaderWidget::autoScaleMinorTickCount()
{
    return myAutoScaleMinorTickcount;
}

void KPTTimeHeaderWidget::repaintMajor(int left, int offsetLeft, int offsetHeight, int hei, int paintwid, QPainter* p)
{
    QColorGroup qcg =QColorGroup( white, black,white, darkGray,black,gray,gray) ;
    QValueList<QString>::iterator it;
    QValueList<int>::iterator intIt = majorTicks.begin();
    for ( it =  majorText.begin(); it !=  majorText.end(); ++it ) {
        int xCoord  = (*intIt++);
        if (((*intIt)>= left && xCoord <= left+paintwid)) {
            qDrawShadeLine ( p,xCoord-offsetLeft ,offsetHeight+hei+1, xCoord-offsetLeft, offsetHeight, qcg, true, 1, 1 );
            p->drawText(xCoord+4-offsetLeft,offsetHeight+hei-4,(*it));
        }
    }
}
void KPTTimeHeaderWidget::repaintMinor(int left, int offsetLeft, int offsetHeight, int hei, int paintwid, QPainter* p)
{
    QColorGroup qcg =QColorGroup( white, black,white, darkGray,black,gray,gray) ;
    int wid1 = myGridMinorWidth;
    int i = 0;
    QValueList<QString>::iterator it = minorText.begin();
    for (; it !=  minorText.end(); ++it ) {
        if (i*wid1 >= left-wid1 && i*wid1 <= left+paintwid) {
            qDrawShadeLine ( p,i*wid1-offsetLeft ,offsetHeight, i*wid1-offsetLeft, offsetHeight+hei, qcg, true, 1, 1 );
            p->drawText(i*wid1+1-offsetLeft,offsetHeight+1,wid1-1,hei, Qt::AlignCenter,(*it));
        }
        ++i;
    }
}
void KPTTimeHeaderWidget::repaintMe(int left,int paintwid, QPainter* painter)
{
    //kdDebug()<<k_funcinfo<<"left,wid="<<left<<","<<paintwid<<" size: "<<width()<<"x"<<height()<<endl;
    if (flagDoNotRecomputeAfterChange) return;
    QColorGroup qcg =QColorGroup( white, black,white, darkGray,black,gray,gray) ;
    QPainter* p;
    int offsetLeft = 0;
    if ( paintwid > paintPix.width()-100 )
        paintPix.resize( paintwid+100, height () );
    if ( painter )
        p = painter;
    else {
        p = new QPainter( &paintPix );
        offsetLeft = left-50;
    }
    if ( mouseDown ) {
        p->fillRect( left-offsetLeft, 0, paintwid, height(), QBrush(paletteBackgroundColor()) );
        int start ;
        int wid;
        if ( beginMouseDown < endMouseDown ) {
            start = beginMouseDown ;
            wid = endMouseDown - beginMouseDown ;
        } else {
            start = endMouseDown ;
            wid = -endMouseDown + beginMouseDown ;
        }
        p->fillRect( start-offsetLeft, 0, wid, height(), QBrush(paletteBackgroundColor().dark()) );
    } else {
        if (! painter )
            p->fillRect( left-offsetLeft, 0, paintwid, height(), QBrush(paletteBackgroundColor()) );
    }
    p->setPen(QColor(40,40,40));
    QFont tempFont = p->font();
    tempFont.setWeight(63);
    p->setFont(tempFont);
    int hei1 = myMajorGridHeight,
        hei2 = height(),
        wid1 = myGridMinorWidth;
    int lwid = 1;

    if (m_minorOnTop) {
        repaintMajor(left, offsetLeft, hei2-hei1, hei1, paintwid, p);
        qDrawShadeLine (p, left-offsetLeft, hei2-hei1, left+paintwid-offsetLeft, hei2-hei1, qcg, true, lwid, 1 );
        repaintMinor(left, offsetLeft, 0, hei2-hei1, paintwid, p);
    } else {
        repaintMajor(left, offsetLeft, 0, hei1, paintwid, p);
        qDrawShadeLine (p, left-offsetLeft, hei1, left+paintwid-offsetLeft, hei1, qcg, true, lwid, 1 );
        repaintMinor(left, offsetLeft, hei1, hei2-hei1, paintwid, p);
    }
    p->setPen(black);
    p->drawLine(left-offsetLeft,hei1,left+paintwid-offsetLeft,hei1);
    qDrawShadeLine ( p,left-offsetLeft  ,hei2-1, left+paintwid-offsetLeft, hei2-1, qcg, true, lwid, 1 );
    p->drawLine(left-offsetLeft,hei2-1,left+paintwid-offsetLeft,hei2-1);
    if ( !painter ) {
        p->end();
        delete p;
        bitBlt ( this, left, 0, &paintPix, 50, 0, paintwid, height() );
    }
    //kdDebug()<<k_funcinfo<<"size: "<<width()<<"x"<<height()<<endl;
    //kdDebug()<<k_funcinfo<<"View size: "<<myView->width()<<"x"<<myView->height()<<endl;
    //kdDebug()<<k_funcinfo<<"Viewport size: "<<myView->viewport()->width()<<"x"<<myView->viewport()->height()<<endl;
}

// cuts the secs in the DateTime if scale is Minute ,
// the minutes and secs if scale is Hour and so on

QDateTime KPTTimeHeaderWidget::getEvenTimeDate(QDateTime tempdatetime ,Scale sc)
{
    QDate tempdate;
    int min, hour;
    int tempMinorScaleCount = myRealMinorScaleCount;
    switch (sc)
        {
        case KPTTimeHeaderWidget::Month:
            tempdate = tempdatetime.date();
            while (tempdate.day ()!= 1 )
                tempdate = tempdate.addDays(-1);
            //while (tempdate.month ()!= 1 )
            //tempdate = tempdate.addMonths(-1);
            tempdatetime = QDateTime (tempdate, QTime (0,0));
            break;
        case KPTTimeHeaderWidget::Week:
            tempdate = tempdatetime.date();
            while (tempdate.dayOfWeek ()!= KGlobal::locale()->weekStartDay())
                tempdate = tempdate.addDays(-1);
            //tempdate = tempdate.addDays(-7);
            tempdatetime = QDateTime (tempdate, QTime (0,0));
            break;
        case KPTTimeHeaderWidget::Day:
            tempdatetime = QDateTime (tempdatetime.date(), QTime ( 0,0 ) );
            break;
        case KPTTimeHeaderWidget::Hour:
            hour = tempdatetime.time().hour();
            while (24%tempMinorScaleCount > 0 && 24%tempMinorScaleCount < 24)
                ++tempMinorScaleCount;
            hour = ( hour /tempMinorScaleCount)*tempMinorScaleCount;
            tempdatetime = QDateTime (tempdatetime.date(), QTime (hour, 0 ));
            break;
        case KPTTimeHeaderWidget::Minute:
            min = tempdatetime.time().minute();
            while (60%tempMinorScaleCount > 0 && 60%tempMinorScaleCount < 60)
                ++tempMinorScaleCount;
            //qDebug("myMinorScaleCount %d %d %d",myMinorScaleCount, myRealMinorScaleCount, tempMinorScaleCount);
            min = (min /tempMinorScaleCount)*tempMinorScaleCount;
            tempdatetime = QDateTime (tempdatetime.date(), QTime (tempdatetime.time().hour(),min ));

            break;
        case KPTTimeHeaderWidget::Auto:
            break;
        }
    return tempdatetime;
}


void KPTTimeHeaderWidget::computeRealScale(QDateTime start)
{

    if (myScale ==KPTTimeHeaderWidget::Auto) {
        //qDebug("Autoscale ");
        //double secsPerMinor = (((double)start.daysTo(myHorizonEnd))* 86400.00)/((double)myAutoScaleMinorTickcount);
        double secsPerMinor = (((double)start.secsTo(myHorizonEnd)))/((double)myAutoScaleMinorTickcount);
        secsPerMinor /= myZoomFactor;
        if (secsPerMinor <= 1800) {
            myRealScale =  KPTTimeHeaderWidget::Minute;
            myRealMinorScaleCount = (int) secsPerMinor/60;
        } else {
            if (secsPerMinor <= 12*3600) {
                myRealScale =  KPTTimeHeaderWidget::Hour;
                myRealMinorScaleCount = (int)  secsPerMinor/3600;
            } else {
                if (secsPerMinor <= 24*3600*3) {
                    myRealScale =  KPTTimeHeaderWidget::Day;
                    myRealMinorScaleCount = (int)  secsPerMinor/(3600*24);
                } else {
                    if (secsPerMinor <= 24*3600*14) {
                        myRealScale =  KPTTimeHeaderWidget::Week;
                        myRealMinorScaleCount =  (int) secsPerMinor/(3600*24*7);
                    } else {
                        myRealScale =  KPTTimeHeaderWidget::Month;
                        myRealMinorScaleCount =  (int) secsPerMinor/(3600*24*30);

                    }
                }
            }
        }
        if(myRealMinorScaleCount == 0)
            myRealMinorScaleCount = 1;
        myRealMajorScaleCount = 1;
    }
    else {
        //qDebug("Fixed scale ");
        myRealScale = myScale;
        if (myRealScale > myMaxScale)
            myRealScale = myMaxScale;
        if (myRealScale < myMinScale)
            myRealScale = myMinScale;
        myRealMinorScaleCount = (int) ( ((double)myMinorScaleCount) /myZoomFactor );
        double tempZoom = myZoomFactor;
        myRealMajorScaleCount =  myMajorScaleCount;
        while (myRealMinorScaleCount == 0) {
            if (myRealScale  == myMinScale) {
                myRealMinorScaleCount = 1;
                break;
            }
            switch (myRealScale)
                {
                case KPTTimeHeaderWidget::Minute:
                    myRealMinorScaleCount = 1;
                    return;
                    break;
                case KPTTimeHeaderWidget::Hour:
                    myRealScale = KPTTimeHeaderWidget::Minute;
                    tempZoom = tempZoom/60;
                    break;
                case KPTTimeHeaderWidget::Day:
                    myRealScale = KPTTimeHeaderWidget::Hour;
                    tempZoom = tempZoom/24;
                    break;
                case KPTTimeHeaderWidget::Week:
                    myRealScale = KPTTimeHeaderWidget::Day;
                    tempZoom = tempZoom/7;
                    break;
                case KPTTimeHeaderWidget::Month:
                    myRealScale =  KPTTimeHeaderWidget::Week ;
                    tempZoom = tempZoom*7/30;
                    break;
                case KPTTimeHeaderWidget::Auto:
                    break;
                }
            myRealMinorScaleCount =  (int) ( myMinorScaleCount /tempZoom );
        }
    }
}


void KPTTimeHeaderWidget::computeTicks(bool doNotComputeRealScale)
{
    if (flagDoNotRecomputeAfterChange) return;
//    bool block = myChartView->myTimeTable->blockUpdating();
//    myChartView->myTimeTable->setBlockUpdating( true );
    
    //kdDebug()<<k_funcinfo<<"size: "<<width()<<"x"<<height()<<endl;

    majorTicks.clear();
    minorText.clear();
    majorText.clear();
    if ( !doNotComputeRealScale )
        saveCenterDateTime();
    if (!doNotComputeRealScale)
        computeRealScale(myHorizonStart);
    myRealStart = getEvenTimeDate(myHorizonStart ,myRealScale);
    if (!doNotComputeRealScale)
        computeRealScale(myRealStart);
    int tempMinorScaleCount = myRealMinorScaleCount,
        tempMajorScaleCount = myRealMajorScaleCount;
    int minorItems,minorPerMajor = 1;
    minorItems = (int)  (secsFromTo( myRealStart, myHorizonEnd)/60.0);
    //qDebug("tempMinorScaleCount %d ", tempMinorScaleCount);
    QPainter p(this);
    int Width, Height;
    QString testTextMinor,testTextMajor, tempStr;
    QRect itemRectMinor, itemRectMajor;
    QDate tempDate = myRealStart.date();
    myRealEnd = myRealStart;
    // preparing the testtext for the differennt scales
    switch (myRealScale)
        {
            // the x in testTextMajor is added to reserve a little bit more space
        case KPTTimeHeaderWidget::Minute:
            testTextMinor = "60";
            if (myHourFormat == KPTTimeHeaderWidget::Hour_12)
                testTextMajor = "Mon Aug 30, 12 AMx";
            else
                testTextMajor = "Mon Aug 30, 24:00x";
            minorPerMajor = 6000;
            break;
        case KPTTimeHeaderWidget::Hour:
            minorItems = minorItems/60;
            if (myHourFormat == KPTTimeHeaderWidget::Hour_24)
                testTextMinor = "24x";
            else
                testTextMinor = "12 AM";
            testTextMajor = "Mon Aug 30, x";
            if ( yearFormat() != KPTTimeHeaderWidget::NoDate )
                testTextMajor += getYear(QDate::currentDate());
            minorPerMajor = 2400;
            break;
        case KPTTimeHeaderWidget::Day:
            minorItems = minorItems/(60*24);
            testTextMinor = "88";
            testTextMajor = "Aug 30, x"+getYear(QDate::currentDate());
            minorPerMajor = 700;
            break;
        case KPTTimeHeaderWidget::Week:
            minorItems = minorItems/(60*24*7);
            testTextMinor = "88";
            testTextMajor = "Aug x"+getYear(QDate::currentDate());
            minorPerMajor = 435; // 435 = 365days/12months/7days * 100
            break;
        case KPTTimeHeaderWidget::Month:
            minorItems = (minorItems*12)/(60*24*365);
            testTextMinor = "M";
            testTextMajor = "x"+getYear(QDate::currentDate());
            minorPerMajor = 1200;
            break;
        case KPTTimeHeaderWidget::Auto:
            //qDebug("KPTTimeHeaderWidget::Internal Error in KPTTimeHeaderWidget::computeTicks() ");
            //qDebug("             RealScale == Auto : This may not be! ");
            break;
        }
    //kdDebug()<<k_funcinfo<<"Major: '"<<testTextMajor<<"' Minor: '"<<testTextMinor<<"'"<<endl;
    itemRectMinor = p.boundingRect ( 10, 10, 2, 2, Qt::AlignLeft,testTextMinor);
    itemRectMajor = p.boundingRect ( 10, 10, 2, 2, Qt::AlignLeft,testTextMajor);
    p.end();
    //qDebug("     tempMinorScaleCount %d ", tempMinorScaleCount);
    Height = itemRectMinor.height()+itemRectMajor.height()+11;
    Width = (itemRectMinor.width()+5);
    if (Width < minimumColumnWidth()) Width = minimumColumnWidth();
    // if the desired width is greater than the maximum width of this widget
    // increase the minorscalecount
    int maxWid = myView->canvasWidth();
    if (!flagZoomToFit)
        maxWid = maximumWidth();
    while((minorItems/tempMinorScaleCount+1)*Width > maxWid)
        ++tempMinorScaleCount;
    //kdDebug()<<k_funcinfo<<"maxWid="<<maxWid<<" tempMinorScaleCount="<< tempMinorScaleCount<<endl;
    mySizeHint = (minorItems/tempMinorScaleCount+1)*Width;
    switch (myRealScale)
        {
        case KPTTimeHeaderWidget::Minute:
            if (tempMinorScaleCount < 60)
                while (60%tempMinorScaleCount > 0 && 60%tempMinorScaleCount < 60)
                    ++tempMinorScaleCount;
            if (tempMinorScaleCount >= 60) {
                myRealScale = KPTTimeHeaderWidget::Hour;
                myRealMinorScaleCount = tempMinorScaleCount/ 60;
                // myRealMinorScaleCount = 1;
                myRealMajorScaleCount = 1;
                //qDebug("KDGantt::Overzoom:Rescaling from Minute to Hour");
                //myChartView->myTimeTable->setBlockUpdating( block );
                //emit myGanttView->rescaling( KPTTimeHeaderWidget::Hour );
                computeTicks(true);
                return;
            }
            break;
        case KPTTimeHeaderWidget::Hour:
            while (24%tempMinorScaleCount > 0 && 24%tempMinorScaleCount < 24)
                ++tempMinorScaleCount;
            if (tempMinorScaleCount >= 24) {
                myRealScale = KPTTimeHeaderWidget::Day;
                myRealMinorScaleCount = tempMinorScaleCount/ 24;
                //myRealMinorScaleCount = 1;
                myRealMajorScaleCount = 1;
                //qDebug("KDGantt::Overzoom:Rescaling from Hour to Day");
                //myChartView->myTimeTable->setBlockUpdating( block );
                //emit myGanttView->rescaling( KPTTimeHeaderWidget::Day );
                computeTicks(true);
                return;
            }
            break;
        default:
            break;
        }
    flagZoomToFit = false;
    while((minorItems/tempMinorScaleCount+1)*Width < myMinimumWidth ) {
        ++minorItems;
    }
    minorItems = (minorItems/tempMinorScaleCount)+1;
    // if not enough space for the text of the major scale, increase majorscalecount
    minorPerMajor = (minorPerMajor*tempMajorScaleCount)/tempMinorScaleCount;
    // checking, if enough space for majorscale
    // if not, increasing MajorScaleCount

    while ((minorPerMajor*Width)/100 < itemRectMajor.width()) {
        minorPerMajor = minorPerMajor/tempMajorScaleCount;
        ++tempMajorScaleCount;
        minorPerMajor = minorPerMajor*tempMajorScaleCount;

    }
    // now we have the fixed  width of the minorscale computed
    myGridMinorWidth = Width;
    // the width of this widget is the gridwidth * the amount of items
    Width *= minorItems;
    // if size changed, reset geometry
    if (width() != Width   || height() != Height  )
        {
            m_minimumSizeHint.setHeight(Height);
            resize( Width, Height );
            emit  sizeChanged( Width );
        }
    myMajorGridHeight = itemRectMajor.height()+5;
    QTime tempTime = myRealStart.time();
    QDateTime tempDateTime;
    int i;
    const KCalendarSystem * calendar = KGlobal::locale()->calendar();
    switch (myRealScale)
        {
        case KPTTimeHeaderWidget::Minute:
            myRealEnd = myRealEnd.addSecs((minorItems)*tempMinorScaleCount*60);
            for ( i = 0; i < minorItems;++i) {
                tempStr.setNum(tempTime.minute());
                minorText.append(tempStr);
                tempTime = tempTime.addSecs(60*tempMinorScaleCount);
            }
            tempDateTime = myRealStart;
            while (tempDateTime.time().minute() != 0)
                tempDateTime = tempDateTime.addSecs(60);
            while (tempDateTime < myRealEnd) {
                majorTicks.append( getCoordX(tempDateTime));
                tempStr.setNum(tempDateTime.date().day());
                if ( yearFormat() == KPTTimeHeaderWidget::NoDate ) {
                    tempStr = calendar->weekDayName(tempDateTime.date().dayOfWeek() )+", "
                        +getHour(tempDateTime.time());
                } else {
                    tempStr = calendar->weekDayName(tempDateTime.date().dayOfWeek())+" "+
                        calendar->monthName(tempDateTime.date().month(), tempDateTime.date().year(), true)+ " "+
                        tempStr+", "+getHour(tempDateTime.time());
                }

                majorText.append(tempStr);
                tempDateTime = tempDateTime.addSecs(3600*tempMajorScaleCount);
            }
            majorTicks.append( getCoordX(tempDateTime));
            break;

        case KPTTimeHeaderWidget::Hour:
            myRealEnd = myRealEnd.addSecs(minorItems*tempMinorScaleCount*60*60);

            for ( i = 0; i < minorItems;++i) {
                tempStr = getHour(tempTime);
                minorText.append(tempStr);
                tempTime = tempTime.addSecs(3600*tempMinorScaleCount);
            }
            tempDateTime = myRealStart;
            while (tempDateTime.time().hour() != 0)
                tempDateTime = tempDateTime.addSecs(3600);
            while (tempDateTime < myRealEnd) {
                majorTicks.append( getCoordX(tempDateTime));
                tempStr.setNum(tempDateTime.date().day());
                if ( yearFormat() == KPTTimeHeaderWidget::NoDate ) {
                    tempStr = calendar->weekDayName(tempDateTime.date());
                } else {
                    tempStr = calendar->weekDayName(tempDateTime.date(), true)
                        +" "+
                        calendar->monthName(tempDateTime.date().month(), tempDateTime.date().year(), true)+ " "+
                        tempStr+", "+getYear(tempDateTime.date());
                }
                majorText.append(tempStr);
                tempDateTime = tempDateTime.addDays(tempMajorScaleCount);
            }
            majorTicks.append( getCoordX(tempDateTime));
            break;
        case KPTTimeHeaderWidget::Day:
            //kdDebug()<<k_funcinfo<<"KPTTimeHeaderWidget::Day: ("<<minorItems*tempMinorScaleCount<<")"<<endl;
            myRealEnd = myRealEnd.addDays(minorItems*tempMinorScaleCount);
            for ( i = 0; i < minorItems;++i) {
                if (tempMinorScaleCount == 1) {
                    minorText.append(calendar->weekDayName(tempDate.dayOfWeek(), true).left(1));
                } else
                    minorText.append(QString::number(tempDate.day()));
                tempDate = tempDate.addDays(tempMinorScaleCount);
            }
            tempDate = myRealStart.date();
            while (tempDate.dayOfWeek() != KGlobal::locale()->weekStartDay())
                tempDate = tempDate.addDays(1);
            while (tempDate < myRealEnd.date()) {
                majorTicks.append( getCoordX(tempDate));
                tempStr.setNum(tempDate.day());
                tempStr = calendar->monthName(tempDate.month(), tempDate.year(), true)+ " "+
                    tempStr+", "+getYear(tempDate);
                majorText.append(tempStr);
                tempDate = tempDate.addDays(7*tempMajorScaleCount);
            }
            majorTicks.append( getCoordX(tempDate));
            //kdDebug()<<k_funcinfo<<"KPTTimeHeaderWidget::Day: append:"<<tempDate.toString()<<" at "<<getCoordX(tempDate)<<endl;
            break;
        case KPTTimeHeaderWidget::Week:
            myRealEnd = myRealEnd.addDays(minorItems*tempMinorScaleCount*7);
            for ( i = 0; i < minorItems;++i) {
                tempStr.setNum(tempDate.day());
                minorText.append(tempStr);
                tempDate = tempDate.addDays(7*tempMinorScaleCount);
            }
            tempDate = myRealStart.date();
            while (tempDate.day() != 1)
                tempDate = tempDate.addDays(1);
            while (tempDate < myRealEnd.date()) {
                majorTicks.append( getCoordX(tempDate));
                tempStr = calendar->monthName(tempDate.month(), tempDate.year(), true)+ " "+getYear(tempDate);
                majorText.append(tempStr);
                tempDate = tempDate.addMonths(tempMajorScaleCount);
            }
            majorTicks.append( getCoordX(tempDate));
            break;
        case KPTTimeHeaderWidget::Month:
            myRealEnd = myRealEnd.addMonths(minorItems*tempMinorScaleCount);
            for ( i = 0; i < minorItems;++i) {
                minorText.append((calendar->monthName(tempDate.month(), tempDate.year(), true)).left(1));
                tempDate = tempDate.addMonths(tempMinorScaleCount);
            }
            tempDate = myRealStart.date();
            while (tempDate.month() != 1)
                tempDate = tempDate.addMonths(1);
            while (tempDate < myRealEnd.date()) {
                majorTicks.append( getCoordX(tempDate));
                tempStr = getYear(tempDate);
                majorText.append(tempStr);
                tempDate = tempDate.addYears(tempMajorScaleCount);
            }
            majorTicks.append( getCoordX(tempDate));
            break;
        case KPTTimeHeaderWidget::Auto:
            break;
        }

    if (flagDoNotRepaintAfterChange) {
        //myChartView->myTimeTable->setBlockUpdating( block );
        return;
    }
    //kdDebug()<<k_funcinfo<<"width: "<<width()<<" viewport: "<<myView->canvasWidth()<<endl;
    //myChartView->myTimeTable->setBlockUpdating( block );
    updateTimeTable();
    centerDateTime(myCenterDateTime);
    //kdDebug()<<k_funcinfo<<"size: "<<width()<<"x"<<height()<<endl;
    repaint();
}


void KPTTimeHeaderWidget::saveCenterDateTime()
{
    double wid = width();
    double allsecs = secsFromTo( myRealStart, myRealEnd );
    double center = myView->canvasWidth();
    center = center / 2;
    center = center;// + myChartView->myCanvasView->contentsX();
    double secs = (allsecs*center)/wid;
    double days = secs/86400.0;
    secs = secs - ( (int) days *86400.0 );
    myCenterDateTime =  (myRealStart.addDays ( (int) days )).addSecs( (int) secs);
}


void KPTTimeHeaderWidget::centerDateTime( const QDateTime& center )
{
     moveTimeLineTo(getCoordX( center )-(myView->canvasWidth() /2));
     //qDebug("centerDateTime %s %d %d", center.toString().latin1(),getCoordX( center ),(myView->canvasWidth() /2) );
   
}


void KPTTimeHeaderWidget::paintEvent(QPaintEvent *p)
{
    //kdDebug()<<k_funcinfo<<"x,width: "<<p->rect().x()<<","<<p->rect().width()<<endl;
    repaintMe(p->rect().x(), p->rect().width());
}


int KPTTimeHeaderWidget::getCoordX(QDate date)
{
    int wid = width();
    int daysAll = myRealStart.daysTo(myRealEnd);
    if (daysAll == 0) return 0;
    int days = myRealStart.daysTo(QDateTime(date));
    return (wid *days) /daysAll;
}


int KPTTimeHeaderWidget::getCoordX(const QDateTime &datetime)
{
    double wid = width();
    double secsAll = secsFromTo( myRealStart, myRealEnd );
    if (secsAll == 0.0) return 0;
    double secs = secsFromTo( myRealStart, datetime);
    return ((int)((wid *(secs /secsAll))+0.5));
}


QString KPTTimeHeaderWidget::getYear(QDate date)
{
    QString ret;
    ret.setNum(date.year());
    switch (yearFormat()) {
    case KPTTimeHeaderWidget::FourDigit:
        // nothing to do
        break;
    case KPTTimeHeaderWidget::TwoDigit:
        ret = ret.right(2);
        break;
    case KPTTimeHeaderWidget::TwoDigitApostrophe:
        ret = "'"+ret.right(2);
        break;
    case KPTTimeHeaderWidget::NoDate:
        // nothing to do
        break;
    }
    return ret;
}


QString KPTTimeHeaderWidget::getHour(QTime time)
{
    QString ret;
    int hour = time.hour();
    if (myHourFormat == KPTTimeHeaderWidget::Hour_12) {
        if (hour >= 12) {
            if (hour > 12) hour -=12;
            ret.setNum(hour);
            ret = ret +" PM";
        } else {
            if (hour == 0) hour = 12;
            ret.setNum(hour);
            ret = ret +" AM";
        }
    } else {
        if (myHourFormat == KPTTimeHeaderWidget::Hour_24)
            ret.setNum(hour);
        else {
            ret.setNum(hour);
            ret += ":00";
        }
    }
    return ret;
}


void KPTTimeHeaderWidget::mousePressEvent ( QMouseEvent * e )
{
    mouseDown = false;
    switch ( e->button() ) {
    case LeftButton:
        mouseDown = true;
        beginMouseDown = e->pos().x();
        endMouseDown = e->pos().x();
        break;
    case RightButton:
        if (flagShowPopupMenu)
            myPopupMenu->popup(e->globalPos());
        break;
    case MidButton:
        break;
    default:
        break;
    }

}


void KPTTimeHeaderWidget::mouseReleaseEvent ( QMouseEvent *  )
{
    if ( mouseDown ) {
        mouseDown = false;
        // zoom to selection getDateTimeForIndex(
        int start, end;
        if ( beginMouseDown < endMouseDown ) {
            start = beginMouseDown;
            end = endMouseDown;
        } else {
            start = endMouseDown;
            end = beginMouseDown;
        }
        if (start < 0 )
            start = 0;
        if ( end > width() )
            end = width();
        //qDebug("start %s ",getDateTimeForIndex(start).toString().latin1() );
        //qDebug("end %s ",getDateTimeForIndex(end).toString().latin1() );
        //emit myChartView->timeIntervalSelected( getDateTimeForIndex(start),getDateTimeForIndex(end) );
        //emit myChartView->timeIntervallSelected( getDateTimeForIndex(start),getDateTimeForIndex(end) );
        //zoomToSelection( getDateTimeForIndex(start),getDateTimeForIndex(end) );
    }
    mouseDown = false;
    repaint();
}


void KPTTimeHeaderWidget::mouseDoubleClickEvent ( QMouseEvent *  )
{

}


void KPTTimeHeaderWidget::mouseMoveEvent ( QMouseEvent * e )
{
    if ( mouseDown ) {
        if ( e->pos().y() < -height() || e->pos().y() > 2* height() ) {
            mouseDown = false;
            repaint();
            return;
        }
        endMouseDown = e->pos().x();
        //repaint;
/*        int val = -1;
        if (endMouseDown <  -x() ) {
            val = myChartView->myCanvasView->horizontalScrollBar()->value() -
                myChartView->myCanvasView->horizontalScrollBar()->lineStep();
            if ( val < 0 ) {
                val = 0;
            }
        }
        if (endMouseDown >  -x() +parentWidget()->width() ) {
            val = myChartView->myCanvasView->horizontalScrollBar()->value() +
                myChartView->myCanvasView->horizontalScrollBar()->lineStep();

        }*/
        repaintMe(-x(),parentWidget()->width());
/*        if ( val > -1 ) {
            if ( val > myChartView->myCanvasView->horizontalScrollBar()->maxValue() ) {
                val = myChartView->myCanvasView->horizontalScrollBar()->maxValue();
            }
            myChartView->myCanvasView->horizontalScrollBar()->setValue( val );
        }*/
        //qDebug("mousemove %d %d %d %d",endMouseDown, -x(),parentWidget()->width() , e->pos().y());
    }
}

void KPTTimeHeaderWidget::slotScaleChanged(int unit) {
    //kdDebug()<<k_funcinfo<<"Unit="<<unit<<" scale="<<myScale<<" realscale="<<myRealScale<<endl;
    switch (unit) {
    case 1:
        if (myScale != KPTTimeHeaderWidget::Minute)
            emit scaleChanged(KPTTimeHeaderWidget::Minute);
        break;
    case 2:
        if (myScale != KPTTimeHeaderWidget::Hour)
            emit scaleChanged(KPTTimeHeaderWidget::Hour);
        break;
    case 3:
        if (myScale != KPTTimeHeaderWidget::Day)
            emit scaleChanged(KPTTimeHeaderWidget::Day);
        break;
    case 4:
        if (myScale != KPTTimeHeaderWidget::Week)
            emit scaleChanged(KPTTimeHeaderWidget::Week);
        break;
    case 5:
        if (myScale != KPTTimeHeaderWidget::Month)
            emit scaleChanged(KPTTimeHeaderWidget::Month);
        break;
    default:    
        break;
    }
}

void KPTTimeHeaderWidget::slotTimeFormatChanged(int i) {
    //kdDebug()<<k_funcinfo<<"i="<<i<<endl;
    //setSettings(i);
    switch (i) {
    case 40:
        emit timeFormatChanged(KPTTimeHeaderWidget::Hour_24);
        break;
    case 41:
        emit timeFormatChanged(KPTTimeHeaderWidget::Hour_12);
        break;
    case 42:
        emit timeFormatChanged(KPTTimeHeaderWidget::Hour_24_FourDigit);
        break;
    default:
        break;
    }
}

} // KPlato namespace

#include "kpttimescale.moc"
