/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
             SPDX-FileCopyrightText: 2004 Tomas Mecir <mecirt@gmail.com>
             SPDX-FileCopyrightText: 2002-2004 Ariya Hidayat <ariya@kde.org>
             SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
             SPDX-FileCopyrightText: 1999-2002 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 1999-2002 Harri Porten <porten@kde.org>
             SPDX-FileCopyrightText: 2000-2001 David Faure <faure@kde.org>
             SPDX-FileCopyrightText: 1998-2000 Torben Weis <weis@kde.org>
             SPDX-FileCopyrightText: 2000 Werner Trobin <trobin@kde.org>
             SPDX-FileCopyrightText: 1999 Reginald Stadlbauer <reggie@kde.org>
             SPDX-FileCopyrightText: 1998-1999 Stephan Kulow <coolo@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "LayoutDialog.h"
#include "LayoutPage_Border.h"
#include "LayoutPage_Float.h"
#include "LayoutPage_Font.h"
#include "LayoutPage_General.h"
#include "LayoutPage_Pattern.h"
#include "LayoutPage_Position.h"
#include "LayoutPage_Protection.h"

#include "core/DocBase.h"
#include "core/Map.h"
#include "core/Sheet.h"
#include "engine/CalculationSettings.h"

#include <KPageWidget>

using namespace Calligra::Sheets;
using namespace Qt::StringLiterals;

/***************************************************************************
 *
 * LayoutDialog
 *
 ***************************************************************************/

LayoutDialog::LayoutDialog(QWidget *parent, Sheet *sheet, StyleManager *manager, bool isStyle)
    : ActionDialog(parent)
    , m_sheet(sheet)
    , m_styleManager(manager)
{
    init(isStyle);
}

LayoutDialog::~LayoutDialog() = default;

Style LayoutDialog::style(bool multicell)
{
    Style style;
    if (!multicell)
        style.setDefault(); // If not multicell, the style needs to override existing formatting
    borderPage->apply(&style, multicell);
    floatPage->apply(&style, multicell);
    fontPage->apply(&style, multicell);
    positionPage->apply(&style, multicell);
    patternPage->apply(&style, multicell);
    protectPage->apply(&style, multicell);
    return style;
}

CustomStyle LayoutDialog::customStyle()
{
    CustomStyle style = CustomStyle(QString());
    generalPage->apply(&style);
    borderPage->apply(&style, false);
    floatPage->apply(&style, false);
    fontPage->apply(&style, false);
    positionPage->apply(&style, false);
    patternPage->apply(&style, false);
    protectPage->apply(&style, false);
    return style;
}

void LayoutDialog::setStyle(const Style &style, bool multicell)
{
    borderPage->loadFrom(style, multicell);
    floatPage->loadFrom(style, multicell);
    fontPage->loadFrom(style, multicell);
    positionPage->loadFrom(style, multicell);
    patternPage->loadFrom(style, multicell);
    protectPage->loadFrom(style, multicell);
}

void LayoutDialog::setCustomStyle(const CustomStyle &style)
{
    generalPage->loadFrom(style);
    borderPage->loadFrom(style, false);
    floatPage->loadFrom(style, false);
    fontPage->loadFrom(style, false);
    positionPage->loadFrom(style, false);
    patternPage->loadFrom(style, false);
    protectPage->loadFrom(style, false);
}

void LayoutDialog::setOkButtonEnabled(bool enabled)
{
    enableButtonApply(enabled);
}

void LayoutDialog::onApply()
{
    Q_EMIT applyStyle();
    accept();
}

void LayoutDialog::init(bool isStyle)
{
    setWindowTitle(i18n("Cell Format"));
    setMinimumWidth(1100);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    setButtonText(Apply, i18n("Set Cell Format"));

    KPageWidget *main = new KPageWidget;
    setMainWidget(main);
    main->setFaceType(KPageWidget::FlatList);

    KPageWidgetItem *page;
    if (isStyle) {
        generalPage = new LayoutPageGeneral(this, m_styleManager);
        connect(generalPage, &LayoutPageGeneral::validDataChanged, this, &LayoutDialog::setOkButtonEnabled);
        page = main->addPage(generalPage, i18n("General"));
        page->setIcon(QIcon::fromTheme(u"settings-configure-symbolic"_s));
    }

    Localization *locale = m_sheet->map()->calculationSettings()->locale();
    floatPage = new LayoutPageFloat(this, locale, m_sheet->fullMap()->formatter());
    page = main->addPage(floatPage, i18n("Data Format"));
    page->setIcon(QIcon::fromTheme(u"cell_layout"_s));

    fontPage = new LayoutPageFont(this);
    page = main->addPage(fontPage, i18n("Font"));
    page->setIcon(QIcon::fromTheme(u"font-symbolic"_s));

    KoUnit unit = m_sheet->doc()->unit();
    positionPage = new LayoutPagePosition(this, unit);
    page = main->addPage(positionPage, i18n("Position"));
    page->setIcon(QIcon::fromTheme(u"align-horizontal-left-to-anchor"_s));

    borderPage = new LayoutPageBorder(this);
    page = main->addPage(borderPage, i18n("Border"));
    page->setIcon(QIcon::fromTheme(u"format-border-set-all"_s));

    patternPage = new LayoutPagePattern(this);
    page = main->addPage(patternPage, i18n("Background"));
    page->setIcon(QIcon::fromTheme(u"color-profile-symbolic"_s));

    protectPage = new LayoutPageProtection(this);
    page = main->addPage(protectPage, i18n("Cell Protection"));
    page->setIcon(QIcon::fromTheme(u"channel-secure-symbolic"_s));
}
