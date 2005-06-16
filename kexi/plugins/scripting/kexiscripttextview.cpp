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
#include "kexiscriptmanager.h"

#include <kexidialogbase.h>

#include <qlayout.h>
//#include <kdebug.h>
#include <ktextbrowser.h>

KexiScriptTextView::KexiScriptTextView(KexiScriptManager* manager, KexiMainWindow *mainWin, QWidget *parent, const char *name)
    : KexiViewBase(mainWin, parent, name)
    , m_manager(manager)
{
    m_browser = new KTextBrowser(this, "KexiScriptTextViewEditor");
    m_browser->setReadOnly(true);
    //m_browser->setFocusPolicy(QTextBrowser::ClickFocus);
    m_browser->setTextFormat(QTextBrowser::RichText);
    //m_browser->setLinkUnderline(true);
    //m_browser->setWordWrap(QTextEdit::WidgetWidth);

    QBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_browser);

    m_scriptcontainer = m_manager->getScriptContainer( parentDialog()->partItem()->name() );
    //plugSharedAction( "script_execute", m_scriptcontainer, SLOT(execute()) );

    connect(m_scriptcontainer, SIGNAL(stdOut(const QString&)),
            this, SLOT(addStdOut(const QString&)));
    connect(m_scriptcontainer, SIGNAL(stdErr(const QString&)),
            this, SLOT(addStdErr(const QString&)));
}

KexiScriptTextView::~KexiScriptTextView()
{
}

void KexiScriptTextView::addStdOut(const QString& message)
{
    m_browser->append( message );
}

void KexiScriptTextView::addStdErr(const QString& message)
{
    m_browser->append( QString("<b>%1</b>").arg(message) );
    //if(! hasFocus()) setFocus();
}

#include "kexiscripttextview.moc"
