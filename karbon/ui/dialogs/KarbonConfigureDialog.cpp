/* This file is part of the KDE project
Copyright (C) 2002, 2003 Laurent Montel <lmontel@mandrakesoft.com>
Copyright (C) 2006-2007 Jan Hambrecht <jaham@gmx.net>

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

#include "KarbonConfigureDialog.h"

#include "KarbonView.h"
#include "KarbonPart.h"
#include "KarbonFactory.h"

#include <KoGridData.h>
#include <KoUnitDoubleSpinBox.h>
#include <KoConfigGridPage.h>
#include <KoConfigDocumentPage.h>
#include <KoConfigMiscPage.h>

#include <klocale.h>
#include <knuminput.h>
#include <kcolorbutton.h>
#include <kcomponentdata.h>
#include <kicon.h>
#include <kvbox.h>
#include <kconfig.h>
#include <kcombobox.h>

#include <QtGui/QCheckBox>
#include <QtGui/QLabel>
#include <QtGui/QGroupBox>
#include <QtGui/QGridLayout>

#include <float.h>


KarbonConfigureDialog::KarbonConfigureDialog(KarbonView* parent)
        : KPageDialog(parent)
{
    setFaceType(List);
    setCaption(i18n("Configure"));
    setButtons(KDialog::Ok | KDialog::Apply | KDialog::Cancel | KDialog::Default);
    setDefaultButton(KDialog::Ok);

    m_interfacePage = new ConfigInterfacePage(parent);
    KPageWidgetItem* item = addPage(m_interfacePage, i18n("Interface"));
    item->setHeader(i18n("Interface"));
    item->setIcon(KIcon(BarIcon("preferences-desktop-theme", KIconLoader::SizeMedium)));

    m_miscPage = new KoConfigMiscPage(parent->part());
    item = addPage(m_miscPage, i18n("Misc"));
    item->setHeader(i18n("Misc"));
    item->setIcon(KIcon(BarIcon("preferences-other", KIconLoader::SizeMedium)));

    m_gridPage = new KoConfigGridPage(parent->part());
    item = addPage(m_gridPage, i18n("Grid"));
    item->setHeader(i18n("Grid"));
    item->setIcon(KIcon(BarIcon("grid", KIconLoader::SizeMedium)));

    connect(m_miscPage, SIGNAL(unitChanged(int)), m_gridPage, SLOT(slotUnitChanged(int)));

    m_defaultDocPage = new KoConfigDocumentPage(parent->part());
    item = addPage(m_defaultDocPage, i18nc("@title:tab Document settings page", "Document"));
    item->setHeader(i18n("Document Settings"));
    item->setIcon(KIcon(BarIcon("document-properties", KIconLoader::SizeMedium)));

    connect(this, SIGNAL(okClicked()), this, SLOT(slotApply()));
    connect(this, SIGNAL(applyClicked()), this, SLOT(slotApply()));
    connect(this, SIGNAL(defaultClicked()), this, SLOT(slotDefault()));
}

void KarbonConfigureDialog::slotApply()
{
    m_interfacePage->apply();
    m_miscPage->apply();
    m_defaultDocPage->apply();
    m_gridPage->apply();
}

void KarbonConfigureDialog::slotDefault()
{
    QWidget* curr = currentPage()->widget();

    if (curr == m_interfacePage)
        m_interfacePage->slotDefault();
    else if (curr == m_miscPage)
        m_miscPage->slotDefault();
    else if (curr == m_gridPage)
        m_gridPage->slotDefault();
    else if (curr == m_defaultDocPage)
        m_defaultDocPage->slotDefault();
}


ConfigInterfacePage::ConfigInterfacePage(KarbonView* view, char* name)
{
    setObjectName(name);

    m_view = view;
    m_config = KarbonFactory::componentData().config();

    m_oldRecentFiles = 10;
    m_oldDockerFontSize = 8;
    m_oldCanvasColor = QColor(Qt::white);
    bool oldShowStatusBar = true;

    QGroupBox* tmpQGroupBox = new QGroupBox(i18n("Interface"), this);

    KConfigGroup emptyGroup = m_config->group("GUI");
    m_oldDockerFontSize = emptyGroup.readEntry("palettefontsize", m_oldDockerFontSize);

    if (m_config->hasGroup("Interface")) {
        KConfigGroup interfaceGroup = m_config->group("Interface");

        m_oldRecentFiles = interfaceGroup.readEntry("NbRecentFile", m_oldRecentFiles);
        oldShowStatusBar = interfaceGroup.readEntry("ShowStatusBar", true);
        m_oldCanvasColor = interfaceGroup.readEntry("CanvasColor", m_oldCanvasColor);
    }

    QVBoxLayout *grpLayout = new QVBoxLayout(tmpQGroupBox);

    m_showStatusBar = new QCheckBox(i18n("Show status bar"), tmpQGroupBox);
    m_showStatusBar->setChecked(oldShowStatusBar);
    grpLayout->addWidget(m_showStatusBar);

    m_recentFiles = new KIntNumInput(tmpQGroupBox);
    m_recentFiles->setRange(1, 20, 1);
    m_recentFiles->setValue(m_oldRecentFiles);
    m_recentFiles->setLabel(i18n("Number of recent files:"));
    grpLayout->addWidget(m_recentFiles);

    m_dockerFontSize = new KIntNumInput(tmpQGroupBox);
    m_dockerFontSize->setRange(5, 20, 1);
    m_dockerFontSize->setValue(m_oldDockerFontSize);
    m_dockerFontSize->setLabel(i18n("Palette font size:"));
    grpLayout->addWidget(m_dockerFontSize);

    QLabel* canvasColorLbl = new QLabel(i18n("Canvas color:"), tmpQGroupBox);
    m_canvasColor = new KColorButton(m_oldCanvasColor, tmpQGroupBox);
    canvasColorLbl->setBuddy(m_canvasColor);
    grpLayout->addWidget(canvasColorLbl);
    grpLayout->addWidget(m_canvasColor);

    grpLayout->addStretch();
}

void ConfigInterfacePage::apply()
{
    bool showStatusBar = m_showStatusBar->isChecked();

    KarbonPart* part = m_view->part();

    KConfigGroup interfaceGroup = m_config->group("Interface");

    int recent = m_recentFiles->value();

    if (recent != m_oldRecentFiles) {
        interfaceGroup.writeEntry("NbRecentFile", recent);
        m_view->setNumberOfRecentFiles(recent);
        m_oldRecentFiles = recent;
    }

    bool refreshGUI = false;

    if (showStatusBar != part->showStatusBar()) {
        interfaceGroup.writeEntry("ShowStatusBar", showStatusBar);
        part->setShowStatusBar(showStatusBar);
        refreshGUI = true;
    }

    int dockerFontSize = m_dockerFontSize->value();

    if (dockerFontSize != m_oldDockerFontSize) {
        m_config->group("GUI").writeEntry("palettefontsize", dockerFontSize);
        m_oldDockerFontSize = dockerFontSize;
        refreshGUI = true;
    }

    QColor canvasColor = m_canvasColor->color();
    if (canvasColor != m_oldCanvasColor) {
        interfaceGroup.writeEntry("CanvasColor", canvasColor);
        refreshGUI = true;
    }

    if (refreshGUI)
        part->reorganizeGUI();
}

void ConfigInterfacePage::slotDefault()
{
    m_recentFiles->setValue(10);
    m_dockerFontSize->setValue(8);
    m_showStatusBar->setChecked(true);
}


#include "KarbonConfigureDialog.moc"
