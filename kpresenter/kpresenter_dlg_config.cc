/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include <kapp.h>
#include <kconfig.h>
#include <kdialogbase.h>
#include <kiconloader.h>
#include <knuminput.h>
#include <kcolorbutton.h>

#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qvbox.h>

#include <kpresenter_dlg_config.h>
#include <kpresenter_view.h>
#include <kpresenter_doc.h>

KPConfig::KPConfig( KPresenterView* parent )
  : KDialogBase(KDialogBase::IconList,i18n("Configure KPresenter") ,
		KDialogBase::Ok | KDialogBase::Cancel| KDialogBase::Default,
		KDialogBase::Ok)

{
    QVBox *page = addVBoxPage( i18n("Interface"), i18n("Interface"),
                        BarIcon("misc", KIcon::SizeMedium) );
    _interfacePage=new configureInterfacePage( parent, page );
    page = addVBoxPage( i18n("Color"), i18n("Color"),
                        BarIcon("colorize", KIcon::SizeMedium) );
    _colorBackground = new configureColorBackground( parent, page );
    connect( this, SIGNAL( okClicked() ),this, SLOT( slotApply() ) );
}

void KPConfig::slotApply()
{
    _interfacePage->apply();
    _colorBackground->apply();
}

void KPConfig::slotDefault()
{
    switch( activePageIndex() ) {
        case 0:
            _interfacePage->slotDefault();
            break;
        case 1:
            _colorBackground->slotDefault();
        default:
            break;
    }
}

configureInterfacePage::configureInterfacePage( KPresenterView *_view, QWidget *parent , char *name )
 :QWidget ( parent,name )
{
    m_pView=_view;
    config = KPresenterFactory::global()->config();
    QVBoxLayout *box = new QVBoxLayout( this );
    box->setMargin( 5 );
    box->setSpacing( 10 );
    QGroupBox* tmpQGroupBox = new QGroupBox( this, "GroupBox" );
    tmpQGroupBox->setTitle( i18n("Interface") );

    QVBoxLayout *lay1 = new QVBoxLayout( tmpQGroupBox );
    lay1->setMargin( 20 );
    lay1->setSpacing( 10 );

    int oldRastX = 10;
    int oldRastY = 10;
    oldAutoSaveValue =  m_pView->kPresenterDoc()->defaultAutoSave()/60;

    if( config->hasGroup("Interface") ) {
        config->setGroup( "Interface" );
        oldRastX = config->readNumEntry( "RastX", 10 );
        oldRastY = config->readNumEntry( "RastY", 10 );
        oldAutoSaveValue = config->readNumEntry( "AutoSave", oldAutoSaveValue );
    }

    autoSave = new KIntNumInput( oldAutoSaveValue, tmpQGroupBox );
    autoSave->setRange( 0, 60, 1 );
    autoSave->setLabel( i18n("Auto save (min):") );
    autoSave->setSpecialValueText( i18n("No auto save") );
    autoSave->setSuffix( i18n("min") );
    lay1->addWidget( autoSave );

    eRastX = new KIntNumInput( oldRastX, tmpQGroupBox );
    eRastX->setRange( 1, 400, 1 );
    eRastX->setLabel( i18n("Horizontal Raster: ") );
    lay1->addWidget( eRastX );

    eRastY = new KIntNumInput( oldRastY, tmpQGroupBox );
    eRastY->setRange( 1, 400, 1 );
    eRastY->setLabel( i18n("Vertical Raster: ") );
    lay1->addWidget( eRastY );

    box->addWidget( tmpQGroupBox );
}

void configureInterfacePage::apply()
{
    unsigned int rastX = eRastX->value();
    unsigned int rastY = eRastY->value();

    KPresenterDoc * doc = m_pView->kPresenterDoc();

    config->setGroup( "Interface" );
    if( rastX != oldRastX || rastY != oldRastX ) {
        config->writeEntry( "RastX", rastX );
        config->writeEntry( "RastY", rastY );
        doc->setRasters( rastX, rastY, true );
        doc->repaint( false );
    }

    int autoSaveVal = autoSave->value();
    if( autoSaveVal != oldAutoSaveValue ) {
        config->writeEntry( "AutoSave", autoSaveVal );
        m_pView->kPresenterDoc()->setAutoSave( autoSaveVal*60 );
    }
}

void configureInterfacePage::slotDefault()
{
    eRastX->setValue( 10 );
    eRastY->setValue( 10 );
    autoSave->setValue( m_pView->kPresenterDoc()->defaultAutoSave()/60 );
}

configureColorBackground::configureColorBackground( KPresenterView* _view, QWidget *parent , char *name )
 :QWidget ( parent,name )
{
    m_pView = _view;
    config = KPresenterFactory::global()->config();

    oldBgColor = Qt::white;
    if(  config->hasGroup( "KPresenter Color" ) ) {
        config->setGroup( "KPresenter Color" );
        oldBgColor = config->readColorEntry( "BackgroundColor", &oldBgColor );
    }

    QVBoxLayout *box = new QVBoxLayout( this );
    box->setMargin( 5 );
    box->setSpacing( 10 );

    QGroupBox* tmpQGroupBox = new QGroupBox( this, "GroupBox" );
    tmpQGroupBox->setTitle( i18n("Objects in editing mode") );
    QGridLayout *grid1 = new QGridLayout( tmpQGroupBox, 5, 1, 15, 7);
    QLabel *lab = new QLabel( tmpQGroupBox, "label20" );
    lab->setText( i18n( "Background color:" ) );
    grid1->addWidget( lab, 0, 0 );

    bgColor = new KColorButton( tmpQGroupBox );
    bgColor->setColor( oldBgColor );
    grid1->addWidget( bgColor, 1, 0 );

    box->addWidget( tmpQGroupBox );
}

void configureColorBackground::apply()
{
    QColor _col = bgColor->color();
    KPresenterDoc * doc = m_pView->kPresenterDoc();
    if( oldBgColor != _col ) {
        doc->setTxtBackCol( _col );
        config->setGroup( "KPresenter Color" );
        config->writeEntry( "BackgroundColor", _col );
        doc->replaceObjs();
        doc->repaint( false );
    }
}

void configureColorBackground::slotDefault()
{
    bgColor->setColor( Qt::white );
}

#include <kpresenter_dlg_config.moc>
