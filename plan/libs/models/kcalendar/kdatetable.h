/*  -*- C++ -*-
    This file is part of the KDE libraries
    Copyright (C) 1997 Tim D. Gilman (tdgilman@best.org)
              (C) 1998-2001 Mirko Boehm (mirko@kde.org)
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
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/
#ifndef KPTDATETBL_H
#define KPTDATETBL_H

#include "../kplatomodels_export.h"

#include <QtGui/QValidator>
#include <QtGui/QLineEdit>
#include <QtCore/QDateTime>
#include <QHash>
#include <QList>
#include <QStyleOptionViewItem>
#include <QStyleOptionHeader>
#include <QFrame>

class KMenu;

namespace KPlato
{

class KDateTableDataModel;
class KDateTableDateDelegate;
class KDateTableWeekDayDelegate;
class KDateTableWeekNumberDelegate;

class StyleOptionHeader;
class StyleOptionViewItem;

class KPLATOMODELS_EXPORT Frame: public QFrame
{
    Q_OBJECT
public:
    Frame( QWidget *parent = 0 );

public slots:
    void updateFocus( QFocusEvent *e );

protected:
    virtual void paintEvent(QPaintEvent *e);
    void drawFrame(QPainter *p);
};


/**
 * Frame with popup menu behavior.
 * @author Tim Gilman, Mirko Boehm
 */
class  KPopupFrame : public QFrame
{
  Q_OBJECT
protected:
  /**
   * Catch key press events.
   */
  virtual void keyPressEvent(QKeyEvent* e);
  
public Q_SLOTS:
  /**
   * Close the popup window. This is called from the main widget, usually.
   * @p r is the result returned from exec().
   */
  void close(int r);
public:
  /**
   * The contructor. Creates a dialog without buttons.
   */
  KPopupFrame(QWidget* parent=0);
  /**
   * The destructor
   */
  ~KPopupFrame();
  /**
   * Set the main widget. You cannot set the main widget from the constructor,
   * since it must be a child of the frame itselfes.
   * Be careful: the size is set to the main widgets size. It is up to you to
   * set the main widgets correct size before setting it as the main
   * widget.
   */
  void setMainWidget(QWidget* m);
  /**
   * The resize event. Simply resizes the main widget to the whole
   * widgets client size.
   */
  virtual void resizeEvent(QResizeEvent*);
  /**
   * Open the popup window at position pos.
   */
  void popup(const QPoint &pos);
  /**
   * Execute the popup window.
   */
  int exec(const QPoint &p);
  /**
   * Execute the popup window.
   */
  int exec(int x, int y);

Q_SIGNALS:
  void leaveModality();

private:
  class KPopupFramePrivate;
  friend class KPopupFramePrivate;
  KPopupFramePrivate * const d;
  
  Q_DISABLE_COPY(KPopupFrame)
};

/**
* Validates user-entered dates.
*/
class  KDateValidator : public QValidator
{
public:
    KDateValidator(QWidget* parent=0);
    virtual State validate(QString&, int&) const;
    virtual void fixup ( QString & input ) const;
    State date(const QString&, QDate&) const;
};

/**
 * Date selection table.
 * This is a support class for the KDatePicker class.  It just
 * draws the calendar table without titles, but could theoretically
 * be used as a standalone.
 *
 * When a date is selected by the user, it emits a signal:
 * dateSelected(QDate)
 *
 * @internal
 * @author Tim Gilman, Mirko Boehm
 */
class KPLATOMODELS_EXPORT KDateTable : public QWidget
{
    Q_OBJECT
    Q_PROPERTY( QDate date READ date WRITE setDate )
    Q_PROPERTY( bool popupMenu READ popupMenuEnabled WRITE setPopupMenuEnabled )

public:
    /**
     * The constructor.
     */
    explicit KDateTable(QWidget* parent = 0);

    /**
     * The constructor.
     */
    explicit KDateTable(const QDate&, QWidget* parent = 0);

    /**
     * The destructor.
     */
    ~KDateTable();

    /**
     * Returns a recommended size for the widget.
     * To save some time, the size of the largest used cell content is
     * calculated in each paintCell() call, since all calculations have
     * to be done there anyway. The size is stored in maxCell. The
     * sizeHint() simply returns a multiple of maxCell.
     */
    virtual QSize sizeHint() const;
    /**
     * Set the font size of the date table.
     */
    void setFontSize(int size);
    /**
     * Select and display this date.
     */
    bool setDate(const QDate&);

    /**
     * @returns the selected date.
     */
    const QDate& date() const;

    /**
     * Enables a popup menu when right clicking on a date.
     *
     * When it's enabled, this object emits a aboutToShowContextMenu signal
     * where you can fill in the menu items.
     */
    void setPopupMenuEnabled( bool enable );

    /**
     * Returns if the popup menu is enabled or not
     */
    bool popupMenuEnabled() const;

    enum BackgroundMode { NoBgMode=0, RectangleMode, CircleMode };

    /**
     * Makes a given date be painted with a given foregroundColor, and background
     * (a rectangle, or a circle/ellipse) in a given color.
     */
    void setCustomDatePainting( const QDate &date, const QColor &fgColor, BackgroundMode bgMode=NoBgMode, const QColor &bgColor=QColor());

    /**
     * Unsets the custom painting of a date so that the date is painted as usual.
     */
    void unsetCustomDatePainting( const QDate &date );

    //----->
    enum ItemDataRole { DisplayRole_1 = Qt::UserRole + 1 };
    
    enum SelectionMode { SingleSelection, ExtendedSelection };
    void setSelectionMode( SelectionMode mode );
    
    void setModel( KDateTableDataModel *model );
    KDateTableDataModel *model() const;
    
    void setDateDelegate( KDateTableDateDelegate *delegate );
    void setDateDelegate( const QDate &date, KDateTableDateDelegate *delegate );
    void setWeekDayDelegate( KDateTableWeekDayDelegate *delegate );
    void setWeekNumberDelegate( KDateTableWeekNumberDelegate *delegate );
    void setWeekNumbersEnabled( bool enable );
    
    void setStyleOptionDate( const StyleOptionViewItem &so );
    void setStyleOptionWeekDay( const StyleOptionHeader &so );
    void setStyleOptionWeekNumber( const StyleOptionHeader &so );
    
    void setGridEnabled( bool enable );
    //<-----
    
protected:
    /**
     * calculate the position of the cell in the matrix for the given date. The result is the 0-based index.
     */
    virtual int posFromDate( const QDate &date ); 
    /**
     * calculate the date that is displayed at a given cell in the matrix. pos is the
     * 0-based index in the matrix. Inverse function to posForDate().
     */
    virtual QDate dateFromPos( int pos ); 

    virtual void paintEvent(QPaintEvent *e);
    /**
     * React on mouse clicks that select a date.
     */
    virtual void mousePressEvent(QMouseEvent *);
    virtual void wheelEvent( QWheelEvent * e );
    virtual void keyPressEvent( QKeyEvent *e );
    virtual void focusInEvent( QFocusEvent *e );
    virtual void focusOutEvent( QFocusEvent *e );

    virtual bool event( QEvent *e );

Q_SIGNALS:
    /**
     * The selected date changed.
     */
    void dateChanged(const QDate&);
    /**
     * This function behaves essentially like the one above.
     * The selected date changed.
     * @param cur The current date
     * @param old The date before the date was changed
     */
    void dateChanged(const QDate& cur, const QDate& old);
    /**
     * A date has been selected by clicking on the table.
     */
    void tableClicked();

    /**
     * A popup menu for a given date is about to be shown (as when the user
     * right clicks on that date and the popup menu is enabled). Connect
     * the slot where you fill the menu to this signal.
     */
    void aboutToShowContextMenu( KMenu * menu, const QDate &date );

    /**
     * A popup menu for selected dates is about to be shown.
     * Connect the slot where you fill the menu to this signal.
     */
    void aboutToShowContextMenu( KMenu * menu, const QList<QDate>& );

    //----->
    void selectionChanged( const QList<QDate>& );
    
    void focusChanged( QFocusEvent *e );
    
protected slots:
    void slotReset();
    void slotDataChanged( const QDate &start, const QDate &end );
    //<------
    
private:
    Q_PRIVATE_SLOT(d, void nextMonth())
    Q_PRIVATE_SLOT(d, void previousMonth())
    Q_PRIVATE_SLOT(d, void beginningOfMonth())
    Q_PRIVATE_SLOT(d, void endOfMonth())
    Q_PRIVATE_SLOT(d, void beginningOfWeek())
    Q_PRIVATE_SLOT(d, void endOfWeek())

private:
    class KDateTablePrivate;
    friend class KDateTablePrivate;
    KDateTablePrivate * const d;

    void initAccels();
    void paintCell(QPainter *painter, int row, int col);
    void init();
    
    Q_DISABLE_COPY(KDateTable)
};

//----->
class KPLATOMODELS_EXPORT KDateTableDataModel : public QObject
{
    Q_OBJECT
public:
    KDateTableDataModel( QObject *parent );
    ~KDateTableDataModel();
    
    /// Fetch data for @p date, @p dataType specifies the type of data
    virtual QVariant data( const QDate &date, int role = Qt::DisplayRole,  int dataType = -1 ) const;
    virtual QVariant weekDayData( int day, int role = Qt::DisplayRole ) const;
    virtual QVariant weekNumberData( int week, int role = Qt::DisplayRole ) const;
    
signals:
    void reset();
    void dataChanged( const QDate &start, const QDate &end );

};

//-------
class KPLATOMODELS_EXPORT KDateTableDateDelegate : public QObject
{
    Q_OBJECT
public:
    KDateTableDateDelegate( QObject *parent = 0 );
    ~KDateTableDateDelegate() {}

    virtual QRectF paint( QPainter *painter, const StyleOptionViewItem &option, const QDate &date,  KDateTableDataModel *model );
    
    virtual QVariant data(  const QDate &date, int role, KDateTableDataModel *model );
};

class KPLATOMODELS_EXPORT KDateTableCustomDateDelegate : public KDateTableDateDelegate
{
    Q_OBJECT
public:
    KDateTableCustomDateDelegate(  QObject *parent = 0 );
    ~KDateTableCustomDateDelegate() {}

    virtual QRectF paint( QPainter *painter, const StyleOptionViewItem &option, const QDate &date,  KDateTableDataModel *model );
    
private:
    friend class KDateTable;
    QColor fgColor;
    QColor bgColor;
    KDateTable::BackgroundMode bgMode;
    
};

class KPLATOMODELS_EXPORT KDateTableWeekDayDelegate : public QObject
{
    Q_OBJECT
public:
    KDateTableWeekDayDelegate( QObject *parent = 0 );
    ~KDateTableWeekDayDelegate() {}

    virtual QRectF paint( QPainter *painter, const StyleOptionHeader &option, int weekday,  KDateTableDataModel *model );

    virtual QVariant data(  int day, int role, KDateTableDataModel *model );
};

class KPLATOMODELS_EXPORT KDateTableWeekNumberDelegate : public QObject
{
    Q_OBJECT
public:
    KDateTableWeekNumberDelegate( QObject *parent = 0 );
    ~KDateTableWeekNumberDelegate() {}

    virtual QRectF paint( QPainter *painter, const StyleOptionHeader &option, int week,  KDateTableDataModel *model );
    
    virtual QVariant data(  int week, int role, KDateTableDataModel *model );
};

class StyleOptionHeader : public QStyleOptionHeader
{
public:
    StyleOptionHeader() : QStyleOptionHeader() {}
    
    QRectF rectF;
};

class StyleOptionViewItem : public QStyleOptionViewItemV3
{
public:
    StyleOptionViewItem() 
    : QStyleOptionViewItemV3()
    {}
    StyleOptionViewItem( const StyleOptionViewItem &style ) 
    : QStyleOptionViewItemV3( style )
    {
        rectF = style.rectF;
    }
    
    QRectF rectF;
};

} //namespace KPlato

#endif // KDATETBL_H
