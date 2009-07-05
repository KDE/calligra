/* This file is part of the KDE project
   Copyright (C) 2007 Martin Pfeiffer <hubipete@gmx.net>

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

#include "FormulaToolWidget.h"
#include "KoFormulaTool.h"
#include "KoFormulaShape.h"
#include "ElementFactory.h"
#include "BasicElement.h"
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

#include <KoShapeSavingContext.h>
#include <KoShapeLoadingContext.h>
#include <KoOdfLoadingContext.h>
#include <KoOdfStylesReader.h>
#include <KoGenStyles.h>
#include <KoEmbeddedDocumentSaver.h>

#include <KAction>

#include <KFileDialog>
#include <KMessageBox>
#include <QFile>
#include <QMenu>

FormulaToolWidget::FormulaToolWidget( KoFormulaTool* tool, QWidget* parent )
                  : QTabWidget( parent )
{
    m_tool = tool;
    setupUi( this );

    // setup the element insert menus
    m_fractionMenu.addAction( m_tool->action( "insert_fraction" ) );
    m_fenceMenu.addAction( m_tool->action( "insert_fence" ) );
    m_tableMenu.addAction( m_tool->action( "insert_33table" ) );
    m_tableMenu.addAction( m_tool->action( "insert_22table" ) );
    m_tableMenu.addAction( m_tool->action( "insert_tablerow" ) );
    m_tableMenu.addAction( m_tool->action( "insert_tablecol" ) );
    m_rootMenu.addAction( m_tool->action( "insert_root" ) );
    m_rootMenu.addAction( m_tool->action( "insert_sqrt" ) );
    m_scriptsMenu.addAction( m_tool->action( "insert_subscript" ) );
    m_scriptsMenu.addAction( m_tool->action( "insert_supscript" ) );
    m_scriptsMenu.addAction( m_tool->action( "insert_subsupscript" ) );

    // assign menus to toolbuttons
    buttonFence->setMenu( &m_fenceMenu );
    buttonFence->setDefaultAction( m_tool->action( "insert_fence" ) );
    buttonRoot->setMenu( &m_rootMenu );
    buttonRoot->setDefaultAction( m_tool->action( "insert_root" ) );
    buttonFraction->setMenu( &m_fractionMenu );
    buttonTable->setMenu( &m_tableMenu );
    buttonScript->setMenu( &m_scriptsMenu );

    // connect signals to the slots
    connect( buttonFraction, SIGNAL( triggered( QAction* ) ),
             m_tool, SLOT( insert( QAction* ) ) );
    connect( buttonFence, SIGNAL( triggered( QAction* ) ),
             m_tool, SLOT( insert( QAction* ) ) );
    connect( buttonTable, SIGNAL( triggered( QAction* ) ),
             m_tool, SLOT( insert( QAction* ) ) );
    connect( buttonRoot, SIGNAL( triggered( QAction* ) ),
             m_tool, SLOT( insert( QAction* ) ) );
    connect( buttonLoad, SIGNAL( clicked() ), this, SLOT( slotLoadFormula() ) );
    connect( buttonSave, SIGNAL( clicked() ), this, SLOT( slotSaveFormula() ) );
}

FormulaToolWidget::~FormulaToolWidget()
{}

void FormulaToolWidget::slotLoadFormula()
{
    // get an url
    KUrl url = KFileDialog::getOpenUrl();
    if( url.isEmpty() || !m_tool->shape() )
        return;

    // open the file the url points to
    QFile file( url.path() );
    if( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
        return;

    KoOdfStylesReader stylesReader;
    KoOdfLoadingContext odfContext( stylesReader, 0 );
    QMap<QString, KoDataCenter *> dataCenterMap;
    KoShapeLoadingContext shapeContext( odfContext, dataCenterMap );

    // setup a DOM structure and start the actual loading process
    KoXmlDocument tmpDocument;
    tmpDocument.setContent( &file, false, 0, 0, 0 );
    m_tool->shape()->update(); //clear the area 
    m_tool->shape()->loadOdf( tmpDocument.documentElement(), shapeContext );
    m_tool->shape()->update(); //paint our shape again
    m_tool->resetFormulaCursor();
}

void FormulaToolWidget::slotSaveFormula()
{
    KUrl url = KFileDialog::getSaveUrl();
    if( url.isEmpty() || !m_tool->shape() )
        return;

    QFile file( url.path() );
    KoXmlWriter writer( &file );
    KoGenStyles styles;
    KoEmbeddedDocumentSaver embeddedSaver;
    KoShapeSavingContext shapeSavingContext( writer, styles, embeddedSaver );

    // TODO this should not use saveOdf 
    m_tool->shape()->saveOdf( shapeSavingContext );
}

void FormulaToolWidget::setFormulaTool( KoFormulaTool* tool )
{
    m_tool = tool;
}

#include "FormulaToolWidget.moc"
