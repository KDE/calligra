/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

// Description: Ruler (header)

/******************************************************************/

#ifndef koRuler_h
#define koRuler_h

#include <koPageLayoutDia.h>

#include <qframe.h>
#include <qwidget.h>
#include <qstring.h>
#include <qpixmap.h>

#include <koGlobal.h>
#include <koTabChooser.h>

class QPainter;

enum KoTabulators { T_LEFT = 0, T_CENTER = 1, T_RIGHT = 2, T_DEC_PNT = 3 };

/**
 * Struct: KoTabulator
 * Defines the position of a tabulation (in pt), and its type
 */
struct KoTabulator {
    /**
     * Position of the tab in pt
     */
    double ptPos;
    /**
     * Type of tab (left/center/right/decimalpoint)
     */
    KoTabulators type;

    bool operator==( const KoTabulator & t ) const {
        return ptPos == t.ptPos && type == t.type;
    }
    // Operators used for sorting
    bool operator < ( const KoTabulator & t ) const {
        return ptPos < t.ptPos;
    }
    bool operator <= ( const KoTabulator & t ) const {
        return ptPos <= t.ptPos;
    }
    bool operator > ( const KoTabulator & t ) const {
        return ptPos > t.ptPos;
    }
};

typedef QValueList<KoTabulator> KoTabulatorList;

class KoRulerPrivate;

/**
 * KoRuler is the horizontal or vertical ruler, to be used around
 * the drawing area of most KOffice programs.
 *
 * It shows the graduated ruler with numbering, in any of the 3 base units (mm/pt/inch),
 * and supports zooming, tabulators, paragraph indents, showing the mouse position, etc.
 *
 * It also offers a popupmenu upon right-clicking, for changing the unit,
 * the page layout, or removing a tab.
 */
class KoRuler : public QFrame
{
    Q_OBJECT
    friend class KoRulerPrivate; // for the Action enum
public:
    static const int F_TABS = 1;
    static const int F_INDENTS = 2;

    KoRuler( QWidget *_parent,  QWidget *_canvas, Orientation _orientation,
             KoPageLayout _layout, int _flags, KoTabChooser *_tabChooser = 0L );
    ~KoRuler();

    /**
     * Set the unit to be used. Currently supported are "mm", "pt" and "inch".
     */
    void setUnit( const QString& _unit );

    void setZoom( const double& zoom=1.0 );
    const double& zoom() const { return m_zoom; }

    /**
     * Set the page layout, @see KoPageLayout.
     * This defines the size of the page and the margins,
     * from which the size of the ruler is deducted.
     */
    void setPageLayout( KoPageLayout _layout )
    { layout = _layout; repaint( false ); }

    /**
     * Call showMousePos(true) if the ruler should indicate the position
     * of the mouse. This is usually only the case for drawing applications,
     * so it is not the case by default.
     */
    void showMousePos( bool _showMPos );
    /**
     * Set the position of the mouse, to update the indication in the ruler.
     * This is only effective if showMousePos(true) was called previously.
     * The position to give is not zoomed, it's in real pixel coordinates!
     */
    void setMousePos( int mx, int my );

    /**
     * Set a global offset to the X and Y coordinates.
     * Usually the main drawing area is a QScrollView, and this is called
     * with contentsX() and contentsY(), each time those values change.
     */
    void setOffset( int _diffx, int _diffy );

    /**
     * Set the [paragraph] left indent to the specified position (in the current unit)
     */
    void setLeftIndent( double _left )
    { i_left = makeIntern( _left ); repaint( false ); }

    /**
     * Set the [paragraph] first-line left indent to the specified position (in the current unit)
     */
    void setFirstIndent( double _first )
    { i_first = makeIntern( _first ); repaint( false ); }

    /**
     * Set the [paragraph] right indent to the specified position (in the current unit)
     */
    void setRightIndent( double _right );

    /**
     * Set the list of tabulators to show in the ruler.
     */
    void setTabList( const KoTabulatorList & tabList );

    /**
     * Set the start and the end of the current 'frame', i.e. the part
     * of the page in which we are currently working. See KWord frames
     * for an example where this is used. The tab positions and paragraph
     * indents then become relative to the beginning of the frame, and the
     * ruler goes from frameStart to frameEnd instead of using the page margins.
     * @p _frameStart et @p _frameEnd are in pixel coordinates.
     */
    void setFrameStartEnd( int _frameStart, int _frameEnd );

    /**
     * KoRuler is in "read write" mode by default.
     * Use setReadWrite(false) to use it in read-only mode.
     */
    void setReadWrite( bool _readWrite );

    /*
     * Change KoRuler flag, for kword is useful to
     * disable Indent function when frame is different
     * from text frame
     */
    void changeFlags(int _flags);

signals:
    void newPageLayout( KoPageLayout );
    void newLeftIndent( double );
    void newFirstIndent( double );
    void newRightIndent( double );
    void openPageLayoutDia();
    void tabListChanged( const KoTabulatorList & );
    void unitChanged( QString );

protected:
    enum Action {A_NONE, A_BR_LEFT, A_BR_RIGHT, A_BR_TOP, A_BR_BOTTOM,
                 A_LEFT_INDENT, A_FIRST_INDENT, A_TAB, A_RIGHT_INDENT};

    void drawContents( QPainter *_painter )
    { orientation == Qt::Horizontal ? drawHorizontal( _painter ) : drawVertical( _painter ); }

    void drawHorizontal( QPainter *_painter );
    void drawVertical( QPainter *_painter );
    void drawTabs( QPainter &_painter );

    void mousePressEvent( QMouseEvent *e );
    void mouseReleaseEvent( QMouseEvent *e );
    void mouseMoveEvent( QMouseEvent *e );
    void mouseDoubleClickEvent( QMouseEvent* );
    void resizeEvent( QResizeEvent *e );

    double makeIntern( double _v );
    double zoomIt(const double &value) const;
    int zoomIt(const int &value) const;
    unsigned int zoomIt(const unsigned int &value) const;
    double unZoomIt(const double &value) const;
    int unZoomIt(const int &value) const;
    unsigned int unZoomIt(const unsigned int &value) const;
    void setupMenu();
    void uncheckMenu();
    void searchTab(int mx);
    void drawLine(int oldX, int newX);

    KoRulerPrivate *d;

    Qt::Orientation orientation;
    int diffx, diffy;
    double i_left, i_first;
    KoPageLayout layout;
    QPixmap buffer;
    double m_zoom, m_1_zoom;
    QString unit;
    bool hasToDelete;
    bool showMPos;
    int mposX, mposY;
    int frameStart;
    bool m_bFrameStartSet;

    bool m_bReadWrite;

protected slots:
    void rbPT() { setUnit( QString::fromLatin1("pt") ); emit unitChanged( unit ); }
    void rbMM() { setUnit( QString::fromLatin1("mm") ); emit unitChanged( unit ); }
    void rbINCH() { setUnit( QString::fromLatin1("inch") ); emit unitChanged( unit ); }
    void pageLayoutDia() { emit openPageLayoutDia(); }
    void rbRemoveTab();

};

inline double KoRuler::zoomIt(const double &value) const {
    if (m_zoom==1.0)
        return value;
    return m_zoom*value;
}

inline int KoRuler::zoomIt(const int &value) const {
    if (m_zoom==1.0)
        return value;
    return qRound(m_zoom*value);
}

inline unsigned int KoRuler::zoomIt(const unsigned int &value) const {
    if (m_zoom==1.0)
        return value;
    return static_cast<unsigned int>(qRound(m_zoom*value));
}

inline double KoRuler::unZoomIt(const double &value) const {
    if(m_zoom==1.0)
        return value;
    return value*m_1_zoom;
}

inline int KoRuler::unZoomIt(const int &value) const {
    if(m_zoom==1.0)
        return value;
    return qRound(value*m_1_zoom);
}

inline unsigned int KoRuler::unZoomIt(const unsigned int &value) const {
    if(m_zoom==1.0)
        return value;
    return static_cast<unsigned int>(qRound(value*m_1_zoom));
}

#endif
