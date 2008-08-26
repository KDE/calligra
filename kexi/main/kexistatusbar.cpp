/* This file is part of the KDE project
   Copyright (C) 2003 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and,or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.

   Loosely based on kdevelop/src/statusbar.cpp
   Copyright (C) 2001 by Bernd Gehrmann <bernd@kdevelop.org>
*/

#include "kexistatusbar.h"

#include <qlayout.h>
#include <qlineedit.h>
#include <qpainter.h>
#include <qtimer.h>
#include <qfontmetrics.h>

#include <kdebug.h>
#include <kglobalsettings.h>
#include <klocale.h>
#include <kparts/part.h>

#if KexiStatusBar_KTEXTEDITOR_USED
#include <ktexteditor/viewcursorinterface.h>
#include <ktexteditor/viewstatusmsginterface.h>
#endif

KexiStatusBar::KexiStatusBar(QWidget *parent)
        : KStatusBar(parent)
#if KexiStatusBar_KTEXTEDITOR_USED
        , m_cursorIface(0)
#endif
        , m_activePart(0)
{
    setObjectName("KexiStatusBar");
    int id = 0;
    m_msgID = id++;
    insertPermanentItem("", m_msgID, 1 /*stretch*/);

    m_readOnlyID = id++;
    insertPermanentItem(i18n("Read only"), m_readOnlyID);
    setReadOnlyFlag(false);

// @todo
// connect(PartController::getInstance(), SIGNAL(activePartChanged(KParts::Part*)),
//  this, SLOT(activePartChanged(KParts::Part*)));

    /// @todo remove parts from the map on PartRemoved() ?
}


KexiStatusBar::~KexiStatusBar()
{
}

void KexiStatusBar::activePartChanged(KParts::Part *part)
{
    if (m_activePart && m_activePart->widget())
        disconnect(m_activePart->widget(), 0, this, 0);

    m_activePart = part;
#if KexiStatusBar_KTEXTEDITOR_USED
    m_cursorIface = 0;
    m_viewmsgIface = 0;
// @todo
    if (part && part->widget()) {
        if ((m_viewmsgIface = dynamic_cast<KTextEditor::ViewStatusMsgInterface*>(part->widget()))) {
            connect(part->widget(), SIGNAL(viewStatusMsg(const QString &)),
                    this, SLOT(setStatus(const QString &)));
        } else if ((m_cursorIface = dynamic_cast<KTextEditor::ViewCursorInterface*>(part->widget()))) {
            connect(part->widget(), SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));
            cursorPositionChanged();
        } else {
            // we can't produce any status data, hide the status box
            changeItem("", m_msgID);
        }
    }
#endif
}


void KexiStatusBar::cursorPositionChanged()
{
#if KexiStatusBar_KTEXTEDITOR_USED
    if (m_cursorIface) {
        uint line, col;
        m_cursorIface->cursorPosition(&line, &col);
        setCursorPosition(line, col);
    }
#endif
}

void KexiStatusBar::setStatus(const QString &str)
{
    kDebug() << "KexiStatusBar::setStatus(" << str << ")";
// m_status->setText(str);
    changeItem(str, m_msgID);
}

void KexiStatusBar::setCursorPosition(int line, int col)
{
// m_status->setText(i18n(" Line: %1 Col: %2 ").arg(line+1).arg(col));
    changeItem(i18n(" Line: %1 Col: %2 ", line + 1, col), m_msgID);
}

/*void KexiStatusBar::addWidget ( QWidget *widget, int stretch, bool permanent)
{
  KStatusBar::addWidget(widget,stretch,permanent);

  if(widget->sizeHint().height() + 4 > height())
    setFixedHeight(widget->sizeHint().height() + 4);
}*/

void KexiStatusBar::setReadOnlyFlag(bool readOnly)
{
    changeItem(readOnly ? i18n("Read only") : QString(), m_readOnlyID);
}

#include "kexistatusbar.moc"
