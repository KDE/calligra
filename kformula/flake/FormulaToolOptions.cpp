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

#include "FormulaToolOptions.h"
#include "KoFormulaTool.h"
#include "KoFormulaShape.h"
#include "ElementFactory.h"
#include "AttributeManager.h"
#include "BasicElement.h"
#include "FormulaRenderer.h"
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

#include <KoShapeSavingContext.h>
#include <KoShapeLoadingContext.h>
#include <KoOasisLoadingContext.h>
#include <KoOdfStylesReader.h>
#include <KoGenStyles.h>
#include <KoEmbeddedDocumentSaver.h>

#include <KFileDialog>
#include <KMessageBox>
#include <KStandardDirs>
#include <QPainter>
#include <QPixmap>
#include <QFile>
#include <QBuffer>
#include <QPushButton>
#include <QComboBox>
#include <QListWidget>
#include <QGridLayout>

FormulaToolOptions::FormulaToolOptions( QWidget* parent, Qt::WindowFlags f )
                  : QWidget( parent, f )
{
    m_templateCombo = new QComboBox( this );
    m_templateCombo->setInsertPolicy( QComboBox::NoInsert );
    m_templateCombo->addItem( "General" );
    m_templateCombo->addItem( "Operators" );
    m_templateCombo->addItem( "Functions" );
    m_templateCombo->addItem( "Custom" );

    m_templateList = new QListWidget( this );
    m_templateList->setViewMode( QListView::IconMode );
    m_templateList->setSelectionMode( QAbstractItemView::SingleSelection );
    m_templateList->setDragEnabled( true );
    m_loadFormula = new QPushButton( this );
    m_loadFormula->setText( "Load Formula" );
    m_saveFormula = new QPushButton( this );
    m_saveFormula->setText( "Save Formula" );
    m_saveFormula->setEnabled( false );

    m_layout = new QGridLayout( this );
    m_layout->addWidget( m_templateCombo, 0, 0, 1, 2 );
    m_layout->addWidget( m_templateList, 1, 0, 1, 2 );
    m_layout->addWidget( m_loadFormula, 2, 0 );
    m_layout->addWidget( m_saveFormula, 2, 1 );

    connect( m_loadFormula, SIGNAL( clicked() ), this, SLOT( slotLoadFormula() ) );
    connect( m_saveFormula, SIGNAL( clicked() ), this, SLOT( slotSaveFormula() ) );
}

FormulaToolOptions::~FormulaToolOptions()
{
    delete m_layout;
    delete m_templateCombo;
    delete m_templateList;
    delete m_loadFormula;
    delete m_saveFormula;
}

void FormulaToolOptions::slotLoadFormula()
{
    KUrl url = KFileDialog::getOpenUrl();
    if( url.isEmpty() || !m_tool->shape() )
        return;

    QFile file( url.path() );
    if( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
        return;

    KoOdfStylesReader stylesReader;
    KoOasisLoadingContext oasisContext( 0, stylesReader, 0 );
    KoShapeLoadingContext shapeContext( oasisContext );

    KoXmlDocument tmpDocument;
    tmpDocument.setContent( &file, false, 0, 0, 0 );
    m_tool->shape()->loadOdf( tmpDocument.documentElement(), shapeContext );
}

void FormulaToolOptions::slotSaveFormula()
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

void FormulaToolOptions::slotTemplateComboIndexChange( int index )
{
    m_templateList->clear();                  // empty the current list
    QList<QListWidgetItem*> tmpList;
    QString path;
    switch( index ) {
        case 0:                               // General selected
            tmpList = m_general;
            path = KStandardDirs::locate( "data", "general.xml" );
            break;
        case 1:                               // Operators selected
            tmpList = m_operators;
            path = KStandardDirs::locate( "data", "operators.xml" );
            break;
        case 2:                               // Functions selected
            tmpList = m_functions;
            path = KStandardDirs::locate( "data", "functions.xml" );
            break;
        case 3:                               // Custom selected
            tmpList = m_custom;
            path = KStandardDirs::locate( "data", "custom.xml" );
            break;
    }

    if( tmpList.isEmpty() )                  // optionally load templates first
        loadTemplates( &tmpList, path );

    foreach( QListWidgetItem* item, tmpList ) // refill template list
        m_templateList->addItem( item );
}

void FormulaToolOptions::loadTemplates( QList<QListWidgetItem*>* list, const QString& p )
{
    QFile file( p );  // open template file
    if( !file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
        KMessageBox::error( this, "Could not find all template files.",
                                  "Formula shape template error" );
        return;
    }

    KoXmlDocument tmpDocument;  // setup DOM for parsing templates
    tmpDocument.setContent( &file, false, 0, 0, 0 );
    if( tmpDocument.documentElement().tagName() != "formulashapetemplates" ) {
        // TODO error not a formula template
    }

    QBuffer buffer;
    KoXmlWriter writer( &buffer );
    QPixmap tmpPixmap;
    QPainter painter( &tmpPixmap );
    AttributeManager manager;
    FormulaRenderer renderer;
    QListWidgetItem* tmpItem = 0;
    BasicElement* tmpElement = 0;
    KoXmlElement tmp;
    forEachElement( tmpDocument.documentElement(), tmp ) {
        if( tmp.tagName() != "template" )
            return;
        tmp = tmp.firstChild().toElement();
        buffer.open( QBuffer::ReadWrite );
        tmpElement = ElementFactory::createElement( tmp.tagName(), 0 );
        tmpElement->readMathML( tmp );
        tmpElement->writeMathML( &writer ); 
        renderer.layoutElement( tmpElement );
        renderer.paintElement( painter, tmpElement );
        tmpItem = new QListWidgetItem( m_templateList );
        tmpItem->setIcon( QIcon( tmpPixmap ) );
        tmpItem->setData( Qt::UserRole, QString( buffer.data() ) );
        list->append( tmpItem );
        delete tmpElement;
        buffer.close();
    }
}

void FormulaToolOptions::setFormulaTool( KoFormulaTool* tool )
{
    m_tool = tool;
}

#include "FormulaToolOptions.moc"
