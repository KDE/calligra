/*
 *  kis_dlg_toolopts.cc - part of Krayon
 *
 *  Copyright (c) 2001 John Califf <jcaliff@compuzone.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qcheckbox.h>

#include <klocale.h>

#include "kis_dlg_toolopts.h"

/*
    ToolOptionsDialog constructor.  This widget shows options for only
    one tool at a time - the current tool in use.  There will be another
    KDialog with an iconview selector for all tools, accessed from the
    setting menu for setting all tool opts int one place. 
*/
ToolOptionsDialog::ToolOptionsDialog( tooltype tt, ToolOptsStruct ts,
    QWidget *parent, const char *name )
    : KDialog( parent, name, true )
{
    setCaption( i18n("Current Tool Options") );
    QVBoxLayout* layout = new QVBoxLayout( this, 4 );
    
    // use tooltype enumerator - 
    switch(tt)
    {
        case tt_linetool:    
        case tt_polylinetool:
        case tt_ellipsetool:    
        case tt_rectangletool:
        
            pLineToolTab = new LineToolTab(ts, 
                static_cast<QWidget *>(this));
            layout->addWidget(pLineToolTab);    
            break;
            
        case tt_brushtool:

            pBrushToolTab = new BrushToolTab(ts, 
                static_cast<QWidget *>(this));
            layout->addWidget(pBrushToolTab);    
            break;

        case tt_airbrushtool:

            pAirBrushToolTab = new AirBrushToolTab(ts, 
                static_cast<QWidget *>(this));
            layout->addWidget(pAirBrushToolTab);    
            break;

        case tt_pentool:

            pPenToolTab = new PenToolTab(ts, 
                static_cast<QWidget *>(this));
            layout->addWidget(pPenToolTab);    
            break;

        case tt_erasertool:

            pEraserToolTab = new EraserToolTab(ts, 
                static_cast<QWidget *>(this));
            layout->addWidget(pEraserToolTab);    
            break;

        case tt_filltool:
        case tt_colorchangertool:

            pFillToolTab = new FillToolTab(ts, 
                static_cast<QWidget *>(this));
            layout->addWidget(pFillToolTab);    
            break;

        case tt_stamptool:
        case tt_pastetool:
        
            pStampToolTab = new StampToolTab(ts, 
                static_cast<QWidget *>(this));
            layout->addWidget(pStampToolTab);    
            break;

        default:
            // we really should show global option settings here
            pNullToolTab = new NullToolTab(ts, 
                static_cast<QWidget *>(this));
            layout->addWidget(pNullToolTab);                    
            break;
    }
    
    QHBoxLayout* buttons = new QHBoxLayout( layout, 3 );
    buttons->addStretch( 3 );

    QPushButton *ok, *cancel, *save;
    ok = new QPushButton( i18n("&OK"), this );
    ok->setDefault( true );
    ok->setMinimumSize( ok->sizeHint() );
    connect( ok, SIGNAL(clicked()), SLOT(accept()) );
    buttons->addWidget( ok );

    cancel = new QPushButton( i18n("&Cancel"), this );
    cancel->setMinimumSize( cancel->sizeHint() );
    connect( cancel, SIGNAL(clicked()), SLOT(reject()) );
    buttons->addWidget( cancel );

    save = new QPushButton( i18n("&Save"), this );
    save->setMinimumSize( save->sizeHint() );
    connect( save, SIGNAL(clicked()), SLOT(reject()) );
    buttons->addWidget( save );
 
    resize( 1, 1 );
}


/*
    KisToolTab - base class for all tool tab widgets
*/
KisToolTab::KisToolTab(ToolOptsStruct ts, QWidget *_parent, const char *_name)
    : QWidget( _parent, _name )
{

}


/*
    NullToolTab - General Settings
*/
NullToolTab::NullToolTab( ToolOptsStruct ts,
    QWidget *_parent, const char *_name  )
    : KisToolTab(ts,  _parent, _name )
{
    QVBoxLayout* lout = new QVBoxLayout( this, 4 );    
    QGridLayout* grid = new QGridLayout(lout, 2, 4);

    mpThickness = new QSpinBox( 1, 16, 1, this );
    mpThickness->setValue( ts.lineThickness );
    QLabel* tlabel = new QLabel( mpThickness, 
        i18n("Line Thickness"), this );

    grid->addWidget( tlabel, 0, 0 );
    grid->addWidget( mpThickness, 0, 1 );

    mpOpacity = new QSpinBox( 0, 255, 32, this );
    mpOpacity->setValue( ts.lineOpacity );
    QLabel* olabel = new QLabel( mpOpacity, 
        i18n("Opacity"), this );

    grid->addWidget( olabel, 1, 0 );
    grid->addWidget( mpOpacity, 1, 1 );

    mpSolid = new QCheckBox( this );
    mpSolid->setChecked( ts.fillShapes );
    QLabel* slabel = new QLabel( mpSolid, 
        i18n("Fill Interior Regions"), this );

    grid->addWidget( slabel, 2, 0 );
    grid->addWidget( mpSolid, 2, 1 );

    mpUsePattern = new QCheckBox( this );
    mpUsePattern->setChecked( ts.usePattern );
    QLabel* plabel = new QLabel( mpUsePattern, 
        i18n("Use Current Pattern"), this );

    grid->addWidget( plabel, 3, 0 );
    grid->addWidget( mpUsePattern, 3, 1 );
}


/*
    LineToolTab - for lines, circle, ellipses, polylines,
    rectangles, pologons.  All Qt drawing primitives except
    text, which needs its own dialog for font selection 
*/
LineToolTab::LineToolTab( ToolOptsStruct ts,
    QWidget *_parent, const char *_name  )
    : KisToolTab(ts,  _parent, _name )
{
    QVBoxLayout* lout = new QVBoxLayout( this, 4 );    
    QGridLayout* grid = new QGridLayout(lout, 2, 4);

    mpThickness = new QSpinBox( 1, 16, 1, this );
    mpThickness->setValue( ts.lineThickness );
    QLabel* tlabel = new QLabel( mpThickness, 
        i18n("T&hickness"), this );

    grid->addWidget( tlabel, 0, 0 );
    grid->addWidget( mpThickness, 0, 1 );

    mpOpacity = new QSpinBox( 0, 255, 32, this );
    mpOpacity->setValue( ts.lineOpacity );
    QLabel* olabel = new QLabel( mpOpacity, 
        i18n("&Opacity"), this );

    grid->addWidget( olabel, 1, 0 );
    grid->addWidget( mpOpacity, 1, 1 );

    mpSolid = new QCheckBox( this );
    mpSolid->setChecked( ts.fillShapes );
    QLabel* slabel = new QLabel( mpSolid, 
        i18n("Fill Interior Regions"), this );

    grid->addWidget( slabel, 2, 0 );
    grid->addWidget( mpSolid, 2, 1 );

    mpUsePattern = new QCheckBox( this );
    mpUsePattern->setChecked( ts.usePattern );
    QLabel* plabel = new QLabel( mpUsePattern, 
        i18n("Use Current Pattern"), this );

    grid->addWidget( plabel, 3, 0 );
    grid->addWidget( mpUsePattern, 3, 1 );
    
    mpUseGradient = new QCheckBox( this );
    mpUseGradient->setChecked( ts.useGradient );
    QLabel* glabel = new QLabel( mpUseGradient, 
        i18n("Fill with Gradient"), this );

    grid->addWidget( glabel, 4, 0 );
    grid->addWidget( mpUseGradient, 4, 1 );
    
}


/*
    FillToolTab - for lines, circle, ellipses, polylines,
    rectangles, pologons.  All Qt drawing primitives except
    text, which needs its own dialog for font selection 
*/
FillToolTab::FillToolTab( ToolOptsStruct ts,
    QWidget *_parent, const char *_name  )
    : KisToolTab(ts, _parent, _name )
{
    QVBoxLayout* lout = new QVBoxLayout( this, 4 );    
    QGridLayout* grid = new QGridLayout(lout, 2, 3);

    mpOpacity = new QSpinBox( 0, 255, 32, this );
    mpOpacity->setValue( ts.opacity );
    QLabel* olabel = new QLabel( mpOpacity, 
        i18n("Opacity"), this );

    grid->addWidget( olabel, 0, 0 );
    grid->addWidget( mpOpacity, 0, 1 );

    mpUsePattern = new QCheckBox( this );
    mpUsePattern->setChecked( ts.usePattern );
    QLabel* plabel = new QLabel( mpUsePattern, 
        i18n("Fill with Pattern"), this );

    grid->addWidget( plabel, 1, 0 );
    grid->addWidget( mpUsePattern, 1, 1 );

    mpUseGradient = new QCheckBox( this );
    mpUseGradient->setChecked( ts.useGradient );
    QLabel* glabel = new QLabel( mpUseGradient, 
        i18n("Fill with Gradient"), this );

    grid->addWidget( glabel, 2, 0 );
    grid->addWidget( mpUseGradient, 2, 1 );

}


/*
    PenToolTab - for lines, circle, ellipses, polylines,
    rectangles, pologons.  All Qt drawing primitives except
    text, which needs its own dialog for font selection 
*/
PenToolTab::PenToolTab( ToolOptsStruct ts,
    QWidget *_parent, const char *_name  )
    : KisToolTab(ts, _parent, _name )
{
    QVBoxLayout* lout = new QVBoxLayout( this, 4 );    
    QGridLayout* grid = new QGridLayout(lout, 2, 4);

    mpOpacity = new QSpinBox( 0, 255, 32, this );
    mpOpacity->setValue( ts.opacity );
    QLabel* olabel = new QLabel( mpOpacity, 
        i18n("Opacity"), this );

    grid->addWidget( olabel, 0, 0 );
    grid->addWidget( mpOpacity, 0, 1 );

    mpPenThreshold = new QSpinBox( 0, 255, 32, this );
    mpPenThreshold->setValue( ts.penThreshold );
    QLabel* tlabel = new QLabel( mpPenThreshold, 
        i18n("Paint Threshold"), this );

    grid->addWidget( tlabel, 1, 0 );
    grid->addWidget( mpPenThreshold, 1, 1 );

    mpUsePattern = new QCheckBox( this );
    mpUsePattern->setChecked( ts.usePattern );
    QLabel* plabel = new QLabel( mpUsePattern, 
        i18n("Paint with pattern"), this );

    grid->addWidget( plabel, 2, 0 );
    grid->addWidget( mpUsePattern, 2, 1 );

    mpUseGradient = new QCheckBox( this );
    mpUseGradient->setChecked( ts.useGradient );
    QLabel* glabel = new QLabel( mpUseGradient, 
        i18n("Paint with gradient"), this );

    grid->addWidget( glabel, 3, 0 );
    grid->addWidget( mpUseGradient, 3, 1 );

}

/*
    BrushToolTab - for lines, circle, ellipses, polylines,
    rectangles, pologons.  All Qt drawing primitives except
    text, which needs its own dialog for font selection 
*/
BrushToolTab::BrushToolTab( ToolOptsStruct ts,
    QWidget *_parent, const char *_name  )
    : KisToolTab(ts, _parent, _name )
{
    QVBoxLayout* lout = new QVBoxLayout( this, 4 );    
    QGridLayout* grid = new QGridLayout(lout, 2, 3);

    mpOpacity = new QSpinBox( 0, 255, 32, this );
    mpOpacity->setValue( ts.opacity );
    QLabel* olabel = new QLabel( mpOpacity, 
        i18n("&Opacity"), this );

    grid->addWidget( olabel, 0, 0 );
    grid->addWidget( mpOpacity, 0, 1 );

    mpUseGradient = new QCheckBox( this );
    mpUseGradient->setChecked( ts.useGradient );
    QLabel* glabel = new QLabel( mpUseGradient, 
        i18n("Blend With Current Gradient"), this );

    grid->addWidget( glabel, 1, 0 );
    grid->addWidget( mpUseGradient, 1, 1 );

    mpUsePattern = new QCheckBox( this );
    mpUsePattern->setChecked( ts.usePattern );
    QLabel* plabel = new QLabel( mpUsePattern, 
        i18n("Blend With Current Pattern"), this );

    grid->addWidget( plabel, 2, 0 );
    grid->addWidget( mpUsePattern, 2, 1 );
}

/*
    EraserToolTab - for lines, circle, ellipses, polylines,
    rectangles, pologons.  All Qt drawing primitives except
    text, which needs its own dialog for font selection 
*/
EraserToolTab::EraserToolTab( ToolOptsStruct ts,
    QWidget *_parent, const char *_name  )
    : KisToolTab(ts, _parent, _name )
{
    QVBoxLayout* lout = new QVBoxLayout( this, 4 );    
    QGridLayout* grid = new QGridLayout(lout, 2, 3);

    mpOpacity = new QSpinBox( 0, 255, 32, this );
    mpOpacity->setValue( ts.opacity );
    QLabel* olabel = new QLabel( mpOpacity, 
        i18n("&Opacity"), this );

    grid->addWidget( olabel, 0, 0 );
    grid->addWidget( mpOpacity, 0, 1 );

    mpUseGradient = new QCheckBox( this );
    mpUseGradient->setChecked( ts.fillShapes );
    QLabel* glabel = new QLabel( mpUseGradient, 
        i18n("Blend With Current Gradient"), this );

    grid->addWidget( glabel, 1, 0 );
    grid->addWidget( mpUseGradient, 1, 1 );

    mpUsePattern = new QCheckBox( this );
    mpUsePattern->setChecked( ts.usePattern );
    QLabel* plabel = new QLabel( mpUsePattern, 
        i18n("Blend With Current Pattern"), this );

    grid->addWidget( plabel, 2, 0 );
    grid->addWidget( mpUsePattern, 2, 1 );
}

/*
    AirBrushToolTab - 
*/
AirBrushToolTab::AirBrushToolTab( ToolOptsStruct ts,
    QWidget *_parent, const char *_name  )
    : KisToolTab(ts, _parent, _name )
{
    QVBoxLayout* lout = new QVBoxLayout( this, 4 );    
    QGridLayout* grid = new QGridLayout(lout, 2, 3);

    mpOpacity = new QSpinBox( 0, 255, 32, this );
    mpOpacity->setValue( ts.opacity );
    QLabel* olabel = new QLabel( mpOpacity, 
        i18n("&Opacity"), this );

    grid->addWidget( olabel, 0, 0 );
    grid->addWidget( mpOpacity, 0, 1 );

    mpUseGradient = new QCheckBox( this );
    mpUseGradient->setChecked( ts.useGradient );
    QLabel* glabel = new QLabel( mpUseGradient, 
        i18n("Use Current Gradient"), this );

    grid->addWidget( glabel, 1, 0 );
    grid->addWidget( mpUseGradient, 1, 1 );

    mpUsePattern = new QCheckBox( this );
    mpUsePattern->setChecked( ts.usePattern );
    QLabel* plabel = new QLabel( mpUsePattern, 
        i18n("Use Current Pattern"), this );

    grid->addWidget( plabel, 2, 0 );
    grid->addWidget( mpUsePattern, 2, 1 );
}



/*
    StampBrushToolTab - 
*/
StampToolTab::StampToolTab( ToolOptsStruct ts,
    QWidget *_parent, const char *_name  )
    : KisToolTab(ts, _parent, _name )
{
    QVBoxLayout* lout = new QVBoxLayout( this, 4 );    
    QGridLayout* grid = new QGridLayout(lout, 2, 2);

    mpOpacity = new QSpinBox( 0, 255, 32, this );
    mpOpacity->setValue( ts.opacity );
    QLabel* olabel = new QLabel( mpOpacity, 
        i18n("&Opacity"), this );

    grid->addWidget( olabel, 0, 0 );
    grid->addWidget( mpOpacity, 0, 1 );

    mpUseGradient = new QCheckBox( this );
    mpUseGradient->setChecked( ts.useGradient );
    QLabel* glabel = new QLabel( mpUseGradient, 
        i18n("Blend With Current Gradient"), this );

    grid->addWidget( glabel, 1, 0 );
    grid->addWidget( mpUseGradient, 1, 1 );
}


#include "kis_dlg_toolopts.moc"
