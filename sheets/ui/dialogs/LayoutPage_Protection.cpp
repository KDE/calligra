/* This file is part of the KDE project
             SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
             SPDX-FileCopyrightText: 2004-2022 Tomas Mecir <mecirt@gmail.com>
             SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
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

#include "LayoutPage_Protection.h"

using namespace Calligra::Sheets;

LayoutPageProtection::LayoutPageProtection(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
    connect(m_bHideAll, &QAbstractButton::toggled, m_bIsProtected, &QWidget::setDisabled);
    connect(m_bHideAll, &QAbstractButton::toggled, m_bHideFormula, &QWidget::setDisabled);

    m_bDontPrint->setStyleKey(Style::DontPrintText);
    m_bHideAll->setStyleKey(Style::HideAll);
    m_bIsProtected->setStyleKey(Style::NotProtected);
    m_bIsProtected->setReversed(true);
    m_bHideFormula->setStyleKey(Style::HideFormula);
}

LayoutPageProtection::~LayoutPageProtection() = default;

void LayoutPageProtection::apply(Style *style, bool partial)
{
    m_bDontPrint->saveTo(*style, partial);
    m_bHideAll->saveTo(*style, partial);
    m_bIsProtected->saveTo(*style, partial);
    m_bHideFormula->saveTo(*style, partial);
}

void LayoutPageProtection::loadFrom(const Style &style, bool partial)
{
    m_bDontPrint->loadFrom(style, partial);
    m_bHideAll->loadFrom(style, partial);
    m_bIsProtected->loadFrom(style, partial);
    m_bHideFormula->loadFrom(style, partial);
}
