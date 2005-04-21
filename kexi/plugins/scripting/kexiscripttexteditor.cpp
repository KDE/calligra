/* This file is part of the KDE project
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

#include "kexiscripttexteditor.h"

//#include <kdebug.h>
//#include <kparts/factory.h>
//#include <klibloader.h>
//#include <kmdimainfrm.h>
//#include <kmainwindow.h>
#include <kpopupmenu.h>

#include <kexidialogbase.h>

#ifdef KTEXTEDIT_BASED_SQL_EDITOR
#else
# include <ktexteditor/view.h>
# include <ktexteditor/highlightinginterface.h>
# include <ktexteditor/popupmenuinterface.h>
#endif

KexiScriptTextEditor::KexiScriptTextEditor(KexiMainWindow *mainWin, QWidget *parent, const char *name)
    : KexiEditor(mainWin, parent, name)
{
#ifdef KTEXTEDIT_BASED_SQL_EDITOR
#else
    // TEMP: let's use python highlighting for now
    KTextEditor::HighlightingInterface *hl = KTextEditor::highlightingInterface(document());
    for(uint i=0; i < hl->hlModeCount(); i++)  {
        //kdDebug() << "hlmode("<<i<<"): " << hl->hlModeName(i) << endl;
        if (hl->hlModeName(i).contains("python", false))  {
            hl->setHlMode(i);
            break;
        }
    }
#endif

    /// \todo plug actions here
    connect(this, SIGNAL(textChanged()), this, SLOT(setDirty()));
}

KexiScriptTextEditor::~KexiScriptTextEditor()
{
}

#include "kexiscripttexteditor.moc"
