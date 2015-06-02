/* This file is part of the KDE project
   Copyright (C) 2003-2010 Jarosław Staniek <staniek@kde.org>

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

#include <QToolButton>
#include <QWidgetAction>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QMenu>

#include <kdebug.h>
#include <klocale.h>

#ifdef KexiStatusBar_KTEXTEDITOR_USED
#include <ktexteditor/viewcursorinterface.h>
#include <ktexteditor/viewstatusmsginterface.h>
#endif

// Smart menu
class Menu : public QMenu
{
public:
    explicit Menu(QWidget *parent) : QMenu(parent) {}
protected:
    void mouseReleaseEvent(QMouseEvent* e);
};

void Menu::mouseReleaseEvent(QMouseEvent* e)
{
    Q_UNUSED(e);
    if (activeAction())
        activeAction()->trigger();
}


KexiStatusBar::KexiStatusBar(QWidget *parent)
        : KStatusBar(parent)
#ifdef KexiStatusBar_KTEXTEDITOR_USED
        , m_cursorIface(0)
#endif
{
    setObjectName("KexiStatusBar");
    setContentsMargins(2, 0, 2, 0);
    int id = 0;
    m_msgID = id++;
    insertPermanentItem("", m_msgID, 1 /*stretch*/);

    m_readOnlyID = id++;
    insertPermanentItem(QString(), m_readOnlyID);
    setReadOnlyFlag(false);

// still disabled as showing hiding would be implemented as side bars
//! @todo use this view for something else
#if 0
    QToolButton *viewButton = new QToolButton(this);
    viewButton->setContentsMargins(0, 0, 0, 0);
    viewButton->setAutoRaise(true);
    viewButton->setText(xi18nc("View menu", "View"));
    viewButton->setToolTip(xi18n("Show view menu"));
    viewButton->setWhatsThis(xi18n("Shows menu with view-related options"));
    addPermanentWidget(viewButton);

//!@ todo add small close button
    m_viewMenu = new Menu(this);
    viewButton->setMenu(m_viewMenu);
    viewButton->setPopupMode(QToolButton::InstantPopup);

    m_showNavigatorAction = m_viewMenu->addAction(xi18n("Project &Navigator"));
    m_showNavigatorAction->setCheckable(true);

    m_showPropertyEditorAction = m_viewMenu->addAction(xi18n("&Property Editor"));
    m_showPropertyEditorAction->setCheckable(true);
#endif
    /// @todo remove parts from the map on PartRemoved() ?
}

KexiStatusBar::~KexiStatusBar()
{
}

void KexiStatusBar::cursorPositionChanged()
{
#ifdef KexiStatusBar_KTEXTEDITOR_USED
    if (m_cursorIface) {
        uint line, col;
        m_cursorIface->cursorPosition(&line, &col);
        setCursorPosition(line, col);
    }
#endif
}

void KexiStatusBar::setStatus(const QString &str)
{
    //kDebug() << str;
    changeItem(str, m_msgID);
}

#if 0
void KexiStatusBar::setCursorPosition(int line, int col)
{
    changeItem(futureI18n(" Line: %1 Col: %2 ", line + 1, col), m_msgID);
}
#endif

void KexiStatusBar::setReadOnlyFlag(bool readOnly)
{
    changeItem(readOnly ? xi18n("Read only") : QString(), m_readOnlyID);
}

