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

#include <KAction>

#include <KMessageBox>
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
    connect( buttonLoad, SIGNAL( clicked() ), m_tool, SLOT( loadFormula() ) );
    connect( buttonSave, SIGNAL( clicked() ), m_tool, SLOT( saveFormula() ) );
}

FormulaToolWidget::~FormulaToolWidget()
{}


void FormulaToolWidget::setFormulaTool( KoFormulaTool* tool )
{
    m_tool = tool;
}

#include "FormulaToolWidget.moc"
