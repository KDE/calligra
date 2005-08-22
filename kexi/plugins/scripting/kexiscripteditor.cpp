/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004-2005 Jaroslaw Staniek <js@iidea.pl>
   Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2005 Sebastian Sauer <mail@dipe.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kexiscripteditor.h"
#include "kexiscriptmanager.h"

#include <kdebug.h>
//#include <kparts/factory.h>
//#include <klibloader.h>
//#include <kmdimainfrm.h>
//#include <kmainwindow.h>
#include <kpopupmenu.h>

#include <kexidialogbase.h>

#ifdef KTEXTEDIT_BASED_SQL_EDITOR
#else
# include <ktexteditor/view.h>
# include <ktexteditor/document.h>
# include <ktexteditor/highlightinginterface.h>
# include <ktexteditor/popupmenuinterface.h>
# include <ktexteditor/undointerface.h>
#endif

KexiScriptEditor::KexiScriptEditor(KexiMainWindow *mainWin, QWidget *parent, const char *name)
    : KexiEditor(mainWin, parent, name)
    , m_scriptcontainer(0)
{
}

KexiScriptEditor::~KexiScriptEditor()
{
}

void KexiScriptEditor::initialize(KexiScriptContainer* scriptcontainer)
{
    disconnect(this, SIGNAL(textChanged()), this, SLOT(slotTextChanged()));

    m_scriptcontainer = scriptcontainer;
    connect(m_scriptcontainer, SIGNAL(lineno(long)), this, SLOT(setLineNo(long)));

    if(m_scriptcontainer) {
        KexiEditor::setText(m_scriptcontainer->getCode());

#ifdef KTEXTEDIT_BASED_SQL_EDITOR
#else
        KTextEditor::HighlightingInterface *hl = KTextEditor::highlightingInterface( document() );
        for(uint i = 0; i < hl->hlModeCount(); i++) {
            //kdDebug() << "hlmode("<<i<<"): " << hl->hlModeName(i) << endl;

            // We assume Kross and the HighlightingInterface are using same
            // names for the support languages...
            if (hl->hlModeName(i).contains(m_scriptcontainer->getInterpreterName(), false))  {
                hl->setHlMode(i);
                break;
            }
        }
#endif

    }
    else {
        KexiEditor::setText("");
    }

#ifdef KTEXTEDIT_BASED_SQL_EDITOR
#else
    KTextEditor::UndoInterface* u = KTextEditor::undoInterface( document() );
    u->clearUndo();
    u->clearRedo();
#endif

    KexiEditor::setDirty(false);
    connect(this, SIGNAL(textChanged()), this, SLOT(slotTextChanged()));
}

void KexiScriptEditor::slotTextChanged()
{
    KexiScriptEditor::setDirty(true);
    if(m_scriptcontainer)
        m_scriptcontainer->setCode( KexiEditor::text() );
}

void KexiScriptEditor::setLineNo(long lineno)
{
    setCursorPosition(lineno, 0);
}

#include "kexiscripteditor.moc"

