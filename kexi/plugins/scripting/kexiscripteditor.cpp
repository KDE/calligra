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

#ifdef KEXI_KROSS_SUPPORT
# include "main/manager.h"
# include "main/scriptcontainer.h"
# include "api/exception.h"
#endif

class KexiScriptEditorPrivate
{
    public:
        QString language;
#ifdef KEXI_KROSS_SUPPORT
        Kross::Api::Manager* manager;
        Kross::Api::ScriptContainer* scriptcontainer;
#endif
};

KexiScriptEditor::KexiScriptEditor(KexiMainWindow *mainWin, QWidget *parent, const char *name)
    : KexiEditor(mainWin, parent, name)
    , d( new KexiScriptEditorPrivate() )
{
    d->language = "python"; // default scripting language

#ifdef KEXI_KROSS_SUPPORT
    d->manager = new Kross::Api::Manager();
    d->scriptcontainer = d->manager->getScriptContainer("test");
#endif
}

KexiScriptEditor::~KexiScriptEditor()
{
#ifdef KEXI_KROSS_SUPPORT
    delete d->manager;
    delete d;
#endif
}

void KexiScriptEditor::initialize()
{
    setLanguage(d->language);

#ifdef KTEXTEDIT_BASED_SQL_EDITOR
#else
    KTextEditor::UndoInterface* u = KTextEditor::undoInterface( document() );
    u->clearUndo();
    u->clearRedo();
#endif

    setDirty(false);
    connect(this, SIGNAL(textChanged()), this, SLOT(textChanged()));
}

QString KexiScriptEditor::getLanguage()
{
    return d->language;
}

bool KexiScriptEditor::setLanguage(const QString& language)
{
    d->language = language;

#ifdef KTEXTEDIT_BASED_SQL_EDITOR
#else
    KTextEditor::HighlightingInterface *hl = KTextEditor::highlightingInterface( document() );
    for(uint i = 0; i < hl->hlModeCount(); i++) {
        //kdDebug() << "hlmode("<<i<<"): " << hl->hlModeName(i) << endl;

        // We assume Kross and the HighlightingInterface are using same
        // names for the support languages...
        if (hl->hlModeName(i).contains(d->language, false))  {
            hl->setHlMode(i);
            break;
        }
    }
#endif

    return true;
}

QString KexiScriptEditor::getCode()
{
    return KexiEditor::text();
}

bool KexiScriptEditor::setCode(const QString& text)
{
    KexiEditor::setText(text);
    return true;
}

void KexiScriptEditor::execute()
{
#ifdef KEXI_KROSS_SUPPORT
    d->scriptcontainer->setCode( getCode() );
    d->scriptcontainer->setInterpreterName( getLanguage() );

    try {
        d->scriptcontainer->execute();
    }
    catch(Kross::Api::Exception& e) {
        kdDebug() << QString("EXCEPTION type='%1' description='%2'").arg(e.type()).arg(e.description()) << endl;
    }
#endif
}

void KexiScriptEditor::textChanged()
{
    KexiScriptEditor::setDirty(true);
}

#include "kexiscripteditor.moc"

