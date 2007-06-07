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
#define CHARACTER_STYLE 1001

#include "StyleManager.h"

#include <KoStyleManager.h>
#include <KoParagraphStyle.h>
#include <KoCharacterStyle.h>

StyleManager::StyleManager(QWidget *parent)
    :QWidget(parent)
{
    widget.setupUi(this);
    layout()->setMargin(0);

    connect (widget.styles, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
                this, SLOT(setStyle(QListWidgetItem*, QListWidgetItem*)));
    connect(widget.bNew, SIGNAL(pressed()), this, SLOT(buttonNewPressed()));
    connect(widget.bDelete, SIGNAL(pressed()), this, SLOT(buttonDeletePressed()));

    connect(widget.paragraphStylePage, SIGNAL(nameChanged(const QString&)), this, SLOT(setStyleName(const QString&)));

    connect(widget.createPage, SIGNAL(newParagraphStyle(KoParagraphStyle*)), this, SLOT(addParagraphStyle(KoParagraphStyle*)));
    //connect(widget.createPage, SIGNAL(newCharacterStyle(KoCharacterStyle*)), this, SLOT(addCharacterStyle(KoCharacterStyle*)));
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
    QListWidgetItem *separator = new QListWidgetItem(widget.styles);
    separator->setBackgroundColor(QColor(Qt::black)); // TODO use theme
    separator->setSizeHint(QSize(20, 2));
    defaultOne = true;
    foreach(KoCharacterStyle *style, m_styleManager->characterStyles()) {
        if(defaultOne) {
            defaultOne = false;
            continue;
        }
        if(separator)
            widget.styles->addItem(separator);
        separator = 0;

        QListWidgetItem *item = new QListWidgetItem(style->name(), widget.styles, CHARACTER_STYLE);
        item->setData(CHARACTER_STYLE, style->styleId());
        widget.styles->addItem(item);
        m_characterStyles.append(style);
    }
    delete separator;

    widget.paragraphStylePage->setParagraphStyles(m_paragraphStyles);
    widget.styles->setCurrentRow(0);
}

void StyleManager::setStyle(QListWidgetItem *item, QListWidgetItem *previous) {
    int styleId = item->data(PARAGRAPH_STYLE).toInt();
    if(styleId > 0) {
        widget.characterStylePage->save();
        widget.characterStylePage->setStyle(0);
        // TODO copy so we can press cancel
        widget.paragraphStylePage->save();
        KoParagraphStyle *style = m_styleManager->paragraphStyle(styleId);
        widget.paragraphStylePage->setStyle(style);
        widget.stackedWidget->setCurrentWidget(widget.paragraphStylePage);
    }
    else {
        widget.paragraphStylePage->save();
        widget.paragraphStylePage->setStyle(0);
        widget.characterStylePage->save();
        styleId = item->data(CHARACTER_STYLE).toInt();
        if(styleId > 0) {
            KoCharacterStyle *style = m_styleManager->characterStyle(styleId);
            widget.characterStylePage->setStyle(style);
            widget.stackedWidget->setCurrentWidget(widget.characterStylePage);
        }
        else {
            // separator clicked.
            const int row = widget.styles->row(item);
            if(widget.styles->row(previous) == row + 1) // moving up.
                widget.styles->setCurrentRow(row -1);
            else if(widget.styles->row(previous) == row - 1) // moving down.
                widget.styles->setCurrentRow(row +1);
            else
                widget.styles->setCurrentItem(previous);
        }
    }
}

void StyleManager::setUnit(const KoUnit &unit) {
    widget.paragraphStylePage->setUnit(unit);
    widget.characterStylePage->setUnit(unit);
}


void StyleManager::save() {
    widget.paragraphStylePage->save();
}


void StyleManager::buttonNewPressed() {
    widget.stackedWidget->setCurrentWidget(widget.createPage);
    // that widget will emit a new style which we will add using addParagraphStyle or addCharacterStyle
}

void StyleManager::addParagraphStyle(KoParagraphStyle *style) {
    QListWidgetItem *item = new QListWidgetItem(style->name(), 0, PARAGRAPH_STYLE);
m_styleManager->add(style); // TODO assign dummy id.
    item->setData(PARAGRAPH_STYLE, style->styleId());
    widget.styles->insertItem(m_paragraphStyles.count(), item);
    m_paragraphStyles.append(style);
    widget.styles->setCurrentItem(item);
    widget.paragraphStylePage->switchToGeneralTab();
}

void StyleManager::addCharacterStyle(KoCharacterStyle *style) {
}

void StyleManager::buttonDeletePressed() {

}

void StyleManager::setStyleName(const QString &name) {
    widget.styles->currentItem()->setText(name);
}

#include <StyleManager.moc>
