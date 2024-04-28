/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "SimpleFootEndNotesWidget.h"
#include "FormattingButton.h"
#include "TextTool.h"

#include <KoIcon.h>
#include <KoInlineNote.h>

#include <QAction>
#include <QDebug>

#include <QWidget>

SimpleFootEndNotesWidget::SimpleFootEndNotesWidget(TextTool *tool, QWidget *parent)
    : QWidget(parent)
{
    widget.setupUi(this);
    widget.addFootnote->addAction(tool->action("insert_autofootnote"));
    widget.addFootnote->addAction(tool->action("insert_labeledfootnote"));
    widget.addFootnote->addAction(tool->action("format_footnotes"));
    widget.addFootnote->setIcon(koIcon("insert-footnote"));
    widget.addFootnote->setToolTip(i18n("Inserts a footnote at the current cursor position"));
    widget.addEndnote->addAction(tool->action("insert_autoendnote"));
    widget.addEndnote->addAction(tool->action("insert_labeledendnote"));
    widget.addEndnote->addAction(tool->action("format_endnotes"));
    widget.addEndnote->setIcon(koIcon("insert-endnote"));
    widget.addEndnote->setToolTip(i18n("Inserts an endnote at the current cursor position"));

    connect(widget.addFootnote, &FormattingButton::doneWithFocus, this, &SimpleFootEndNotesWidget::doneWithFocus);
    connect(widget.addEndnote, &FormattingButton::doneWithFocus, this, &SimpleFootEndNotesWidget::doneWithFocus);
}
