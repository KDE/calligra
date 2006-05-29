/* This file is part of the KDE project
   Copyright (C) 2002 Nash Hoogwater <nrhoogwater@wanadoo.nl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; using
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

#include "KWTableTemplateSelector.h"
#include "KWTableTemplateSelector.moc"

#include "KWTableTemplate.h"
#include "KWTableFrameSet.h"
#include "KWDocument.h"
#include "KWTextDocument.h"
#include <KoTextParag.h>

#include <KoRect.h>

#include <klocale.h>
#include <kurl.h>
#include <kio/netaccess.h>
#include <kstandarddirs.h>
#include <kdebug.h>

#include <QLabel>
#include <q3buttongroup.h>
#include <QCheckBox>
#include <QComboBox>
#include <QFile>
#include <qdom.h>
#include <QRect>
#include <q3ptrlist.h>
#include <QLayout>
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3ListBox>
#include <QApplication>
#include <stdlib.h>
#include <math.h>

/******************************************************************/
/* Class: KWTableTemplatePreview                                  */
/******************************************************************/

KWTableTemplatePreview::KWTableTemplatePreview( const QString& title, KWTableStyle *_emptyStyle, QWidget* parent, const char* name )
    : Q3GroupBox( title, parent, name )
{
    m_emptyStyle = _emptyStyle;
    m_zoomHandler = new KoTextZoomHandler;
    m_textdoc = new KWTextDocument( m_zoomHandler );
    tableTemplate = 0L;
    origTableTemplate = 0L;
    m_disableRepaint = false;
    fillContents();
}

KWTableTemplatePreview::~KWTableTemplatePreview()
{

    delete m_textdoc;
    delete m_zoomHandler;

//    if (tableTemplate)
//        delete tableTemplate;

}

int KWTableTemplatePreview::bottomBorder(const int rows, const int cols, const int rowpos, const int colpos)
{
    if ( (rowpos<0) || (colpos<0) ) return 0;

    if ( (rowpos==0) && (colpos==0) ) // TOP LEFT CORNER
        return int( tableTemplate->pTopLeftCorner()->frameStyle()->bottomBorder().width() );
    else
    if ( (rowpos==0) && ( colpos==(cols-1) ) ) // TOP RIGHT CORNER
        return int( tableTemplate->pTopRightCorner()->frameStyle()->bottomBorder().width() );
    else
    if ( ( rowpos==(rows-1) ) && (colpos==0) ) // BOTTOM LEFT CORNER
        return int( tableTemplate->pBottomLeftCorner()->frameStyle()->bottomBorder().width() );
    else
    if ( ( rowpos==(rows-1) ) && ( colpos==(cols-1) ) ) // BOTTOM RIGHT CORNER
        return int( tableTemplate->pBottomRightCorner()->frameStyle()->bottomBorder().width() );
    else
    if ( ( rowpos==0 ) && ( colpos>0 ) && ( colpos<(cols-1) ) ) // FIRST ROW
        return int( tableTemplate->pFirstRow()->frameStyle()->bottomBorder().width() );
    else
    if ( ( colpos==0 ) && ( rowpos>0 ) && ( rowpos<(rows-1) ) ) // FIRST COL
        return int( tableTemplate->pFirstCol()->frameStyle()->bottomBorder().width() );
    else
    if ( ( rowpos==(rows-1) ) && ( colpos>0 ) && ( colpos<(cols-1) ) )  // LAST ROW
        return int( tableTemplate->pLastRow()->frameStyle()->bottomBorder().width() );
    else
    if ( ( colpos==(cols-1) ) && ( rowpos>0 ) && ( rowpos<(rows-1) ) ) // LAST COL
        return int( tableTemplate->pLastCol()->frameStyle()->bottomBorder().width() );
    else
    if ( (rowpos>0) && (colpos>0) && (rowpos<(rows-1)) && (colpos<(cols-1)) ) // BODY
        return int( tableTemplate->pBodyCell()->frameStyle()->bottomBorder().width() );

    return 0;
}

int KWTableTemplatePreview::rightBorder(const int rows, const int cols, const int rowpos, const int colpos)
{
    if ( (rowpos<0) || (colpos<0) ) return 0;

    if ( (rowpos==0) && (colpos==0) ) // TOP LEFT CORNER
        return int( tableTemplate->pTopLeftCorner()->frameStyle()->rightBorder().width() );
    else
    if ( (rowpos==0) && ( colpos==(cols-1) ) ) // TOP RIGHT CORNER
        return int( tableTemplate->pTopRightCorner()->frameStyle()->rightBorder().width() );
    else
    if ( ( rowpos==(rows-1) ) && (colpos==0) ) // BOTTOM LEFT CORNER
        return int( tableTemplate->pBottomLeftCorner()->frameStyle()->rightBorder().width() );
    else
    if ( ( rowpos==(rows-1) ) && ( colpos==(cols-1) ) ) // BOTTOM RIGHT CORNER
        return int( tableTemplate->pBottomRightCorner()->frameStyle()->rightBorder().width() );
    else
    if ( ( rowpos==0 ) && ( colpos>0 ) && ( colpos<(cols-1) ) ) // FIRST ROW
        return int( tableTemplate->pFirstRow()->frameStyle()->rightBorder().width() );
    else
    if ( ( colpos==0 ) && ( rowpos>0 ) && ( rowpos<(rows-1) ) ) // FIRST COL
        return int( tableTemplate->pFirstCol()->frameStyle()->rightBorder().width() );
    else
    if ( ( rowpos==(rows-1) ) && ( colpos>0 ) && ( colpos<(cols-1) ) )  // LAST ROW
        return int( tableTemplate->pLastRow()->frameStyle()->rightBorder().width() );
    else
    if ( ( colpos==(cols-1) ) && ( rowpos>0 ) && ( rowpos<(rows-1) ) ) // LAST COL
        return int( tableTemplate->pLastCol()->frameStyle()->rightBorder().width() );
    else
    if ( (rowpos>0) && (colpos>0) && (rowpos<(rows-1)) && (colpos<(cols-1)) ) // BODY
        return int( tableTemplate->pBodyCell()->frameStyle()->rightBorder().width() );

    return 0;
}

void KWTableTemplatePreview::drawCell( QPainter *p, const KWTableStyle *ts, const QRect globalRect,
                                       const int rows, int cols, int rowpos, int colpos, const QString & txt)
{
    if (!ts) return;

    QRect insRect;

    p->resetXForm();
    p->setClipping( false );

    // 1. Calculate insRect
    int wid = int( globalRect.width()/cols ); // inner width
    int hei = int( globalRect.height()/rows ); // inner height

    insRect.setX( colpos*wid + globalRect.x() );
    insRect.setY( rowpos*hei + globalRect.y() );
    insRect.setWidth( static_cast<int>(wid + ts->frameStyle()->rightBorder().width()) );
    insRect.setHeight( static_cast<int>(hei + ts->frameStyle()->bottomBorder().width()) );

    // 2. Set background
    // caching
    int rb = rightBorder(rows,cols,rowpos,colpos-1);
    int bb = bottomBorder(rows,cols,rowpos-1,colpos);
    int hbb = bottomBorder(rows,cols,rowpos,colpos-1);
    int wrb = rightBorder(rows,cols,rowpos-1,colpos);

    if (rb==0)
        rb = rightBorder(rows,cols,rowpos-1,colpos-1);
    if (bb==0)
        bb = bottomBorder(rows,cols,rowpos-1,colpos-1);

    p->fillRect( QRect( colpos*wid + globalRect.x() + ( (rb <= ts->frameStyle()->rightBorder().width()) ? int(ts->frameStyle()->rightBorder().width()) : rb ),
                        rowpos*hei + globalRect.y() + ( (bb <= ts->frameStyle()->topBorder().width()) ? int(ts->frameStyle()->topBorder().width()) : bb ),
                        wid + ( ( (wrb > ts->frameStyle()->rightBorder().width()) && ((rb > ts->frameStyle()->rightBorder().width()) || ((rb==0) && (ts->frameStyle()->rightBorder().width()==0) ) ) && ((wrb-rb)>0) )  ? wrb : 0 ),
                        hei + ( ( (hbb > ts->frameStyle()->bottomBorder().width()) && ((bb > ts->frameStyle()->topBorder().width()) || ((bb==0) && (ts->frameStyle()->topBorder().width()==0) ) ) && ((hbb-bb)>0) ) ? hbb : 0 )),
                        ts->frameStyle()->backgroundColor() );

    // 2. set Style
    KoTextParag * parag = m_textdoc->firstParag();

    parag->remove( 0, parag->string()->length()-1 );
    parag->insert( 0, txt );
    parag->applyStyle( ts->paragraphStyle() );

    int widthLU = m_zoomHandler->pixelToLayoutUnitX( wid - 2 ); // keep one pixel border horizontally
    if ( m_textdoc->width() != widthLU )
    {
        // For centering to work, and to even get word wrapping when the thing is too big :)
        m_textdoc->setWidth( widthLU );
        parag->invalidate(0);
    }
    double x_add = ( (rb < ts->frameStyle()->rightBorder().width()) ?
        ts->frameStyle()->rightBorder().width() : rb ) +1;
    double y_add = ( (bb < ts->frameStyle()->topBorder().width()) ?
        ts->frameStyle()->topBorder().width() : bb ) + 1;

    QRect textRect = parag->pixelRect( m_zoomHandler );
    textRect.moveTopLeft( QPoint( insRect.x() + static_cast<int>(x_add),
                                  insRect.y() + static_cast<int>(y_add)) );


    textRect.setWidth(wid-2);

    p->setClipRect( textRect.intersect( globalRect ) );
    p->translate( textRect.x(), textRect.y() );

    QColorGroup cg = QApplication::palette().active();
    cg.setBrush( QColorGroup::Base, ts->frameStyle()->backgroundColor() );

    m_textdoc->drawWYSIWYG( p, 1, 0, textRect.width() - 1, textRect.height(), cg, m_zoomHandler );

    // 4. Set borders
    p->resetXForm();
    p->setClipping( false );

    QRect cell(globalRect.x(), globalRect.y(),
        int( globalRect.width() + ts->frameStyle()->rightBorder().width() ),
        int( globalRect.height() + ts->frameStyle()->bottomBorder().width() ));
    p->setClipRect( insRect.intersect( cell ) );

    p->translate( insRect.x(), insRect.y() );

    if (ts->frameStyle()->topBorder().width()>0) {
        p->setPen( KoBorder::borderPen(ts->frameStyle()->topBorder(),
            int(ts->frameStyle()->topBorder().width()), Qt::black) ); // Top border

        p->drawLine( 0, int( floor( ts->frameStyle()->topBorder().width()/2 ) ),
            int(wid + ts->frameStyle()->rightBorder().width()), int( floor( ts->frameStyle()->topBorder().width()/2 ) ) );
    }
    if (ts->frameStyle()->leftBorder().width()>0) {
        p->setPen( KoBorder::borderPen(ts->frameStyle()->leftBorder(),
            int(ts->frameStyle()->leftBorder().width()), Qt::black) ); // Left border

        p->drawLine( int( floor( ts->frameStyle()->leftBorder().width()/2 ) ), 0,
            int( floor( ts->frameStyle()->leftBorder().width()/2 ) ), hei + int(ts->frameStyle()->bottomBorder().width()) );
    }
    if (ts->frameStyle()->bottomBorder().width()>0) {
        p->setPen( KoBorder::borderPen(ts->frameStyle()->bottomBorder(),
            int(ts->frameStyle()->bottomBorder().width()), Qt::black) ); // Bottom border

        p->drawLine( 0, hei+int( floor( ts->frameStyle()->bottomBorder().width()/2 ) ),
            int(wid + ts->frameStyle()->rightBorder().width()), hei + int( floor( ts->frameStyle()->bottomBorder().width()/2 ) ) );
    }
    if (ts->frameStyle()->rightBorder().width()>0) {
        p->setPen( KoBorder::borderPen(ts->frameStyle()->rightBorder(),
            int(ts->frameStyle()->rightBorder().width()), Qt::black) ); // Right border

        p->drawLine( wid + int( floor( ts->frameStyle()->rightBorder().width()/2 ) ), 0,
            wid + int( floor( ts->frameStyle()->rightBorder().width()/2 ) ), hei + int(ts->frameStyle()->bottomBorder().width()) );
    }
}


void KWTableTemplatePreview::drawPreviewTable( QPainter *p, int rows, int cols, QRect globalRect )
{
    KWTableStyle *cell = 0L;
    QString txt;

    for ( int i = 0; i < rows; i++ )
    {
        for ( int j = 0; j < cols; j++ )
        {
            if ( (i==0) && (j==0) ) // TOP LEFT CORNER
                cell = tableTemplate->pTopLeftCorner();
            else
            if ( (i==0) && ( j==(cols-1) ) ) // TOP RIGHT CORNER
                cell = tableTemplate->pTopRightCorner();
            else
            if ( ( i==(rows-1) ) && (j==0) ) // BOTTOM LEFT CORNER
                cell = tableTemplate->pBottomLeftCorner();
            else
            if ( ( i==(rows-1) ) && ( j==(cols-1) ) ) // BOTTOM RIGHT CORNER
                cell = tableTemplate->pBottomRightCorner();
            else
            if ( ( i==0 ) && ( j>0 ) && ( j<(cols-1) ) ) // FIRST ROW
                cell = tableTemplate->pFirstRow();
            else
            if ( ( j==0 ) && ( i>0 ) && ( i<(rows-1) ) ) // FIRST COL
                cell = tableTemplate->pFirstCol();
            else
            if ( ( i==(rows-1) ) && ( j>0 ) && ( j<(cols-1) ) )  // LAST ROW
                cell = tableTemplate->pLastRow();
            else
            if ( ( j==(cols-1) ) && ( i>0 ) && ( i<(rows-1) ) ) // LAST COL
                cell = tableTemplate->pLastCol();
            else
            if ( (i>0) && (j>0) && (i<(rows-1)) && (j<(cols-1)) ) // BODY
                cell = tableTemplate->pBodyCell();

            if ( (i==0) && (j==0) ) // TOP LEFT CORNER
                txt = m_contents[0][0];
            else
            if ( ( i==0 ) && ( j>0 ) )
                txt = m_contents[i][j];
            else
            if ( ( j==0 ) && ( i>0 ) )
                txt = m_contents[1][i];
            else
                txt = QString::number(i) + QString::number(j);

            drawCell( p, cell, globalRect, rows, cols, i, j, txt );
        }
    }
}

void KWTableTemplatePreview::drawContents( QPainter *p )
{
    p->save();
    QRect r = contentsRect();

    // 1. Draw fake document white background
    p->fillRect( QRect( 10, 20, r.width() - 20, r.height() - 20 ), QColor("white") );

    // 2. Draw preview table
    if (tableTemplate) {

        QRect tableRect;
        int x,y;

        // x
        if ( tableTemplate->pTopRightCorner()->frameStyle()->rightBorder().width() >
             tableTemplate->pLastCol()->frameStyle()->rightBorder().width() )
             x = int( tableTemplate->pTopRightCorner()->frameStyle()->rightBorder().width() );
        else
             x = int( tableTemplate->pLastCol()->frameStyle()->rightBorder().width() );
        if ( tableTemplate->pBottomRightCorner()->frameStyle()->rightBorder().width() > x )
             x = int( tableTemplate->pBottomRightCorner()->frameStyle()->rightBorder().width() );

        // y
        if ( tableTemplate->pBottomRightCorner()->frameStyle()->bottomBorder().width() >
             tableTemplate->pLastRow()->frameStyle()->bottomBorder().width() )
             y = int( tableTemplate->pBottomRightCorner()->frameStyle()->bottomBorder().width() );
        else
             y = int( tableTemplate->pLastRow()->frameStyle()->bottomBorder().width() );
        if ( tableTemplate->pBottomLeftCorner()->frameStyle()->bottomBorder().width() > y )
             y = int( tableTemplate->pBottomLeftCorner()->frameStyle()->leftBorder().width() );

        tableRect.setX( 20 - int(x/2) );
        tableRect.setY( 30 - int(y/2) );
        tableRect.setWidth( r.width() - 40 );
        tableRect.setHeight( r.height() - 40 );

        drawPreviewTable( p, 5, 4, tableRect );
    }

    p->restore();
}

void KWTableTemplatePreview::setSpecialCells( KWTableTemplate *_tableTemplate )
{
    if (_tableTemplate->pFirstRow()==_tableTemplate->pBodyCell())
        tableTemplate->setFirstRow( tableTemplate->pBodyCell() );
    else
        tableTemplate->setFirstRow( _tableTemplate->pFirstRow() );

    if (_tableTemplate->pFirstCol()==_tableTemplate->pBodyCell())
        tableTemplate->setFirstCol( tableTemplate->pBodyCell() );
    else
        tableTemplate->setFirstCol( _tableTemplate->pFirstCol() );

    if (_tableTemplate->pLastRow()==_tableTemplate->pBodyCell())
        tableTemplate->setLastRow( tableTemplate->pBodyCell() );
    else
        tableTemplate->setLastRow( _tableTemplate->pLastRow() );

    if (_tableTemplate->pLastCol()==_tableTemplate->pBodyCell())
        tableTemplate->setLastCol( tableTemplate->pBodyCell() );
    else
        tableTemplate->setLastCol( _tableTemplate->pLastCol() );

    if (_tableTemplate->pTopLeftCorner()==_tableTemplate->pBodyCell())
        tableTemplate->setTopLeftCorner( tableTemplate->pBodyCell() );
    else
        tableTemplate->setTopLeftCorner( _tableTemplate->pTopLeftCorner() );

    if (_tableTemplate->pTopRightCorner()==_tableTemplate->pBodyCell())
        tableTemplate->setTopRightCorner( tableTemplate->pBodyCell() );
    else
        tableTemplate->setTopRightCorner( _tableTemplate->pTopRightCorner() );

    if (_tableTemplate->pBottomLeftCorner()==_tableTemplate->pBodyCell())
        tableTemplate->setBottomLeftCorner( tableTemplate->pBodyCell() );
    else
        tableTemplate->setBottomLeftCorner( _tableTemplate->pBottomLeftCorner() );

    if (_tableTemplate->pBottomRightCorner()==_tableTemplate->pBodyCell())
        tableTemplate->setBottomRightCorner( tableTemplate->pBodyCell() );
    else
        tableTemplate->setBottomRightCorner( _tableTemplate->pBottomRightCorner() );
}

void KWTableTemplatePreview::setTableTemplate( KWTableTemplate *_tableTemplate )
{
    origTableTemplate = _tableTemplate;

    delete tableTemplate;

    tableTemplate = new KWTableTemplate(_tableTemplate->displayName());
    tableTemplate->setBodyCell( _tableTemplate->pBodyCell() );

    setSpecialCells(_tableTemplate);

    repaint( true );
}

void KWTableTemplatePreview::fillContents()
{
    // First row
    m_contents[0][0] = "A";
    m_contents[0][1] = "B";
    m_contents[0][2] = "C";
    m_contents[0][3] = "D";
    m_contents[0][4] = "E";
    // First col
    m_contents[1][0] = ""; // Is overwritten by m_contents[0][0]
    m_contents[1][1] = "I";
    m_contents[1][2] = "II";
    m_contents[1][3] = "III";
    m_contents[1][4] = "IV";

}

KWTableTemplate* KWTableTemplatePreview::getTableTemplate() const
{
      return tableTemplate;
}

void KWTableTemplatePreview::cbFirstRowChanged( bool enable )
{
    if ( enable && (origTableTemplate->pFirstRow()!=origTableTemplate->pBodyCell()) )
        tableTemplate->setFirstRow( origTableTemplate->pFirstRow() );
    else
        tableTemplate->setFirstRow( tableTemplate->pBodyCell() );

    if ( enable && ( origTableTemplate->pTopLeftCorner()==origTableTemplate->pFirstRow() ) )
        tableTemplate->setTopLeftCorner( origTableTemplate->pTopLeftCorner() );
    else
    if ( (!enable) && ( origTableTemplate->pTopLeftCorner()==origTableTemplate->pFirstRow() ) )
        tableTemplate->setTopLeftCorner( tableTemplate->pBodyCell() );

    if ( enable && ( origTableTemplate->pTopRightCorner()==origTableTemplate->pFirstRow() ) )
        tableTemplate->setTopRightCorner( origTableTemplate->pTopRightCorner() );
    else
    if ( (!enable) && ( origTableTemplate->pTopRightCorner()==origTableTemplate->pFirstRow() ) )
        tableTemplate->setTopRightCorner( tableTemplate->pBodyCell() );
    if ( !m_disableRepaint )
        repaint( true );
}

void KWTableTemplatePreview::cbFirstColChanged( bool enable )
{
    if ( enable && (origTableTemplate->pFirstCol()!=origTableTemplate->pBodyCell()) )
        tableTemplate->setFirstCol( origTableTemplate->pFirstCol() );
    else
        tableTemplate->setFirstCol( tableTemplate->pBodyCell() );

    if ( enable && ( origTableTemplate->pTopLeftCorner()==origTableTemplate->pFirstCol() ) )
        tableTemplate->setTopLeftCorner( origTableTemplate->pTopLeftCorner() );
    else
    if ( (!enable) && ( origTableTemplate->pTopLeftCorner()==origTableTemplate->pFirstCol() ) )
        tableTemplate->setTopLeftCorner( tableTemplate->pBodyCell() );

    if ( enable && ( origTableTemplate->pBottomLeftCorner()==origTableTemplate->pFirstCol() ) )
        tableTemplate->setBottomLeftCorner( origTableTemplate->pBottomLeftCorner() );
    else
    if ( (!enable) && ( origTableTemplate->pBottomLeftCorner()==origTableTemplate->pFirstCol() ) )
        tableTemplate->setBottomLeftCorner( tableTemplate->pBodyCell() );

    if ( !m_disableRepaint )
        repaint( true );
}

void KWTableTemplatePreview::cbLastRowChanged( bool enable )
{
    if ( enable && (origTableTemplate->pLastRow()!=origTableTemplate->pBodyCell()) )
        tableTemplate->setLastRow( origTableTemplate->pLastRow() );
    else
        tableTemplate->setLastRow( tableTemplate->pBodyCell() );

    if ( enable && ( origTableTemplate->pBottomRightCorner()==origTableTemplate->pLastRow() ) )
        tableTemplate->setBottomRightCorner( origTableTemplate->pBottomRightCorner() );
    else
    if ( (!enable) && ( origTableTemplate->pBottomRightCorner()==origTableTemplate->pLastRow() ) )
        tableTemplate->setBottomRightCorner( tableTemplate->pBodyCell() );

    if ( enable && ( origTableTemplate->pBottomLeftCorner()==origTableTemplate->pLastRow() ) )
        tableTemplate->setBottomLeftCorner( origTableTemplate->pBottomLeftCorner() );
    else
    if ( (!enable) && ( origTableTemplate->pBottomLeftCorner()==origTableTemplate->pLastRow() ) )
        tableTemplate->setBottomLeftCorner( tableTemplate->pBodyCell() );

    if ( !m_disableRepaint )
        repaint( true );
}

void KWTableTemplatePreview::cbLastColChanged( bool enable )
{
    if ( enable && (origTableTemplate->pLastCol()!=origTableTemplate->pBodyCell()) )
        tableTemplate->setLastCol( origTableTemplate->pLastCol() );
    else
        tableTemplate->setLastCol( tableTemplate->pBodyCell() );

    if ( enable && ( origTableTemplate->pTopRightCorner()==origTableTemplate->pLastCol() ) )
        tableTemplate->setTopRightCorner( origTableTemplate->pTopRightCorner() );
    else
    if ( (!enable) && ( origTableTemplate->pTopRightCorner()==origTableTemplate->pLastCol() ) )
        tableTemplate->setTopRightCorner( tableTemplate->pBodyCell() );

    if ( enable && ( origTableTemplate->pBottomRightCorner()==origTableTemplate->pLastCol() ) )
        tableTemplate->setBottomRightCorner( origTableTemplate->pBottomRightCorner() );
    else
    if ( (!enable) && ( origTableTemplate->pBottomRightCorner()==origTableTemplate->pLastCol() ) )
        tableTemplate->setBottomRightCorner( tableTemplate->pBodyCell() );

    if ( !m_disableRepaint )
        repaint( true );
}

void KWTableTemplatePreview::cbBodyChanged( bool enable )
{
    KWTableTemplate *oldTemplate = new KWTableTemplate( *tableTemplate );

    if ( enable )
        tableTemplate->setBodyCell( origTableTemplate->pBodyCell() );
    else
        tableTemplate->setBodyCell( m_emptyStyle );

    setSpecialCells(oldTemplate);

    delete oldTemplate;

    if ( !m_disableRepaint )
        repaint( true );

}


/******************************************************************/
/* Class: KWTableTemplateSelector                                    */
/******************************************************************/

KWTableTemplateSelector::KWTableTemplateSelector( KWDocument *_doc, QWidget *_parent, const QString & _tableTemplate, int _type,const char *_name)
    : QWidget( _parent, _name )
{
    m_doc = _doc;

    Q3GridLayout *grid = new Q3GridLayout( this, 3, 2, 0, KDialog::spacingHint() );

    QWidget *innerHolder = new QWidget( this );
    Q3GridLayout *innerGrid = new Q3GridLayout( innerHolder, 2, 1, 0, KDialog::spacingHint() );

    lbTemplates = new Q3ListBox( innerHolder );

    Q3PtrListIterator<KWTableTemplate> it( m_doc->tableTemplateCollection()->tableTemplateList() );
    for ( ; it.current() ; ++it )
    {
        lbTemplates->insertItem( it.current()->displayName() );
    }

    innerGrid->addWidget( lbTemplates, 0, 0 );

    pbCustomize = new QPushButton( i18n( "&Customize" ), innerHolder );
    pbCustomize->setEnabled(false);
    pbCustomize->hide();
    innerGrid->addWidget( pbCustomize, 1, 0 );

    grid->addMultiCellWidget( innerHolder, 0, 2, 0, 0 );

    preview = new KWTableTemplatePreview( i18n( "Preview" ), m_doc->tableStyleCollection()->findStyle("Plain"), this );
    grid->addWidget( preview, 0, 1 );

    bgCustomize = new Q3ButtonGroup( 3, Qt::Horizontal, i18n( "Apply To" ), this );

    cbFirstRow = new QCheckBox( i18n( "First row" ), bgCustomize );
    cbLastRow = new QCheckBox( i18n( "Last row" ), bgCustomize );
    cbBody = new QCheckBox( i18n( "Body" ), bgCustomize );
    cbFirstCol = new QCheckBox( i18n( "First column" ), bgCustomize );
    cbLastCol = new QCheckBox( i18n( "Last column" ), bgCustomize );

    grid->addWidget( bgCustomize, 1, 1 );

    grid->setRowStretch( 0, 1 );
    grid->setRowStretch( 1, 0 );
    grid->addColSpacing( 0, innerHolder->width() );
    grid->setColStretch( 0, 0 );
    grid->setColStretch( 1, 1 );
    grid->activate();

    connect( cbFirstRow, SIGNAL( toggled( bool ) ), preview, SLOT( cbFirstRowChanged( bool ) ) );
    connect( cbFirstCol, SIGNAL( toggled( bool ) ), preview, SLOT( cbFirstColChanged( bool ) ) );
    connect( cbLastRow, SIGNAL( toggled( bool ) ), preview, SLOT( cbLastRowChanged( bool ) ) );
    connect( cbLastCol,  SIGNAL( toggled( bool ) ), preview, SLOT( cbLastColChanged( bool ) ) );
    connect( cbBody,  SIGNAL( toggled( bool ) ), preview, SLOT( cbBodyChanged( bool ) ) );

    connect( lbTemplates, SIGNAL(  selectionChanged () ), this, SLOT( changeTableTemplate() ) );
    Q3ListBoxItem * item = lbTemplates->findItem( _tableTemplate );
    int index = 0;
    if ( item  )
        index = lbTemplates->index ( item );
    if (m_doc->tableTemplateCollection()->tableTemplateList().count() > 0) {
      preview->setTableTemplate( m_doc->tableTemplateCollection()->tableTemplateAt(index) );
      selectedTableTemplate = 0L;
      lbTemplates->setSelected( index, true );
    }
    initFormat( _type );
}

void KWTableTemplateSelector::changeTableTemplate()
{
    preview->setTableTemplate( m_doc->tableTemplateCollection()->tableTemplateAt( lbTemplates->currentItem() ) );
    selectedTableTemplate = lbTemplates->currentItem();
    initPreview();
}

KWTableTemplate* KWTableTemplateSelector::getTableTemplate() const
{
  return (selectedTableTemplate) ? preview->getTableTemplate() : 0L;
}

void KWTableTemplateSelector::initFormat( int format)
{
    if ( format & FirstRow)
        cbFirstRow->setChecked( true );

    if ( format & FirstColumn)
        cbFirstCol->setChecked( true );

    if ( format & LastRow)
        cbLastRow->setChecked( true );

    if ( format & LastCol)
        cbLastCol->setChecked( true );

    if ( format & Body)
        cbBody->setChecked( true );
    initPreview();
}

void KWTableTemplateSelector::initPreview()
{
    preview->disableRepaint(true);
    preview->cbFirstRowChanged( cbFirstRow->isChecked() );
    preview->cbFirstColChanged( cbFirstCol->isChecked(  ) );
    preview->cbLastRowChanged(cbLastRow->isChecked() );
    preview->cbLastColChanged( cbLastCol->isChecked() );
    preview->cbBodyChanged( cbBody->isChecked() );
    preview->disableRepaint(false);
    preview->repaint();
}

int KWTableTemplateSelector::getFormatType() const
{
    int type = 0;
    if ( cbFirstRow->isChecked())
    {
        type = type | FirstRow;
    }
    if ( cbFirstCol->isChecked())
    {
        type = type |FirstColumn;

    }
    if ( cbLastRow->isChecked())
    {
        type = type | LastRow;

    }
    if ( cbLastCol->isChecked())
    {
        type = type | LastCol;
    }
    if ( cbBody->isChecked())
    {
        type = type | Body;
    }
    return type;
}
