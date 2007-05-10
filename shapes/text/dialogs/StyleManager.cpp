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

#define PARAGRAPH_STYLE 1000
// #define CHARACTER_STYLE 1001

#include "StyleManager.h"

#include <KoStyleManager.h>
#include <KoParagraphStyle.h>

StyleManager::StyleManager(QWidget *parent)
    :QWidget(parent)
{
    widget.setupUi(this);
    layout()->setMargin(0);

    connect (widget.styles, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
                this, SLOT(setStyle(QListWidgetItem*)));
}

void StyleManager::setStyleManager(KoStyleManager *sm) {
    Q_ASSERT(sm);
    m_styleManager = sm;
    widget.styles->clear();
    bool defaultOne = true;
    foreach(KoParagraphStyle *style, m_styleManager->paragraphStyles()) {
        if(defaultOne) {
            defaultOne = false;
            continue;
        }
        QListWidgetItem *item = new QListWidgetItem(style->name(), widget.styles, PARAGRAPH_STYLE);
        item->setData(PARAGRAPH_STYLE, style->styleId());
        widget.styles->addItem(item);
        m_paragraphStyles.append(style);
    }

    widget.paragraphStylePage->setParagraphStyles(m_paragraphStyles);
    widget.styles->setCurrentRow(0);
}

void StyleManager::setStyle(QListWidgetItem *item) {
    int styleId = item->data(PARAGRAPH_STYLE).toInt();
    KoParagraphStyle *style = m_styleManager->paragraphStyle(styleId);
    // TODO copy so we can press cancel
    widget.paragraphStylePage->setStyle(style);
}

#include <StyleManager.moc>
