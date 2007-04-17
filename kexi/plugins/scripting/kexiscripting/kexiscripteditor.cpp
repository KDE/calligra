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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexiscripteditor.h"

#include <kross/main/scriptaction.h>

#include <kdebug.h>
//#include <kparts/factory.h>
//#include <klibloader.h>
//#include <k3mdimainfrm.h>
//#include <kxmlguiwindow.h>
#include <kmenu.h>

#include <kexidialogbase.h>

/// \internal d-pointer class
class KexiScriptEditor::Private
{
    public:
        Kross::Api::ScriptAction* scriptaction;
        Private() : scriptaction(0) {}
};

KexiScriptEditor::KexiScriptEditor(KexiMainWindow *mainWin, QWidget *parent, const char *name)
    : KexiEditor(mainWin, parent, name)
    , d( new Private() )
{
}

KexiScriptEditor::~KexiScriptEditor()
{
    delete d;
}

bool KexiScriptEditor::isInitialized() const
{
    return d->scriptaction != 0;
}

void KexiScriptEditor::initialize(Kross::Api::ScriptAction* scriptaction)
{
    d->scriptaction = scriptaction;
    Q_ASSERT(d->scriptaction);

    disconnect(this, SIGNAL(textChanged()), this, SLOT(slotTextChanged()));

    QString code = d->scriptaction->getCode();
    if(code.isNull()) {
        // If there is no code we just add some information.
///@todo remove after release
        code = "# " + QStringList::split("\n", i18n(
            "This note will appear for a user in the script's source code "
            "as a comment. Keep every row not longer than 60 characters and use '\n.'",

            "This is Technology Preview (BETA) version of scripting\n"
            "support in Kexi. The scripting API may change in details\n"
            "in the next Kexi version.\n"
            "For more information and documentation see\n%1"
        ).arg("http://www.kexi-project.org/scripting/"), true).join("\n# ") + "\n";
    }
    KexiEditor::setText(code);
    // We assume Kross and the HighlightingInterface are using same
    // names for the support languages...
    setHighlightMode(d->scriptaction->getInterpreterName());

    clearUndoRedo();
    KexiEditor::setDirty(false);
    connect(this, SIGNAL(textChanged()), this, SLOT(slotTextChanged()));
}

void KexiScriptEditor::slotTextChanged()
{
    KexiScriptEditor::setDirty(true);
    if(d->scriptaction)
        d->scriptaction->setCode( KexiEditor::text() );
}

void KexiScriptEditor::setLineNo(long lineno)
{
    setCursorPosition(lineno, 0);
}

#include "kexiscripteditor.moc"

