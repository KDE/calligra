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

#include <kapp.h>

#include <qframe.h>
#include <qwidget.h>
#include <qpainter.h>
#include <qcolor.h>
#include <qpen.h>
#include <qbrush.h>
#include <qstring.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qrect.h>
#include <qevent.h>
#include <qcursor.h>
#include <qpixmap.h>
#include <qlist.h>
#include <qpopupmenu.h>
#include <qpoint.h>

#include <koGlobal.h>
#include <koTabChooser.h>

/******************************************************************/
/* Struct: KoTabulator                                            */
/* Defines the position of a tabulation (in pt), and its type     */
/******************************************************************/

enum KoTabulators { T_LEFT = 0, T_CENTER = 1, T_RIGHT = 2, T_DEC_PNT = 3 };

struct KoTabulator {
    double ptPos;
    KoTabulators type;

    bool operator==( const KoTabulator & t ) const {
        return ptPos == t.ptPos && type == t.type;
    }
};

typedef QValueList<KoTabulator> KoTabulatorList;

class KoRulerPrivate;

/******************************************************************/
/* Class: KoRuler                                                 */
/******************************************************************/

class KoRuler : public QFrame
{
    Q_OBJECT
    friend class KoRulerPrivate;

public:
    static const int F_TABS = 1;
    static const int F_INDENTS = 2;

    KoRuler( QWidget *_parent,  QWidget *_canvas, Orientation _orientation,
             KoPageLayout _layout, int _flags, KoTabChooser *_tabChooser = 0L );
    ~KoRuler();

    void setUnit( const QString& _unit );

    void setZoom( const double& zoom=1.0 );
    const double& zoom() const { return m_zoom; }

    void setPageLayout( KoPageLayout _layout )
    { layout = _layout; repaint( false ); }

    void showMousePos( bool _showMPos )
    { showMPos = _showMPos; hasToDelete = false; mposX = -1; mposY = -1; repaint( false ); }
    // Not zoomed - real pixel coords!
    void setMousePos( int mx, int my );

    void setOffset( int _diffx, int _diffy )
    { diffx = _diffx; diffy = _diffy; repaint( false ); }

    void setLeftIndent( double _left )
    { i_left = makeIntern( _left ); repaint( false ); }
    void setFirstIndent( double _first )
    { i_first = makeIntern( _first ); repaint( false ); }

    void setTabList( const KoTabulatorList & tabList );
    void setFrameStart( int _frameStart ) { frameStart = _frameStart; repaint( false ); }

    void setAllowUnits( bool _allow ) { allowUnits = _allow; }
    /**
     * put m_bReadWrite to true as default
     * and used setReadWrite(false) to make in readOnly mode
     */
    void setReadWrite(bool _readWrite);

signals:
    void newPageLayout( KoPageLayout );
    void newLeftIndent( double );
    void newFirstIndent( double );
    void openPageLayoutDia();
    void tabListChanged( const KoTabulatorList & );
    void unitChanged( QString );

protected:
    enum Action {A_NONE, A_BR_LEFT, A_BR_RIGHT, A_BR_TOP, A_BR_BOTTOM,
                 A_LEFT_INDENT, A_FIRST_INDENT, A_TAB};

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
    int double2Int(const double &value) const;
    void setupMenu();
    void uncheckMenu();

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
    bool allowUnits;

    bool m_bReadWrite;

protected slots:
    void rbPT() { setUnit( QString::fromLatin1("pt") ); emit unitChanged( QString::fromLatin1("pt") ); }
    void rbMM() { setUnit( QString::fromLatin1("mm") ); emit unitChanged( QString::fromLatin1("mm") ); }
    void rbINCH() { setUnit( QString::fromLatin1("inch") ); emit unitChanged( QString::fromLatin1("inch") ); }
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
    return double2Int(m_zoom*value);
}

inline unsigned int KoRuler::zoomIt(const unsigned int &value) const {
    if (m_zoom==1.0)
        return value;
    return static_cast<unsigned int>(double2Int(m_zoom*value));
}

inline double KoRuler::unZoomIt(const double &value) const {
    if(m_zoom==1.0)
        return value;
    return value*m_1_zoom;
}

inline int KoRuler::unZoomIt(const int &value) const {
    if(m_zoom==1.0)
        return value;
    return double2Int(value*m_1_zoom);
}

inline unsigned int KoRuler::unZoomIt(const unsigned int &value) const {
    if(m_zoom==1.0)
        return value;
    return static_cast<unsigned int>(double2Int(value*m_1_zoom));
}

inline int KoRuler::double2Int(const double &value) const {

    if( static_cast<double>((value-static_cast<int>(value)))>=0.5 )
        return static_cast<int>(value)+1;
    else if( static_cast<double>((value-static_cast<int>(value)))<=-0.5 )
        return static_cast<int>(value)-1;
    else
        return static_cast<int>(value);
}

#endif
