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
#include <KoOdfLoadingContext.h>
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
#include <kcombobox.h>
#include <QListWidget>
#include <QGridLayout>

FormulaToolOptions::FormulaToolOptions( QWidget* parent, Qt::WindowFlags f )
                  : QWidget( parent, f )
{
    // create a combo box with the different categories of templates
    m_templateCombo = new KComboBox( this );
    m_templateCombo->setInsertPolicy( QComboBox::NoInsert );
    m_templateCombo->addItem( "General" );
    m_templateCombo->addItem( "Operators" );
    m_templateCombo->addItem( "Functions" );
    m_templateCombo->addItem( "Custom" );

    // setup a list widget and some buttons
    m_templateList = new QListWidget( this );
    m_templateList->setViewMode( QListView::IconMode );
    m_templateList->setSelectionMode( QAbstractItemView::SingleSelection );
    m_templateList->setDragEnabled( true );
    m_loadFormula = new QPushButton( this );
    m_loadFormula->setText( "Load Formula" );
    m_saveFormula = new QPushButton( this );
    m_saveFormula->setText( "Save Formula" );
    m_saveFormula->setEnabled( false );

    // put the gui elements into a simple layout
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
    // open template file
    QFile file( p );
    if( !file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
        KMessageBox::error( this, "Could not find all template files.",
                                  "Formula shape template error" );
        return;
    }

    // setup DOM for parsing templates
    KoXmlDocument tmpDocument;
    tmpDocument.setContent( &file, false, 0, 0, 0 );
    if( tmpDocument.documentElement().tagName() != "formulashapetemplates" ) {
        KMessageBox::error( this, "Formula template files corrupted.",
                                  "Formula shape template error" );
        return;
    }

    // Split the raw data into the single element raw data
    QString fileContent( file.readAll() );
    fileContent.remove( "<formulashapetemplates>" );
    fileContent.remove( "</formulashapetemplates>" );
    fileContent.remove( "</template>" );
    QStringList elementsRawString = fileContent.split( "<template>" );

    // iterate through the template elements and create all valid templates
    KoXmlElement tmpElement;
    QListWidgetItem* tmpItem = 0;
    int i = 0;
    forEachElement( tmpDocument.documentElement(), tmpElement ) {
        tmpItem = createListItem( tmpElement.firstChild().toElement() );
        tmpItem->setData( Qt::UserRole, elementsRawString[ i ] );
        list->append( tmpItem );
        i++;
    }
}

QListWidgetItem* FormulaToolOptions::createListItem( const KoXmlElement& xml )
{
    // create an element out of DOM
    BasicElement* tmpElement = ElementFactory::createElement( xml.tagName(), 0 );
    tmpElement->readMathML( xml );

    // render the element into a pixmap buffer for icon 
    FormulaRenderer renderer;
    QPixmap tmpPixmap;
    QPainter painter( &tmpPixmap );
    renderer.layoutElement( tmpElement );
    renderer.paintElement( painter, tmpElement );
 
    // fix size of the pixmap, cleanup and return the new list item
    tmpPixmap = tmpPixmap.scaled( 32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation );
    delete tmpElement;
    return new QListWidgetItem( QIcon( tmpPixmap ), QString(), m_templateList );
}

void FormulaToolOptions::setFormulaTool( KoFormulaTool* tool )
{
    m_tool = tool;
}

#include "FormulaToolOptions.moc"
