/*
    $Id$

    kCalculator, a simple scientific calculator for KDE

    Copyright (C) 1996 Bernd Johannes Wuebben wuebben@math.cornell.edu

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.

*/

#include "configdlg.h"
#include "version.h"
#include <klocale.h>
#include <knotifyclient.h>
#include <qlayout.h>
#include <qobject.h>
#include <qpushbutton.h>
//Added by qt3to4:
#include <QPixmap>
#include <QCloseEvent>
#include <QGridLayout>
#include <Q3PtrList>
#include <QKeyEvent>
#include <QFrame>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <kapplication.h>
#include <kglobalsettings.h>
#include <qstyle.h>
#include <kdialog.h>
#include <kconfig.h>
#include <kglobal.h>
#include "kcalc.h"

extern last_input_type last_input;
extern item_contents   display_data;
extern num_base        current_base;

Q3PtrList<CALCAMNT>       temp_stack;

QtCalculator :: QtCalculator( Calculator *_corba, QWidget *parent, const char *name )
  : QDialog( parent, name )
{

    corba = _corba;
    mInternalSpacing=4;
    key_pressed = false;
    selection_timer = new QTimer;
    status_timer = new QTimer;

    connect(status_timer,SIGNAL(timeout()),this,SLOT(clear_status_label()));
    connect(selection_timer,SIGNAL(timeout()),this,SLOT(selection_timed_out()));

    readSettings();

    QFont buttonfont( KGlobalSettings::generalFont() );
    buttonfont.setStyleStrategy( QFont::PreferAntialias );

    // Set the window caption/title

    // connect(kapp,SIGNAL(kdisplayPaletteChanged()),this,SLOT(set_colors()));
    // setCaption( KInstance::caption() );

    // create help button

    QPushButton *pb;

    pb = new QPushButton( this, "helpbutton" );
    pb->setText( "kCalc" );
    pb->setFont( QFont("times",12,QFont::Bold,false) );
    pb->setToolTip( i18n("KCalc Setup/Help") );

    connect( pb, SIGNAL(clicked()), SLOT(configclicked()) );

    // Create the display
    calc_display = new DLabel( this, "display" );
    calc_display->setFrameStyle( QFrame::WinPanel | QFrame::Sunken );
    calc_display->setAlignment( Qt::AlignRight|Qt::AlignVCenter );
    calc_display->setFocus();
    calc_display->setFocusPolicy( Qt::StrongFocus );

    connect(calc_display,SIGNAL(clicked()),this,SLOT(display_selected()));

    statusINVLabel = new QLabel( this, "INV" );
    Q_CHECK_PTR( statusINVLabel );
    statusINVLabel->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    statusINVLabel->setAlignment( Qt::AlignCenter );
    statusINVLabel->setText("NORM");

    statusHYPLabel = new QLabel( this, "HYP" );
    Q_CHECK_PTR( statusHYPLabel );
    statusHYPLabel->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    statusHYPLabel->setAlignment( Qt::AlignCenter );

    statusERRORLabel = new QLabel( this, "ERROR" );
    Q_CHECK_PTR( statusERRORLabel );
    statusERRORLabel->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    statusERRORLabel->setAlignment( Qt::AlignLeft|Qt::AlignVCenter );

    // create angle button group

    Q3Accel *accel = new Q3Accel( this );

    Q3ButtonGroup *angle_group = new Q3ButtonGroup( 3, Qt::Horizontal,this, "AngleButtons" );
    angle_group->setTitle(i18n( "Angle") );

    anglebutton[0] = new QRadioButton( angle_group );
    anglebutton[0]->setText( "&Deg" )   ;
    anglebutton[0]->setChecked(   true);
    accel->connectItem( accel->insertItem(Qt::Key_D + Qt::ALT), this ,
                        SLOT(Deg_Selected()) );

    anglebutton[1] = new QRadioButton( angle_group );
    anglebutton[1]->setText( "&Rad" );
    accel->connectItem( accel->insertItem(Qt::Key_R + Qt::ALT), this ,
                        SLOT(Rad_Selected()) );

    anglebutton[2] = new QRadioButton( angle_group );
    anglebutton[2]->setText( "&Gra" );
    accel->connectItem( accel->insertItem(Qt::Key_G + Qt::ALT), this ,
                        SLOT(Gra_Selected()) );

    connect( angle_group, SIGNAL(clicked(int)), SLOT(angle_selected(int)) );


//////////////////////////////////////////////////////////////////////
//
// Create Number Base Button Group
//



    Q3ButtonGroup *base_group = new Q3ButtonGroup( 4, Qt::Horizontal,this, "BaseButtons" );
    base_group->setTitle( i18n("Base") );

    basebutton[0] = new QRadioButton( base_group );
    basebutton[0]->setText( "&Hex" );
    accel->connectItem( accel->insertItem(Qt::Key_H + Qt::ALT), this ,
                        SLOT(Hex_Selected()) );

    basebutton[1] = new QRadioButton( base_group );
    basebutton[1]->setText( "D&ec" );
    basebutton[1]->setChecked(true);
    accel->connectItem( accel->insertItem(Qt::Key_E + Qt::ALT), this ,
                        SLOT(Dec_Selected()) );

    basebutton[2] = new QRadioButton( base_group );
    basebutton[2]->setText( "&Oct" );
    accel->connectItem( accel->insertItem(Qt::Key_O + Qt::ALT), this ,
                        SLOT(Oct_Selected()) );

    basebutton[3] = new QRadioButton( base_group);
    basebutton[3]->setText( "&Bin" );
    accel->connectItem( accel->insertItem(Qt::Key_B + Qt::ALT), this ,
                        SLOT(Bin_Selected()) );

    connect( base_group, SIGNAL(clicked(int)), SLOT(base_selected(int)) );

////////////////////////////////////////////////////////////////////////
//
//  Create Calculator Buttons
//


    // First the widgets that are the parents of the buttons
    mSmallPage = new QWidget(this);
    mLargePage = new QWidget(this);

    pbhyp = new QPushButton("Hyp", mSmallPage, "hypbutton" );
    connect( pbhyp, SIGNAL(toggled(bool)), SLOT(pbhyptoggled(bool)));
    pbhyp->setToggleButton(true);

    pbinv = new QPushButton( "Inv", mSmallPage, "InverseButton" );
    connect( pbinv, SIGNAL(toggled(bool)), SLOT(pbinvtoggled(bool)));
    pbinv->setToggleButton(true);

    pbA = new QPushButton("A",mSmallPage, "Abutton" );
    connect( pbA, SIGNAL(toggled(bool)), SLOT(pbAtoggled(bool)));
    pbA->setToggleButton(true);

    pbSin = new QPushButton("Sin",mSmallPage, "Sinbutton" );
    connect( pbSin, SIGNAL(toggled(bool)), SLOT(pbSintoggled(bool)));
    pbSin->setToggleButton(true);

    pbplusminus = new QPushButton( "+/-", mSmallPage, "plusminusbutton" );
    connect( pbplusminus, SIGNAL(toggled(bool)), SLOT(pbplusminustoggled(bool)));
    pbplusminus->setToggleButton(true);
    pbplusminus->setFont(buttonfont);

    pbB = new QPushButton("B", mSmallPage, "Bbutton" );
    connect( pbB, SIGNAL(toggled(bool)), SLOT(pbBtoggled(bool)));
    pbB->setToggleButton(true);

    pbCos = new QPushButton("Cos",mSmallPage, "Cosbutton" );
    pbCos->setText( "Cos" );
    connect( pbCos, SIGNAL(toggled(bool)), SLOT(pbCostoggled(bool)));
    pbCos->setToggleButton(true);


    pbreci = new QPushButton("1/x",mSmallPage, "recibutton" );
    connect( pbreci, SIGNAL(toggled(bool)), SLOT(pbrecitoggled(bool)));
    pbreci->setToggleButton(true);

    pbC = new QPushButton( "C", mSmallPage, "Cbutton" );
    connect( pbC, SIGNAL(toggled(bool)), SLOT(pbCtoggled(bool)));
    pbC->setToggleButton(true);

    pbTan = new QPushButton("Tan" ,mSmallPage,  "Tanbutton" );
    connect( pbTan, SIGNAL(toggled(bool)), SLOT(pbTantoggled(bool)));
    pbTan->setToggleButton(true);

    pbfactorial = new QPushButton("x!",mSmallPage, "factorialbutton" );
    connect( pbfactorial, SIGNAL(toggled(bool)), SLOT(pbfactorialtoggled(bool)));
    pbfactorial->setToggleButton(true);

    pbD = new QPushButton("D",mSmallPage,  "Dbutton" );
    connect( pbD, SIGNAL(toggled(bool)), SLOT(pbDtoggled(bool)));
    pbD->setToggleButton(true);

    pblog = new QPushButton("Log",mSmallPage, "logbutton" );
    connect( pblog, SIGNAL(toggled(bool)), SLOT(pblogtoggled(bool)));
    pblog->setToggleButton(true);

    pbsquare = new QPushButton( "x^2", mSmallPage,  "squarebutton" );
    connect( pbsquare, SIGNAL(toggled(bool)), SLOT(pbsquaretoggled(bool)));
    pbsquare->setToggleButton(true);

    pbE = new QPushButton("E",mSmallPage, "Ebutton" );
    connect( pbE, SIGNAL(toggled(bool)), SLOT(pbEtoggled(bool)));
    pbE->setToggleButton(true);


    pbln = new QPushButton( "Ln", mSmallPage, "lnbutton" );
    connect( pbln, SIGNAL(toggled(bool)), SLOT(pblntoggled(bool)));
    pbln->setToggleButton(true);

    pbpower = new QPushButton("x^y",mSmallPage,  "powerbutton" );
    connect( pbpower, SIGNAL(toggled(bool)), SLOT(pbpowertoggled(bool)));
    pbpower->setToggleButton(true);

    pbF = new QPushButton("F",mSmallPage, "Fbutton" );
    connect( pbF, SIGNAL(toggled(bool)), SLOT(pbFtoggled(bool)));
    pbF->setToggleButton(true);

    pbEE = new QPushButton("EE",mLargePage,  "EEbutton" );
    pbEE->setToggleButton(true);
    connect( pbEE, SIGNAL(toggled(bool)), SLOT(EEtoggled(bool)));

    pbMR = new QPushButton("MR",mLargePage,  "MRbutton" );
    connect( pbMR, SIGNAL(toggled(bool)), SLOT(pbMRtoggled(bool)));
    pbMR->setToggleButton(true);
    pbMR->setFont(buttonfont);

    pbMplusminus = new QPushButton("M+-",mLargePage,  "Mplusminusbutton" );
    connect( pbMplusminus, SIGNAL(toggled(bool)), SLOT(pbMplusminustoggled(bool)));
    pbMplusminus->setToggleButton(true);

    pbMC = new QPushButton("MC",mLargePage,  "MCbutton" );
    connect( pbMC, SIGNAL(toggled(bool)), SLOT(pbMCtoggled(bool)));
    pbMC->setToggleButton(true);

    pbClear = new QPushButton("C",mLargePage,  "Clearbutton" );
    connect( pbClear, SIGNAL(toggled(bool)), SLOT(pbCleartoggled(bool)));
    pbClear->setToggleButton(true);

    pbAC = new QPushButton("AC", mLargePage,  "ACbutton" );
    connect( pbAC, SIGNAL(toggled(bool)), SLOT(pbACtoggled(bool)));
    pbAC->setToggleButton(true);

    pb7 = new QPushButton("7",mLargePage, "7button" );
    connect( pb7, SIGNAL(toggled(bool)), SLOT(pb7toggled(bool)));
    pb7->setToggleButton(true);

    pb8 = new QPushButton("8",mLargePage, "8button" );
    connect( pb8, SIGNAL(toggled(bool)), SLOT(pb8toggled(bool)));
    pb8->setToggleButton(true);

    pb9 = new QPushButton("9",mLargePage,"9button" );
    connect( pb9, SIGNAL(toggled(bool)), SLOT(pb9toggled(bool)));
    pb9->setToggleButton(true);

    pbparenopen = new QPushButton("(",mLargePage,  "parenopenbutton" );
    connect( pbparenopen, SIGNAL(toggled(bool)), SLOT(pbparenopentoggled(bool)));
    pbparenopen->setToggleButton(true);

    pbparenclose = new QPushButton(")",mLargePage, "parenclosebutton" );
    connect( pbparenclose, SIGNAL(toggled(bool)), SLOT(pbparenclosetoggled(bool)));
    pbparenclose->setToggleButton(true);

    pband = new QPushButton("And",mLargePage, "andbutton" );
    connect( pband, SIGNAL(toggled(bool)), SLOT(pbandtoggled(bool)));
    pband->setToggleButton(true);
    pband->setFont(buttonfont);

    pb4 = new QPushButton("4",mLargePage,  "4button" );
    connect( pb4, SIGNAL(toggled(bool)), SLOT(pb4toggled(bool)));
    pb4->setToggleButton(true);

    pb5 = new QPushButton("5",mLargePage,  "5button" );
    connect( pb5, SIGNAL(toggled(bool)), SLOT(pb5toggled(bool)));
    pb5->setToggleButton(true);

    pb6 = new QPushButton("6",mLargePage,  "6button" );
    connect( pb6, SIGNAL(toggled(bool)), SLOT(pb6toggled(bool)));
    pb6->setToggleButton(true);

    pbX = new QPushButton("X",mLargePage, "Multiplybutton" );
    connect( pbX, SIGNAL(toggled(bool)), SLOT(pbXtoggled(bool)));
    pbX->setToggleButton(true);

    pbdivision = new QPushButton("/",mLargePage,  "divisionbutton" );
    connect( pbdivision, SIGNAL(toggled(bool)), SLOT(pbdivisiontoggled(bool)));
    pbdivision->setToggleButton(true);

    pbor = new QPushButton("Or",mLargePage,  "orbutton" );
    connect( pbor, SIGNAL(toggled(bool)), SLOT(pbortoggled(bool)));
    pbor->setToggleButton(true);

    pb1 = new QPushButton("1",mLargePage,  "1button" );
    connect( pb1, SIGNAL(toggled(bool)), SLOT(pb1toggled(bool)));
    pb1->setToggleButton(true);

    pb2 = new QPushButton("2",mLargePage,  "2button" );

    connect( pb2, SIGNAL(toggled(bool)), SLOT(pb2toggled(bool)));
    pb2->setToggleButton(true);

    pb3 = new QPushButton("3",mLargePage,  "3button" );
    connect( pb3, SIGNAL(toggled(bool)), SLOT(pb3toggled(bool)));
    pb3->setToggleButton(true);

    pbplus = new QPushButton("+",mLargePage, "plusbutton" );
    connect( pbplus, SIGNAL(toggled(bool)), SLOT(pbplustoggled(bool)));
    pbplus->setToggleButton(true);


    pbminus = new QPushButton("-",mLargePage,  "minusbutton" );
    connect( pbminus, SIGNAL(toggled(bool)), SLOT(pbminustoggled(bool)));
    pbminus->setToggleButton(true);

    pbshift = new QPushButton("Lsh",mLargePage, "shiftbutton" );
    connect( pbshift, SIGNAL(toggled(bool)), SLOT(pbshifttoggled(bool)));
    pbshift->setToggleButton(true);

    pbperiod = new QPushButton(".",mLargePage,  "periodbutton" );
    connect( pbperiod, SIGNAL(toggled(bool)), SLOT(pbperiodtoggled(bool)));
    pbperiod->setToggleButton(true);

    pb0 = new QPushButton("0",mLargePage,  "0button" );
    connect( pb0, SIGNAL(toggled(bool)), SLOT(pb0toggled(bool)));
    pb0->setToggleButton(true);

    pbequal = new QPushButton("=",mLargePage,  "equalbutton" );
    connect( pbequal, SIGNAL(toggled(bool)), SLOT(pbequaltoggled(bool)));
    pbequal->setToggleButton(true);

    pbpercent = new QPushButton("%",mLargePage, "percentbutton" );
    connect( pbpercent, SIGNAL(toggled(bool)), SLOT(pbpercenttoggled(bool)));
    pbpercent->setToggleButton(true);

    pbnegate = new QPushButton("Cmp",mLargePage,  "OneComplementbutton" );
    connect( pbnegate, SIGNAL(toggled(bool)), SLOT(pbnegatetoggled(bool)));
    pbnegate->setToggleButton(true);

    pbmod = new QPushButton("Mod",mLargePage,  "modbutton" );
    connect( pbmod, SIGNAL(toggled(bool)), SLOT(pbmodtoggled(bool)));
    pbmod->setToggleButton(true);

    QGridLayout *smallBtnLayout = new QGridLayout(mSmallPage);
    smallBtnLayout->setMargin(0);
    smallBtnLayout->setSpacing(mInternalSpacing);
    QGridLayout *largeBtnLayout = new QGridLayout(mLargePage);
    largeBtnLayout->setMargin(0);
    largeBtnLayout->setSpacing(mInternalSpacing);

    QHBoxLayout *topLayout		= new QHBoxLayout();
    QHBoxLayout *radioLayout	= new QHBoxLayout();
    QHBoxLayout *btnLayout		= new QHBoxLayout();
    QHBoxLayout *statusLayout	= new QHBoxLayout();

    // bring them all together
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(mInternalSpacing);
    mainLayout->setSpacing(mInternalSpacing);

    mainLayout->addLayout(topLayout );
    mainLayout->addLayout(radioLayout, 1);
    mainLayout->addLayout(btnLayout);
    mainLayout->addLayout(statusLayout);

    // button layout
    btnLayout->addWidget(mSmallPage,0,Qt::AlignTop);
    btnLayout->addSpacing(mInternalSpacing);
    btnLayout->addWidget(mLargePage,0,Qt::AlignTop);

    // small button layout
    smallBtnLayout->addWidget(pbhyp, 0, 0);
    smallBtnLayout->addWidget(pbinv, 0, 1);
    smallBtnLayout->addWidget(pbA, 0, 2);

    smallBtnLayout->addWidget(pbSin, 1, 0);
    smallBtnLayout->addWidget(pbplusminus, 1, 1);
    smallBtnLayout->addWidget(pbB, 1, 2);

    smallBtnLayout->addWidget(pbCos, 2, 0);
    smallBtnLayout->addWidget(pbreci, 2, 1);
    smallBtnLayout->addWidget(pbC, 2, 2);

    smallBtnLayout->addWidget(pbTan, 3, 0);
    smallBtnLayout->addWidget(pbfactorial, 3, 1);
    smallBtnLayout->addWidget(pbD, 3, 2);

    smallBtnLayout->addWidget(pblog, 4, 0);
    smallBtnLayout->addWidget(pbsquare, 4, 1);
    smallBtnLayout->addWidget(pbE, 4, 2);

    smallBtnLayout->addWidget(pbln, 5, 0);
    smallBtnLayout->addWidget(pbpower, 5, 1);
    smallBtnLayout->addWidget(pbF, 5, 2);

    smallBtnLayout->setRowStretch(0, 0);
    smallBtnLayout->setRowStretch(1, 0);
    smallBtnLayout->setRowStretch(2, 0);
    smallBtnLayout->setRowStretch(3, 0);
    smallBtnLayout->setRowStretch(4, 0);
    smallBtnLayout->setRowStretch(5, 0);

    // large button layout
    largeBtnLayout->addWidget(pbEE, 0, 0);
    largeBtnLayout->addWidget(pbMR, 0, 1);
    largeBtnLayout->addWidget(pbMplusminus, 0, 2);
    largeBtnLayout->addWidget(pbMC, 0, 3);
    largeBtnLayout->addWidget(pbClear, 0, 4);
    largeBtnLayout->addWidget(pbAC, 0, 5);

    largeBtnLayout->addWidget(pb7, 1, 0);
    largeBtnLayout->addWidget(pb8, 1, 1);
    largeBtnLayout->addWidget(pb9, 1, 2);
    largeBtnLayout->addWidget(pbparenopen, 1, 3);
    largeBtnLayout->addWidget(pbparenclose, 1, 4);
    largeBtnLayout->addWidget(pband, 1, 5);

    largeBtnLayout->addWidget(pb4, 2, 0);
    largeBtnLayout->addWidget(pb5, 2, 1);
    largeBtnLayout->addWidget(pb6, 2, 2);
    largeBtnLayout->addWidget(pbX, 2, 3);
    largeBtnLayout->addWidget(pbdivision, 2, 4);
    largeBtnLayout->addWidget(pbor, 2, 5);

    largeBtnLayout->addWidget(pb1, 3, 0);
    largeBtnLayout->addWidget(pb2, 3, 1);
    largeBtnLayout->addWidget(pb3, 3, 2);
    largeBtnLayout->addWidget(pbplus, 3, 3);
    largeBtnLayout->addWidget(pbminus, 3, 4);
    largeBtnLayout->addWidget(pbshift, 3, 5);

    largeBtnLayout->addWidget(pbperiod, 4, 0);
    largeBtnLayout->addWidget(pb0, 4, 1);
    largeBtnLayout->addWidget(pbequal, 4, 2);
    largeBtnLayout->addWidget(pbpercent, 4, 3);
    largeBtnLayout->addWidget(pbnegate, 4, 4);
    largeBtnLayout->addWidget(pbmod, 4, 5);

    largeBtnLayout->addItem(new QSpacerItem( 10 , 0), 0, 0 );
    largeBtnLayout->addItem(new QSpacerItem( 10 , 0), 0, 1 );
    largeBtnLayout->addItem(new QSpacerItem( 10 , 0), 0, 2 );
    largeBtnLayout->addItem(new QSpacerItem( 10 , 0), 0, 3 );
    largeBtnLayout->addItem(new QSpacerItem( 10 , 0), 0, 4 );
    topLayout->addWidget(pb);
    topLayout->addWidget(calc_display, 10);


    // radiobutton layout
    radioLayout->addWidget(base_group);
    radioLayout->addWidget(angle_group);

    // status layout
    statusLayout->addWidget(statusINVLabel);
    statusLayout->addWidget(statusHYPLabel);
    statusLayout->addWidget(statusERRORLabel, 10);

    mNumButtonList.append(pb0);
    mNumButtonList.append(pb1);
    mNumButtonList.append(pb2);
    mNumButtonList.append(pb3);
    mNumButtonList.append(pb4);
    mNumButtonList.append(pb5);
    mNumButtonList.append(pb6);
    mNumButtonList.append(pb7);
    mNumButtonList.append(pb8);
    mNumButtonList.append(pb9);

    mFunctionButtonList.append(pbhyp);
    mFunctionButtonList.append(pbinv);
    mFunctionButtonList.append(pbSin);
    mFunctionButtonList.append(pbplusminus);
    mFunctionButtonList.append(pbCos);
    mFunctionButtonList.append(pbreci);
    mFunctionButtonList.append(pbTan);
    mFunctionButtonList.append(pbfactorial);
    mFunctionButtonList.append(pblog);
    mFunctionButtonList.append(pbsquare);
    mFunctionButtonList.append(pbln);
    mFunctionButtonList.append(pbpower);

    mHexButtonList.append(pbA);
    mHexButtonList.append(pbB);
    mHexButtonList.append(pbC);
    mHexButtonList.append(pbD);
    mHexButtonList.append(pbE);
    mHexButtonList.append(pbF);

    mMemButtonList.append(pbEE);
    mMemButtonList.append(pbMR);
    mMemButtonList.append(pbMplusminus);
    mMemButtonList.append(pbMC);
    mMemButtonList.append(pbClear);
    mMemButtonList.append(pbAC);

    mOperationButtonList.append(pbX);
    mOperationButtonList.append(pbparenopen);
    mOperationButtonList.append(pbparenclose);
    mOperationButtonList.append(pband);
    mOperationButtonList.append(pbdivision);
    mOperationButtonList.append(pbor);
    mOperationButtonList.append(pbplus);
    mOperationButtonList.append(pbminus);
    mOperationButtonList.append(pbshift);
    mOperationButtonList.append(pbperiod);
    mOperationButtonList.append(pbequal);
    mOperationButtonList.append(pbpercent);
    mOperationButtonList.append(pbnegate);
    mOperationButtonList.append(pbmod);

    set_colors();
    set_precision();
    set_style();
    updateGeometry();
    setFixedHeight(minimumHeight());
    InitializeCalculator();
}

QtCalculator::~QtCalculator()
{
    delete selection_timer;
    delete status_timer;

}

void QtCalculator::set_display_font()
{
    calc_display->setFont(kcalcdefaults.font);
}

void QtCalculator::updateGeometry()
{
    QObjectList *l;
    QSize s;
    int margin;

    //
    // Uppermost bar
    //
    calc_display->setMinimumWidth(calc_display->fontMetrics().maxWidth() * 15);

    //
    // Button groups (base and angle)
    //
    //QButtonGroup *g;
    //g = (QButtonGroup*)(anglebutton[0]->parentWidget());
    //g = (QButtonGroup*)(basebutton[0]->parentWidget());

    //
    // Calculator buttons
    //
    s.setWidth(mSmallPage->fontMetrics().width("MMM"));
    s.setHeight(mSmallPage->fontMetrics().lineSpacing());

    l = (QObjectList*)mSmallPage->children(); // silence please

    for(uint i=0; i < l->count(); i++)
    {
        QObject *o = l->at(i);
        if( o->isWidgetType() )
        {
            margin = QApplication::style().
                pixelMetric(QStyle::PM_ButtonMargin, ((QWidget *)o))*2;
            ((QWidget*)o)->setMinimumSize(s.width()+margin, s.height()+margin);
            ((QWidget*)o)->installEventFilter( this );
            ((QWidget*)o)->setAcceptDrops(true);
        }
    }

    l = (QObjectList*)mLargePage->children(); // silence please

    int h1 = pbF->minimumSize().height();
    int h2 = (int)((((float)h1 + 4.0) / 5.0));
    s.setWidth(mLargePage->fontMetrics().width("MMM") +
               QApplication::style().
               pixelMetric(QStyle::PM_ButtonMargin, pbF)*2);
    s.setHeight(h1 + h2);

    for(uint i = 0; i < l->count(); i++)
    {
        QObject *o = l->at(i);
        if(o->isWidgetType())
        {
            ((QWidget*)o)->setFixedSize(s);
            ((QWidget*)o)->installEventFilter(this);
            ((QWidget*)o)->setAcceptDrops(true);
        }
    }

    //
    // The status bar
    //
    s.setWidth( statusINVLabel->fontMetrics().width("NORM") +
                statusINVLabel->frameWidth() * 2 + 10);
    statusINVLabel->setMinimumWidth(s.width());
    statusHYPLabel->setMinimumWidth(s.width());

    //setFixedSize(minimumSize());
}

void QtCalculator::exit()
{
    // QApplication::exit();
}

void QtCalculator::Hex_Selected()
{
  basebutton[0]->setChecked(true);
  basebutton[1]->setChecked(false);
  basebutton[2]->setChecked(false);
  basebutton[3]->setChecked(false);
  SetHex();
}


void QtCalculator::Dec_Selected()
{
  basebutton[0]->setChecked(false);
  basebutton[1]->setChecked(true);
  basebutton[2]->setChecked(false);
  basebutton[3]->setChecked(false);
  SetDec();
}


void QtCalculator::Oct_Selected()
{
  basebutton[0]->setChecked(false);
  basebutton[1]->setChecked(false);
  basebutton[2]->setChecked(true);
  basebutton[3]->setChecked(false);
  SetOct();
}



void QtCalculator::Bin_Selected()
{
  basebutton[0]->setChecked(false);
  basebutton[1]->setChecked(false);
  basebutton[2]->setChecked(false);
  basebutton[3]->setChecked(true);
  SetBin();
}

void QtCalculator::Deg_Selected()
{
  anglebutton[0]->setChecked(true);
  anglebutton[1]->setChecked(false);
  anglebutton[2]->setChecked(false);
  SetDeg();
}


void QtCalculator::Rad_Selected()
{
  anglebutton[0]->setChecked(false);
  anglebutton[1]->setChecked(true);
  anglebutton[2]->setChecked(false);
  SetRad();

}


void QtCalculator::Gra_Selected()
{
  anglebutton[0]->setChecked(false);
  anglebutton[1]->setChecked(false);
  anglebutton[2]->setChecked(true);
  SetGra();
}


void QtCalculator::helpclicked(){
}

void QtCalculator::keyPressEvent( QKeyEvent *e ){

  switch (e->key() ){

  case Qt::Key_F1:
     helpclicked();
     break;
  case Qt::Key_F2:
     configclicked();
     break;
  case Qt::Key_F3:
    if(kcalcdefaults.style == 0)
      kcalcdefaults.style = 1;
    else if(kcalcdefaults.style == 1)
      kcalcdefaults.style = 2;
    else
      kcalcdefaults.style = 0;
     set_style();
     break;
  case Qt::Key_Up:
     temp_stack_prev();
     break;
  case Qt::Key_Down:
     temp_stack_next();
     break;

  case Qt::Key_PageDown:
     key_pressed = true;
     pbAC->setChecked(true);
     break;
  case Qt::Key_PageUp:
     key_pressed = true;
     pbClear->setChecked(true);
     break;

  case Qt::Key_H:
     key_pressed = true;
     pbhyp->setChecked(true);
     break;
  case Qt::Key_I:
     key_pressed = true;
     pbinv->setChecked(true);
     break;
  case Qt::Key_A:
     key_pressed = true;
     pbA->setChecked(true);

     break;
  case Qt::Key_E:
     key_pressed = true;
    if (current_base == NB_HEX)
     pbE->setChecked(true);
    else
     pbEE->setChecked(true);
     break;
  case Qt::Key_Escape:
     key_pressed = true;
     pbClear->setChecked(true);
     break;
  case Qt::Key_Delete:
     key_pressed = true;
     pbAC->setChecked(true);
     break;
  case Qt::Key_S:
     key_pressed = true;
     pbSin->setChecked(true);
     break;
  case Qt::Key_Backslash:
     key_pressed = true;
     pbplusminus->setChecked(true);
     break;
  case Qt::Key_B:
     key_pressed = true;
     pbB->setChecked(true);
     break;
  case Qt::Key_7:
     key_pressed = true;
     pb7->setChecked(true);
     break;
  case Qt::Key_8:
     key_pressed = true;
     pb8->setChecked(true);
     break;
  case Qt::Key_9:
     key_pressed = true;
     pb9->setChecked(true);
     break;
  case Qt::Key_ParenLeft:
     key_pressed = true;
     pbparenopen->setChecked(true);
     break;
  case Qt::Key_ParenRight:
     key_pressed = true;
     pbparenclose->setChecked(true);
     break;
  case Qt::Key_Ampersand:
     key_pressed = true;
     pband->setChecked(true);
     break;
  case Qt::Key_C:
     key_pressed = true;
    if (current_base == NB_HEX)
     pbC->setChecked(true);
    else
     pbCos->setChecked(true);
     break;
  case Qt::Key_4:
     key_pressed = true;
     pb4->setChecked(true);
     break;
  case Qt::Key_5:
     key_pressed = true;
     pb5->setChecked(true);
     break;
  case Qt::Key_6:
     key_pressed = true;
     pb6->setChecked(true);
     break;
  case Qt::Key_Asterisk:
     key_pressed = true;
     pbX->setChecked(true);
     break;
  case Qt::Key_Slash:
     key_pressed = true;
     pbdivision->setChecked(true);
     break;
  case Qt::Key_O:
     key_pressed = true;
     pbor->setChecked(true);
     break;
  case Qt::Key_T:
     key_pressed = true;
     pbTan->setChecked(true);
     break;
  case Qt::Key_Exclam:
     key_pressed = true;
     pbfactorial->setChecked(true);
     break;
  case Qt::Key_D:
     key_pressed = true;
     if(kcalcdefaults.style == 0)
       pbD->setChecked(true); // trig mode
     else
       pblog->setChecked(true); // stat mode
    break;
  case Qt::Key_1:
     key_pressed = true;
     pb1->setChecked(true);
     break;
  case Qt::Key_2:
     key_pressed = true;
     pb2->setChecked(true);
     break;
  case Qt::Key_3:
     key_pressed = true;
     pb3->setChecked(true);
     break;
  case Qt::Key_Plus:
     key_pressed = true;
     pbplus->setChecked(true);
     break;
  case Qt::Key_Minus:
     key_pressed = true;
     pbminus->setChecked(true);
     break;
  case Qt::Key_Less:
     key_pressed = true;
     pbshift->setChecked(true);
     break;
  case Qt::Key_N:
     key_pressed = true;
     pbln->setChecked(true);
     break;
  case Qt::Key_L:
     key_pressed = true;
     pblog->setChecked(true);
     break;
  case Qt::Key_AsciiCircum:
     key_pressed = true;
     pbpower->setChecked(true);
     break;
  case Qt::Key_F:
     key_pressed = true;
     pbF->setChecked(true);
     break;
  case Qt::Key_Period:
     key_pressed = true;
     pbperiod->setChecked(true);
     break;
  case Qt::Key_Comma:
     key_pressed = true;
     pbperiod->setChecked(true);
     break;
  case Qt::Key_0:
     key_pressed = true;
     pb0->setChecked(true);
     break;
     case Qt::Key_Equal:
     key_pressed = true;
     pbequal->setChecked(true);
     break;
  case Qt::Key_Return:
     key_pressed = true;
     pbequal->setChecked(true);
     break;
  case Qt::Key_Enter:
     key_pressed = true;
     pbequal->setChecked(true);
     break;
  case Qt::Key_Percent:
     key_pressed = true;
     pbpercent->setChecked(true);
     break;
  case Qt::Key_AsciiTilde:
     key_pressed = true;
     pbnegate->setChecked(true);
     break;
  case Qt::Key_Colon:
     key_pressed = true;
     pbmod->setChecked(true);
     break;
  case Qt::Key_BracketLeft:
     key_pressed = true;
     pbsquare->setChecked(true);
     break;
 case Qt::Key_Backspace:
     key_pressed = true;
     pbAC->setChecked(true);
     break;
  case Qt::Key_R:
     key_pressed = true;
     pbreci->setChecked(true);
     break;
  }
}

void QtCalculator::keyReleaseEvent( QKeyEvent *e ){
  switch (e->key() ){

  case Qt::Key_PageDown:
     key_pressed = false;
     pbAC->setChecked(false);
     break;
  case Qt::Key_PageUp:
     key_pressed = false;
     pbClear->setChecked(false);
     break;

  case Qt::Key_H:
    key_pressed = false;
     pbhyp->setChecked(false);
     break;
  case Qt::Key_I:
    key_pressed = false;
     pbinv->setChecked(false);
     break;
  case Qt::Key_A:
    key_pressed = false;
     pbA->setChecked(false);
     break;
  case Qt::Key_E:
    key_pressed = false;
    if (current_base == NB_HEX)
     pbE->setChecked(false);
    else
     pbEE->setChecked(false);
     break;
  case Qt::Key_Escape:
    key_pressed = false;
     pbClear->setChecked(false);
     break;
  case Qt::Key_Delete:
    key_pressed = false;
     pbAC->setChecked(false);
     break;
  case Qt::Key_S:
    key_pressed = false;
     pbSin->setChecked(false);
     break;
  case Qt::Key_Backslash:
    key_pressed = false;
     pbplusminus->setChecked(false);
     break;
  case Qt::Key_B:
    key_pressed = false;
     pbB->setChecked(false);
     break;
  case Qt::Key_7:
    key_pressed = false;
     pb7->setChecked(false);
     break;
  case Qt::Key_8:
    key_pressed = false;
     pb8->setChecked(false);
     break;
  case Qt::Key_9:
    key_pressed = false;
     pb9->setChecked(false);
     break;
  case Qt::Key_ParenLeft:
    key_pressed = false;
     pbparenopen->setChecked(false);
     break;
  case Qt::Key_ParenRight:
    key_pressed = false;
     pbparenclose->setChecked(false);
     break;
  case Qt::Key_Ampersand:
    key_pressed = false;
     pband->setChecked(false);
     break;
  case Qt::Key_C:
    key_pressed = false;
    if (current_base == NB_HEX)
     pbC->setChecked(false);
    else
     pbCos->setChecked(false);
     break;
  case Qt::Key_4:
    key_pressed = false;
     pb4->setChecked(false);
     break;
  case Qt::Key_5:
    key_pressed = false;
     pb5->setChecked(false);
     break;
  case Qt::Key_6:
    key_pressed = false;
     pb6->setChecked(false);
     break;
  case Qt::Key_Asterisk:
    key_pressed = false;
     pbX->setChecked(false);
     break;
  case Qt::Key_Slash:
    key_pressed = false;
     pbdivision->setChecked(false);
     break;
  case Qt::Key_O:
    key_pressed = false;
     pbor->setChecked(false);
     break;
  case Qt::Key_T:
    key_pressed = false;
     pbTan->setChecked(false);
     break;
  case Qt::Key_Exclam:
    key_pressed = false;
     pbfactorial->setChecked(false);
     break;
  case Qt::Key_D:
    key_pressed = false;
    if(kcalcdefaults.style == 0)
      pbD->setChecked(false); // trig mode
    else
      pblog->setChecked(false);// stat mode
     break;
  case Qt::Key_1:
    key_pressed = false;
     pb1->setChecked(false);
     break;
  case Qt::Key_2:
    key_pressed = false;
     pb2->setChecked(false);
     break;
  case Qt::Key_3:
    key_pressed = false;
     pb3->setChecked(false);
     break;
  case Qt::Key_Plus:
    key_pressed = false;
     pbplus->setChecked(false);
     break;
  case Qt::Key_Minus:
    key_pressed = false;
     pbminus->setChecked(false);
     break;
  case Qt::Key_Less:
    key_pressed = false;
     pbshift->setChecked(false);
     break;
  case Qt::Key_N:
    key_pressed = false;
     pbln->setChecked(false);
     break;
  case Qt::Key_L:
    key_pressed = false;
     pblog->setChecked(false);
     break;
  case Qt::Key_AsciiCircum:
    key_pressed = false;
     pbpower->setChecked(false);
     break;
  case Qt::Key_F:
    key_pressed = false;
     pbF->setChecked(false);
     break;
  case Qt::Key_Period:
    key_pressed = false;
     pbperiod->setChecked(false);
     break;
  case Qt::Key_Comma:
    key_pressed = false;
     pbperiod->setChecked(false);
     break;
  case Qt::Key_0:
    key_pressed = false;
     pb0->setChecked(false);
     break;
  case Qt::Key_Equal:
    key_pressed = false;
     pbequal->setChecked(false);
     break;
  case Qt::Key_Return:
    key_pressed = false;
     pbequal->setChecked(false);
     break;
  case Qt::Key_Enter:
     key_pressed = false;
     pbequal->setChecked(false);
     break;
  case Qt::Key_Percent:
    key_pressed = false;
     pbpercent->setChecked(false);
     break;
  case Qt::Key_AsciiTilde:
    key_pressed = false;
     pbnegate->setChecked(false);
     break;
  case Qt::Key_Colon:
    key_pressed = false;
     pbmod->setChecked(false);
     break;
  case Qt::Key_BracketLeft:
     key_pressed = false;
     pbsquare->setChecked(false);
     break;
  case Qt::Key_Backspace:
     key_pressed = false;
     pbAC->setChecked(false);
     break;
  case Qt::Key_R:
     key_pressed = false;
     pbreci->setChecked(false);
     break;
  }

  clear_buttons();
}

void QtCalculator::clear_buttons(){

}

void QtCalculator::EEtoggled(bool myboolean){

  if(myboolean)
    EE();
  if(pbEE->isChecked() && (!key_pressed))
    pbEE->setChecked(false);
}

void QtCalculator::pbinvtoggled(bool myboolean){
  if(myboolean)
    SetInverse();
  if(pbinv->isChecked() && (!key_pressed))
    pbinv->setChecked(false);
}

void QtCalculator::pbhyptoggled(bool myboolean){
  if(myboolean)
    EnterHyp();
  if(pbhyp->isChecked() && (!key_pressed))
    pbhyp->setChecked(false);
}
void QtCalculator::pbMRtoggled(bool myboolean){
  if(myboolean)
    MR();
  if(pbMR->isChecked() && (!key_pressed))
    pbMR->setChecked(false);
}

void QtCalculator::pbAtoggled(bool myboolean){
  if(myboolean)
    buttonA();
    if(pbA->isChecked() && (!key_pressed))
   pbA->setChecked(false);
}

void QtCalculator::pbSintoggled(bool myboolean){
  if(myboolean)
    ExecSin();
  if(pbSin->isChecked() && (!key_pressed))
    pbSin->setChecked(false);
}
void QtCalculator::pbplusminustoggled(bool myboolean){
  if(myboolean)
    EnterNegate();
  if(pbplusminus->isChecked() && (!key_pressed))
    pbplusminus->setChecked(false);
}
void QtCalculator::pbMplusminustoggled(bool myboolean){
  if(myboolean)
    Mplusminus();
  if(pbMplusminus->isChecked() && (!key_pressed))
    pbMplusminus->setChecked(false);
}
void QtCalculator::pbBtoggled(bool myboolean){
  if(myboolean)
    buttonB();
  if(pbB->isChecked() && (!key_pressed))
    pbB->setChecked(false);
}
void QtCalculator::pbCostoggled(bool myboolean){
  if(myboolean)
    ExecCos();
  if(pbCos->isChecked() && (!key_pressed))
    pbCos->setChecked(false);
}
void QtCalculator::pbrecitoggled(bool myboolean){
  if(myboolean)
    EnterRecip();
  if(pbreci->isChecked() && (!key_pressed))
    pbreci->setChecked(false);
}
void QtCalculator::pbCtoggled(bool myboolean){
  if(myboolean)
    buttonC();
  if(pbC->isChecked() && (!key_pressed))
    pbC->setChecked(false);
}
void QtCalculator::pbTantoggled(bool myboolean){
  if(myboolean)
    ExecTan();
  if(pbTan->isChecked() && (!key_pressed))
    pbTan->setChecked(false);
}
void QtCalculator::pbfactorialtoggled(bool myboolean){
  if(myboolean)
    EnterFactorial();
  if(pbfactorial->isChecked() && (!key_pressed))
    pbfactorial->setChecked(false);
}
void QtCalculator::pbDtoggled(bool myboolean){
  if(myboolean)
    buttonD();
  if(pbD->isChecked() && (!key_pressed))
    pbD->setChecked(false);
}
void QtCalculator::pblogtoggled(bool myboolean){
  if(myboolean)
   EnterLogr();
  if(pblog->isChecked() && (!key_pressed))
    pblog->setChecked(false);
}
void QtCalculator::pbsquaretoggled(bool myboolean){
  if(myboolean)
    EnterSquare();
  if(pbsquare->isChecked() && (!key_pressed))
    pbsquare->setChecked(false);
}
void QtCalculator::pbEtoggled(bool myboolean){
  if(myboolean)
    buttonE();
  if(pbE->isChecked() && (!key_pressed))
    pbE->setChecked(false);
}
void QtCalculator::pblntoggled(bool myboolean){
  if(myboolean)
    EnterLogn();
  if(pbln->isChecked() && (!key_pressed))
    pbln->setChecked(false);
}
void QtCalculator::pbpowertoggled(bool myboolean){
  if(myboolean)
    Power();
  if(pbpower->isChecked() && (!key_pressed))
    pbpower->setChecked(false);
}
void QtCalculator::pbFtoggled(bool myboolean){
  if(myboolean)
    buttonF();
  if(pbF->isChecked() && (!key_pressed))
    pbF->setChecked(false);
}
void QtCalculator::pbMCtoggled(bool myboolean){
  if(myboolean)
    MC();
  if(pbMC->isChecked() && (!key_pressed))
    pbMC->setChecked(false);
}
void QtCalculator::pbCleartoggled(bool myboolean){
  if(myboolean)
    Clear();
  if(pbClear->isChecked() && (!key_pressed))
    pbClear->setChecked(false);
}
void QtCalculator::pbACtoggled(bool myboolean){
  if(myboolean)
    ClearAll();
  if(pbAC->isChecked() && (!key_pressed))
    pbAC->setChecked(false);
}
void QtCalculator::pb7toggled(bool myboolean){
  if(myboolean)
    button7();
  if(pb7->isChecked() && (!key_pressed))
    pb7->setChecked(false);
}
void QtCalculator::pb8toggled(bool myboolean){
  if(myboolean)
    button8();
  if(pb8->isChecked() && (!key_pressed))
    pb8->setChecked(false);
}
void QtCalculator::pb9toggled(bool myboolean){
  if(myboolean)
    button9();
  if(pb9->isChecked() && (!key_pressed))
    pb9->setChecked(false);
}
void QtCalculator::pbparenopentoggled(bool myboolean){
  if(myboolean)
    EnterOpenParen();
  if(pbparenopen->isChecked() && (!key_pressed))
    pbparenopen->setChecked(false);
}
void QtCalculator::pbparenclosetoggled(bool myboolean){
  if(myboolean)
    EnterCloseParen();
  if(pbparenclose->isChecked() && (!key_pressed))
    pbparenclose->setChecked(false);
}
void QtCalculator::pbandtoggled(bool myboolean){
  if(myboolean)
    And();
  if(pband->isChecked() && (!key_pressed))
    pband->setChecked(false);
}
void QtCalculator::pb4toggled(bool myboolean){
  if(myboolean)
    button4();
  if(pb4->isChecked() && (!key_pressed))
    pb4->setChecked(false);
}
void QtCalculator::pb5toggled(bool myboolean){
  if(myboolean)
    button5();
  if(pb5->isChecked() && (!key_pressed))
    pb5->setChecked(false);
}
void QtCalculator::pb6toggled(bool myboolean){
  if(myboolean)
    button6();
  if(pb6->isChecked() && (!key_pressed))
    pb6->setChecked(false);
}
void QtCalculator::pbXtoggled(bool myboolean){
  if(myboolean)
    Multiply();
  if(pbX->isChecked() && (!key_pressed))
    pbX->setChecked(false);
}
void QtCalculator::pbdivisiontoggled(bool myboolean){
  if(myboolean)
    Divide();
  if(pbdivision->isChecked() && (!key_pressed))
    pbdivision->setChecked(false);
}
void QtCalculator::pbortoggled(bool myboolean){
  if(myboolean)
    Or();
  if(pbor->isChecked() && (!key_pressed))
    pbor->setChecked(false);
}
void QtCalculator::pb1toggled(bool myboolean){
  if(myboolean)
    button1();
  if(pb1->isChecked() && (!key_pressed))
    pb1->setChecked(false);
}
void QtCalculator::pb2toggled(bool myboolean){
  if(myboolean)
    button2();
  if(pb2->isChecked() && (!key_pressed))
    pb2->setChecked(false);
}
void QtCalculator::pb3toggled(bool myboolean){
  if(myboolean)
    button3();
  if(pb3->isChecked() && (!key_pressed))
    pb3->setChecked(false);
}
void QtCalculator::pbplustoggled(bool myboolean){
  if(myboolean)
    Plus();
  if(pbplus->isChecked() && (!key_pressed))
    pbplus->setChecked(false);
}
void QtCalculator::pbminustoggled(bool myboolean){
  if(myboolean)
    Minus();
  if(pbminus->isChecked() && (!key_pressed))
    pbminus->setChecked(false);
}
void QtCalculator::pbshifttoggled(bool myboolean){
  if(myboolean)
    Shift();
  if(pbshift->isChecked() && (!key_pressed))
    pbshift->setChecked(false);
}
void QtCalculator::pbperiodtoggled(bool myboolean){
  if(myboolean)
    EnterDecimal();
  if(pbperiod->isChecked() && (!key_pressed))
    pbperiod->setChecked(false);
}
void QtCalculator::pb0toggled(bool myboolean){
  if(myboolean)
    button0();
  if(pb0->isChecked() && (!key_pressed))
    pb0->setChecked(false);
}
void QtCalculator::pbequaltoggled(bool myboolean){
  if(myboolean)
    EnterEqual();
  if(pbequal->isChecked() && (!key_pressed))
    pbequal->setChecked(false);
}
void QtCalculator::pbpercenttoggled(bool myboolean){
  if(myboolean)
    EnterPercent();
  if(pbpercent->isChecked() && (!key_pressed))
    pbpercent->setChecked(false);
}
void QtCalculator::pbnegatetoggled(bool myboolean){
  if(myboolean)
    EnterNotCmp();
  if(pbnegate->isChecked() && (!key_pressed))
    pbnegate->setChecked(false);
}
void QtCalculator::pbmodtoggled(bool myboolean)  {
  if(myboolean)
    Mod();
  if(pbmod->isChecked() && (!key_pressed))
    pbmod->setChecked(false);
}

void QtCalculator::configclicked(){


  Q3TabDialog * tabdialog;
  tabdialog = new Q3TabDialog(0,"tabdialog",true);

  tabdialog->setCaption( i18n("KCalc Configuration") );
  tabdialog->resize( 360, 390 );
  tabdialog->setCancelButton( i18n("&Cancel") );
  tabdialog->setOKButton(i18n("&OK"));

  QWidget *about = new QWidget(tabdialog );
  QVBoxLayout *lay1 = new QVBoxLayout( about );
  lay1->setMargin( KDialog::marginHint() );
  lay1->setSpacing( KDialog::spacingHint() );

  Q3GroupBox *box = new Q3GroupBox(0,Qt::Vertical,about,"box");
  box->layout()->setSpacing(KDialog::spacingHint());
  box->layout()->setMargin(KDialog::marginHint());
  QGridLayout *grid1 = new QGridLayout(box->layout());
  QLabel  *label = new QLabel(box,"label");
  QLabel  *label2 = new QLabel(box,"label2");

  box->setTitle(i18n("About"));
  grid1->addWidget(label,0,1);
  grid1->addWidget(label2,2,2,0,1);

  QString labelstring = "KCalc "KCALCVERSION"\n"\
    "Bernd Johannes Wuebben\n"\
    "wuebben@math.cornell.edu\n"\
    "wuebben@kde.org\n"\
    "Copyright (C) 1996-98\n"\
    "\n\n";

  QString labelstring2 =
#ifdef HAVE_LONG_DOUBLE
                i18n( "Base type: long double\n");
#else
                i18n( "Due to broken glibc's everywhere, "\
                      "I had to reduce KCalc's precision from 'long double' "\
                      "to 'double'. "\
                      "Owners of systems with a working libc "\
                      "should recompile KCalc with 'long double' precision "\
                      "enabled. See the README for details.");
#endif

  label->setAlignment(Qt::AlignLeft|Qt::TextWordWrap|Qt::TextExpandTabs);
  label->setText(labelstring);

  label2->setAlignment(Qt::AlignLeft|Qt::TextWordWrap|Qt::TextExpandTabs);
  label2->setText(labelstring2);

  // HACK
  // QPixmap pm( BarIcon( "kcalclogo" ) );
  QPixmap pm;
  QLabel *logo = new QLabel(box);
  logo->setIcon(pm);
  grid1->addWidget(logo,0,0);
  lay1->addWidget(box);


  DefStruct newdefstruct;
  newdefstruct.forecolor  = kcalcdefaults.forecolor;
  newdefstruct.backcolor  = kcalcdefaults.backcolor;
  newdefstruct.precision  = kcalcdefaults.precision;
  newdefstruct.fixedprecision  = kcalcdefaults.fixedprecision;
  newdefstruct.fixed  = kcalcdefaults.fixed;
  newdefstruct.style  = kcalcdefaults.style;
  newdefstruct.beep  = kcalcdefaults.beep;

  ConfigDlg *configdlg;
  configdlg = new ConfigDlg(tabdialog,"configdlg",&newdefstruct);

  tabdialog->addTab(configdlg,i18n("Defaults"));
  tabdialog->addTab(about,i18n("About"));


  if(tabdialog->exec() == QDialog::Accepted){


    kcalcdefaults.forecolor  = newdefstruct.forecolor;
    kcalcdefaults.backcolor  = newdefstruct.backcolor;
    kcalcdefaults.precision  = newdefstruct.precision;
    kcalcdefaults.fixedprecision  = newdefstruct.fixedprecision;
    kcalcdefaults.fixed  = newdefstruct.fixed;
    kcalcdefaults.style  = newdefstruct.style;
    kcalcdefaults.beep  = newdefstruct.beep;

    set_colors();
    set_precision();
    set_style();
    updateGeometry();
    resize(minimumSize());
  }
  delete configdlg;
}


void QtCalculator::set_style(){

  switch(kcalcdefaults.style){
  case  0:{
    pbhyp->setText( "Hyp" );
    pbSin->setText( "Sin" );
    pbCos->setText( "Cos" );
    pbTan->setText( "Tan" );
    pblog->setText( "Log" );
    pbln ->setText( "Ln"  );
    break;
  }
  case 1:{
    pbhyp->setText( "N" );
    pbSin->setText( "Mea" );
    pbCos->setText( "Std" );
    pbTan->setText( "Med" );
    pblog->setText( "Dat" );
    pbln ->setText( "CSt"  );
    break;
  }
  case 2:{
    pbhyp->setText( "N" );
    pbSin->setText( "Min" );
    pbCos->setText( "Max" );
    pbTan->setText( "Med" );
    pblog->setText( "Sum" );
    pbln ->setText( "Mul"  );
    break;
  }

  default:
    break;
  }
}

void QtCalculator::readSettings()
{
    QColor tmpC(189, 255, 180);
    QColor blackC(0,0,0);

    KConfig *config = KGlobal::config();
	config->setGroup("CalcPlugin");
    kcalcdefaults.forecolor = config->readColorEntry("ForeColor", &blackC);
	kcalcdefaults.backcolor = config->readColorEntry("BackColor", &tmpC);

#ifdef HAVE_LONG_DOUBLE
	kcalcdefaults.precision	= config->readNumEntry("precision", (int)14);
#else
	kcalcdefaults.precision	= config->readNumEntry("precision", (int)10);
#endif
    kcalcdefaults.fixedprecision = config->readNumEntry("fixedprecision", (int)2);
	kcalcdefaults.fixed = config->readBoolEntry("fixed", false);

	kcalcdefaults.style	= config->readNumEntry("style", (int)0);
	kcalcdefaults.beep	= config->readBoolEntry("beep", true);
}

void QtCalculator::writeSettings()
{
    KConfig *config = KGlobal::config();

	config->setGroup("CalcPlugin");
	config->writeEntry("ForeColor",kcalcdefaults.forecolor);
	config->writeEntry("BackColor",kcalcdefaults.backcolor);

	config->writeEntry("precision",  kcalcdefaults.precision);
	config->writeEntry("fixedprecision",  kcalcdefaults.fixedprecision);
	config->writeEntry("fixed",  kcalcdefaults.fixed);

	config->writeEntry("style",(int)kcalcdefaults.style);
	config->writeEntry("beep", kcalcdefaults.beep);

	config->sync();
}

void QtCalculator::display_selected(){

  if(calc_display->Button() == Qt::LeftButton){

    if(calc_display->isLit()){

      QClipboard *cb = QApplication::clipboard();
      cb->setText(calc_display->text());
      selection_timer->start(100);

    }
    else{

      selection_timer->stop();

    }

    invertColors();
  }
  else{

    QClipboard *cb = QApplication::clipboard();

    CALCAMNT result;
    result = (CALCAMNT) cb->text().toDouble();
    //    printf("%Lg\n",result);
    last_input = PASTE;
    DISPLAY_AMOUNT = result;
    UpdateDisplay();
  }

}

void QtCalculator::selection_timed_out(){

  //  printf("selection timed out\n");
  selection_timer->stop();
  calc_display->setLit(false);
  invertColors();


}


void QtCalculator::clear_status_label(){

  statusERRORLabel->setText("");
  status_timer->stop();
}

void QtCalculator::setStatusLabel(const QString& string){

  statusERRORLabel->setText(string);
  status_timer->start(3000,true);

}


void QtCalculator::invertColors(){

  QColor tmpcolor;

  if(calc_display->isLit()){
    tmpcolor = kcalcdefaults.backcolor;
    kcalcdefaults.backcolor = kcalcdefaults.forecolor;
    kcalcdefaults.forecolor = tmpcolor;
    set_colors();
    tmpcolor = kcalcdefaults.backcolor;
    kcalcdefaults.backcolor = kcalcdefaults.forecolor;
    kcalcdefaults.forecolor = tmpcolor;
  }
  else{
    set_colors();
    //printf("normal Colors\n");
  }
}

void QtCalculator::closeEvent( QCloseEvent*e )
{
    writeSettings();
    e->accept();
}

void QtCalculator::set_colors(){


  QPalette mypalette = (calc_display->palette()).copy();

  QColorGroup cgrp = mypalette.active();
  QColorGroup ncgrp(kcalcdefaults.forecolor,
                    cgrp.background(),
                    cgrp.light(),
                    cgrp.dark(),
                    cgrp.mid(),
                    kcalcdefaults.forecolor,
                    kcalcdefaults.backcolor);

  mypalette.setActive(ncgrp);
  mypalette.setDisabled(ncgrp);
  mypalette.setActive(ncgrp);

  calc_display->setPalette(mypalette);
  calc_display->setBackgroundColor(kcalcdefaults.backcolor);

}

void QtCalculator::set_precision(){

  UpdateDisplay();
}

void QtCalculator::temp_stack_next(){

  CALCAMNT *number;

  if( temp_stack.current() == temp_stack.getLast()){
        KNotifyClient::beep();
        return;
  }

  number = temp_stack.next();

  if(number == NULL){
       KNotifyClient::beep();
    return;
  }
  else{
    //    printf("Number: %Lg\n",*number);
    last_input = RECALL;
    DISPLAY_AMOUNT = *number;
    UpdateDisplay();

  }


}

void QtCalculator::temp_stack_prev(){

  CALCAMNT *number;

  if( temp_stack.current() == temp_stack.getFirst()){
        KNotifyClient::beep();
        return;
  }

  number = temp_stack.prev();

  if(number == NULL){
    KNotifyClient::beep();
    return;
  }
  else{
    //    printf("Number: %Lg\n",*number);
    last_input = RECALL;
    DISPLAY_AMOUNT = *number;
    UpdateDisplay();

  }

}

#include "kcalc.moc"
