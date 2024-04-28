/* This file is part of the KDE project
SPDX-FileCopyrightText: 2002, 2003 Laurent Montel <lmontel@mandrakesoft.com>
SPDX-FileCopyrightText: 2006-2007 Jan Hambrecht <jaham@gmx.net>

SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KarbonConfigInterfacePage.h"

#include "KarbonDocument.h"
#include "KarbonFactory.h"
#include "KarbonPart.h"
#include "KarbonView.h"

#include <KoConfigDocumentPage.h>
#include <KoConfigGridPage.h>
#include <KoConfigMiscPage.h>

#include <KConfig>
#include <KConfigGroup>
#include <KLocalizedString>
#include <kcolorbutton.h>

#include <QCheckBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QSpinBox>

KarbonConfigInterfacePage::KarbonConfigInterfacePage(KarbonView *view, char *name)
{
    setObjectName(name);

    m_view = view;
    m_config = KarbonFactory::karbonConfig();

    m_oldRecentFiles = 10;
    m_oldDockerFontSize = 8;
    m_oldCanvasColor = QColor(Qt::white);
    bool oldShowStatusBar = true;

    QGroupBox *tmpQGroupBox = new QGroupBox(i18n("Interface"), this);

    KConfigGroup emptyGroup = m_config->group("GUI");
    m_oldDockerFontSize = emptyGroup.readEntry("palettefontsize", m_oldDockerFontSize);

    if (m_config->hasGroup("Interface")) {
        KConfigGroup interfaceGroup = m_config->group("Interface");

        m_oldRecentFiles = interfaceGroup.readEntry("NbRecentFile", m_oldRecentFiles);
        oldShowStatusBar = interfaceGroup.readEntry("ShowStatusBar", true);
        m_oldCanvasColor = interfaceGroup.readEntry("CanvasColor", m_oldCanvasColor);
    }

    QFormLayout *interfaceLayout = new QFormLayout(tmpQGroupBox);

    m_showStatusBar = new QCheckBox(tmpQGroupBox);
    m_showStatusBar->setChecked(oldShowStatusBar);
    interfaceLayout->addRow(i18n("Show status bar:"), m_showStatusBar);

    m_recentFiles = new QSpinBox(tmpQGroupBox);
    m_recentFiles->setRange(1, 20);
    m_recentFiles->setSingleStep(1);
    m_recentFiles->setValue(m_oldRecentFiles);
    interfaceLayout->addRow(i18n("Number of recent files:"), m_recentFiles);

    m_dockerFontSize = new QSpinBox(tmpQGroupBox);
    m_dockerFontSize->setRange(5, 20);
    m_dockerFontSize->setSingleStep(1);
    m_dockerFontSize->setValue(m_oldDockerFontSize);
    interfaceLayout->addRow(i18n("Palette font size:"), m_dockerFontSize);

    // TODO or move or remove?
    //     m_canvasColor = new KColorButton(m_oldCanvasColor, tmpQGroupBox);
    //     interfaceLayout->addRow(i18n("Canvas color:"), m_canvasColor);
}

void KarbonConfigInterfacePage::apply()
{
    bool showStatusBar = m_showStatusBar->isChecked();

    KarbonDocument *part = m_view->part();

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

    //     QColor canvasColor = m_canvasColor->color();
    //     if (canvasColor != m_oldCanvasColor) {
    //         interfaceGroup.writeEntry("CanvasColor", canvasColor);
    //         refreshGUI = true;
    //     }

    if (refreshGUI)
        part->reorganizeGUI();
}

void KarbonConfigInterfacePage::slotDefault()
{
    m_recentFiles->setValue(10);
    m_dockerFontSize->setValue(8);
    m_showStatusBar->setChecked(true);
}
