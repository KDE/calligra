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
#include "SimpleStyleWidget.h"
#include "TextTool.h"

SimpleStyleWidget::SimpleStyleWidget(TextTool *tool, QWidget *parent)
    : QWidget(parent)
{
    widget.setupUi(this);
    widget.bold->setDefaultAction(tool->m_actionFormatBold);
    widget.italic->setDefaultAction(tool->m_actionFormatItalic);
    widget.strikeOut->setDefaultAction(tool->m_actionFormatStrikeOut);
    widget.underline->setDefaultAction(tool->m_actionFormatUnderline);
    widget.alignLeft->setDefaultAction(tool->m_actionAlignLeft);
    widget.alignRight->setDefaultAction(tool->m_actionAlignRight);
    widget.alignCenter->setDefaultAction(tool->m_actionAlignCenter);
    widget.alignBlock->setDefaultAction(tool->m_actionAlignBlock);
    widget.superscript->setDefaultAction(tool->m_actionFormatSuper);
    widget.subscript->setDefaultAction(tool->m_actionFormatSub);
    widget.decreaseIndent->setDefaultAction(tool->m_actionFormatDecreaseIndent);
    widget.increaseIndent->setDefaultAction(tool->m_actionFormatIncreaseIndent);
}

#include <SimpleStyleWidget.moc>
