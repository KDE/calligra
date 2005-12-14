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

#ifndef KEXISCRIPTTEXTVIEW_H
#define KEXISCRIPTTEXTVIEW_H

#include <kross/main/scriptaction.h>

#include <kexiviewbase.h>

// Forward declarations.
class KexiScriptTextViewPrivate;

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
        KexiScriptTextView(KexiMainWindow *mainWin, QWidget *parent, Kross::Api::ScriptAction* scriptaction);

        /**
         * Destructor.
         */
        virtual ~KexiScriptTextView();

    public slots:

        /// Clear the logs.
        void clearLog();

        /// Add a string to the logs.
        void addLog(const QString&);

    private:
        /// Private d-pointer.
        KexiScriptTextViewPrivate* d;
};

#endif
