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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexiscripttextview.h"

#include <kexidialogbase.h>

#include <qstringlist.h>
#include <qlayout.h>
#include <qstylesheet.h>
#include <ktextbrowser.h>
//#include <kdebug.h>

/// @internal
class KexiScriptTextViewPrivate
{
    public:

        /**
         * The \a Kross::Api::ScriptAction instance which provides
         * us access to the scripting framework Kross.
         */
        Kross::Api::ScriptAction* scriptaction;

        /**
         * The textbrowser used to display feedback like the
         * stdOut and stdErr messages of the scripting code.
         */
        KTextBrowser* browser;
};

KexiScriptTextView::KexiScriptTextView(KexiMainWindow *mainWin, QWidget *parent, Kross::Api::ScriptAction* scriptaction)
    : KexiViewBase(mainWin, parent, "KexiScriptTextView")
    , d( new KexiScriptTextViewPrivate() )
{
    d->scriptaction = scriptaction;

    connect(d->scriptaction, SIGNAL( success() ), 
            this, SLOT( activateSuccess() ));
    connect(d->scriptaction, SIGNAL( failed(const QString&, const QString&) ), 
            this, SLOT( activateFailed(const QString&, const QString&) ));

    d->browser = new KTextBrowser(this, "KexiScriptTextViewEditor");
    d->browser->setReadOnly(true);
    //d->browser->setFocusPolicy(QTextBrowser::ClickFocus);
    d->browser->setTextFormat(QTextBrowser::RichText);
    //d->browser->setLinkUnderline(true);
    //d->browser->setWordWrap(QTextEdit::WidgetWidth);

    QStringList logs = d->scriptaction->getLogs();
    for(QStringList::ConstIterator it = logs.constBegin(); it != logs.constEnd(); ++it)
        d->browser->append( *it );

    QBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(d->browser);
}

KexiScriptTextView::~KexiScriptTextView()
{
}

void KexiScriptTextView::activateSuccess()
{
    d->browser->clear();
}

void KexiScriptTextView::activateFailed(const QString& errormessage, const QString& tracedetails)
{
    d->browser->clear();
    d->browser->append( QString("<b>%1</b><br>").arg( QStyleSheet::escape(errormessage) ) );
    d->browser->append( QStyleSheet::escape(tracedetails) );
    if(! hasFocus()) {
        setFocus();
    }
}

#include "kexiscripttextview.moc"
