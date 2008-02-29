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

#include <KFileDialog>
#include <KMessageBox>
#include <QFile>

FormulaToolWidget::FormulaToolWidget( QWidget* parent )
                  : QTabWidget( parent )
{
    setupUi( this );

    connect( buttonLoad, SIGNAL( clicked() ), this, SLOT( slotLoadFormula() ) );
    connect( buttonSave, SIGNAL( clicked() ), this, SLOT( slotSaveFormula() ) );
}

FormulaToolWidget::~FormulaToolWidget()
{
}

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
    KoShapeLoadingContext shapeContext( odfContext, 0 );

    // setup a DOM structure and start the actual loading process
    KoXmlDocument tmpDocument;
    tmpDocument.setContent( &file, false, 0, 0, 0 );
    m_tool->shape()->loadOdf( tmpDocument.documentElement(), shapeContext );
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
