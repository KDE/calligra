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
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qspinbox.h>
#include <qvbox.h>
#include <qwidget.h>

#include <kcolorbutton.h>
#include <kconfig.h>
#include <kdialog.h>
#include <kfontdialog.h>
#include <klocale.h>

#include "contextstyle.h"
#include "kformulaconfigpage.h"
#include "kformuladocument.h"


KFORMULA_NAMESPACE_BEGIN


ConfigurePage::ConfigurePage( Document* document, QWidget* view, KConfig* config, QVBox* box, char* name )
    : QObject( box->parent(), name ), m_document( document ), m_view( view ), m_config( config )
{
    const ContextStyle& contextStyle = document->getContextStyle( true );


    // fonts

    QWidget* fontWidget = new QWidget( box );
    QGridLayout* fontLayout = new QGridLayout( fontWidget, 5, 1, KDialog::marginHint(), KDialog::spacingHint() );

    defaultFont = contextStyle.getDefaultFont();
    nameFont = contextStyle.getNameFont();
    numberFont = contextStyle.getNumberFont();
    operatorFont = contextStyle.getOperatorFont();

    connect( buildFontLine( fontWidget, fontLayout, 0,
                            defaultFont, i18n( "Default font" ), defaultFontName ), SIGNAL( clicked() ),
             this, SLOT( selectNewDefaultFont() ) );
    connect( buildFontLine( fontWidget, fontLayout, 1,
                            nameFont, i18n( "Name font" ), nameFontName ), SIGNAL( clicked() ),
             this, SLOT( selectNewNameFont() ) );
    connect( buildFontLine( fontWidget, fontLayout, 2,
                            numberFont, i18n( "Number font" ), numberFontName ), SIGNAL( clicked() ),
             this, SLOT( selectNewNumberFont() ) );
    connect( buildFontLine( fontWidget, fontLayout, 3,
                            operatorFont, i18n( "Operator font" ), operatorFontName ), SIGNAL( clicked() ),
             this, SLOT( selectNewOperatorFont() ) );

    QWidget* sizeContainer = new QWidget( fontWidget );
    QGridLayout* sizeLayout = new QGridLayout( sizeContainer, 1, 3 );

    sizeLayout->setColStretch(0, 0);
    sizeLayout->setColStretch(1, 1);
    sizeLayout->setColStretch(2, 0);

    QLabel* sizeTitle = new QLabel( i18n( "Base Size" ), sizeContainer );
    QLabel* empty = new QLabel( "", sizeContainer );
    sizeSpin = new QSpinBox( 8, 72, 1, sizeContainer );
    sizeSpin->setValue( contextStyle.baseSize() );

    sizeLayout->addWidget( sizeTitle, 0, 0 );
    sizeLayout->addWidget( empty, 0, 1 );
    sizeLayout->addWidget( sizeSpin, 0, 2 );

    fontLayout->addWidget( sizeContainer, 4, 0 );

    connect( sizeSpin, SIGNAL( valueChanged( int ) ), this, SLOT( baseSizeChanged( int ) ) );

    // syntax highlighting

    syntaxHighlighting = new QCheckBox( i18n( "Syntax Highlighting" ), box );
    syntaxHighlighting->setChecked( contextStyle.syntaxHighlighting() );

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
    numberColorBtn->setColor( contextStyle.getNumberColorPlain() );
    layout->addWidget( numberColorBtn, 1, 1 );


    QLabel* operatorLabel = new QLabel( widget, "operatorLabel" );
    operatorLabel->setText( i18n( "operator color" ) );
    layout->addWidget( operatorLabel, 2, 0 );

    operatorColorBtn = new KColorButton( widget, "operatorColor" );
    operatorColorBtn->setColor( contextStyle.getOperatorColorPlain() );
    layout->addWidget( operatorColorBtn, 2, 1 );


    QLabel* emptyLabel = new QLabel( widget, "emptyLabel" );
    emptyLabel->setText( i18n( "empty color" ) );
    layout->addWidget( emptyLabel, 3, 0 );

    emptyColorBtn = new KColorButton( widget, "emptyColor" );
    emptyColorBtn->setColor( contextStyle.getEmptyColorPlain() );
    layout->addWidget( emptyColorBtn, 3, 1 );


    QLabel* errorLabel = new QLabel( widget, "errorLabel" );
    errorLabel->setText( i18n( "error color" ) );
    layout->addWidget( errorLabel, 4, 0 );

    errorColorBtn = new KColorButton( widget, "errorColor" );
    errorColorBtn->setColor( contextStyle.getErrorColorPlain() );
    layout->addWidget( errorColorBtn, 4, 1 );

//     boxLayout->addLayout( layout );
//     QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
//     boxLayout->addItem( spacer );

    connect( syntaxHighlighting, SIGNAL( clicked() ),
             this, SLOT( syntaxHighlightingClicked() ) );

    syntaxHighlightingClicked();
}


QPushButton* ConfigurePage::buildFontLine( QWidget* fontWidget, QGridLayout* layout, int number,
                                           QFont font, QString name, QLabel*& fontName )
{
    QWidget* fontContainer = new QWidget( fontWidget );
    QGridLayout* fontLayout = new QGridLayout( fontContainer, 1, 3 );

    fontLayout->setColStretch(0, 0);
    fontLayout->setColStretch(1, 1);
    fontLayout->setColStretch(2, 0);

    QLabel* fontTitle = new QLabel( name, fontContainer );

    //font = new QFont();
    //font->fromString( defaultFont );

    QString labelName = font.family() + ' ' + QString::number( font.pointSize() );
    fontName = new QLabel( labelName, fontContainer );
    fontName->setFont( font );

    QPushButton* chooseButton = new QPushButton( i18n( "Choose..." ), fontContainer );

    fontLayout->addWidget( fontTitle, 0, 0 );
    fontLayout->addWidget( fontName, 0, 1 );
    fontLayout->addWidget( chooseButton, 0, 2 );

    layout->addWidget( fontContainer, number, 0 );
    return chooseButton;
}


void ConfigurePage::apply()
{
    ContextStyle& contextStyle = m_document->getContextStyle( true );

    contextStyle.setDefaultFont( defaultFont );
    contextStyle.setNameFont( nameFont );
    contextStyle.setNumberFont( numberFont );
    contextStyle.setOperatorFont( operatorFont );
    contextStyle.setBaseSize( sizeSpin->value() );

    contextStyle.setSyntaxHighlighting( syntaxHighlighting->isChecked() );
    contextStyle.setDefaultColor( defaultColorBtn->color() );
    contextStyle.setNumberColor( numberColorBtn->color() );
    contextStyle.setOperatorColor( operatorColorBtn->color() );
    contextStyle.setEmptyColor( emptyColorBtn->color() );
    contextStyle.setErrorColor( errorColorBtn->color() );

    m_config->setGroup( "kformula Font" );
    m_config->writeEntry( "defaultFont", defaultFont.toString() );
    m_config->writeEntry( "nameFont", nameFont.toString() );
    m_config->writeEntry( "numberFont", numberFont.toString() );
    m_config->writeEntry( "operatorFont", operatorFont.toString() );
    m_config->writeEntry( "baseSize", QString::number( sizeSpin->value() ) );

    m_config->setGroup( "kformula Color" );
    m_config->writeEntry( "syntaxHighlighting", syntaxHighlighting->isChecked() );
    m_config->writeEntry( "defaultColor", defaultColorBtn->color() );
    m_config->writeEntry( "numberColor",  numberColorBtn->color() );
    m_config->writeEntry( "operatorColor", operatorColorBtn->color() );
    m_config->writeEntry( "emptyColor", emptyColorBtn->color() );
    m_config->writeEntry( "errorColor", errorColorBtn->color() );

    // notify!!!
    m_document->updateConfig();
}

void ConfigurePage::slotDefault()
{
    defaultFont = QFont( "Times", 12, QFont::Normal, true );
    nameFont = QFont( "Times" );
    numberFont = QFont( "Times" );
    operatorFont = QFont( "Times" );

    sizeSpin->setValue( 20 );

    updateFontLabel( defaultFont, defaultFontName );
    updateFontLabel( nameFont, nameFontName );
    updateFontLabel( numberFont, numberFontName );
    updateFontLabel( operatorFont, operatorFontName );

    syntaxHighlighting->setChecked( true );
    syntaxHighlightingClicked();
    defaultColorBtn->setColor( Qt::black );
    numberColorBtn->setColor( Qt::blue );
    operatorColorBtn->setColor( Qt::darkGreen );
    emptyColorBtn->setColor( Qt::blue );
    errorColorBtn->setColor( Qt::darkRed );
}

void ConfigurePage::syntaxHighlightingClicked()
{
    bool checked = syntaxHighlighting->isChecked();
    defaultColorBtn->setEnabled( checked );
    numberColorBtn->setEnabled( checked );
    operatorColorBtn->setEnabled( checked );
    emptyColorBtn->setEnabled( checked );
    errorColorBtn->setEnabled( checked );
}

void ConfigurePage::selectNewDefaultFont() {
    QStringList list;
    KFontChooser::getFontList(list,  KFontChooser::SmoothScalableFonts);
    //KFontDialog dlg( m_pView, i18n( "Font Selector" ), false, true, list, true );
    KFontDialog dlg( m_view, "Font Selector", false, true, list, true );
    dlg.setFont( defaultFont );
    int result = dlg.exec();
    if ( KDialog::Accepted == result ) {
        defaultFont = dlg.font();
        defaultFont.setPointSize( 12 );
        updateFontLabel( defaultFont, defaultFontName );
    }
}

void ConfigurePage::selectNewNameFont()
{
    QStringList list;
    KFontChooser::getFontList(list,  KFontChooser::SmoothScalableFonts);
    //KFontDialog dlg( m_pView, i18n( "Font Selector" ), false, true, list, true );
    KFontDialog dlg( m_view, "Font Selector", false, true, list, true );
    dlg.setFont( defaultFont );
    int result = dlg.exec();
    if ( KDialog::Accepted == result ) {
        nameFont = dlg.font();
        nameFont.setPointSize( 12 );
        updateFontLabel( nameFont, nameFontName );
    }
}

void ConfigurePage::selectNewNumberFont()
{
    QStringList list;
    KFontChooser::getFontList(list,  KFontChooser::SmoothScalableFonts);
    //KFontDialog dlg( m_pView, i18n( "Font Selector" ), false, true, list, true );
    KFontDialog dlg( m_view, "Font Selector", false, true, list, true );
    dlg.setFont( defaultFont );
    int result = dlg.exec();
    if ( KDialog::Accepted == result ) {
        numberFont = dlg.font();
        numberFont.setPointSize( 12 );
        updateFontLabel( numberFont, numberFontName );
    }
}

void ConfigurePage::selectNewOperatorFont()
{
    QStringList list;
    KFontChooser::getFontList(list,  KFontChooser::SmoothScalableFonts);
    //KFontDialog dlg( m_pView, i18n( "Font Selector" ), false, true, list, true );
    KFontDialog dlg( m_view, "Font Selector", false, true, list, true );
    dlg.setFont( defaultFont );
    int result = dlg.exec();
    if ( KDialog::Accepted == result ) {
        operatorFont = dlg.font();
        operatorFont.setPointSize( 12 );
        updateFontLabel( operatorFont, operatorFontName );
    }
}

void ConfigurePage::baseSizeChanged( int /*value*/ )
{
}

void ConfigurePage::updateFontLabel( QFont font, QLabel* label )
{
    label->setText( font.family() + ' ' + QString::number( font.pointSize() ) );
    label->setFont( font );
}

KFORMULA_NAMESPACE_END

using namespace KFormula;
#include "kformulaconfigpage.moc"
