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
#include <kfilemetainfo.h>
#include <kstringhandler.h>

#include <qcombobox.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qhbox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qwhatsthis.h>
#include <qlistbox.h>
#include <kcolorbutton.h>

#include "kchart_params.h"

namespace KChart
{

KChartBackgroundPixmapConfigPage::KChartBackgroundPixmapConfigPage( KChartParams* params, QWidget* parent )
    : QWidget( parent, "KChartBackgroundPixmapConfigPage" ),
      _params( params )
{
    QWhatsThis::add( this,
                     i18n( "On this page, you can select colors or images "
                           "to be displayed behind the different areas. You "
                           "can also select whether the images should be "
                           "stretched or scaled or centered or used as "
                           "background tiles." ) );

    QHBoxLayout* toplevel = new QHBoxLayout( this, 10 );

#if 0
    QVBoxLayout* left=new QVBoxLayout(10);
    toplevel->addLayout(left,2);
    regionList=new QListBox(this);
    left->addWidget(regionList);
#endif

    QVBoxLayout* center = new QVBoxLayout( 10 );
    toplevel->addLayout( center, 2 );


    QLabel* backgroundLA = new QLabel( i18n( "&Background color:" ), this );
    center->addWidget( backgroundLA );
    _backgroundCB = new KColorButton( this );
    backgroundLA->setBuddy( _backgroundCB );
    center->addWidget( _backgroundCB);
    QString wtstr = i18n( "Here you set the color in which the background "
                          "of the chart is painted." );
    QWhatsThis::add( backgroundLA, wtstr );
    QWhatsThis::add( _backgroundCB, wtstr );


    QLabel* wallpaperLA = new QLabel( i18n( "Background wallpaper:" ), this );
    center->addWidget( wallpaperLA );

    wallCB = new QComboBox( false, this, "wallCombo" );
    wallpaperLA->setBuddy(wallCB);
    QWhatsThis::add( wallCB, i18n( "You can select a background image from "
                                   "this list. Initially, the installed KDE "
                                   "wallpapers will be offered. If you do not "
                                   "find what you are looking for here, you can "
                                   "select any image file by clicking on the "
                                   "<i>Browse</i> button below." ) );
    center->addWidget( wallCB );
    wallCB->insertItem( i18n("None") );

    loadWallpaperFilesList();
    QPushButton* browsePB = new QPushButton( i18n("&Browse..."), this );
    QWhatsThis::add( browsePB, i18n( "Click this button to select a background "
                                     "image not yet present in the list above. " ) );
    center->addWidget( browsePB );
    connect( browsePB, SIGNAL( clicked() ), SLOT( slotBrowse() ) );

    wallWidget = new QWidget( this );
    QWhatsThis::add( wallWidget, i18n( "This area will always display the "
                                       "currently selected background image. "
                                       "Note that the image will be scaled and "
                                       "thus might have a different ratio than "
                                       "it originally had." ) );
    center->addWidget( wallWidget );

    connect( wallCB, SIGNAL( activated( int ) ),
             this, SLOT( slotWallPaperChanged( int ) ) );

    right = new QVGroupBox( i18n( "Wallpaper Configuration" ), this );
    QWhatsThis::add( right, i18n( "In this box, you can set various settings "
                                  "that control how the background image is "
                                  "displayed." ) );
    toplevel->addWidget( right );

    QHBox* intensityHB = new QHBox( right );
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
    QWhatsThis::add( intensityLA, ttstr );
    QWhatsThis::add( intensitySB, ttstr );


    stretchedRB = new QRadioButton( i18n( "Stretched" ), right );
    QWhatsThis::add( stretchedRB,
                     i18n( "If you check this box, the selected image will "
                           "be scaled to fit the total size of the selected "
                           "area. Image ratio will be adjusted to match "
                           "the area size and height if necessary." ) );
    stretchedRB->setChecked( true );
    scaledRB = new QRadioButton( i18n( "Scaled" ), right );
    QWhatsThis::add( scaledRB,
                     i18n( "If you check this box, the selected image will "
                           "be scaled to match the height or width of the "
                           "selected area - whichever is reached first." ) );
    centeredRB = new QRadioButton( i18n( "Centered" ), right );
    QWhatsThis::add( centeredRB,
                     i18n( "If you check this box, the selected image will "
                           "be centered over the selected area. If the image "
                           "is larger then the area, you will only see the "
                           "middle part of it." ) );

    tiledRB = new QRadioButton( i18n( "Tiled" ), right );
    QWhatsThis::add( tiledRB,
                     i18n( "If you check this box, the selected image will "
                           "be used as a background tile. If the image is "
                           "larger then the selected area, you will only see "
                           "the upper left part of it." ) );
    QButtonGroup* alignmentBG;
    alignmentBG = new QButtonGroup( right, "GroupBox_Alignment" );
    alignmentBG->setFrameStyle( QFrame::NoFrame );
    alignmentBG->insert( stretchedRB );
    alignmentBG->insert( scaledRB );
    alignmentBG->insert( centeredRB );
    alignmentBG->insert( tiledRB );

    intensitySB->hide(); //the property doesn't work atm
}

//Code from kcontrol/background/bgdialog.cc
void KChartBackgroundPixmapConfigPage::loadWallpaperFilesList()
{
   // Wallpapers
   // the following QMap is lower cased names mapped to cased names and URLs
   // this way we get case insensitive sorting
   QMap<QString, QPair<QString, QString> > papers;

   //search for .desktop files before searching for images without .desktop files
   QStringList lst =  KGlobal::dirs()->findAllResources("wallpaper", "*desktop", false, true);
   QStringList files;
   for (QStringList::ConstIterator it = lst.begin(); it != lst.end(); ++it)
   {
      KSimpleConfig fileConfig(*it);
      fileConfig.setGroup("Wallpaper");

      QString imageCaption = fileConfig.readEntry("Name");
      QString fileName = fileConfig.readEntry("File");

      if (imageCaption.isEmpty())
      {
         imageCaption = fileName;
         imageCaption.replace('_', ' ');
         imageCaption = KStringHandler::capwords(imageCaption);
      }

      // avoid name collisions
      QString rs = imageCaption;
      QString lrs = rs.lower();
      for (int n = 1; papers.find(lrs) != papers.end(); ++n)
      {
         rs = imageCaption + " (" + QString::number(n) + ')';
         lrs = rs.lower();
      }
      int slash = (*it).findRev('/') + 1;
      QString directory = (*it).left(slash);
      bool canLoadScaleable = false;
#ifdef HAVE_LIBART
      canLoadScaleable = true;
#endif
      if ( fileConfig.readEntry("ImageType") == "pixmap" || canLoadScaleable ) {
	      papers[lrs] = qMakePair(rs, directory + fileName);
	      files.append(directory + fileName);
      }
   }

   //now find any wallpapers that don't have a .desktop file
   lst =  KGlobal::dirs()->findAllResources("wallpaper", "*", false, true);
   for (QStringList::ConstIterator it = lst.begin(); it != lst.end(); ++it)
   {
      if ( !(*it).endsWith(".desktop") && files.grep(*it).empty() ) {
         // First try to see if we have a comment describing the image.  If we do
         // just use the first line of said comment.
         KFileMetaInfo metaInfo(*it);
         QString imageCaption;

         if (metaInfo.isValid() && metaInfo.item("Comment").isValid())
            imageCaption = metaInfo.item("Comment").string().section('\n', 0, 0);

         if (imageCaption.isEmpty())
         {
            int slash = (*it).findRev('/') + 1;
            int endDot = (*it).findRev('.');

            // strip the extension if it exists
            if (endDot != -1 && endDot > slash)
               imageCaption = (*it).mid(slash, endDot - slash);
            else
               imageCaption = (*it).mid(slash);

            imageCaption.replace('_', ' ');
            imageCaption = KStringHandler::capwords(imageCaption);
         }

         // avoid name collisions
         QString rs = imageCaption;
         QString lrs = rs.lower();
         for (int n = 1; papers.find(lrs) != papers.end(); ++n)
         {
            rs = imageCaption + " (" + QString::number(n) + ')';
            lrs = rs.lower();
         }
         papers[lrs] = qMakePair(rs, *it);
      }
   }

   m_wallpaper.clear();
   //don't start to 0, we have None element
   int i = 1;
   for (QMap<QString, QPair<QString, QString> >::Iterator it = papers.begin();
        it != papers.end();
        ++it)
   {
      wallCB->insertItem(it.data().first);
      m_wallpaper[it.data().second] = i;
      i++;
   }
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
        bool isTiledMode = false;
        KDFrame::BackPixmapMode backPixmapMode;
        const QBrush& background = innerFrame->frame().background( backPixmap, backPixmapMode );
        const QColor backColor( background.color() );

        // Bg color has to be be set in any case,
        // even when a (tiled) pixmap is specified:
        _backgroundCB->setColor( backColor );


        if( !backPixmap || backPixmap->isNull() )
        {
            // A pixmap can be in the brush, if used as Tile.
            backPixmap = background.pixmap();
            if( backPixmap )
                isTiledMode = true;
        }

        if( !backPixmap || backPixmap->isNull() ) //color as background
        {
            right->setEnabled( false );
            wallCB->setCurrentItem( 0 );
        }
        else //pixmap as background
        {
            _backgroundCB->setEnabled( false );
            wallWidget->setPaletteBackgroundPixmap( *backPixmap );
            wallCB->setCurrentItem( 1 );
            if( isTiledMode )
            {
                tiledRB->setChecked( true );
            }
            else
            {
                switch( backPixmapMode ){
                    case KDFrame::PixCentered:
                        centeredRB->setChecked( true );
                        break;
                    case KDFrame::PixScaled:
                        scaledRB->setChecked( true );
                        break;
                    default:
                        stretchedRB->setChecked( true );
                }
            }
        }
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

    //
    // temp. hack: the background is removed if "None" is selected in the combo box
    //
    //             For KOffice 1.5/2.0 this is to be removed by a checkbox.
    bool bFound;
    const KDChartParams::KDChartFrameSettings * innerFrame =
        _params->frameSettings( KDChartEnums::AreaInnermost, bFound );
    if( bFound )
    {
        const QColor backColor( _backgroundCB->color() );
        KDFrame& frame( const_cast<KDFrame&>(innerFrame->frame()) );
        if ( wallCB->currentItem() == 0 )
        {
            frame.setBackPixmap( 0 );
            frame.setBackground( backColor );
        }
        else
        {
            const QPixmap* pixmap = wallWidget->paletteBackgroundPixmap();
            if ( tiledRB->isChecked() ){
                // We remove the frame's extra pixmap,
                frame.setBackPixmap( 0 );
                // because a tiled image is set via a QBrush.
                if( pixmap )
                    frame.setBackground( QBrush( backColor, *pixmap ) );
                else
                    frame.setBackground( backColor );
            }else{
                KDFrame::BackPixmapMode backPixmapMode;
                if ( centeredRB->isChecked() )
                    backPixmapMode = KDFrame::PixCentered;
                else if ( scaledRB->isChecked() )
                    backPixmapMode = KDFrame::PixScaled;
                else
                    backPixmapMode = KDFrame::PixStretched;
                // We reset the background color, removing any tiled brush,
                frame.setBackground( backColor );
                // because we specify an extra pixmap instead.
                frame.setBackPixmap( pixmap, backPixmapMode );
            }
        }
    }
}


void KChartBackgroundPixmapConfigPage::showSettings( const QString& fileName )
{
   wallCB->blockSignals(true);

   if (m_wallpaper.find(fileName) == m_wallpaper.end())
   {
      int i = wallCB->count();
      QString imageCaption;
      int slash = fileName.findRev('/') + 1;
      int endDot = fileName.findRev('.');

      // strip the extension if it exists
      if (endDot != -1 && endDot > slash)
         imageCaption = fileName.mid(slash, endDot - slash);
      else
         imageCaption = fileName.mid(slash);
      if (wallCB->text(i-1) == imageCaption)
      {
         i--;
         wallCB->removeItem(i);
      }
      wallCB->insertItem(imageCaption);
      m_wallpaper[fileName] = i;
      wallCB->setCurrentItem(i);
   }
   else
   {
      wallCB->setCurrentItem(m_wallpaper[fileName]);
   }
   wallCB->blockSignals(false);
    loadWallPaper();
}


void KChartBackgroundPixmapConfigPage::slotBrowse()
{
    //TODO: Support non-local wallpapers
    KURL url = KFileDialog::getOpenFileName( 0 );
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
        for(QMap<QString,int>::ConstIterator it = m_wallpaper.begin();
            it != m_wallpaper.end();
            ++it)
        {
            if (it.data() == i)
            {
                wallFile = it.key();
                break;
            }
        }
        QString file = locate("wallpaper", wallFile);
	if( file.isEmpty() ) {
            kdWarning(35001) << "Couldn't locate wallpaper " << wallFile << endl;
            wallPixmap.resize(0,0);
            wallFile = "";
            return false;
	} else {
            wallPixmap.load( file );

            if( wallPixmap.isNull() )
                return false;
// 		kdWarning(35001) << "Could not load wallpaper " << file << endl;
	}
    }
    wallWidget->setPaletteBackgroundPixmap( wallPixmap );
    return true;
}

}  //KChart namespace
