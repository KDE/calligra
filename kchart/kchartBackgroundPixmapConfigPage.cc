/*
 * $Id$
 *
 * Copyright 2000 by Matthias Kalle Dalheimer, released under Artistic License.
 */

#include "kchartBackgroundPixmapConfigPage.h"
#include "kchartBackgroundPixmapConfigPage.moc"

#include "kchartparams.h"

#include <klocale.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kdebug.h>

#include <qcombobox.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qhbox.h>
#include <qvgroupbox.h>
#include <qpushbutton.h>
#include <qlabel.h>

KChartBackgroundPixmapConfigPage::KChartBackgroundPixmapConfigPage( KChartParameters* params, QWidget * parent )
  : QWidget( parent, "KChartBackgroundPixmapConfigPage" ),
	_params( params )
{
  QHBoxLayout* toplevel = new QHBoxLayout( this, 10 );
  QVBoxLayout* left = new QVBoxLayout( 10 );
  toplevel->addLayout( left, 2 );
  
  wallCB = new QComboBox( false, this, "wallCombo" );
  left->addWidget( wallCB );
  wallCB->insertItem( i18n("None") );

  QStringList list = KGlobal::dirs()->findAllResources( "wallpaper" );

  for( QStringList::ConstIterator it = list.begin(); it != list.end(); it++ )
	wallCB->insertItem( ( (*it).at(0)=='/' ) ?        // if absolute path
						KURL( *it ).fileName() :    // then only fileName
						*it );

  QPushButton* browsePB = new QPushButton( i18n("&Browse..."), this );
  left->addWidget( browsePB );
  connect( browsePB, SIGNAL( clicked() ), SLOT( slotBrowse() ) );

  wallWidget = new QWidget( this );
  left->addWidget( wallWidget, 2 );

  connect( wallCB, SIGNAL( activated( int ) ), 
		   this, SLOT( slotWallPaperChanged( int ) ) );

  QVGroupBox* right = new QVGroupBox( this );
  toplevel->addWidget( right );
  
  QHBox* intensityHB = new QHBox( right );
  new QLabel( i18n( "Intensity in %d" ), intensityHB );
  intensitySB = new QSpinBox( 1, 100, 1, intensityHB );
  
  scaledCB = new QCheckBox( right );
  centeredCB = new QCheckBox( scaledCB );
  connect( scaledCB, SIGNAL( toggled( bool ) ),
		   this, SLOT( setScaledToggled( bool ) ) );
}



void KChartBackgroundPixmapConfigPage::init()
{
  showSettings( _params->backgroundPixmapName );
  intensitySB->setValue( _params->backgroundPixmapIntensity * 100 );
  scaledCB->setChecked( _params->backgroundPixmapScaled );
  centeredCB->setChecked( _params->backgroundPixmapCentered );
  if( scaledCB )
	centeredCB->setEnabled( false );
}


void KChartBackgroundPixmapConfigPage::apply()
{
  if( wallCB->currentText() != _params->backgroundPixmapName ) {
	_params->backgroundPixmapName = wallCB->currentText();
	_params->backgroundPixmap.load( _params->backgroundPixmapName );
	_params->backgroundPixmapIsDirty = true;
  }
  _params->backgroundPixmapIntensity = (float)(intensitySB->value()) / 100.0;
  _params->backgroundPixmapScaled = scaledCB->isChecked();
  _params->backgroundPixmapCentered = centeredCB->isChecked();
}


void KChartBackgroundPixmapConfigPage::setScaledToggled( bool b )
{
  centeredCB->setEnabled( !b );
}


void KChartBackgroundPixmapConfigPage::showSettings( const QString& fileName )
{
  for( int i = 1; i < wallCB->count(); i++ ) {
	if( fileName == wallCB->text( i ) ) {
	  wallCB->setCurrentItem( i );
	  loadWallPaper();
	  return;
	}
  }

  if( !fileName.isEmpty() ) {
	wallCB->insertItem( fileName );
	wallCB->setCurrentItem( wallCB->count()-1 );
  } else 
	wallCB->setCurrentItem( 0 );

  loadWallPaper();
}


void KChartBackgroundPixmapConfigPage::slotBrowse()
{
    KURL url = KFileDialog::getOpenURL( 0 );
    if( url.isEmpty() )
      return;
    if( !url.isLocalFile() ) {
      KMessageBox::sorry(this, i18n("Currently are only local wallpapers allowed."));
    } else
      showSettings( url.path() );
}

void KChartBackgroundPixmapConfigPage::slotWallPaperChanged( int )
{
    loadWallPaper();
}


void KChartBackgroundPixmapConfigPage::loadWallPaper()
{
  int i = wallCB->currentItem();
  if ( i == -1 || i == 0 ) {  // 0 is 'None'
	wallPixmap.resize(0,0);
	wallFile = "";
  } else {
	wallFile = wallCB->text( i );
	QString file = locate("wallpaper", wallFile);
	if( file.isEmpty() ) {
	  kdWarning(35001) << "Couldn't locate wallpaper " << wallFile << endl;
	  wallPixmap.resize(0,0);
	  wallFile = "";
	} else {
	  wallPixmap.load( file );

	  if( wallPixmap.isNull() )
		kdWarning(35001) << "Could not load wallpaper " << file << endl;
	}
  }
  wallWidget->setBackgroundPixmap( wallPixmap );
}

