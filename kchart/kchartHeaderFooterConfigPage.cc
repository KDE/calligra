/*
 * Copyright 2001 by Laurent Montel, released under Artistic License.
 */

#include "kchartHeaderFooterConfigPage.h"
#include "kchartHeaderFooterConfigPage.moc"

#include <kapplication.h>
#include <klocale.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qbuttongroup.h>
#include <kfontdialog.h>

#include "kchart_params.h"

KChartHeaderFooterConfigPage::KChartHeaderFooterConfigPage( KChartParams* params,
                                                      QWidget* parent ) :
    QWidget( parent ),_params( params )
{
    QVBoxLayout* toplevel = new QVBoxLayout( this, 10 );

    QGridLayout* layout = new QGridLayout( 1, 3 );
    toplevel->addLayout( layout );

    QButtonGroup* gb1 = new QButtonGroup( i18n("Header/Footer"), this );
    QGridLayout *grid1 = new QGridLayout(gb1,3,4,15,8);

    QLabel *lab=new QLabel(i18n("Title:"),gb1);
    grid1->addWidget(lab,0,0);

    titleEdit=new QLineEdit(gb1);
    grid1->addWidget(titleEdit,0,1);

    titleColorButton=new KColorButton(gb1);
    grid1->addWidget(titleColorButton,0,2);

    titleFontButton=new QPushButton(i18n("Font"),gb1);
    grid1->addWidget(titleFontButton,0,3);
    connect( titleFontButton, SIGNAL(clicked()), this, SLOT(changeTitleFont()));

    lab=new QLabel(i18n("Subtitle:"),gb1);
    grid1->addWidget(lab,1,0);

    subtitleEdit=new QLineEdit(gb1);
    grid1->addWidget(subtitleEdit,1,1);

    subtitleColorButton=new KColorButton(gb1);
    grid1->addWidget(subtitleColorButton,1,2);

    subtitleFontButton=new QPushButton(i18n("Font"),gb1);
    grid1->addWidget(subtitleFontButton,1,3);
    connect( subtitleFontButton, SIGNAL(clicked()), this, SLOT(changeSubtitleFont()));

    lab=new QLabel(i18n("Footer:"),gb1);
    grid1->addWidget(lab,2,0);

    footerEdit=new QLineEdit(gb1);
    grid1->addWidget(footerEdit,2,1);

    footerColorButton=new KColorButton(gb1);
    grid1->addWidget(footerColorButton,2,2);

    footerFontButton=new QPushButton(i18n("Font"),gb1);
    connect( footerFontButton, SIGNAL(clicked()), this, SLOT(changeFooterFont()));
    grid1->addWidget(footerFontButton,2,3);

    layout->addWidget(gb1,0,0);
    grid1->activate();
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
