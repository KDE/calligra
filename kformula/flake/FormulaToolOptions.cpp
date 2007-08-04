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
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

#include <KoShapeSavingContext.h>
#include <KoShapeLoadingContext.h>
#include <KoOasisLoadingContext.h>
#include <KoOasisStyles.h>

#include <KFileDialog>
#include <QFile>
#include <QPushButton>
#include <QComboBox>
#include <QListWidget>
#include <QGridLayout>

FormulaToolOptions::FormulaToolOptions( QWidget* parent, Qt::WindowFlags f )
                  : QWidget( parent, f )
{
    m_templateCombo = new QComboBox( this );
    m_templateCombo->addItem( "General" );
    m_templateCombo->addItem( "Operators" );
    m_templateCombo->addItem( "Functions" );

    m_templateList = new QListWidget( this );
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

    KoOasisStyles styles;
    KoOasisLoadingContext oasisContext( 0, styles, 0 );
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
//    m_tool->shape()->saveOdf( );
}

void FormulaToolOptions::slotTemplateComboIndexChange( int index )
{
}

void FormulaToolOptions::setFormulaTool( KoFormulaTool* tool )
{
    m_tool = tool;
}

#include "FormulaToolOptions.moc"
