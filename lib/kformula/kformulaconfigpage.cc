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
#include <qmap.h>
#include <qspinbox.h>
#include <qstringlist.h>
#include <qvbox.h>
#include <qwidget.h>

#include <algorithm>

#include <kcolorbutton.h>
#include <kconfig.h>
#include <kdialog.h>
#include <kfontdialog.h>
#include <klistview.h>
#include <klocale.h>
#include <kpushbutton.h>

#include "contextstyle.h"
#include "kformulaconfigpage.h"
#include "kformuladocument.h"
#include "symboltable.h"


KFORMULA_NAMESPACE_BEGIN


ConfigurePage::ConfigurePage( Document* document, QWidget* view, KConfig* config, QVBox* box, char* name )
    : QObject( box->parent(), name ), m_document( document ), m_view( view ), m_config( config )
{
    const ContextStyle& contextStyle = document->getContextStyle( true );


    // fonts

    QWidget* fontWidget = new QWidget( box );
    QGridLayout* fontLayout = new QGridLayout( fontWidget, 5, 3, KDialog::marginHint(), KDialog::spacingHint() );

    fontLayout->setColStretch(0, 0);
    fontLayout->setColStretch(1, 1);
    fontLayout->setColStretch(2, 0);

    defaultFont = contextStyle.getDefaultFont();
    nameFont = contextStyle.getNameFont();
    numberFont = contextStyle.getNumberFont();
    operatorFont = contextStyle.getOperatorFont();

    connect( buildFontLine( fontWidget, fontLayout, 0,
                            defaultFont, i18n( "Default font:" ), defaultFontName ), SIGNAL( clicked() ),
             this, SLOT( selectNewDefaultFont() ) );
    connect( buildFontLine( fontWidget, fontLayout, 1,
                            nameFont, i18n( "Name font:" ), nameFontName ), SIGNAL( clicked() ),
             this, SLOT( selectNewNameFont() ) );
    connect( buildFontLine( fontWidget, fontLayout, 2,
                            numberFont, i18n( "Number font:" ), numberFontName ), SIGNAL( clicked() ),
             this, SLOT( selectNewNumberFont() ) );
    connect( buildFontLine( fontWidget, fontLayout, 3,
                            operatorFont, i18n( "Operator font:" ), operatorFontName ), SIGNAL( clicked() ),
             this, SLOT( selectNewOperatorFont() ) );

    QLabel* sizeTitle = new QLabel( i18n( "Base size:" ), fontWidget );
    fontLayout->addWidget( sizeTitle, 4, 0 );

    QWidget* baseSpinContainer = new QWidget( fontWidget );
    QGridLayout* baseSpinLayout = new QGridLayout( baseSpinContainer, 1, 2 );
    baseSpinLayout->setColStretch(1, 1);

    sizeSpin = new QSpinBox( 8, 72, 1, baseSpinContainer );
    sizeSpin->setValue( contextStyle.baseSize() );
    sizeSpin->setFixedSize(sizeSpin->sizeHint());
    baseSpinLayout->addWidget( sizeSpin, 0, 0);

    fontLayout->addWidget( baseSpinContainer, 4, 1 );

    connect( sizeSpin, SIGNAL( valueChanged( int ) ), this, SLOT( baseSizeChanged( int ) ) );

    // syntax highlighting

    syntaxHighlighting = new QCheckBox( i18n( "Syntax highlighting" ), box );
    syntaxHighlighting->setChecked( contextStyle.syntaxHighlighting() );

    QWidget* widget = new QWidget( box );

    QGridLayout* layout = new QGridLayout( widget );
    layout->setSpacing( KDialog::spacingHint() );
    layout->setMargin( KDialog::marginHint() );

    QLabel* defaultLabel = new QLabel( widget, "defaultLabel" );
    defaultLabel->setText( i18n( "Default color:" ) );
    layout->addWidget( defaultLabel, 0, 0 );

    defaultColorBtn = new KColorButton( widget, "defaultColor" );
    defaultColorBtn->setColor( contextStyle.getDefaultColor() );
    layout->addWidget( defaultColorBtn, 0, 1 );


    QLabel* numberLabel = new QLabel( widget, "numberLabel" );
    numberLabel->setText( i18n( "Number color:" ) );
    layout->addWidget( numberLabel, 1, 0 );

    numberColorBtn = new KColorButton( widget, "numberColor" );
    numberColorBtn->setColor( contextStyle.getNumberColorPlain() );
    layout->addWidget( numberColorBtn, 1, 1 );


    QLabel* operatorLabel = new QLabel( widget, "operatorLabel" );
    operatorLabel->setText( i18n( "Operator color:" ) );
    layout->addWidget( operatorLabel, 2, 0 );

    operatorColorBtn = new KColorButton( widget, "operatorColor" );
    operatorColorBtn->setColor( contextStyle.getOperatorColorPlain() );
    layout->addWidget( operatorColorBtn, 2, 1 );


    QLabel* emptyLabel = new QLabel( widget, "emptyLabel" );
    emptyLabel->setText( i18n( "Empty color:" ) );
    layout->addWidget( emptyLabel, 3, 0 );

    emptyColorBtn = new KColorButton( widget, "emptyColor" );
    emptyColorBtn->setColor( contextStyle.getEmptyColorPlain() );
    layout->addWidget( emptyColorBtn, 3, 1 );


    QLabel* errorLabel = new QLabel( widget, "errorLabel" );
    errorLabel->setText( i18n( "Error color:" ) );
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
    QLabel* fontTitle = new QLabel( name, fontWidget );

    QString labelName = font.family() + ' ' + QString::number( font.pointSize() );
    fontName = new QLabel( labelName, fontWidget );
    fontName->setFont( font );
    fontName->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);

    QPushButton* chooseButton = new QPushButton( i18n( "Choose..." ), fontWidget );

    layout->addWidget( fontTitle, number, 0 );
    layout->addWidget( fontName, number, 1 );
    layout->addWidget( chooseButton, number, 2 );

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


class UsedFontItem : public KListViewItem {
public:
    UsedFontItem( MathFontsConfigurePage* page, QListView* parent, QString font )
        : KListViewItem( parent, font ), m_page( page ) {}

    int compare( QListViewItem* i, int col, bool ascending ) const;

private:
    MathFontsConfigurePage* m_page;
};

int UsedFontItem::compare( QListViewItem* i, int, bool ) const
{
    QValueVector<QString>::iterator lhsIt = m_page->findUsedFont( text( 0 ) );
    QValueVector<QString>::iterator rhsIt = m_page->findUsedFont( i->text( 0 ) );
    if ( lhsIt < rhsIt ) {
        return -1;
    }
    else if ( lhsIt > rhsIt ) {
        return 1;
    }
    return 0;
}

MathFontsConfigurePage::MathFontsConfigurePage( Document* document, QWidget* view,
                                                KConfig* config, QVBox* box, char* name )
    : QObject( box->parent(), name ), m_document( document ), m_view( view ), m_config( config )
{
    QWidget* fontWidget = new QWidget( box );
    QGridLayout* fontLayout = new QGridLayout( fontWidget, 1, 1, KDialog::marginHint(), KDialog::spacingHint() );

    QHBoxLayout* hLayout = new QHBoxLayout( 0, 0, 6 );

    availableFonts = new KListView( fontWidget );
    availableFonts->addColumn( i18n( "Available Fonts" ) );
    hLayout->addWidget( availableFonts );

    QVBoxLayout* vLayout = new QVBoxLayout( 0, 0, 6 );
    QSpacerItem* spacer1 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    vLayout->addItem( spacer1 );

    addFont = new KPushButton( fontWidget );
    addFont->setText( "->" );
    vLayout->addWidget( addFont );

    removeFont = new KPushButton( fontWidget );
    removeFont->setText( "<-" );
    vLayout->addWidget( removeFont );

    QSpacerItem* spacer2 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    vLayout->addItem( spacer2 );

    hLayout->addLayout( vLayout );

    vLayout = new QVBoxLayout( 0, 0, 6 );

    moveUp = new KPushButton( fontWidget );
    moveUp->setText( i18n( "Up" ) );
    vLayout->addWidget( moveUp );

    requestedFonts = new KListView( fontWidget );
    requestedFonts->addColumn( i18n( "Used Fonts" ) );
    vLayout->addWidget( requestedFonts );

    moveDown = new KPushButton( fontWidget );
    moveDown->setText( i18n( "Down" ) );
    vLayout->addWidget( moveDown );

    hLayout->addLayout( vLayout );

    fontLayout->addLayout( hLayout, 0, 0 );

//     connect( availableFonts, SIGNAL( executed( QListViewItem* ) ),
//              this, SLOT( slotAddFont() ) );
//     connect( requestedFonts, SIGNAL( executed( QListViewItem* ) ),
//              this, SLOT( slotRemoveFont() ) );
    connect( addFont, SIGNAL( clicked() ), this, SLOT( slotAddFont() ) );
    connect( removeFont, SIGNAL( clicked() ), this, SLOT( slotRemoveFont() ) );
    connect( moveUp, SIGNAL( clicked() ), this, SLOT( slotMoveUp() ) );
    connect( moveDown, SIGNAL( clicked() ), this, SLOT( slotMoveDown() ) );

    const ContextStyle& contextStyle = document->getContextStyle( true );
    const SymbolTable& symbolTable = contextStyle.symbolTable();
    const QStringList& usedFonts = contextStyle.requestedFonts();

    QMap<QString, QString> fontMap;
    symbolTable.findAvailableFonts( &fontMap );

    setupLists( usedFonts );
}

void MathFontsConfigurePage::apply()
{
    QStringList strings;
    std::copy( usedFontList.begin(), usedFontList.end(), std::back_inserter( strings ) );

    m_config->setGroup( "kformula Font" );
    m_config->writeEntry( "usedMathFonts", strings );

    ContextStyle& contextStyle = m_document->getContextStyle( true );
    contextStyle.setRequestedFonts( strings );
}

void MathFontsConfigurePage::slotDefault()
{
    QStringList usedFonts;

    usedFonts.push_back( "esstixone" );
    usedFonts.push_back( "esstixtwo" );
    usedFonts.push_back( "esstixthree" );
    usedFonts.push_back( "esstixfour" );
    usedFonts.push_back( "esstixfive" );
    usedFonts.push_back( "esstixsix" );
    usedFonts.push_back( "esstixseven" );
    usedFonts.push_back( "esstixeight" );
    usedFonts.push_back( "esstixnine" );
    usedFonts.push_back( "esstixten" );
    usedFonts.push_back( "esstixeleven" );
    usedFonts.push_back( "esstixtwelve" );
    usedFonts.push_back( "esstixthirteen" );
    usedFonts.push_back( "esstixfourteen" );
    usedFonts.push_back( "esstixfifteen" );
    usedFonts.push_back( "esstixsixteen" );
    usedFonts.push_back( "esstixseventeen" );

    usedFontList.clear();
    requestedFonts->clear();
    availableFonts->clear();

    setupLists( usedFonts );
}

QValueVector<QString>::iterator MathFontsConfigurePage::findUsedFont( QString name )
{
    return std::find( usedFontList.begin(), usedFontList.end(), name );
}

void MathFontsConfigurePage::setupLists( const QStringList& usedFonts )
{
    const ContextStyle& contextStyle = m_document->getContextStyle( true );
    const SymbolTable& symbolTable = contextStyle.symbolTable();

    QMap<QString, QString> fontMap;
    symbolTable.findAvailableFonts( &fontMap );

    for ( QStringList::const_iterator it = usedFonts.begin(); it != usedFonts.end(); ++it ) {
        QMap<QString, QString>::iterator font = fontMap.find( *it );
        if ( font != fontMap.end() ) {
            fontMap.erase( font );
            new UsedFontItem( this, requestedFonts, *it );
            usedFontList.push_back( *it );
        }
    }
    for ( QMap<QString, QString>::iterator it = fontMap.begin(); it != fontMap.end(); ++it ) {
        new KListViewItem( availableFonts, it.key() );
    }
}

void MathFontsConfigurePage::slotAddFont()
{
    QListViewItem* fontItem = availableFonts->selectedItem();
    if ( fontItem ) {
        QString fontName = fontItem->text( 0 );
        //availableFonts->takeItem( fontItem );
        delete fontItem;

        new UsedFontItem( this, requestedFonts, fontName );
        usedFontList.push_back( fontName );
    }
}

void MathFontsConfigurePage::slotRemoveFont()
{
    QListViewItem* fontItem = requestedFonts->selectedItem();
    if ( fontItem ) {
        QString fontName = fontItem->text( 0 );
        QValueVector<QString>::iterator it = std::find( usedFontList.begin(), usedFontList.end(), fontName );
        if ( it != usedFontList.end() ) {
            usedFontList.erase( it );
        }
        //requestedFonts->takeItem( fontItem );
        delete fontItem;
        new KListViewItem( availableFonts, fontName );
    }
}

void MathFontsConfigurePage::slotMoveUp()
{
    QListViewItem* fontItem = requestedFonts->selectedItem();
    if ( fontItem ) {
        QString fontName = fontItem->text( 0 );
        QValueVector<QString>::iterator it = std::find( usedFontList.begin(), usedFontList.end(), fontName );
        if ( it != usedFontList.end() ) {
            uint pos = it - usedFontList.begin();
            if ( pos > 0 ) {
                QValueVector<QString>::iterator before = it-1;
                std::swap( *it, *before );
                requestedFonts->sort();
            }
        }
    }
}

void MathFontsConfigurePage::slotMoveDown()
{
    QListViewItem* fontItem = requestedFonts->selectedItem();
    if ( fontItem ) {
        QString fontName = fontItem->text( 0 );
        QValueVector<QString>::iterator it = std::find( usedFontList.begin(), usedFontList.end(), fontName );
        if ( it != usedFontList.end() ) {
            uint pos = it - usedFontList.begin();
            if ( pos < usedFontList.size()-1 ) {
                QValueVector<QString>::iterator after = it+1;
                std::swap( *it, *after );
                requestedFonts->sort();
            }
        }
    }
}

KFORMULA_NAMESPACE_END

using namespace KFormula;
#include "kformulaconfigpage.moc"
