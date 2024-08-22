/* This file is part of the KDE project
             SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
             SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "LayoutWidgets.h"

using namespace Calligra::Sheets;

LayoutCheckbox::LayoutCheckbox(QWidget *parent)
    : QCheckBox(parent)
    , m_defaultValue(false)
    , m_reversed(false)
{
}

LayoutCheckbox::~LayoutCheckbox() = default;

void LayoutCheckbox::setStyleKey(const Style::Key &key, bool defaultValue)
{
    m_styleKey = key;
    m_defaultValue = defaultValue;
}

void LayoutCheckbox::loadFrom(const Style &style, bool partial)
{
    if (partial && (!style.hasAttribute(m_styleKey))) {
        setTristate(true);
        setCheckState(Qt::PartiallyChecked);
        return;
    }

    setTristate(false);
    bool val = style.getBoolValue(m_styleKey);
    if (m_reversed)
        val = !val;
    setCheckState(val ? Qt::Checked : Qt::Unchecked);
}

void LayoutCheckbox::saveTo(Style &style, bool partial)
{
    Qt::CheckState st = checkState();
    if (st == Qt::PartiallyChecked)
        return;
    bool checked = (st == Qt::Checked) ? true : false;
    if (m_reversed)
        checked = !checked;

    bool include = false;
    if (partial) {
        // The "partially checked" status was already checked, so the checkbox has something else and we need to include it
        include = true;
    } else {
        if (checked != m_defaultValue)
            include = true;
    }

    if (include)
        style.insertSubStyle(m_styleKey, checked);
}
