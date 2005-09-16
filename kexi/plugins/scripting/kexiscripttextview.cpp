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

#include "kexiscripttextview.h"

#include <kexidialogbase.h>
#include <kexiscripting.h>

#include <qstringlist.h>
#include <qlayout.h>
//#include <kdebug.h>
#include <ktextbrowser.h>

/// @internal
class KexiScriptTextViewPrivate
{
    public:
        /**
         * The \a KexiScriptContainer instance is used to
         * wrap the \a Kross::Api::ScriptContainer functionality
         * to work with scripting code.
         */
        KexiScriptContainer* scriptcontainer;

        /**
         * The textbrowser used to display feedback like the
         * stdOut and stdErr messages of the scripting code.
         */
        KTextBrowser* browser;
};

KexiScriptTextView::KexiScriptTextView(KexiMainWindow *mainWin, QWidget *parent, const char *name)
    : KexiViewBase(mainWin, parent, name)
    , d( new KexiScriptTextViewPrivate() )
{
    d->browser = new KTextBrowser(this, "KexiScriptTextViewEditor");
    d->browser->setReadOnly(true);
    //d->browser->setFocusPolicy(QTextBrowser::ClickFocus);
    d->browser->setTextFormat(QTextBrowser::RichText);
    //d->browser->setLinkUnderline(true);
    //d->browser->setWordWrap(QTextEdit::WidgetWidth);

    QBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(d->browser);

    d->scriptcontainer = Kexi::scriptManager(mainWin)->getScriptContainer(parentDialog()->partItem()->name(), true);
    //plugSharedAction( "script_execute", scriptcontainer, SLOT(execute()) );

    QStringList output = d->scriptcontainer->getOutput();
    for (QStringList::ConstIterator it=output.constBegin(); it!=output.constEnd(); ++it)
        d->browser->append( *it );

    connect(d->scriptcontainer, SIGNAL(clearOutput()), this, SLOT(clearLog()));
    connect(d->scriptcontainer, SIGNAL(addOutput(const QString&)), this, SLOT(addLog(const QString&)));
}

KexiScriptTextView::~KexiScriptTextView()
{
    /*
    disconnect(d->scriptcontainer, SIGNAL(stdOut(const QString&)),
            this, SLOT(addStdOut(const QString&)));
    disconnect(d->scriptcontainer, SIGNAL(stdErr(const QString&)),
            this, SLOT(addStdErr(const QString&)));
    */
}

void KexiScriptTextView::clearLog()
{
    d->browser->clear();
}

void KexiScriptTextView::addLog(const QString& message)
{
    d->browser->append( message );
    //if(! hasFocus()) setFocus();
}

#include "kexiscripttextview.moc"
