/* This file is part of the KDE project
   Copyright (C) 2000 Matthias Kalle Dalheimer <kalle@kde.org>

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

#include "kchartBackgroundPixmapConfigPage.h"
#include "kchartBackgroundPixmapConfigPage.moc"

#include <klocale.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kdebug.h>

#include <QComboBox>
#include <qradiobutton.h>
#include <q3buttongroup.h>
#include <QSpinBox>
#include <QLayout>

#include <QPushButton>
#include <QLabel>

#include <q3listbox.h>
//Added by qt3to4:
#include <QPixmap>
#include <Q3Frame>
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>
#include <kcolorbutton.h>
#include <kvbox.h>

#include "kchart_params.h"

namespace KChart
{

KChartBackgroundPixmapConfigPage::KChartBackgroundPixmapConfigPage( KChartParams* params, QWidget* parent )
    : QWidget( parent, "KChartBackgroundPixmapConfigPage" ),
      _params( params )
{
    this->setWhatsThis(
                     i18n( "On this page, you can select colors or images "
                           "to be displayed behind the different areas. You "
                           "can also select whether the images should be "
                           "stretched or scaled or centered or used as "
                           "background tiles." ) );

    Q3HBoxLayout* toplevel = new Q3HBoxLayout( this, 10 );

#if 0
    Q3VBoxLayout* left=new Q3VBoxLayout(10);
    toplevel->addLayout(left,2);
    regionList=new Q3ListBox(this);
    left->addWidget(regionList);
#endif

    Q3VBoxLayout* center = new Q3VBoxLayout( 10 );
    toplevel->addLayout( center, 2 );


    QLabel* backgroundLA = new QLabel( i18n( "&Background color:" ), this );
    center->addWidget( backgroundLA );
    _backgroundCB = new KColorButton( this );
    backgroundLA->setBuddy( _backgroundCB );
    center->addWidget( _backgroundCB);
    QString wtstr = i18n( "Here you set the color in which the background "
                          "of the chart is painted." );
    backgroundLA->setWhatsThis( wtstr );
    _backgroundCB->setWhatsThis( wtstr );


    QLabel* wallpaperLA = new QLabel( i18n( "Background wallpaper:" ), this );
    center->addWidget( wallpaperLA );
    
    wallCB = new QComboBox( false, this, "wallCombo" );
    wallpaperLA->setBuddy(wallCB);
    wallCB->setWhatsThis( i18n( "You can select a background image from "
                                   "this list. Initially, the installed KDE "
                                   "wallpapers will be offered. If you do not "
                                   "find what you are looking for here, you can "
                                   "select any image file by clicking on the "
                                   "<i>Browse</i> button below." ) );
    center->addWidget( wallCB );
    wallCB->insertItem( i18n("None") );

    QStringList list = KGlobal::dirs()->findAllResources( "wallpaper" );

    for( QStringList::ConstIterator it = list.begin(); it != list.end(); it++ )
	wallCB->insertItem( ( (*it).at(0)=='/' ) ?        // if absolute path
                            KUrl( *it ).fileName() :    // then only fileName
                            *it );

    QPushButton* browsePB = new QPushButton( i18n("&Browse..."), this );
    browsePB->setWhatsThis( i18n( "Click this button to select a background "
                                     "image not yet present in the list above. " ) );
    center->addWidget( browsePB );
    connect( browsePB, SIGNAL( clicked() ), SLOT( slotBrowse() ) );

    wallWidget = new QWidget( this );
    wallWidget->setWhatsThis( i18n( "This area will always display the "
                                       "currently selected background image. "
                                       "Note that the image will be scaled and "
                                       "thus might have a different ratio than "
                                       "it originally had." ) );
    center->addWidget( wallWidget );

    connect( wallCB, SIGNAL( activated( int ) ),
             this, SLOT( slotWallPaperChanged( int ) ) );

    right = new Q3GroupBox(1, Qt::Horizontal, i18n( "Wallpaper Configuration" ), this );
    right->setWhatsThis( i18n( "In this box, you can set various settings "
                                  "that control how the background image is "
                                  "displayed." ) );
    toplevel->addWidget( right );

    KHBox* intensityHB = new KHBox( right );
    intensityHB->setSpacing( 10 );
    QLabel* intensityLA = new QLabel(
      // xgettext:no-c-format
      i18n( "&Intensity in %:" ), intensityHB );
    intensitySB = new QSpinBox( 1, 100, 1, intensityHB );
    intensityLA->setBuddy( intensitySB );
    // xgettext:no-c-format
    QString ttstr = i18n( "Here you can select how much the image should be "
                          "brightened up so that it does not disturb the "
                          "selected area too much.<br> Different images require "
                          "different settings, but 25% is a good value to start "
                          "with." );
    intensityLA->setWhatsThis( ttstr );
    intensitySB->setWhatsThis( ttstr );


    stretchedRB = new QRadioButton( i18n( "Stretched" ), right );
    stretchedRB->setWhatsThis(
                     i18n( "If you check this box, the selected image will "
                           "be scaled to fit the total size of the selected "
                           "area. Image ratio will be adjusted to match "
                           "the area size and height if necessary." ) );
    stretchedRB->setChecked( true );
    scaledRB = new QRadioButton( i18n( "Scaled" ), right );
    scaledRB->setWhatsThis(
                     i18n( "If you check this box, the selected image will "
                           "be scaled to match the height or width of the "
                           "selected area - whichever is reached first." ) );
    centeredRB = new QRadioButton( i18n( "Centered" ), right );
    centeredRB->setWhatsThis(
                     i18n( "If you check this box, the selected image will "
                           "be centered over the selected area. If the image "
                           "is larger then the area, you will only see the "
                           "middle part of it." ) );

//     tiledRB = new QRadioButton( i18n( "Tiled" ), right );
//     QWhatsThis::add( tiledRB,
//                      i18n( "If you check this box, the selected image will "
//                            "be used as a background tile. If the image is "
//                            "larger then the selected area, you will only see "
//                            "the upper left part of it." ) );
    Q3ButtonGroup* alignmentBG;
    alignmentBG = new Q3ButtonGroup( right, "GroupBox_Alignment" );
    //alignmentBG->setFrameStyle( Q3Frame::NoFrame );
    alignmentBG->insert( stretchedRB );
    alignmentBG->insert( scaledRB );
    alignmentBG->insert( centeredRB );
//     alignmentBG->insert( tiledRB );

    intensitySB->hide(); //the property doesn't work atm
}



void KChartBackgroundPixmapConfigPage::init()
{
#if 0
    QStringList lst;
    lst.append(i18n( "Outermost Region" ));
    lst.append(i18n( "Innermost Region" ));
    lst.append(i18n( "Header+Title+Subtitle" ));
    lst.append(i18n( "Footers" ));
    lst.append(i18n( "Data+Axes+Legend" ));
    lst.append(i18n( "Data+Axes" ));
    lst.append(i18n( "Data" ));
    lst.append(i18n( "Legend" ));
    lst.append(i18n( "Left Axis" ));
    lst.append(i18n( "Bottom Axis" ));
    lst.append(i18n( "Right Axis" ));
    lst.append(i18n( "Header A" ));
    lst.append(i18n( "Header B" ));
    lst.append(i18n( "Header C" ));
    lst.append(i18n( "Title A" ));
    lst.append(i18n( "Title B" ));
    lst.append(i18n( "Title C" ));
    lst.append(i18n( "Subtitle A" ));
    lst.append(i18n( "Subtitle B" ));
    lst.append(i18n( "Subtitle C" ));
    lst.append(i18n( "Footer 1 A" ));
    lst.append(i18n( "Footer 1 B" ));
    lst.append(i18n( "Footer 1 C" ));
    lst.append(i18n( "Footer 2 A" ));
    lst.append(i18n( "Footer 2 B" ));
    lst.append(i18n( "Footer 2 C" ));
    lst.append(i18n( "Footer 3 A" ));
    lst.append(i18n( "Footer 3 B" ));
    lst.append(i18n( "Footer 3 C" ));
    regionList->insertStringList(lst);
#endif
    // PENDING(kalle) Readapt
    //     showSettings( _params->backgroundPixmapName );
//     intensitySB->setValue( (int)(_params->backgroundPixmapIntensity * 100.0) );
//     scaledCB->setChecked( _params->backgroundPixmapScaled );
//     centeredCB->setChecked( _params->backgroundPixmapCentered );

    bool bFound;
    const KDChartParams::KDChartFrameSettings * innerFrame =
        _params->frameSettings( KDChartEnums::AreaInnermost, bFound );
    if( bFound )
    {
        const QPixmap* backPixmap;
        KDFrame::BackPixmapMode backPixmapMode;
        const QBrush& background = innerFrame->frame().background( backPixmap, backPixmapMode );
        if( !backPixmap || backPixmap->isNull() ) //color as background
        {
            _backgroundCB->setColor( background.color() );
            right->setEnabled( false );
            wallCB->setCurrentItem( 0 );
        }
        else //pixmap as background
        {
            _backgroundCB->setEnabled( false );
            wallWidget->setPaletteBackgroundPixmap( *backPixmap );
            wallCB->setCurrentItem( 1 );
        }

        if ( backPixmapMode == KDFrame::PixCentered )
            centeredRB->setChecked( true );
        else if ( backPixmapMode == KDFrame::PixScaled )
            scaledRB->setChecked( true );
        else // PixStretched
            stretchedRB->setChecked( true );
        // pending KHZ
        // else
        //     ..  // set the background pixmap
    }
    else
        _backgroundCB->setColor(QColor(230, 222, 222) );
}

void KChartBackgroundPixmapConfigPage::apply()
{
    // PENDING(kalle) Readapt
    //     if( wallCB->currentText() != _params->backgroundPixmapName ) {
//             bool load=true;
//             if(wallCB->currentText()==i18n("None")) {
//                     load=false;
//                 } else {
//                     _params->backgroundPixmapName = wallCB->currentText();
//                     bool load=_params->backgroundPixmap.load( locate( "wallpaper", _params->backgroundPixmapName ) );
//                     if(load)
//                         _params->backgroundPixmapIsDirty = true;
//                 }

//             if( !load ) {
//                     _params->backgroundPixmapName = "";
//                     _params->backgroundPixmap=QPixmap("");
//                     _params->backgroundPixmapIsDirty = false;
//                 }
//         }
//     if( (int)(_params->backgroundPixmapIntensity * 100.0) !=
//         intensitySB->value() ) {
// 	_params->backgroundPixmapIntensity = (float)(intensitySB->value()) / 100.0;
// 	_params->backgroundPixmapIsDirty = true;
//     }

//     if( _params->backgroundPixmapScaled !=
//         scaledCB->isChecked() ) {
// 	_params->backgroundPixmapScaled = scaledCB->isChecked();
// 	_params->backgroundPixmapIsDirty = true;
//     }
//     if( _params->backgroundPixmapCentered !=
//         centeredCB->isChecked() ) {
// 	_params->backgroundPixmapCentered = centeredCB->isChecked();
// 	_params->backgroundPixmapIsDirty = true;
//     }

    const QColor backColor( _backgroundCB->color() );
    //
    // temp. hack: the background is removed if "None" is selected in the combo box
    //
    //             For KOffice 1.5/2.0 this is to be removed by a checkbox.
        bool bFound;
        const KDChartParams::KDChartFrameSettings * innerFrame =
            _params->frameSettings( KDChartEnums::AreaInnermost, bFound );
        if( bFound )
        {
            KDFrame& frame( (KDFrame&)innerFrame->frame() );
            if ( wallCB->currentItem() == 0 )
            {
                frame.setBackPixmap( 0  );
                frame.setBackground( _backgroundCB->color() );
            }
            else
            {
                KDFrame::BackPixmapMode backPixmapMode;
                if ( centeredRB->isChecked() )
                    backPixmapMode = KDFrame::PixScaled;
                else if ( scaledRB->isChecked() )
                    backPixmapMode = KDFrame::PixCentered;
                else
                    backPixmapMode = KDFrame::PixStretched;
                frame.setBackPixmap( wallWidget->paletteBackgroundPixmap(), backPixmapMode );
            }
        }
}


void KChartBackgroundPixmapConfigPage::showSettings( const QString& fileName )
{
    for( int i = 1; i < wallCB->count(); i++ )
    {
        if( fileName == wallCB->text( i ) )
        {
            wallCB->setCurrentItem( i );
            loadWallPaper();
            return;
        }
    }

    if( !fileName.isEmpty() )
    {
        wallCB->insertItem( fileName );
        wallCB->setCurrentItem( wallCB->count()-1 );
    }
    else
        wallCB->setCurrentItem( 0 );

    loadWallPaper();
}


void KChartBackgroundPixmapConfigPage::slotBrowse()
{
    //TODO: Support non-local wallpapers
    KUrl url = KFileDialog::getOpenFileName( 0 );
    if( url.isEmpty() )
        return;
    showSettings( url.path() );
}

void KChartBackgroundPixmapConfigPage::slotWallPaperChanged( int index )
{
    if ( index != 0 && loadWallPaper() )
    {
        _backgroundCB->setEnabled( false );
        right->setEnabled( true );
    }
    else
    {
        wallWidget->setPaletteBackgroundPixmap( 0 );
        _backgroundCB->setEnabled( true );
        right->setEnabled( false );
    }
}


bool KChartBackgroundPixmapConfigPage::loadWallPaper()
{
    int i = wallCB->currentItem();
    if ( i == -1 || i == 0 ) {  // 0 is 'None'
	wallPixmap.resize(0,0);
	wallFile = "";
    } else {
	wallFile = wallCB->text( i );
	QString file = locate("wallpaper", wallFile);
	if( file.isEmpty() ) {
            kWarning(35001) << "Couldn't locate wallpaper " << wallFile << endl;
            wallPixmap.resize(0,0);
            wallFile = "";
            return false;
	} else {
            wallPixmap.load( file );

            if( wallPixmap.isNull() )
                return false;
// 		kWarning(35001) << "Could not load wallpaper " << file << endl;
	}
    }
    wallWidget->setPaletteBackgroundPixmap( wallPixmap );
    return true;
}

}  //KChart namespace
