/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include "StylesWidget.h"

#include <KoStyleManager.h>
#include <KoCharacterStyle.h>
#include <KoParagraphStyle.h>

#include <kdebug.h>

StylesWidget::StylesWidget(Type type, QWidget *parent)
    : QWidget(parent),
    m_type(type),
    m_styleManager(0)
{
    widget.setupUi(this);
}

void StylesWidget::setStyleManager(KoStyleManager *sm) {
    if(sm == m_styleManager)
        return;
    m_styleManager = sm;
    widget.styleList->clear();
    if(m_styleManager == 0)
        return;

    if(m_type == CharacterStyle)
        foreach(KoCharacterStyle *style, m_styleManager->characterStyles())
            m_items.append(Entry(style->name(), style->styleId()));
    else
        foreach(KoParagraphStyle *style, m_styleManager->paragraphStyles())
            m_items.append(Entry(style->name(), style->styleId()));

    foreach(Entry entry, m_items)
        widget.styleList->addItem(entry.first);
}

#include <StylesWidget.moc>
