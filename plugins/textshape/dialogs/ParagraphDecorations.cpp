/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001, 2002 Montel Laurent <lmontel@mandrakesoft.com>
   SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ParagraphDecorations.h"

#include <QDebug>

ParagraphDecorations::ParagraphDecorations(QWidget *parent)
    : QWidget(parent)
{
    widget.setupUi(this);
    widget.mainLayout->insertStretch(0);
    widget.mainLayout->insertStretch(2);
    widget.resetBackgroundColor->setAccessibleName(i18nc("@action:button", "Reset background color"));

    connect(widget.backgroundColor, &KColorButton::changed, this, &ParagraphDecorations::slotBackgroundColorChanged);
    connect(widget.resetBackgroundColor, &QAbstractButton::clicked, this, &ParagraphDecorations::clearBackgroundColor);
}

void ParagraphDecorations::slotBackgroundColorChanged()
{
    m_backgroundColorReset = false;
    m_backgroundColorChanged = true;
    Q_EMIT parStyleChanged();
}

void ParagraphDecorations::setDisplay(KoParagraphStyle *style, bool directFormattingMode)
{
    Q_UNUSED(directFormattingMode);
    m_backgroundColorChanged = false;
    m_backgroundColorReset = style->background().style() == Qt::NoBrush;
    if (m_backgroundColorReset) {
        clearBackgroundColor();
    } else {
        widget.backgroundColor->setColor(style->background().color());
    }
}

void ParagraphDecorations::save(KoParagraphStyle *style) const
{
    Q_ASSERT(style);
    if (m_backgroundColorReset)
        // clearing the property doesn't work since ParagraphSettingsDialog does a mergeBlockFormat
        // so we'll set it to a Qt::NoBrush brush instead
        style->setBackground(QBrush(Qt::NoBrush));
    else if (m_backgroundColorChanged)
        style->setBackground(QBrush(widget.backgroundColor->color()));
}

void ParagraphDecorations::clearBackgroundColor()
{
    widget.backgroundColor->setColor(widget.backgroundColor->defaultColor());
    m_backgroundColorReset = true;
    Q_EMIT parStyleChanged();
}
