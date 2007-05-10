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
#include "ParagraphGeneral.h"

#include <KoStyleManager.h>
#include <KoCharacterStyle.h>
#include <KoParagraphStyle.h>

#include <kdebug.h>

StylesWidget::StylesWidget(Type type, QWidget *parent)
    : QWidget(parent),
    m_type(type),
    m_styleManager(0),
    m_blockSignals(false)
{
    widget.setupUi(this);
    connect(widget.styleList, SIGNAL(itemPressed(QListWidgetItem *)), this, SLOT(itemSelected(QListWidgetItem*)));
    connect(widget.newStyle, SIGNAL(pressed()), this, SLOT(newStyleClicked()));
    connect(widget.deleteStyle, SIGNAL(pressed()), this, SLOT(deleteStyleClicked()));
    connect(widget.styleList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(editStyle(QListWidgetItem*)));
}

void StylesWidget::setStyleManager(KoStyleManager *sm) {
    if(sm == m_styleManager)
        return;
    m_styleManager = sm;
    widget.styleList->clear();
    if(m_styleManager == 0)
        return;

    if(m_type == CharacterStyle) {
        foreach(KoCharacterStyle *style, m_styleManager->characterStyles()) {
            QListWidgetItem *item = new QListWidgetItem(style->name(), widget.styleList);
            item->setData(99, style->styleId());
            widget.styleList->addItem(item);
        }
    }
    else {
        foreach(KoParagraphStyle *style, m_styleManager->paragraphStyles()) {
            QListWidgetItem *item = new QListWidgetItem(style->name(), widget.styleList);
            item->setData(99, style->styleId());
            widget.styleList->addItem(item);
        }
    }
}

void StylesWidget::itemSelected(QListWidgetItem *item) {
    if(m_blockSignals)
        return;
    int styleId = item->data(99).toInt();

    if(m_type == CharacterStyle)
        emit characterStyleSelected(m_styleManager->characterStyle(styleId));
    else
        emit paragraphStyleSelected(m_styleManager->paragraphStyle(styleId));
}

void StylesWidget::setCurrentFormat(const QTextBlockFormat &format) {
    if(m_type == CharacterStyle || format == m_currentBlockFormat)
        return;
    m_currentBlockFormat = format;
    int id = m_currentBlockFormat.intProperty(KoParagraphStyle::StyleId);
    int index = 0;
    while(index < widget.styleList->count()) {
        if(widget.styleList->item(index)->data(99).toInt() == id)
            break;
        index++;
    }
    if(index >= widget.styleList->count()) // not here, so default to the first one.
        index = 0;
    m_blockSignals = true;
    widget.styleList->setCurrentItem(widget.styleList->item(index));
    m_blockSignals = false;
}

void StylesWidget::setCurrentFormat(const QTextCharFormat &format) {
    if(format == m_currentCharFormat)
        return;
    m_currentCharFormat = format;

    int id = m_currentCharFormat.intProperty(KoCharacterStyle::StyleId);
    if(m_type == CharacterStyle) { // update the list-selection
        int index = 0;
        while(index < widget.styleList->count()) {
            if(widget.styleList->item(index)->data(99).toInt() == id)
                break;
            index++;
        }
        if(index >= widget.styleList->count()) // not here, so default to the first one.
            index = 0;
        m_blockSignals = true;
        widget.styleList->setCurrentItem(widget.styleList->item(index));
        m_blockSignals = false;
    }
    else { // if the characterStyle is not the same as our parag style's one, mark it.
        // TODO
    }
}

void StylesWidget::newStyleClicked() {
    // TODO
}

void StylesWidget::deleteStyleClicked() {
    // TODO
}

void StylesWidget::editStyle(QListWidgetItem *item) {
    QWidget *widget = 0;
    if(m_type == CharacterStyle) {
        //KoCharacterStyle *style = m_styleManager->characterStyle(item->data(99).toInt());
        // TODO
    }
    else {
        KoParagraphStyle *style = m_styleManager->paragraphStyle(item->data(99).toInt());
        ParagraphGeneral *p = new ParagraphGeneral();
        p->setStyle(style);
        widget = p;
    }
    if(widget) {
        KDialog *dia = new KDialog(this);
        dia->setMainWidget(widget);
        dia->show();
    }
}

#include <StylesWidget.moc>
