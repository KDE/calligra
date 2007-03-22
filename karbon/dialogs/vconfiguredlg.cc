/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Laurent Montel <lmontel@mandrakesoft.com>
   Copyright (C) 2006 Jan Hambrecht <jaham@gmx.net>

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

#include <float.h>

#include <QCheckBox>
#include <QLabel>
#include <QLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QGridLayout>

#include <kiconloader.h>
#include <kconfig.h>
#include <klocale.h>
#include <knuminput.h>
#include <kcolorbutton.h>
#include <KoUnitWidgets.h>
#include <kcomponentdata.h>
#include <kicon.h>
#include <KoGridData.h>

#include "karbon_view.h"
#include "karbon_part.h"
#include "karbon_factory.h"

#include "vconfiguredlg.h"


VConfigureDlg::VConfigureDlg( KarbonView* parent )
	: KPageDialog( parent )
{
	setFaceType( List );
	setCaption( i18n( "Configure" ) );
	setButtons( KDialog::Ok | KDialog::Apply | KDialog::Cancel | KDialog::Default );
	setDefaultButton( KDialog::Ok );

	m_interfacePage = new VConfigInterfacePage( parent );
	KPageWidgetItem* item = addPage( m_interfacePage, i18n( "Interface" ) );
	item->setHeader( i18n( "Interface" ) );
	item->setIcon(KIcon(BarIcon("misc", K3Icon::SizeMedium)));

	m_miscPage = new VConfigMiscPage( parent );
	item = addPage( m_miscPage, i18n( "Misc" ) );
	item->setHeader( i18n( "Misc" ) );
	item->setIcon(KIcon(BarIcon("misc", K3Icon::SizeMedium)));

	m_gridPage = new VConfigGridPage( parent );
	item = addPage( m_gridPage, i18n( "Grid" ) );
	item->setHeader( i18n( "Grid" ) );
	item->setIcon(KIcon(BarIcon("grid", K3Icon::SizeMedium)));

	connect( m_miscPage, SIGNAL( unitChanged( int ) ), m_gridPage, SLOT( slotUnitChanged( int ) ) );

	m_defaultDocPage = new VConfigDefaultPage( parent );
	item = addPage( m_defaultDocPage, i18n( "Document" ) );
	item->setHeader( i18n( "Document Settings" ) );
	item->setIcon(KIcon(BarIcon("document", K3Icon::SizeMedium)));

	connect( this, SIGNAL( okClicked() ), this, SLOT( slotApply() ) );
	connect( this, SIGNAL( defaultClicked() ), this, SLOT( slotDefault() ) );
}

void VConfigureDlg::slotApply()
{
	m_interfacePage->apply();
	m_miscPage->apply();
	m_defaultDocPage->apply();
	m_gridPage->apply();
}

void VConfigureDlg::slotDefault()
{
	QWidget* curr = currentPage()->widget();

	if( curr == m_interfacePage )
		m_interfacePage->slotDefault();
	else if( curr == m_miscPage )
		m_miscPage->slotDefault();
	else if( curr == m_gridPage )
		m_gridPage->slotDefault();
	else if( curr == m_defaultDocPage )
		m_defaultDocPage->slotDefault();
}


VConfigInterfacePage::VConfigInterfacePage( KarbonView* view, char* name )
{
	setObjectName(name);

	m_view = view;
	m_config = KarbonFactory::componentData().config();

	m_oldRecentFiles = 10;
	m_oldCopyOffset = 10;
	m_oldDockerFontSize = 8;
	bool oldShowStatusBar = true;

	QGroupBox* tmpQGroupBox = new QGroupBox( i18n( "Interface" ), this );

    KConfigGroup emptyGroup = m_config->group( "" );
    m_oldDockerFontSize = emptyGroup.readEntry( "palettefontsize", m_oldDockerFontSize );

    if( m_config->hasGroup( "Interface" ) )
    {
        KConfigGroup interfaceGroup = m_config->group( "Interface" );

        m_oldRecentFiles = interfaceGroup.readEntry("NbRecentFile", m_oldRecentFiles);
        oldShowStatusBar = interfaceGroup.readEntry("ShowStatusBar", true);
        m_oldCopyOffset = interfaceGroup.readEntry("CopyOffset", m_oldCopyOffset);
    }

	QVBoxLayout *grpLayout = new QVBoxLayout( tmpQGroupBox );

	m_showStatusBar = new QCheckBox( i18n( "Show status bar" ), tmpQGroupBox );
	m_showStatusBar->setChecked( oldShowStatusBar );
	grpLayout->addWidget( m_showStatusBar );

	m_recentFiles = new KIntNumInput( m_oldRecentFiles, tmpQGroupBox );
	m_recentFiles->setRange( 1, 20, 1 );
	m_recentFiles->setLabel( i18n( "Number of recent files:" ) );
	grpLayout->addWidget( m_recentFiles );

	m_copyOffset = new KIntNumInput( m_oldCopyOffset, tmpQGroupBox );
	m_copyOffset->setRange( 1, 50, 1 );
	m_copyOffset->setLabel( i18n( "Copy offset:" ) );
	grpLayout->addWidget( m_copyOffset );

	m_dockerFontSize = new KIntNumInput( m_oldDockerFontSize, tmpQGroupBox );
	m_dockerFontSize->setRange( 5, 20, 1 );
	m_dockerFontSize->setLabel( i18n( "Palette font size:" ) );
	grpLayout->addWidget( m_dockerFontSize );
	grpLayout->addStretch();
}

void VConfigInterfacePage::apply()
{
	bool showStatusBar = m_showStatusBar->isChecked();

	KarbonPart* part = m_view->part();

    KConfigGroup interfaceGroup = m_config->group( "Interface" );

	int recent = m_recentFiles->value();

	if( recent != m_oldRecentFiles )
	{
        interfaceGroup.writeEntry( "NbRecentFile", recent );
		m_view->setNumberOfRecentFiles( recent );
		m_oldRecentFiles = recent;
	}

	int copyOffset = m_copyOffset->value();

	if( copyOffset != m_oldCopyOffset )
	{
        interfaceGroup.writeEntry( "CopyOffset", copyOffset );
		m_oldCopyOffset = copyOffset;
	}

	bool refreshGUI = false;

	if( showStatusBar != part->showStatusBar() )
	{
        interfaceGroup.writeEntry( "ShowStatusBar", showStatusBar );
		part->setShowStatusBar( showStatusBar );
		refreshGUI = true;
	}

	int dockerFontSize = m_dockerFontSize->value();

	if( dockerFontSize != m_oldDockerFontSize )
	{
        m_config->group( "" ).writeEntry( "palettefontsize", dockerFontSize );
		m_oldDockerFontSize = dockerFontSize;
		refreshGUI = true;
	}

	if( refreshGUI )
		part->reorganizeGUI();

}

void VConfigInterfacePage::slotDefault()
{
	m_recentFiles->setValue( 10 );
	m_dockerFontSize->setValue( 8 );
	m_showStatusBar->setChecked( true );
}


VConfigMiscPage::VConfigMiscPage( KarbonView* view, char* name )
{
    setObjectName(name);

    m_view = view;
    m_config = KarbonFactory::componentData().config();

    KoUnit unit = view->part()->unit();

    QGroupBox* tmpQGroupBox = new QGroupBox( i18n( "Misc" ), this );

    QGridLayout* grid = new QGridLayout();
    grid->setSpacing(KDialog::spacingHint());
    grid->setMargin(KDialog::marginHint());

    QString unitType = KoUnit::unitName( unit );
    //#################"laurent
    //don't load unitType from config file because unit is
    //depend from kword file => unit can be different from config file

    grid->addWidget( new QLabel(  i18n(  "Units:" ), tmpQGroupBox ), 0, 0 );

    m_unit = new QComboBox( tmpQGroupBox );
    m_unit->addItems( KoUnit::listOfUnitName() );
    grid->addWidget( m_unit, 0, 1 );
    m_oldUnit = KoUnit::unit( unitType );
    m_unit->setCurrentIndex( m_oldUnit.indexInList() );

    grid->setRowStretch( 2, 1 );

    tmpQGroupBox->setLayout(grid);

    connect( m_unit, SIGNAL( activated( int ) ), SIGNAL( unitChanged( int ) ) );
}

void VConfigMiscPage::apply()
{
    KarbonPart * part = m_view->part();

    KConfigGroup miscGroup = m_config->group( "Misc" );

    int currentUnit = m_unit->currentIndex();
    if( currentUnit >= 0 && m_oldUnit.indexInList() != static_cast<uint>( currentUnit ) )
    {
        m_oldUnit = KoUnit((KoUnit::Unit)currentUnit);
        part->setUnit( m_oldUnit );
        part->document().setUnit(part->unit());
        miscGroup.writeEntry( "Units", KoUnit::unitName( part->unit() ) );
    }
}

void VConfigMiscPage::slotDefault()
{
    m_unit->setCurrentIndex( 0 );
}

VConfigGridPage::VConfigGridPage( KarbonView* view, char* name )
{
	setObjectName(name);

	m_config = KarbonFactory::componentData().config();

	m_view = view;
	KoUnit unit = view->part()->document().unit();
	KoGridData &gd = view->part()->gridData();
	double pgw = view->part()->document().width();
	double pgh = view->part()->document().height();

	QGroupBox* generalGrp = new QGroupBox( i18n("Grid"), this );
	QGridLayout *layoutGeneral = new QGridLayout( generalGrp );
	m_gridChBox = new QCheckBox( i18n( "Show &grid" ), generalGrp );
	m_gridChBox->setChecked( gd.showGrid() );
	//m_snapChBox = new QCheckBox( i18n( "Snap to g&rid" ), generalGrp );
	//m_snapChBox->setChecked( gd.snapping() );
	QLabel* gridColorLbl = new QLabel( i18n( "Grid &color:" ), generalGrp);
	m_gridColorBtn = new KColorButton( gd.gridColor(), generalGrp );
	gridColorLbl->setBuddy( m_gridColorBtn );
	layoutGeneral->addWidget( m_gridChBox, 0, 0 );
	//layoutGeneral->addWidget( m_snapChBox, 1, 0 );
	layoutGeneral->addWidget( gridColorLbl, 2, 0 );
	layoutGeneral->addWidget( m_gridColorBtn, 2, 1 );

	QGroupBox* spacingGrp = new QGroupBox( i18n( "Spacing" ), this );
	QGridLayout* layoutSpacingGrp = new QGridLayout( spacingGrp );
	QLabel* spaceHorizLbl = new QLabel( i18n( "&Horizontal:" ) );
	m_spaceHorizUSpin = new KoUnitDoubleSpinBox( spacingGrp, 0.0, pgw, 0.1, gd.gridX(), unit );
	spaceHorizLbl->setBuddy( m_spaceHorizUSpin );
	QLabel* spaceVertLbl = new QLabel( i18n( "&Vertical:" ) );
	m_spaceVertUSpin = new KoUnitDoubleSpinBox( spacingGrp, 0.0, pgh, 0.1, gd.gridY(), unit );
	spaceVertLbl->setBuddy( m_spaceVertUSpin );
	layoutSpacingGrp->addWidget(spaceHorizLbl, 0, 0);
	layoutSpacingGrp->addWidget(m_spaceHorizUSpin, 0, 1);
	layoutSpacingGrp->addWidget(spaceVertLbl, 1, 0);
	layoutSpacingGrp->addWidget(m_spaceVertUSpin, 1, 1);

    /*
    double sw, sh;
    gd.snap( &sw, &sh );
	QGroupBox* snapGrp = new QGroupBox( i18n( "Snap Distance" ), this );
	QGridLayout* layoutSnapGrp = new QGridLayout( snapGrp );
	QLabel* snapHorizLbl = new QLabel( i18n( "H&orizontal:" ) );
	m_snapHorizUSpin = new KoUnitDoubleSpinBox( snapGrp, 0.0, fw, 0.1, sw, unit );
	snapHorizLbl->setBuddy( m_snapHorizUSpin );
	QLabel* snapVertLbl = new QLabel( i18n( "V&ertical:" ) );
	m_snapVertUSpin = new KoUnitDoubleSpinBox( snapGrp, 0.0, fh, 0.1, sh, unit );
	snapVertLbl->setBuddy( m_snapVertUSpin );
	layoutSnapGrp->addWidget(snapHorizLbl, 0, 0);
	layoutSnapGrp->addWidget(m_snapHorizUSpin, 0, 1);
	layoutSnapGrp->addWidget(snapVertLbl, 1, 0);
	layoutSnapGrp->addWidget(m_snapVertUSpin, 1, 1);
    */

	QGridLayout* gl = new QGridLayout( this );
	gl->setSpacing( KDialog::spacingHint() );
	gl->setMargin(KDialog::marginHint());
	gl->addWidget( generalGrp, 0, 0, 1, 2 );
	gl->addItem( new QSpacerItem( 0, 0 ), 1, 1 );
	gl->addWidget( spacingGrp, 2, 0, 1, 2 );
	//gl->addWidget( snapGrp, 3, 0, 1, 2 );
	gl->addItem( new QSpacerItem( 0, 0 ), 4, 0, 1, 2 );

	setValuesFromGrid( view->part()->gridData() );

	connect( m_spaceHorizUSpin, SIGNAL( valueChangedPt( double ) ), SLOT( setMaxHorizSnap( double ) ) );
	connect( m_spaceVertUSpin, SIGNAL( valueChangedPt( double ) ), SLOT( setMaxVertSnap( double ) ) ) ;
}

void VConfigGridPage::setMaxHorizSnap( double /*v*/ )
{
	//m_snapHorizUSpin->setMaximum( v );
}

void VConfigGridPage::setMaxVertSnap( double /*v*/ )
{
	//m_snapVertUSpin->setMaximum( v );
}

void VConfigGridPage::slotUnitChanged( int u )
{
	KoUnit unit = KoUnit((KoUnit::Unit) u );
	//m_snapHorizUSpin->setUnit( unit );
	//m_snapVertUSpin->setUnit( unit );
	m_spaceHorizUSpin->setUnit( unit );
	m_spaceVertUSpin->setUnit( unit );
}

void VConfigGridPage::apply()
{
	KoGridData &gd = m_view->part()->gridData();
	gd.setGrid( m_spaceHorizUSpin->value(), m_spaceVertUSpin->value() );
	//gd.setSnap( m_snapHorizUSpin->value(), m_snapVertUSpin->value() );
	gd.setShowGrid( m_gridChBox->isChecked() );
	//gd.setSnapping( m_snapChBox->isChecked() );
	gd.setGridColor( m_gridColorBtn->color() );

    KConfigGroup gridGroup = m_config->group( "Grid" );
    gridGroup.writeEntry( "SpacingX", gd.gridX() );
    gridGroup.writeEntry( "SpacingY", gd.gridY() );
    //gridGroup.writeEntry( "SnapX", gd.snapX() );
    //gridGroup.writeEntry( "SnapY", gd.snapY() );
    gridGroup.writeEntry( "Color", gd.gridColor() );
}

void VConfigGridPage::slotDefault()
{
	KoGridData defGrid;
	setValuesFromGrid( defGrid );
}

void VConfigGridPage::setValuesFromGrid( const KoGridData &grid )
{
	double docW = m_view->part()->document().width();
	double docH = m_view->part()->document().height();

	m_spaceHorizUSpin->setMaximum( docW );
	m_spaceHorizUSpin->changeValue( grid.gridX() );

	m_spaceVertUSpin->setMaximum( docH );
	m_spaceVertUSpin->changeValue( grid.gridY() );

    /*
	m_snapHorizUSpin->setMaximum( grid.spacingX() );
	m_snapHorizUSpin->changeValue( grid.snapX() );

	m_snapVertUSpin->setMaximum( grid.spacingY() );
	m_snapVertUSpin->changeValue( grid.snapY() );
    */
	m_gridChBox->setChecked( grid.showGrid() );
	//m_snapChBox->setChecked( grid.snapping() );
	m_gridColorBtn->setColor( grid.gridColor() );
}

VConfigDefaultPage::VConfigDefaultPage( KarbonView* view, char* name )
{
    setObjectName(name);

    m_view = view;

    m_config = KarbonFactory::componentData().config();

    QGroupBox* gbDocumentSettings = new QGroupBox( i18n( "Document Settings" ), this );
    QVBoxLayout *layout = new QVBoxLayout( gbDocumentSettings );
    layout->setSpacing(KDialog::spacingHint());
    layout->setMargin(KDialog::marginHint());

    m_oldAutoSave = m_view->part()->defaultAutoSave() / 60;

    m_oldBackupFile = true;

    m_oldSaveAsPath = true;

    if( m_config->hasGroup( "Interface" ) )
    {
        KConfigGroup interfaceGroup = m_config->group( "Interface" );
        m_oldAutoSave = interfaceGroup.readEntry( "AutoSave", m_oldAutoSave );
        m_oldBackupFile = interfaceGroup.readEntry( "BackupFile", m_oldBackupFile );
        m_oldSaveAsPath = interfaceGroup.readEntry( "SaveAsPath", m_oldSaveAsPath );
    }

    m_autoSave = new KIntNumInput( m_oldAutoSave, gbDocumentSettings );
    m_autoSave->setRange( 0, 60, 1 );
    m_autoSave->setLabel( i18n( "Auto save (min):" ) );
    m_autoSave->setSpecialValueText( i18n( "No auto save" ) );
    m_autoSave->setSuffix( i18n( "min" ) );
    layout->addWidget( m_autoSave );

    m_createBackupFile = new QCheckBox( i18n( "Create backup file" ), gbDocumentSettings );
    m_createBackupFile->setChecked( m_oldBackupFile );
    layout->addWidget( m_createBackupFile );

    m_saveAsPath = new QCheckBox( i18n( "Save as path" ), gbDocumentSettings );
    m_saveAsPath->setChecked( m_oldSaveAsPath );
    layout->addWidget( m_saveAsPath );
    layout->addStretch();
}

void VConfigDefaultPage::apply()
{
    KConfigGroup interfaceGroup = m_config->group( "Interface" );

	int autoSave = m_autoSave->value();

	if( autoSave != m_oldAutoSave )
	{
        interfaceGroup.writeEntry( "AutoSave", autoSave );
		m_view->part()->setAutoSave( autoSave * 60 );
		m_oldAutoSave = autoSave;
	}

	bool state = m_createBackupFile->isChecked();

	if( state != m_oldBackupFile )
	{
        interfaceGroup.writeEntry( "BackupFile", state );
		m_view->part()->setBackupFile( state );
		m_oldBackupFile = state;
	}

	state = m_saveAsPath->isChecked();

	//if( state != m_oldSaveAsPath )
	//{
        interfaceGroup.writeEntry( "SaveAsPath", state );
		m_view->part()->document().saveAsPath( state );
		m_oldSaveAsPath = state;
	//}
}

void VConfigDefaultPage::slotDefault()
{
	m_autoSave->setValue( m_view->part()->defaultAutoSave() / 60 );
	m_createBackupFile->setChecked( true );
	m_saveAsPath->setChecked( true );
}

#include "vconfiguredlg.moc"

