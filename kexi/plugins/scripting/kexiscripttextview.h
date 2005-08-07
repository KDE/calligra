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

#ifndef KEXISCRIPTTEXTVIEW_H
#define KEXISCRIPTTEXTVIEW_H

#include <kexiviewbase.h>

class KexiScriptManager;
class KexiScriptContainer;
class KTextBrowser;

/**
 * The KexiScriptTextView class provides the \a KexiViewBase to
 * manage script modules in the text-view. The text-view is
 * mainly used to display scripting feedback.
 */
class KexiScriptTextView : public KexiViewBase
{
        Q_OBJECT

    public:

        /**
         * Constructor.
         */
        KexiScriptTextView(KexiScriptManager* manager, KexiMainWindow *mainWin, QWidget *parent, const char *name = 0);

        /**
         * Destructor.
         */
        virtual ~KexiScriptTextView();

    public slots:
        /// Add a string to our stdOut handler.
        void addStdOut(const QString&);
        /// Add a string to our stdErr handler.
        void addStdErr(const QString&);

    private:

        /**
         * The \a KexiScriptManager instance used to handle
         * the different \a KexiScriptContainer instances.
         */
        KexiScriptManager* m_manager;

        /**
         * The \a KexiScriptContainer instance is used to
         * wrap the \a Kross::Api::ScriptContainer functionality
         * to work with scripting code.
         */
        KexiScriptContainer* m_scriptcontainer;

        /**
         * The textbrowser used to display feedback like the
         * stdOut and stdErr messages of the scripting code.
         */
        KTextBrowser* m_browser;
};

#endif
