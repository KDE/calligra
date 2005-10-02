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


#ifndef KPTTIMESCALE_H
#define KPTTIMESCALE_H

#include <qscrollview.h>
#include <qwidget.h>

#include <qtooltip.h>
#include <qdatetime.h>
#include <qcolor.h>
#include <qpixmap.h>

class QCanvasRectangle;
class QPopupMenu;
class QSize;

namespace KPlato
{

class KPTTimeScale;
class KPTTimeHeaderToolTip;

/* ***************************************************************************
   KPTTimeHeaderWidget:: KPTTimeHeaderWidget
   This is a (modified) copy of the KDTimeHeaderWidget class from KDGanttView,
   to keep the look and feel of the KDGanttView timescale.
   The main changes are:
   - KDE'ifyed, using KGLobal and kdebug.
   - The widget can be used both at the top of the chart and at the bottom,
     the major- and minor scale is swapped accordingly.
   ************************************************************************* */
class KPTTimeHeaderWidget : public QWidget
{
   Q_OBJECT

public:
    enum Scale { Minute, Hour, Day, Week, Month, Auto };
    enum YearFormat { FourDigit, TwoDigit, TwoDigitApostrophe, NoDate };
    enum HourFormat { Hour_24, Hour_12, Hour_24_FourDigit };
    
    struct DateTimeColor {
        QDateTime datetime;
        QDateTime end;
        QColor color;
        Scale minScaleView;
        Scale maxScaleView;
        QCanvasRectangle* canvasRect;
    };
    typedef QValueList<DateTimeColor> ColumnColorList;
    typedef QValueList<DateTimeColor> IntervalColorList;
    
    
    
    KPTTimeHeaderWidget (QWidget* parent, KPTTimeScale *view, bool minorOnTop=false);
    ~KPTTimeHeaderWidget();
    
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
    KPTTimeHeaderWidget::Scale maximumScale() const;
    void setMinimumScale( Scale );
    KPTTimeHeaderWidget::Scale minimumScale() const;
    KPTTimeHeaderWidget::Scale scale() const;
    KPTTimeHeaderWidget::Scale realScale() const { return myRealScale; }
    void setMajorScaleCount( int count );
    int majorScaleCount() const;
    void setMinorScaleCount( int count );
    int minorScaleCount() const;
    void setMinimumColumnWidth( int width );
    int minimumColumnWidth() const;
    void setYearFormat( YearFormat format );
    KPTTimeHeaderWidget::YearFormat yearFormat() const;
    void setHourFormat( HourFormat format );
    KPTTimeHeaderWidget::HourFormat hourFormat() const;
    void setShowMajorTicks( bool );
    bool showMajorTicks() const;
    void setShowMinorTicks( bool );
    void setScale( Scale unit);
    bool showMinorTicks() const;
    void setColumnBackgroundColor( const QDateTime& column,
                    const QColor& color,
                    Scale mini =  KPTTimeHeaderWidget::Minute ,
                    Scale maxi =  KPTTimeHeaderWidget::Month);
    void setIntervalBackgroundColor( const QDateTime& start,
                        const QDateTime& end,
                    const QColor& color,
                    Scale mini =  KPTTimeHeaderWidget::Minute ,
                    Scale maxi =  KPTTimeHeaderWidget::Month);
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
    int getCoordX(const QDateTime &);
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

    virtual QSize minimumSizeHint () const { return m_minimumSizeHint; }
    
public slots:
    void setSettings(int);
    void checkWidth( int );
    void addTickRight( int num = 1 );
    void addTickLeft( int num = 1 );
    void preparePopupMenu();
signals:
    void sizeChanged( int );
    void scaleChanged(int);
    void timeFormatChanged(int);

protected slots:
    void slotScaleChanged(int);
    void slotTimeFormatChanged(int);
    
protected:
    void repaintMajor(int left, int offsetLeft, int offsetHeight, int hei, int paintwid, QPainter* p);
    void repaintMinor(int left, int offsetLeft, int offsetHeight, int hei, int paintwid, QPainter* p);

private:
    friend class KPTTimeScale;
    
    virtual void mousePressEvent ( QMouseEvent * e );
    virtual void mouseReleaseEvent ( QMouseEvent * e );
    virtual void mouseDoubleClickEvent ( QMouseEvent * e );
    virtual void mouseMoveEvent ( QMouseEvent * e );
    double secsFromTo( const QDateTime &begin, const QDateTime &end );
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
    KPTTimeScale *myView;
    double myZoomFactor;
    int myAutoScaleMinorTickcount;
    bool flagZoomToFit;
    int mySizeHint;
    int myMinimumWidth;
    int getTickTime();
    KPTTimeHeaderToolTip* myToolTip;
    bool mouseDown;
    int beginMouseDown;
    int endMouseDown;
    bool autoComputeTimeLine;
    QPixmap paintPix;
    
    QSize m_minimumSizeHint;
    bool m_minorOnTop;

};

class KPTTimeHeaderToolTip :public QToolTip
{

public:
  KPTTimeHeaderToolTip( QWidget *wid, KPTTimeHeaderWidget* header ) : QToolTip( wid ), _wid(wid),_header (header) {

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
  KPTTimeHeaderWidget * _header;
};


class KPTTimeScale : public QScrollView
{
   Q_OBJECT
public:
    KPTTimeScale(QWidget *parent, bool bottom=true, bool enableScrollbar=true);
    ~KPTTimeScale();
    
    int posX(const QDateTime &dt);
    
    int fullHeight();
    
    void setScale(KPTTimeHeaderWidget::Scale unit);
    void setRange(const QDateTime& start, const QDateTime& end);
    void setHorizonStart(const QDateTime& start);
    QDateTime horizonStart() const;
    void setHorizonEnd(const QDateTime& start);
    QDateTime horizonEnd() const;
    void setHourFormat(KPTTimeHeaderWidget::HourFormat format);
    
    int canvasWidth() { return m_canvasWidth; }
    
    void setShowPopupMenu(bool show, bool showZoom=false, bool showScale=true, bool showTime=true, bool showYear=true,bool showGrid=false, bool showPrint=false);

    const QValueList<int> &majorGridValues() const;
    
signals:
    void unitChanged(int unit);
    void timeFormatChanged(int format);
    void headerWidthChanged(int w);
    
protected slots:
    void slotHeaderWidthChanged(int w);
    void slotUnitChanged(int unit);
    void slotTimeFormatChanged(int format);
    
private:
    bool m_bottom;
    KPTTimeHeaderWidget* m_header;
    int m_canvasWidth;
    QDateTime m_startTime;
    QDateTime m_endTime;

    
};


} // KPlato namespace

#endif //KPTTIMESCALE_H
