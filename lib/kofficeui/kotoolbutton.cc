/* This file is part of the KDE project
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>

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

#include <qtooltip.h>
#include <qpainter.h>
#include <qstyle.h>

#include <kotoolbutton.h>
#include <kdebug.h>

namespace {
    const int COLUMNS = 15;
    const int TILESIZE = 16;
}

KoColorPanel::KoColorPanel( QWidget* parent, const char* name ) :
    QWidget( parent, name, WStaticContents | WRepaintNoErase | WResizeNoErase )
{
    setMouseTracking( true );
    init();
}

KoColorPanel::~KoColorPanel()
{
}

QSize KoColorPanel::sizeHint() const
{
    return minimumSizeHint();
}

QSize KoColorPanel::minimumSizeHint() const
{
    return QSize( COLUMNS << 4, lines() << 4 );
}

void KoColorPanel::clear()
{
    QSize area( minimumSizeHint() );
    m_colorMap.clear();
    init();
    updateGeometry();
    erase( 0, 0, area.width(), area.height() );
}

void KoColorPanel::insertColor( const QColor& color )
{
    Position pos = m_nextPosition;
    if ( insertColor( color, true ) ) // we want checking for external users
        paint( pos );
    finalizeInsertion();
}

void KoColorPanel::insertColor( const QColor& color, const QString& toolTip )
{
    Position pos = m_nextPosition;
    if ( insertColor( color, toolTip, true ) ) // we want checking for external users
        paint( pos );
    finalizeInsertion();
}

void KoColorPanel::insertDefaultColors()
{
    if ( m_defaultsAdded )
        return;
    m_defaultsAdded = true;

    int currentRow = m_nextPosition.y; // we have to repaint this row below

    // For all i18n gurus: I don't think we can translate these color names, can we? (Werner)
    // Note: No checking for duplicates, so take care when you modify that list
    insertColor(qRgb( 255 ,     0 ,     0 ),  "Red", false);
    insertColor(qRgb( 255 ,   165 ,     0 ),  "Orange", false);
    insertColor(qRgb( 255 ,     0 ,   255 ),  "Magenta", false);
    insertColor(qRgb(   0 ,     0 ,   255 ),  "Blue", false);
    insertColor(qRgb(   0 ,   255 ,   255 ),  "Cyan", false);
    insertColor(qRgb(   0 ,   255 ,     0 ),  "Green", false);
    insertColor(qRgb( 255 ,   255 ,     0 ),  "Yellow", false);
    insertColor(qRgb( 165 ,    42 ,    42 ),  "Brown", false);
    insertColor(qRgb( 139 ,     0 ,     0 ),  "Darkred", false);
    insertColor(qRgb( 255 ,   140 ,     0 ),  "Dark Orange", false);
    insertColor(qRgb( 139 ,     0 ,   139 ),  "Dark Magenta", false);
    insertColor(qRgb(   0 ,     0 ,   139 ),  "Dark Blue", false);
    insertColor(qRgb(   0 ,   139 ,   139 ),  "Dark Cyan", false);
    insertColor(qRgb(   0 ,   100 ,     0 ),  "Dark Green", false);
    insertColor(qRgb( 130 ,   127 ,     0 ),  "Dark Yellow", false);
    insertColor(qRgb( 255 ,   255 ,   255 ),  "White", false);
    insertColor(qRgb( 229 ,   229 ,   229 ),  "Grey 90%", false);
    insertColor(qRgb( 204 ,   204 ,   204 ),  "Grey 80%", false);
    insertColor(qRgb( 178 ,   178 ,   178 ),  "Grey 70%", false);
    insertColor(qRgb( 153 ,   153 ,   153 ),  "Grey 60%", false);
    insertColor(qRgb( 127 ,   127 ,   127 ),  "Grey 50%", false);
    insertColor(qRgb( 102 ,   102 ,   102 ),  "Grey 40%", false);
    insertColor(qRgb(  76 ,    76 ,    76 ),  "Grey 30%", false);
    insertColor(qRgb(  51 ,    51 ,    51 ),  "Grey 20%", false);
    insertColor(qRgb(  25 ,    25 ,    25 ),  "Grey 10%", false);
    insertColor(qRgb(   0 ,     0 ,     0 ),  "Black", false);
    insertColor(qRgb( 255 ,   255 ,   240 ),  "Ivory", false);
    insertColor(qRgb( 255 ,   250 ,   250 ),  "Snow", false);
    insertColor(qRgb( 245 ,   255 ,   250 ),  "Mint Cream", false);
    insertColor(qRgb( 255 ,   250 ,   240 ),  "Floral White", false);
    insertColor(qRgb( 255 ,   255 ,   224 ),  "Light Yellow", false);
    insertColor(qRgb( 240 ,   255 ,   255 ),  "Azure", false);
    insertColor(qRgb( 248 ,   248 ,   255 ),  "Ghost White", false);
    insertColor(qRgb( 240 ,   255 ,   240 ),  "Honeydew", false);
    insertColor(qRgb( 255 ,   245 ,   238 ),  "Seashell", false);
    insertColor(qRgb( 240 ,   248 ,   255 ),  "Alice Blue", false);
    insertColor(qRgb( 255 ,   248 ,   220 ),  "Cornsilk", false);
    insertColor(qRgb( 255 ,   240 ,   245 ),  "Lavender Blush", false);
    insertColor(qRgb( 253 ,   245 ,   230 ),  "Old Lace", false);
    insertColor(qRgb( 245 ,   245 ,   245 ),  "White Smoke", false);
    insertColor(qRgb( 255 ,   250 ,   205 ),  "Lemon Chiffon", false);
    insertColor(qRgb( 224 ,   255 ,   255 ),  "Light Cyan", false);
    insertColor(qRgb( 250 ,   250 ,   210 ),  "Light Goldenrod Yellow", false);
    insertColor(qRgb( 250 ,   240 ,   230 ),  "Linen", false);
    insertColor(qRgb( 245 ,   245 ,   220 ),  "Beige", false);
    insertColor(qRgb( 255 ,   239 ,   213 ),  "Papaya Whip", false);
    insertColor(qRgb( 255 ,   235 ,   205 ),  "Blanched Almond", false);
    insertColor(qRgb( 250 ,   235 ,   215 ),  "Antique White", false);
    insertColor(qRgb( 255 ,   228 ,   225 ),  "Misty Rose", false);
    insertColor(qRgb( 230 ,   230 ,   250 ),  "Lavender", false);
    insertColor(qRgb( 255 ,   228 ,   196 ),  "Bisque", false);
    insertColor(qRgb( 255 ,   228 ,   181 ),  "Moccasin", false);
    insertColor(qRgb( 255 ,   222 ,   173 ),  "Navajo White", false);
    insertColor(qRgb( 255 ,   218 ,   185 ),  "Peach Puff", false);
    insertColor(qRgb( 238 ,   232 ,   170 ),  "Pale Goldenrod", false);
    insertColor(qRgb( 245 ,   222 ,   179 ),  "Wheat", false);
    insertColor(qRgb( 220 ,   220 ,   220 ),  "Gainsboro", false);
    insertColor(qRgb( 240 ,   230 ,   140 ),  "Khaki", false);
    insertColor(qRgb( 175 ,   238 ,   238 ),  "Pale Turquoise", false);
    insertColor(qRgb( 255 ,   192 ,   203 ),  "Pink", false);
    insertColor(qRgb( 238 ,   221 ,   130 ),  "Light Goldenrod", false);
    insertColor(qRgb( 211 ,   211 ,   211 ),  "Light Grey", false);
    insertColor(qRgb( 255 ,   182 ,   193 ),  "Light Pink", false);
    insertColor(qRgb( 176 ,   224 ,   230 ),  "Powder Blue", false);
    insertColor(qRgb( 127 ,   255 ,   212 ),  "Aquamarine", false);
    insertColor(qRgb( 216 ,   191 ,   216 ),  "Thistle", false);
    insertColor(qRgb( 173 ,   216 ,   230 ),  "Light Blue", false);
    insertColor(qRgb( 152 ,   251 ,   152 ),  "Pale Green", false);
    insertColor(qRgb( 255 ,   215 ,     0 ),  "Gold", false);
    insertColor(qRgb( 173 ,   255 ,    47 ),  "Green Yellow", false);
    insertColor(qRgb( 176 ,   196 ,   222 ),  "Light Steel Blue", false);
    insertColor(qRgb( 144 ,   238 ,   144 ),  "Light Green", false);
    insertColor(qRgb( 221 ,   160 ,   221 ),  "Plum", false);
    insertColor(qRgb( 190 ,   190 ,   190 ),  "Gray", false);
    insertColor(qRgb( 222 ,   184 ,   135 ),  "Burly Wood", false);
    insertColor(qRgb( 135 ,   206 ,   250 ),  "Light Skyblue", false);
    insertColor(qRgb( 255 ,   160 ,   122 ),  "Light Salmon", false);
    insertColor(qRgb( 135 ,   206 ,   235 ),  "Sky Blue", false);
    insertColor(qRgb( 210 ,   180 ,   140 ),  "Tan", false);
    insertColor(qRgb( 238 ,   130 ,   238 ),  "Violet", false);
    insertColor(qRgb( 244 ,   164 ,    96 ),  "Sandy Brown", false);
    insertColor(qRgb( 233 ,   150 ,   122 ),  "Dark Salmon", false);
    insertColor(qRgb( 189 ,   183 ,   107 ),  "Dark khaki", false);
    insertColor(qRgb( 127 ,   255 ,     0 ),  "Chartreuse", false);
    insertColor(qRgb( 169 ,   169 ,   169 ),  "Dark Gray", false);
    insertColor(qRgb( 124 ,   252 ,     0 ),  "Lawn Green", false);
    insertColor(qRgb( 255 ,   105 ,   180 ),  "Hot Pink", false);
    insertColor(qRgb( 250 ,   128 ,   114 ),  "Salmon", false);
    insertColor(qRgb( 240 ,   128 ,   128 ),  "Light Coral", false);
    insertColor(qRgb(  64 ,   224 ,   208 ),  "Turquoise", false);
    insertColor(qRgb( 143 ,   188 ,   143 ),  "Dark Seagreen", false);
    insertColor(qRgb( 218 ,   112 ,   214 ),  "Orchid", false);
    insertColor(qRgb( 102 ,   205 ,   170 ),  "Medium Aquamarine", false);
    insertColor(qRgb( 255 ,   127 ,    80 ),  "Coral", false);
    insertColor(qRgb( 154 ,   205 ,    50 ),  "Yellow Green", false);
    insertColor(qRgb( 218 ,   165 ,    32 ),  "Goldenrod", false);
    insertColor(qRgb(  72 ,   209 ,   204 ),  "Medium Turquoise", false);
    insertColor(qRgb( 188 ,   143 ,   143 ),  "Rosy Brown", false);
    insertColor(qRgb( 219 ,   112 ,   147 ),  "Pale VioletRed", false);
    insertColor(qRgb(   0 ,   250 ,   154 ),  "Medium Spring Green", false);
    insertColor(qRgb( 255 ,    99 ,    71 ),  "Tomato", false);
    insertColor(qRgb( 0   ,   255 ,   127 ),  "Spring Green", false);
    insertColor(qRgb( 205 ,   133 ,    63 ),  "Peru", false);
    insertColor(qRgb( 100 ,   149 ,   237 ),  "Cornflower Blue", false);
    insertColor(qRgb( 132 ,   112 ,   255 ),  "Light Slate Blue", false);
    insertColor(qRgb( 147 ,   112 ,   219 ),  "Medium Purple", false);
    insertColor(qRgb( 186 ,    85 ,   211 ),  "Medium Orchid", false);
    insertColor(qRgb(  95 ,   158 ,   160 ),  "Cadet Blue", false);
    insertColor(qRgb(   0 ,   206 ,   209 ),  "Dark Turquoise", false);
    insertColor(qRgb(   0 ,   191 ,   255 ),  "Deep Skyblue", false);
    insertColor(qRgb( 119 ,   136 ,   153 ),  "Light Slate Grey", false);
    insertColor(qRgb( 184 ,   134 ,    11 ),  "Dark Goldenrod", false);
    insertColor(qRgb( 123 ,   104 ,   238 ),  "MediumSlate Blue", false);
    insertColor(qRgb( 205 ,    92 ,    92 ),  "IndianRed", false);
    insertColor(qRgb( 210 ,   105 ,    30 ),  "Chocolate", false);
    insertColor(qRgb(  60 ,   179 ,   113 ),  "Medium Sea Green", false);
    insertColor(qRgb(  50 ,   205 ,    50 ),  "Lime Ggreen", false);
    insertColor(qRgb(  32 ,   178 ,   170 ),  "Light Sea Green", false);
    insertColor(qRgb( 112 ,   128 ,   144 ),  "Slate Gray", false);
    insertColor(qRgb(  30 ,   144 ,   255 ),  "Dodger Blue", false);
    insertColor(qRgb( 255 ,    69 ,     0 ),  "Orange Red", false);
    insertColor(qRgb( 255 ,    20 ,   147 ),  "Deep Pink", false);
    insertColor(qRgb(  70 ,   130 ,   180 ),  "Steel Blue", false);
    insertColor(qRgb( 106 ,    90 ,   205 ),  "Slate Blue", false);
    insertColor(qRgb( 107 ,   142 ,    35 ),  "Olive Drab", false);
    insertColor(qRgb(  65 ,   105 ,   225 ),  "Royal Blue", false);
    insertColor(qRgb( 208 ,    32 ,   144 ),  "Violet Red", false);
    insertColor(qRgb( 153 ,    50 ,   204 ),  "Dark Orchid", false);
    insertColor(qRgb( 160 ,    32 ,   240 ),  "Purple", false);
    insertColor(qRgb( 105 ,   105 ,   105 ),  "Dim Gray", false);
    insertColor(qRgb( 138 ,    43 ,   226 ),  "Blue Violet", false);
    insertColor(qRgb( 160 ,    82 ,    45 ),  "Sienna", false);
    insertColor(qRgb( 199 ,    21 ,   133 ),  "Medium Violet Red", false);
    insertColor(qRgb( 176 ,    48 ,    96 ),  "Maroon", false);
    insertColor(qRgb(  46 ,   139 ,    87 ),  "Sea Green", false);
    insertColor(qRgb(  85 ,   107 ,    47 ),  "Dark Olive Green", false);
    insertColor(qRgb(  34 ,   139 ,    34 ),  "Forest Green", false);
    insertColor(qRgb( 139 ,    69 ,    19 ),  "Saddle Brown", false);
    insertColor(qRgb( 148 ,     0 ,   211 ),  "Darkviolet", false);
    insertColor(qRgb( 178 ,    34 ,    34 ),  "Fire Brick", false);
    insertColor(qRgb(  72 ,    61 ,   139 ),  "Dark Slate Blue", false);
    insertColor(qRgb(  47 ,    79 ,    79 ),  "Dark Slate Gray", false);
    insertColor(qRgb(  25 ,    25 ,   112 ),  "Midnight Blue", false);
    insertColor(qRgb(   0 ,     0 ,   205 ),  "Medium Blue", false);
    insertColor(qRgb(   0 ,     0 ,   128 ),  "Navy", false);

    finalizeInsertion();
    updateGeometry();
    // we have to repaint the "old" current row explicitly due
    // to WStaticContents
    repaint( 0, currentRow << 4, COLUMNS << 4, 16 );
}

void KoColorPanel::mousePressEvent( QMouseEvent* )
{
}

void KoColorPanel::mouseReleaseEvent( QMouseEvent* )
{
}

void KoColorPanel::mouseMoveEvent( QMouseEvent* e )
{
    updateFocusPosition( mapToPosition( e->pos() ) );
}

void KoColorPanel::paintEvent( QPaintEvent* e )
{
    int lns = lines();
    int startRow, endRow, startCol, endCol;
    paintArea( e->rect(), startRow, endRow, startCol, endCol );

    QPainter p( this );

    // First clear all the areas we won't paint on later (only if the widget isn't erased)
    if ( !e->erased() ) {
        // vertical rects
        int tmp = TILESIZE * lns;
        if ( startCol == 0 )
            erase( 0, 0, 2, tmp );
        if ( endCol == COLUMNS )
            erase( width() - 2, 0, 2, tmp );
        else
            erase( ( endCol << 4 ) - 2, 0, 2, tmp );
        int i = startCol == 0 ? 1 : startCol;
        for ( ; i < endCol; ++i )
            erase( ( i << 4 ) - 2, 0, 4, tmp );

        // horizontal rects
        tmp = TILESIZE * COLUMNS;
        if ( startRow == 0 )
            erase( 0, 0, tmp, 2 );
        if ( endRow == lns )
            erase( 0, height() - 2, tmp, 2 );
        else
            erase( 0, ( endRow << 4 ) - 2, tmp, 2 );
        i = startRow == 0 ? 1 : startRow;
        for ( ; i < endRow; ++i )
            erase( 0, ( i << 4 ) - 2, tmp, 4 );
    }

    // The "active" element (if there is one)
    if ( hasFocus() && m_focusPosition.x != -1 && m_focusPosition.y != -1 &&
         mapFromPosition( m_focusPosition ).intersects( e->rect() ) )
        style().drawPrimitive( QStyle::PE_Panel, &p, QRect( m_focusPosition.x << 4, m_focusPosition.y << 4, TILESIZE, TILESIZE ),
                               colorGroup(), QStyle::Style_Sunken | QStyle::Style_Enabled );

    --lns;  // Attention: We just avoid some lns - 1 statements

    // ...all color tiles
    if ( !m_colorMap.isEmpty() ) {
        int currentRow = startRow, currentCol = startCol;
        while ( currentRow < endRow && currentCol < endCol ) {
            QMap<Position, QColor>::ConstIterator it = m_colorMap.find( Position( currentCol, currentRow ) );
            if( it != m_colorMap.end() )
                p.fillRect( ( currentCol << 4 ) + 2, ( currentRow << 4 ) + 2, 12, 12, it.data() );

            // position of the next cell
            ++currentCol;
            if ( currentCol == endCol ) {
                ++currentRow;
                currentCol = startCol;
            }
        }
    }

    // clean up the last line (it's most likely that it's not totally filled)
    if ( !e->erased() && endRow > lns ) {
        int fields = m_colorMap.count() % COLUMNS;
        erase( fields << 4, lns * TILESIZE, ( COLUMNS - fields ) << 4, 16 );
    }
}

void KoColorPanel::keyPressEvent( QKeyEvent* e )
{
    Position newPos( m_focusPosition );
    if ( e->key() == Qt::Key_Up ) {
        if ( newPos.y == 0 )
            e->ignore();
        else
            --newPos.y;
    }
    if ( e->key() == Qt::Key_Down ) {
        if ( newPos < Position( m_colorMap.count() % COLUMNS, lines() - 2 ) )
            ++newPos.y;
        else
            e->ignore();
    }
    if ( e->key() == Qt::Key_Left ) {
        if ( newPos.x == 0 )
            e->ignore();
        else
            --newPos.x;
    }
    if ( e->key() == Qt::Key_Right ) {
        if ( newPos.x < COLUMNS - 1 && newPos < Position( m_colorMap.count() % COLUMNS - 1, lines() - 1 ) )
            ++newPos.x;
        else
            e->ignore();
    }
    updateFocusPosition( newPos );
}

void KoColorPanel::focusInEvent( QFocusEvent* e )
{
    if ( !m_colorMap.isEmpty() && m_focusPosition.x == -1 && m_focusPosition.y == -1 ) {
        m_focusPosition.x = 0;
        m_focusPosition.y = 0;
    }
    QWidget::focusInEvent( e );
}

bool KoColorPanel::insertColor( const QColor& color, bool checking )
{
    if ( checking && isAvailable( color ) )
        return false;

    m_colorMap.insert( m_nextPosition, color );

    ++m_nextPosition.x;
    if ( m_nextPosition.x == COLUMNS ) {
        m_nextPosition.x = 0;
        ++m_nextPosition.y;
    }
    return true;
}

void KoColorPanel::finalizeInsertion()
{
    if ( !isFocusEnabled() )
        setFocusPolicy( QWidget::StrongFocus );
    // Did we start a new row?
    if ( m_nextPosition.x == 1 )
        updateGeometry();
}

bool KoColorPanel::insertColor( const QColor& color, const QString& toolTip, bool checking )
{
    if ( checking && isAvailable( color ) )
        return false;

    // Remember the "old" m_nextPosition -- this is the place where the newly
    // inserted color will be located
    QRect rect( mapFromPosition( m_nextPosition ) );
    insertColor( color, false ); // check only once ;)
    QToolTip::add( this, rect, toolTip );
    return true;
}

bool KoColorPanel::isAvailable( const QColor& color )
{
    // O(n) checking on insert, but this is better than O(n) checking
    // on every mouse move...
    QMap<Position, QColor>::ConstIterator it = m_colorMap.begin();
    QMap<Position, QColor>::ConstIterator end = m_colorMap.end();
    for ( ; it != end; ++it )
        if ( it.data() == color )
            return true;
    return false;
}

KoColorPanel::Position KoColorPanel::mapToPosition( const QPoint& point ) const
{
    return Position ( point.x() >> 4, point.y() >> 4 );
}

QRect KoColorPanel::mapFromPosition( const KoColorPanel::Position& position ) const
{
    return QRect( position.x << 4, position.y << 4, TILESIZE, TILESIZE );
}

int KoColorPanel::lines() const
{
    if ( m_colorMap.isEmpty() )
        return 1;
    return ( m_colorMap.count() - 1 ) / COLUMNS + 1;
}

void KoColorPanel::paintArea( const QRect& rect, int& startRow, int& endRow, int& startCol, int& endCol ) const
{
    startRow = rect.top() >> 4;
    endRow = ( rect.bottom() >> 4 ) + 1;
    endRow = endRow > lines() ? lines() : endRow;
    startCol = rect.left() >> 4;
    endCol = ( rect.right() >> 4 ) + 1;
    endCol = endCol > COLUMNS ? COLUMNS : endCol;
}

void KoColorPanel::updateFocusPosition( const Position& newPosition )
{
    QPainter p( this );

    // restore the old tile where we had the focus before
    if ( m_focusPosition.x != -1 && m_focusPosition.y != -1 ) {
        erase( mapFromPosition( m_focusPosition ) );
        QMap<Position, QColor>::ConstIterator it = m_colorMap.find( m_focusPosition );
        if ( it != m_colorMap.end() )
            p.fillRect( ( m_focusPosition.x << 4 ) + 2, ( m_focusPosition.y << 4 ) + 2, 12, 12, it.data() );
    }

    m_focusPosition = newPosition;

    QMap<Position, QColor>::ConstIterator it = m_colorMap.find( m_focusPosition );
    if ( it != m_colorMap.end() ) {
        // draw at the new focus position
        style().drawPrimitive( QStyle::PE_Panel, &p, QRect( m_focusPosition.x << 4, m_focusPosition.y << 4, TILESIZE, TILESIZE ),
                               colorGroup(), QStyle::Style_Sunken | QStyle::Style_Enabled );
        p.fillRect( ( m_focusPosition.x << 4 ) + 2, ( m_focusPosition.y << 4 ) + 2, 12, 12, it.data() );
    }

}

void KoColorPanel::paint( const Position& position )
{
    QMap<Position, QColor>::ConstIterator it = m_colorMap.find( position );
    if ( it == m_colorMap.end() )
        return;

    erase( mapFromPosition( position ) );
    QPainter p( this );
    p.fillRect( ( position.x << 4 ) + 2, ( position.y << 4 ) + 2, 12, 12, it.data() );
}

void KoColorPanel::init()
{
    setFocusPolicy( QWidget::NoFocus ); // it's empty
    m_nextPosition.x = 0;
    m_nextPosition.y = 0;
    m_focusPosition.x = -1;
    m_focusPosition.y = -1;
    m_defaultsAdded = false;
}

bool operator<( const KoColorPanel::Position& lhs, const KoColorPanel::Position& rhs )
{
    return ( lhs.y * COLUMNS + lhs.x ) < ( rhs.y * COLUMNS + rhs.x );
}


KoToolButton::KoToolButton( const QString& icon, int id, QWidget* parent,
                            const char* name, const QString& txt, KInstance* _instance ) :
    KToolBarButton( icon, id, parent, name, txt, _instance )
{
}

KoToolButton::KoToolButton( const QPixmap& pixmap, int id, QWidget* parent,
                            const char* name, const QString& txt ) :
    KToolBarButton( pixmap, id, parent, name, txt )
{
}

KoToolButton::~KoToolButton()
{
}

#include <kotoolbutton.moc>
