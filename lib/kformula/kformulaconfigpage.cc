/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

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

#include <qvariant.h>   // first for gcc 2.7.2
#include <qlabel.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qwidget.h>

#include <kcolorbtn.h>
#include <kconfig.h>
#include <kdialog.h>
#include <klocale.h>

#include "contextstyle.h"
#include "kformulaconfigpage.h"
#include "kformuladocument.h"


KFORMULA_NAMESPACE_BEGIN


ConfigurePage::ConfigurePage( Document* document, KConfig* config, QVBox* box, char* name )
    : QObject( box->parent(), name ), m_document( document ), m_config( config )
{
    const ContextStyle& contextStyle = document->getContextStyle();

//     QVBoxLayout* boxLayout =
//         new QVBoxLayout( box, KDialog::marginHint(), KDialog::spacingHint() );

    QWidget* widget = new QWidget( box );

    QGridLayout* layout = new QGridLayout( widget );
    layout->setSpacing( KDialog::spacingHint() );
    layout->setMargin( KDialog::marginHint() );

    QLabel* defaultLabel = new QLabel( widget, "defaultLabel" );
    defaultLabel->setText( i18n( "default color" ) );
    layout->addWidget( defaultLabel, 0, 0 );

    defaultColorBtn = new KColorButton( widget, "defaultColor" );
    defaultColorBtn->setColor( contextStyle.getDefaultColor() );
    layout->addWidget( defaultColorBtn, 0, 1 );


    QLabel* numberLabel = new QLabel( widget, "numberLabel" );
    numberLabel->setText( i18n( "number color" ) );
    layout->addWidget( numberLabel, 1, 0 );

    numberColorBtn = new KColorButton( widget, "numberColor" );
    numberColorBtn->setColor( contextStyle.getNumberColor() );
    layout->addWidget( numberColorBtn, 1, 1 );


    QLabel* operatorLabel = new QLabel( widget, "operatorLabel" );
    operatorLabel->setText( i18n( "operator color" ) );
    layout->addWidget( operatorLabel, 2, 0 );

    operatorColorBtn = new KColorButton( widget, "operatorColor" );
    operatorColorBtn->setColor( contextStyle.getOperatorColor() );
    layout->addWidget( operatorColorBtn, 2, 1 );


    QLabel* emptyLabel = new QLabel( widget, "emptyLabel" );
    emptyLabel->setText( i18n( "empty color" ) );
    layout->addWidget( emptyLabel, 3, 0 );

    emptyColorBtn = new KColorButton( widget, "emptyColor" );
    emptyColorBtn->setColor( contextStyle.getEmptyColor() );
    layout->addWidget( emptyColorBtn, 3, 1 );


    QLabel* errorLabel = new QLabel( widget, "errorLabel" );
    errorLabel->setText( i18n( "error color" ) );
    layout->addWidget( errorLabel, 4, 0 );

    errorColorBtn = new KColorButton( widget, "errorColor" );
    errorColorBtn->setColor( contextStyle.getErrorColor() );
    layout->addWidget( errorColorBtn, 4, 1 );


//     boxLayout->addLayout( layout );
//     QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
//     boxLayout->addItem( spacer );
}

void ConfigurePage::apply()
{
    ContextStyle& contextStyle = m_document->getContextStyle();
    contextStyle.setDefaultColor( defaultColorBtn->color() );
    contextStyle.setNumberColor( numberColorBtn->color() );
    contextStyle.setOperatorColor( operatorColorBtn->color() );
    contextStyle.setEmptyColor( emptyColorBtn->color() );
    contextStyle.setErrorColor( errorColorBtn->color() );

    // notify!!!
    //document->
}

void ConfigurePage::slotDefault()
{
    defaultColorBtn->setColor( Qt::black );
    numberColorBtn->setColor( Qt::blue );
    operatorColorBtn->setColor( Qt::darkGreen );
    emptyColorBtn->setColor( Qt::blue );
    errorColorBtn->setColor( Qt::darkRed );
}

KFORMULA_NAMESPACE_END

using namespace KFormula;
#include "kformulaconfigpage.moc"
