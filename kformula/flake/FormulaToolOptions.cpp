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
}

FormulaToolOptions::~FormulaToolOptions()
{
    delete m_layout;
    delete m_templateCombo;
    delete m_templateList;
    delete m_loadFormula;
    delete m_saveFormula;
}

