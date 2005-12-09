/* This file is part of the KDE project
   Copyright (C) 2001,2002,2003,2004 Laurent Montel <montel@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#include "kchartHeaderFooterConfigPage.h"
#include "kchartHeaderFooterConfigPage.moc"

#include <kapplication.h>
#include <kdialog.h>
#include <klocale.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qbuttongroup.h>
#include <qwhatsthis.h>
#include <kfontdialog.h>

#include "kchart_params.h"

namespace KChart
{

KChartHeaderFooterConfigPage::KChartHeaderFooterConfigPage( KChartParams* params,
                                                      QWidget* parent ) :
    QWidget( parent ),_params( params )
{
    QGridLayout* layout = new QGridLayout( this, 4, 3 );
    layout->setSpacing( KDialog::spacingHint() );
    layout->setMargin( KDialog::marginHint() );

    QLabel *lab=new QLabel(i18n("Title:"), this);
    QWhatsThis::add(lab, i18n("Write here the title of your chart if you want one. The title will be centered on top above your chart."));
    layout->addWidget(lab,0,0);

    titleEdit=new QLineEdit(this);
    QWhatsThis::add(titleEdit, i18n("Write here the title of your chart if you want one. The title will be centered on top above your chart."));
    layout->addWidget(titleEdit,0,1);

    titleColorButton=new KColorButton(this);
    QWhatsThis::add(titleColorButton, i18n("Click on this button to choose the color for the title font."));
    layout->addWidget(titleColorButton,0,2);

    titleFontButton=new QPushButton(i18n("Font..."),this);
    QWhatsThis::add(titleFontButton, i18n("Click on this button to choose the font family, style and size for the title."));
    layout->addWidget(titleFontButton,0,3);
    connect( titleFontButton, SIGNAL(clicked()), this, SLOT(changeTitleFont()));

    lab=new QLabel(i18n("Subtitle:"),this);
    QWhatsThis::add(lab, i18n("Write here the subtitle of your chart if you want one. The subtitle will be centered on top just below the title."));
    layout->addWidget(lab,1,0);

    subtitleEdit=new QLineEdit(this);
    QWhatsThis::add(subtitleEdit, i18n("Write here the subtitle of your chart if you want one. The subtitle will be centered on top just below the title."));
    layout->addWidget(subtitleEdit,1,1);

    subtitleColorButton=new KColorButton(this);
    QWhatsThis::add(subtitleColorButton, i18n("Click on this button to choose the color for the subtitle font."));
    layout->addWidget(subtitleColorButton,1,2);

    subtitleFontButton=new QPushButton(i18n("Font..."),this);
    QWhatsThis::add(subtitleFontButton, i18n("Click on this button to choose the font family, style and size for the subtitle."));
    layout->addWidget(subtitleFontButton,1,3);
    connect( subtitleFontButton, SIGNAL(clicked()), this, SLOT(changeSubtitleFont()));

    lab=new QLabel(i18n("Footer:"),this);
    QWhatsThis::add(lab, i18n("Write here the footer of your chart if you want one. The footer will be centered at the bottom just below your chart."));
    layout->addWidget(lab,2,0);

    footerEdit=new QLineEdit(this);
    QWhatsThis::add(footerEdit, i18n("Write here the subtitle of your chart if you want one. The subtitle will be centered on top just below the title."));
    layout->addWidget(footerEdit,2,1);

    footerColorButton=new KColorButton(this);
    QWhatsThis::add(footerColorButton, i18n("Click on this button to choose the color for the footer font."));
    layout->addWidget(footerColorButton,2,2);

    footerFontButton=new QPushButton(i18n("Font..."),this);
    QWhatsThis::add(footerFontButton, i18n("Click on this button to choose the font family, style and size for the footer."));
    connect( footerFontButton, SIGNAL(clicked()), this, SLOT(changeFooterFont()));
    layout->addWidget(footerFontButton,2,3);

    layout->addItem( new QSpacerItem( 5, 5, QSizePolicy::Minimum, QSizePolicy::Expanding ), 3, 0 );

    layout->activate();
}

void KChartHeaderFooterConfigPage::init()
{
    titleColorButton->setColor(_params->headerFooterColor( KDChartParams::HdFtPosHeader ) );
    subtitleColorButton->setColor(_params->headerFooterColor( KDChartParams::HdFtPosHeader2 ));
    footerColorButton->setColor(_params->headerFooterColor( KDChartParams::HdFtPosFooter ) );
    titleEdit->setText(_params->header1Text());
    subtitleEdit->setText(_params->header2Text());
    footerEdit->setText(_params->footerText());


    titleFont = _params->header1Font();
    titleFontIsRelative = _params->headerFooterFontUseRelSize(KDChartParams::HdFtPosHeader)
                          ? QButton::On
                          : QButton::Off;
    if( QButton::On == titleFontIsRelative )
        titleFont.setPointSize( _params->headerFooterFontRelSize(KDChartParams::HdFtPosHeader) );

    subtitleFont = _params->header2Font();
    subtitleFontIsRelative = _params->headerFooterFontUseRelSize(KDChartParams::HdFtPosHeader2)
                          ? QButton::On
                          : QButton::Off;
    if( QButton::On == subtitleFontIsRelative )
        subtitleFont.setPointSize( _params->headerFooterFontRelSize(KDChartParams::HdFtPosHeader2) );

    footerFont = _params->footerFont();
    footerFontIsRelative = _params->headerFooterFontUseRelSize(KDChartParams::HdFtPosFooter)
                          ? QButton::On
                          : QButton::Off;
    if( QButton::On == footerFontIsRelative )
        footerFont.setPointSize( _params->headerFooterFontRelSize(KDChartParams::HdFtPosFooter) );
}


void KChartHeaderFooterConfigPage::apply()
{
    _params->setHeaderFooterColor( KDChartParams::HdFtPosHeader,titleColorButton->color() );
    _params->setHeaderFooterColor( KDChartParams::HdFtPosHeader2,subtitleColorButton->color() );
    _params->setHeaderFooterColor( KDChartParams::HdFtPosFooter, footerColorButton->color() );


    _params->setHeader1Text(titleEdit->text());
    _params->setHeader2Text(subtitleEdit->text());
    _params->setFooterText(footerEdit->text());


    _params->setHeaderFooterFont( KDChartParams::HdFtPosHeader, titleFont,
                                  titleFontIsRelative,
                                  titleFont.pointSize() );
    _params->setHeaderFooterFont( KDChartParams::HdFtPosHeader2, subtitleFont,
                                  subtitleFontIsRelative,
                                  subtitleFont.pointSize() );
    _params->setHeaderFooterFont( KDChartParams::HdFtPosFooter, footerFont,
                                  footerFontIsRelative,
                                  footerFont.pointSize() );
}

void KChartHeaderFooterConfigPage::changeTitleFont()
{
    QButton::ToggleState state = titleFontIsRelative;
    if (    KFontDialog::getFont( titleFont,false,this, true,&state ) != QDialog::Rejected
            && QButton::NoChange != state )
        titleFontIsRelative = state;
}

void KChartHeaderFooterConfigPage::changeSubtitleFont()
{
    QButton::ToggleState state = subtitleFontIsRelative;
    if (    KFontDialog::getFont( subtitleFont,false,this, true,&state ) != QDialog::Rejected
            && QButton::NoChange != state )
        subtitleFontIsRelative = state;
}

void KChartHeaderFooterConfigPage::changeFooterFont()
{
    QButton::ToggleState state = footerFontIsRelative;
    if (    KFontDialog::getFont( footerFont,false,this, true,&state ) != QDialog::Rejected
            && QButton::NoChange != state )
        footerFontIsRelative = state;
}

}  //KChart namespace
