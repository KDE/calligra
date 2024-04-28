/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "SimpleEntryWidget.h"
#include "../SimpleEntryTool.h"

#include <QAction>

SimpleEntryWidget::SimpleEntryWidget(SimpleEntryTool *tool, QWidget *parent)
    : QWidget(parent)
    , m_tool(tool)
{
    widget.setupUi(this);

    widget.addBarsButton->setDefaultAction(tool->action("add_bars"));
    widget.eraserButton->setDefaultAction(tool->action("eraser"));
    widget.dotButton->setDefaultAction(tool->action("dots"));
    widget.tiedNoteButton->setDefaultAction(tool->action("tiednote"));
    //    widget.selectButton->setDefaultAction(tool->action("select"));

    widget.breveNoteButton->setDefaultAction(tool->action("note_breve"));
    widget.wholeNoteButton->setDefaultAction(tool->action("note_whole"));
    widget.halfNoteButton->setDefaultAction(tool->action("note_half"));
    widget.quarterNoteButton->setDefaultAction(tool->action("note_quarter"));
    widget.note8Button->setDefaultAction(tool->action("note_eighth"));
    widget.note16Button->setDefaultAction(tool->action("note_16th"));
    widget.note32Button->setDefaultAction(tool->action("note_32nd"));
    widget.note64Button->setDefaultAction(tool->action("note_64th"));
    widget.note128Button->setDefaultAction(tool->action("note_128th"));

    widget.breveRestButton->setDefaultAction(tool->action("rest_breve"));
    widget.wholeRestButton->setDefaultAction(tool->action("rest_whole"));
    widget.halfRestButton->setDefaultAction(tool->action("rest_half"));
    widget.quarterRestButton->setDefaultAction(tool->action("rest_quarter"));
    widget.rest8Button->setDefaultAction(tool->action("rest_eighth"));
    widget.rest16Button->setDefaultAction(tool->action("rest_16th"));
    widget.rest32Button->setDefaultAction(tool->action("rest_32nd"));
    widget.rest64Button->setDefaultAction(tool->action("rest_64th"));
    widget.rest128Button->setDefaultAction(tool->action("rest_128th"));

    widget.doubleFlatButton->setDefaultAction(tool->action("accidental_doubleflat"));
    widget.flatButton->setDefaultAction(tool->action("accidental_flat"));
    widget.naturalButton->setDefaultAction(tool->action("accidental_natural"));
    widget.sharpButton->setDefaultAction(tool->action("accidental_sharp"));
    widget.doubleSharpButton->setDefaultAction(tool->action("accidental_doublesharp"));

    widget.importButton->setDefaultAction(tool->action("import"));
    widget.exportButton->setDefaultAction(tool->action("export"));

    connect(widget.voiceList, &QComboBox::currentIndexChanged, this, &SimpleEntryWidget::voiceChanged);
}

void SimpleEntryWidget::setVoiceListEnabled(bool enabled)
{
    widget.voiceList->setEnabled(enabled);
}
