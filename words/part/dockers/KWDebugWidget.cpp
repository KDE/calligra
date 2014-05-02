/* This file is part of the KDE project
 * Copyright (C) 2014 Denis Kuplyakov <dener.kup@gmail.com>
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

#include "KWDebugWidget.h"
#include <KoTextEditor.h>
#include <KoParagraphStyle.h>
#include <KoSection.h>

#include <QHBoxLayout>
#include <QTimer>

KWDebugWidget::KWDebugWidget(QWidget *parent)
    : QWidget(parent)
    , m_canvas(0)
{
    initUi();
    initLayout();

    updateData();
}

KWDebugWidget::~KWDebugWidget()
{
}

void KWDebugWidget::initUi()
{
    m_label = new QLabel(this);
    m_label->setText("Some debug info will be here."); // No i18n as it's for debug only
}

void KWDebugWidget::initLayout()
{
    QHBoxLayout *mainBox = new QHBoxLayout(this);
    mainBox->addWidget(m_label);

    setLayout(mainBox);
}

void KWDebugWidget::updateData()
{
    QTimer().singleShot(100, this, SLOT(updateData()));
    if (!isVisible()) {
        return;
    }

    KoTextEditor *editor = KoTextEditor::getTextEditorFromCanvas(m_canvas);
    if (!editor) {
        return;
    }

    QTextBlock cur_block = editor->block();
    QTextBlockFormat fmt = cur_block.blockFormat();

    QString will_show;
    if (fmt.hasProperty(KoParagraphStyle::SectionStartings))
    {
        will_show = "This sections starts here :";
        QVariant var = fmt.property(KoParagraphStyle::SectionStartings);
        QList<QVariant> open_list = var.value< QList<QVariant> >();
        foreach (const QVariant &sv, open_list)
        {
            KoSection *sec = static_cast<KoSection *>(sv.value<void *>());
            will_show += sec->name() + " ";
        }
        will_show += "\n";
    }

    if (fmt.hasProperty(KoParagraphStyle::SectionEndings))
    {
        will_show += "This sections end here :";
        QVariant var = fmt.property(KoParagraphStyle::SectionEndings);
        QList<QVariant> close_list = var.value< QList<QVariant> >();
        foreach (const QVariant &sv, close_list)
        {
            KoSectionEnd *sec = static_cast<KoSectionEnd *>(sv.value<void *>());
            will_show += sec->name + " ";
        }
        will_show += "\n";
    }

    will_show += "block number is " + QString::number(editor->constCursor().block().blockNumber());

    m_label->setText(will_show);
}

void KWDebugWidget::setCanvas(KWCanvas* canvas)
{
    m_canvas = canvas;
}

void KWDebugWidget::unsetCanvas()
{
}
