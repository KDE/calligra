/* This file is part of the KDE project
   Copyright (C) 2002, Laurent Montel <lmontel@mandrakesoft.com>

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

#include <kapplication.h>
#include <klocale.h>
#include <kconfig.h>
#include <kdialogbase.h>
#include <kiconloader.h>
#include <knuminput.h>
#include <kcolorbutton.h>

#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qcheckbox.h>

#include <karbon_dlg_config.h>
#include <karbon_view.h>
#include <karbon_part.h>
#include <karbon_factory.h>

#include <float.h>
#include <kspell.h>
#include <knumvalidator.h>
#include <qlineedit.h>
#include <qvgroupbox.h>
#include <klineedit.h>

KarbonConfig::KarbonConfig( KarbonView* parent )
  : KDialogBase(KDialogBase::IconList,i18n("Configure Karbon") ,
		KDialogBase::Ok | KDialogBase::Apply | KDialogBase::Cancel| KDialogBase::Default,
		KDialogBase::Ok)

{
    QVBox *page = addVBoxPage( i18n("Interface"), i18n("Interface"),
                        BarIcon("misc", KIcon::SizeMedium) );
    _interfacePage=new configureInterfacePage( parent, page );

    page = addVBoxPage( i18n("Misc"), i18n("Misc"),
                        BarIcon("misc", KIcon::SizeMedium) );
    _miscPage=new ConfigureMiscPage(parent, page);

    page = addVBoxPage( i18n("Document"), i18n("Document Settings"),
                        BarIcon("document", KIcon::SizeMedium) );

    _defaultDocPage=new ConfigureDefaultDocPage(parent, page);

    connect( this, SIGNAL( okClicked() ),this, SLOT( slotApply() ) );
}

void KarbonConfig::slotApply()
{
    _interfacePage->apply();
    _miscPage->apply();
    _defaultDocPage->apply();
}

void KarbonConfig::slotDefault()
{
    switch( activePageIndex() ) {
        case 0:
            _interfacePage->slotDefault();
            break;
        case 1:
           _miscPage->slotDefault();
            break;
        case 2:
            _defaultDocPage->slotDefault();
            break;
        default:
            break;
    }
}

configureInterfacePage::configureInterfacePage( KarbonView *_view, QWidget *parent , char *name )
 :QWidget ( parent,name )
{
    m_pView=_view;
    config = KarbonFactory::instance()->config();

    QVBoxLayout *box = new QVBoxLayout( this );
    box->setMargin( 5 );
    box->setSpacing( 10 );

    oldNbRecentFiles=10;
    bool oldShowStatusBar = true;

    QGroupBox* tmpQGroupBox = new QGroupBox( this, "GroupBox" );
    tmpQGroupBox->setTitle( i18n("Interface") );

    QVBoxLayout *lay1 = new QVBoxLayout( tmpQGroupBox );
    lay1->setMargin( 20 );
    lay1->setSpacing( 10 );

    if( config->hasGroup("Interface") ) {
        config->setGroup( "Interface" );
        oldNbRecentFiles=config->readNumEntry("NbRecentFile",oldNbRecentFiles);
        oldShowStatusBar = config->readBoolEntry( "ShowStatusBar" , true );
    }

    showStatusBar = new QCheckBox(i18n("Show status bar"),tmpQGroupBox);
    showStatusBar->setChecked(oldShowStatusBar);
    lay1->addWidget(showStatusBar);

    recentFiles=new KIntNumInput( oldNbRecentFiles, tmpQGroupBox );
    recentFiles->setRange(1, 20, 1);
    recentFiles->setLabel(i18n("Number of recent file:"));

    lay1->addWidget(recentFiles);

    box->addWidget( tmpQGroupBox );
}

void configureInterfacePage::apply()
{
    bool statusBar=showStatusBar->isChecked();

    KarbonPart * part = m_pView->part();

    config->setGroup( "Interface" );

    int nbRecent=recentFiles->value();
    if(nbRecent!=oldNbRecentFiles)
    {
        config->writeEntry( "NbRecentFile", nbRecent);
        m_pView->changeNbOfRecentFiles(nbRecent);
        oldNbRecentFiles=nbRecent;
    }
    bool refreshGUI=false;

    if( statusBar != part->showStatusBar() )
    {
        config->writeEntry( "ShowStatusBar", statusBar );
        part->setShowStatusBar( statusBar );
        refreshGUI=true;
    }

    if( refreshGUI )
        part->reorganizeGUI();
}

void configureInterfacePage::slotDefault()
{
    recentFiles->setValue(10);
    showStatusBar->setChecked(true);
}

ConfigureMiscPage::ConfigureMiscPage( KarbonView *_view, QVBox *box, char *name )
    : QObject( box->parent(), name )
{
    m_pView=_view;
    config = KarbonFactory::instance()->config();
    QGroupBox* tmpQGroupBox = new QGroupBox( box, "GroupBox" );
    tmpQGroupBox->setTitle(i18n("Misc"));

    QGridLayout *grid = new QGridLayout( tmpQGroupBox , 8, 1, KDialog::marginHint()+7, KDialog::spacingHint() );

    m_oldNbRedo=30;
    if( config->hasGroup("Misc") )
    {
        config->setGroup( "Misc" );
        m_oldNbRedo=config->readNumEntry("UndoRedo",m_oldNbRedo);
    }

    m_undoRedoLimit=new KIntNumInput( m_oldNbRedo, tmpQGroupBox );
    m_undoRedoLimit->setLabel(i18n("Undo/redo limit:"));
    m_undoRedoLimit->setRange(10, 60, 1);
    grid->addWidget(m_undoRedoLimit,0,0);

}

void ConfigureMiscPage::apply()
{
    config->setGroup( "Misc" );
    int newUndo=m_undoRedoLimit->value();
    KarbonPart * part = m_pView->part();

    if(newUndo!=m_oldNbRedo)
    {
        config->writeEntry("UndoRedo",newUndo);
        part->setUndoRedoLimit(newUndo);
        m_oldNbRedo=newUndo;
    }
}

void ConfigureMiscPage::slotDefault()
{
   m_undoRedoLimit->setValue(30);
}


ConfigureDefaultDocPage::ConfigureDefaultDocPage(KarbonView *_view, QVBox *box, char *name )
    : QObject( box->parent(), name )
{
    m_pView=_view;
    config = KarbonFactory::instance()->config();

    QVGroupBox* gbDocumentSettings = new QVGroupBox( i18n("Document Settings"), box );
    gbDocumentSettings->setMargin( 10 );
    gbDocumentSettings->setInsideSpacing( KDialog::spacingHint() );

    oldAutoSaveValue =  m_pView->part()->defaultAutoSave()/60;

    if( config->hasGroup("Interface") ) {
        config->setGroup( "Interface" );
        oldAutoSaveValue = config->readNumEntry( "AutoSave", oldAutoSaveValue );
    }

    autoSave = new KIntNumInput( oldAutoSaveValue, gbDocumentSettings );
    autoSave->setRange( 0, 60, 1 );
    autoSave->setLabel( i18n("Auto save (min):") );
    autoSave->setSpecialValueText( i18n("No auto save") );
    autoSave->setSuffix( i18n("min") );

}

void ConfigureDefaultDocPage::apply()
{
    config->setGroup( "Document defaults" );
    KarbonPart * part = m_pView->part();

    config->setGroup( "Interface" );
    int autoSaveVal = autoSave->value();
    if( autoSaveVal != oldAutoSaveValue ) {
        config->writeEntry( "AutoSave", autoSaveVal );
        m_pView->part()->setAutoSave( autoSaveVal*60 );
        oldAutoSaveValue=autoSaveVal;
    }
}

void ConfigureDefaultDocPage::slotDefault()
{
    autoSave->setValue( m_pView->part()->defaultAutoSave()/60 );
}


#include <karbon_dlg_config.moc>
