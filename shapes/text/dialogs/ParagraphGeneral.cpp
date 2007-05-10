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

#include "ParagraphGeneral.h"
#include "ParagraphIndentSpacing.h"
#include "ParagraphLayout.h"
#include "ParagraphBulletsNumbers.h"

#include <KoParagraphStyle.h>

ParagraphGeneral::ParagraphGeneral(QWidget *parent)
    :QWidget(parent),
    m_blockSignals(false),
    m_style(0),
    m_styleManager(0)
{
    widget.setupUi(this);
    //layout()->setMargin(0);

    m_paragraphIndentSpacing = new ParagraphIndentSpacing (this);
    m_paragraphLayout = new ParagraphLayout (this);
    m_paragraphBulletsNumbers = new ParagraphBulletsNumbers (this);

    widget.tabs->addTab(m_paragraphIndentSpacing, "Indent/Spacing");
    widget.tabs->addTab(m_paragraphLayout, "General Layout");
    widget.tabs->addTab(m_paragraphBulletsNumbers, "Bullets/Numbers");
}

void ParagraphGeneral::setStyle(KoParagraphStyle *style) {
    Q_ASSERT(style);
    m_blockSignals = true;

    widget.inheritStyle->clear();
    widget.inheritStyle->addItem(i18n("None"));
    widget.inheritStyle->setCurrentIndex(0);
    foreach(KoParagraphStyle *s, m_paragraphStyles) {
        if(s->styleId() == style->styleId()) continue; // can't inherit from myself.
        widget.inheritStyle->addItem(s->name(), s->styleId());
        if(s == style->parent())
            widget.inheritStyle->setCurrentIndex( widget.inheritStyle->count()-1 );
    }
    widget.name->setText(style->name());
    for(int i=0; i < widget.nextStyle->count(); i++) {
        if(widget.nextStyle->itemData(i).toInt() == style->nextStyle()) {
            widget.nextStyle->setCurrentIndex(i);
            break;
        }
    }

    m_paragraphIndentSpacing->open(style);
    m_paragraphLayout->open(style);
    m_paragraphBulletsNumbers->open(style);

    m_blockSignals = false;
}

void ParagraphGeneral::setParagraphStyles(const QList<KoParagraphStyle*> styles) {
    m_paragraphStyles = styles;
    foreach(KoParagraphStyle *style, m_paragraphStyles)
        widget.nextStyle->addItem(style->name(), style->styleId());
}

#include <ParagraphGeneral.moc>
