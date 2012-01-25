/* This file is part of the KDE project
   Copyright (C)  2001, 2002 Montel Laurent <lmontel@mandrakesoft.com>
   Copyright (C)  2006-2007 Thomas Zander <zander@kde.org>
   Copyright (C)  2009 Pierre Stirnweiss <pstirnweiss@googlemail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "FontDecorations.h"

#include <KDebug>

FontDecorations::FontDecorations(bool uniqueFormat, QWidget* parent)
        : QWidget(parent),
        m_uniqueFormat(uniqueFormat)
{
    widget.setupUi(this);

    widget.resetTextColor->setIcon(KIcon("edit-clear"));
    widget.resetBackground->setIcon(KIcon("edit-clear"));

    connect(widget.textColor, SIGNAL(changed(const QColor&)), this, SLOT(textColorChanged()));
    connect(widget.backgroundColor, SIGNAL(changed(const QColor&)), this, SLOT(backgroundColorChanged()));

    connect(widget.resetTextColor, SIGNAL(clicked()), this, SLOT(clearTextColor()));
    connect(widget.resetBackground, SIGNAL(clicked()), this, SLOT(clearBackgroundColor()));

    widget.shadowGroupBox->setVisible(false);
}

void FontDecorations::backgroundColorChanged()
{
    m_backgroundColorReset = false; m_backgroundColorChanged = true;
    if (!m_ignoreSignals) {
        m_backgroundColorInherited = false;
    }
    if (widget.backgroundColor->color().isValid()) {
        emit backgroundColorChanged(widget.backgroundColor->color());
    }
}

void FontDecorations::textColorChanged()
{
    m_textColorReset = false; m_textColorChanged = true;
    if (!m_ignoreSignals) {
        m_foregroundColorInherited = false;
    }
    if (widget.textColor->color().isValid()) {
        emit textColorChanged(widget.textColor->color());
    }
}

void FontDecorations::textToggled(bool state)
{
    widget.textColor->setEnabled(state);
    widget.resetTextColor->setEnabled(state);
}

void FontDecorations::backgroundToggled(bool state)
{
    widget.backgroundColor->setEnabled(state);
    widget.resetBackground->setEnabled(state);
}

void FontDecorations::setDisplay(KoCharacterStyle *style)
{
    if (!style)
        return;

    m_ignoreSignals = true;
    m_textColorChanged = false;
    m_backgroundColorChanged = false;
    m_textColorReset = !style->hasProperty(QTextFormat::ForegroundBrush);
    m_foregroundColorInherited = m_textColorReset;
    if (m_textColorReset || (style->foreground().style() == Qt::NoBrush)) {
        clearTextColor();
    } else {
        widget.textColor->setColor(style->foreground().color());
    }
    m_backgroundColorReset = !style->hasProperty(QTextFormat::BackgroundBrush);
    m_backgroundColorInherited = m_backgroundColorReset;
    if (m_backgroundColorReset || (style->background().style() == Qt::NoBrush)) {
        clearBackgroundColor();
    } else {
        widget.backgroundColor->setColor(style->background().color());
    }
    m_ignoreSignals = false;
}

void FontDecorations::save(KoCharacterStyle *style) const
{
    if (!style)
        return;

    if (!m_backgroundColorInherited && m_backgroundColorReset) {
        style->setBackground(QBrush(Qt::NoBrush));
    }
    else if (!m_backgroundColorInherited && m_backgroundColorChanged) {
        style->setBackground(QBrush(widget.backgroundColor->color()));
    }
    if (!m_foregroundColorInherited && m_textColorReset) {
        style->setForeground(QBrush(Qt::NoBrush));
    }
    else if (!m_foregroundColorInherited && m_textColorChanged) {
        style->setForeground(QBrush(widget.textColor->color()));
    }
}

void FontDecorations::clearTextColor()
{
    widget.textColor->setColor(widget.textColor->defaultColor());
    m_textColorReset = true;
    if (!m_ignoreSignals)
        m_foregroundColorInherited = false;
    emit textColorChanged(QColor(Qt::black));
}

void FontDecorations::clearBackgroundColor()
{
    widget.backgroundColor->setColor(widget.backgroundColor->defaultColor());
    m_backgroundColorReset = true;
    if (!m_ignoreSignals)
        m_backgroundColorInherited = false;
    emit backgroundColorChanged(QColor(Qt::transparent));
}

#include <FontDecorations.moc>
