/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2005 Thomas Zander <zander@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#include "KWCanvas.h"
#include "KWTableDia.h"
#include "KWTableDia.moc"

#include "KWTableTemplateSelector.h"
#include "KWCommand.h"
#include "KWDocument.h"
#include "KWTableFrameSet.h"
#include "KWTableTemplate.h"
#include "KWPageManager.h"
#include "KWPage.h"
#include <kcommand.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qcombobox.h>

#include <klocale.h>

#include <stdlib.h>


/******************************************************************/
/* Class: KWTablePreview                                          */
/******************************************************************/

void KWTablePreview::paintEvent( QPaintEvent * )
{
    int wid = ( width() - 10 ) / cols;
    int hei = ( height() - 10 ) / rows;

    QPainter p;
    p.begin( this );

    p.setPen( QPen( black ) );

    for ( int i = 0; i < rows; i++ )
    {
        for ( int j = 0; j < cols; j++ )
            p.drawRect( j * wid + 5, i * hei + 5, wid + 1, hei + 1 );
    }

    p.end();
}

/******************************************************************/
/* Class: KWTableDia                                              */
/******************************************************************/

KWTableDia::KWTableDia( QWidget* parent, const char* name, UseMode _useMode, KWCanvas *_canvas, KWDocument *_doc,
        int rows, int cols, CellSize wid, CellSize hei, bool floating , const QString & _templateName, int format)
    : KDialogBase( Tabbed, i18n("Table Settings"), Ok | Cancel, Ok, parent, name, true)
{
    m_useMode = _useMode;
    canvas = _canvas;
    doc = _doc;

    setupTab1( rows, cols, wid, hei, floating );
    setupTab2( _templateName,format );

    setInitialSize( QSize(500, 480) );

    oldRowCount = rows;
    oldColCount = cols;
    oldTemplateName = _templateName;
#ifdef ALLOW_NON_INLINE_TABLES
    if ( m_useMode==NEW )
    {
        slotInlineTable( cbIsFloating->isChecked());
    }
#endif
}

void KWTableDia::setupTab1( int rows, int cols, CellSize wid, CellSize hei, bool floating )
{
    tab1 = addPage( i18n( "Geometry" ) );

    QGridLayout *grid = new QGridLayout( tab1, 9, 2, 0, KDialog::spacingHint() );

    lRows = new QLabel( i18n( "Number of rows:" ), tab1 );
    grid->addWidget( lRows, 0, 0 );

    nRows = new QSpinBox( 1, 128, 1, tab1 );
    nRows->setValue( rows );
    grid->addWidget( nRows, 1, 0 );

    lCols = new QLabel( i18n( "Number of columns:" ), tab1 );
    grid->addWidget( lCols, 2, 0 );

    nCols = new QSpinBox( 1, 128, 1, tab1 );
    nCols->setValue( cols );
    grid->addWidget( nCols, 3, 0 );

#ifdef ALLOW_NON_INLINE_TABLES
    if ( m_useMode==NEW )
    {
        lHei = new QLabel( i18n( "Cell heights:" ), tab1 );
        grid->addWidget( lHei, 4, 0 );

        cHei = new QComboBox( FALSE, tab1 );
        cHei->insertItem( i18n( "Automatic" ) );
        cHei->insertItem( i18n( "Manual" ) );
        cHei->setCurrentItem( (int)hei );
        grid->addWidget( cHei, 5, 0 );

        lWid = new QLabel( i18n( "Cell widths:" ), tab1 );
        grid->addWidget( lWid, 6, 0 );

        cWid = new QComboBox( FALSE, tab1 );
        cWid->insertItem( i18n( "Automatic" ) );
        cWid->insertItem( i18n( "Manual" ) );
        cWid->setCurrentItem( (int)wid );
        grid->addWidget( cWid, 7, 0 );
    }
#else
    Q_UNUSED( wid );
    Q_UNUSED( hei );
    Q_UNUSED( floating );
#endif

    preview = new KWTablePreview( tab1, rows, cols );
    preview->setBackgroundColor( white );
    grid->addMultiCellWidget( preview, 0, 8, 1, 1 );

    if ( m_useMode==NEW )
    {
#ifdef ALLOW_NON_INLINE_TABLES
        // Checkbox for floating/fixed location. The default is floating (aka inline).
        cbIsFloating = new QCheckBox( i18n( "The table is &inline" ), tab1 );
        //cbIsFloating->setEnabled(false);
        cbIsFloating->setChecked( floating );

        grid->addMultiCellWidget( cbIsFloating, 9, 9, 0, 2 );
        connect( cbIsFloating, SIGNAL( toggled ( bool )  ), this, SLOT( slotInlineTable( bool ) ) );
#endif
    }
    else
    if (m_useMode==EDIT)
    {
        cbReapplyTemplate1 = new QCheckBox( i18n("Reapply template to table"), tab1 );
        grid->addMultiCellWidget( cbReapplyTemplate1, 9, 9, 0, 2);

        connect( cbReapplyTemplate1, SIGNAL( toggled ( bool )  ), this, SLOT( slotSetReapply( bool ) ) );
    }

    grid->addRowSpacing( 0, lRows->height() );
    grid->addRowSpacing( 1, nRows->height() );
    grid->addRowSpacing( 2, lCols->height() );
    grid->addRowSpacing( 3, nCols->height() );
#ifdef ALLOW_NON_INLINE_TABLES
    if ( m_useMode==NEW )
    {
        grid->addRowSpacing( 4, lHei->height() );
        grid->addRowSpacing( 5, cHei->height() );
        grid->addRowSpacing( 6, lWid->height() );
        grid->addRowSpacing( 7, cWid->height() );
    }
#endif
    grid->addRowSpacing( 8, 150 - ( lRows->height() + nRows->height() + lCols->height() + nCols->height() ) );
#ifdef ALLOW_NON_INLINE_TABLES
    if ( m_useMode==NEW )
        grid->addRowSpacing( 9, cbIsFloating->height() );
#endif
    grid->setRowStretch( 0, 0 );
    grid->setRowStretch( 1, 0 );
    grid->setRowStretch( 2, 0 );
    grid->setRowStretch( 3, 0 );
    grid->setRowStretch( 4, 0 );
    grid->setRowStretch( 5, 0 );
    grid->setRowStretch( 6, 0 );
    grid->setRowStretch( 7, 0 );
    grid->setRowStretch( 8, 1 );
    grid->setRowStretch( 9, 0 );

    grid->addColSpacing( 0, lRows->width() );
    grid->addColSpacing( 0, nRows->width() );
    grid->addColSpacing( 0, lCols->width() );
    grid->addColSpacing( 0, nCols->width() );
#ifdef ALLOW_NON_INLINE_TABLES
    if ( m_useMode==NEW )
    {
        grid->addColSpacing( 0, lHei->width() );
        grid->addColSpacing( 0, cHei->width() );
        grid->addColSpacing( 0, lWid->width() );
        grid->addColSpacing( 0, cWid->width() );
    }
#endif
    grid->addColSpacing( 1, 150 );
    grid->setColStretch( 0, 0 );
    grid->setColStretch( 1, 1 );

    grid->activate();

    connect( nRows, SIGNAL( valueChanged( int ) ), this, SLOT( rowsChanged( int ) ) );
    connect( nCols, SIGNAL( valueChanged( int ) ), this, SLOT( colsChanged( int ) ) );
}

void KWTableDia::setupTab2(const QString & _templateName, int format )
{
    QWidget *tab2 = addPage( i18n("Templates"));

    QGridLayout *grid = new QGridLayout( tab2, 2, 1, 0, KDialog::spacingHint() );

    tableTemplateSelector = new KWTableTemplateSelector( doc, tab2, _templateName,format );
    grid->addWidget(tableTemplateSelector, 0, 0);

    if (m_useMode==EDIT)
    {
        cbReapplyTemplate2 = new QCheckBox( i18n("Reapply template to table"), tab2 );
        grid->addWidget( cbReapplyTemplate2, 1, 0);

        grid->setRowStretch( 0, 1);
        grid->setRowStretch( 1, 0);

        connect( cbReapplyTemplate2, SIGNAL( toggled ( bool )  ), this, SLOT( slotSetReapply( bool ) ) );
    }
    grid->activate();
}

void KWTableDia::slotOk()
{
    if ( m_useMode==NEW )
        /// ###### This should be done AFTER this dialog is closed.
        // Otherwise we have two modal dialogs fighting each other
        canvas->createTable( nRows->value(), nCols->value(),
#ifdef ALLOW_NON_INLINE_TABLES
                             cWid->currentItem(),
                             cHei->currentItem(),
                             cbIsFloating->isChecked(),
#else
                             0,
                             0,
                             true,
#endif
                             tableTemplateSelector->getTableTemplate(),
                             tableTemplateSelector->getFormatType());
    else
    {
        KWTableFrameSet *table = canvas->getCurrentTable();
        if ( table )
        {
            KMacroCommand *macroCmd = 0L;
            KCommand *cmd = 0L;

            // Add or delete rows
            int rowsDiff = nRows->value()-oldRowCount;
            if ( rowsDiff!=0 )
            {
                macroCmd = new KMacroCommand( (rowsDiff>0 ) ? i18n("Add New Rows to Table") : i18n("Remove Rows From Table") );
                for ( int i = 0 ; i < abs( rowsDiff ) ; i++ )
                {
                    if ( rowsDiff < 0 )
                        cmd = new KWRemoveRowCommand( i18n("Remove Row"), table, oldRowCount-i-1 );
                    else
                    {
                        cmd = new KWInsertRowCommand( i18n("Insert Row"), table, oldRowCount+i );
                    }

                    if (cmd)
                        macroCmd->addCommand( cmd );
                }
                canvas->setTableRows( nRows->value() );
            }

            // Add or delete cols
            int colsDiff = nCols->value()-oldColCount;
            if ( colsDiff!=0 )
            {
                double maxRightOffset;
                if (table->isFloating())
                    // inline table: max offset of containing frame
                    maxRightOffset = table->anchorFrameset()->frame(0)->right();
                else { // non inline table: max offset of the page
                    KWPage *page = doc->pageManager()->page(table->cell(0,0)->frame(0));
                    maxRightOffset = page->width() - page->rightMargin();
                }
                if ( !macroCmd )
                    macroCmd = new KMacroCommand( (colsDiff>0 ) ? i18n("Add New Columns to Table") : i18n("Remove Columns From Table") );
                cmd = 0L;
                for ( int i = 0 ; i < abs( colsDiff ) ; i++ )
                {
                    if ( colsDiff < 0 )
                        cmd = new KWRemoveColumnCommand( i18n("Remove Column"), table, oldColCount-i-1 );
                    else
                    {
                        cmd = new KWInsertColumnCommand( i18n("Insert Column"), table, oldColCount+i,  maxRightOffset);
                    }

                    if (cmd)
                        macroCmd->addCommand( cmd );
                }
                canvas->setTableCols( nCols->value() );
            }

            // Apply template
            if ( tableTemplateSelector->getTableTemplate() && (( oldTemplateName!=tableTemplateSelector->getTableTemplate()->name() ) || (cbReapplyTemplate1->isChecked()) ))
            {
                if ( !macroCmd )
                    macroCmd = new KMacroCommand( i18n("Apply Template to Table") );
                KWTableTemplateCommand *ttCmd=new KWTableTemplateCommand( i18n("Apply Template to Table"), table, tableTemplateSelector->getTableTemplate() );
                macroCmd->addCommand( ttCmd );
                canvas->setTableTemplateName( tableTemplateSelector->getTableTemplate()->name() );
            }
            if ( macroCmd )
            {
                macroCmd->execute();
                doc->addCommand( macroCmd );
            }
        }
    }
    KDialogBase::slotOk();
}

void KWTableDia::rowsChanged( int _rows )
{
    preview->setRows( _rows );
}

void KWTableDia::colsChanged( int _cols )
{
    preview->setCols( _cols );
}

void KWTableDia::slotSetReapply( bool _reapply )
{
    if ( cbReapplyTemplate1->isChecked()!=_reapply ) cbReapplyTemplate1->setChecked( _reapply );
    if ( cbReapplyTemplate2->isChecked()!=_reapply ) cbReapplyTemplate2->setChecked( _reapply );
}

void KWTableDia::slotInlineTable( bool state)
{
#ifdef ALLOW_NON_INLINE_TABLES
    if ( m_useMode==NEW )
    {
        lWid->setEnabled( !state );
        lHei->setEnabled( !state );
        cHei->setEnabled( !state );
        cWid->setEnabled( !state );
    }
#else
    Q_UNUSED( state );
#endif
}
