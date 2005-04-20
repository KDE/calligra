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

#ifndef KEXISCRIPTTEXTEDITOR_H
#define KEXISCRIPTTEXTEDITOR_H

#include "kexieditor.h"

/**
 * From \a KexiEditor inherited class to implement the
 * texteditor widget to edit the scripting code.
 */
class KexiScriptTextEditor : public KexiEditor
{
        Q_OBJECT

    public:

        /**
         * Constructor.
         */
        KexiScriptTextEditor(KexiMainWindow *mainWin, QWidget *parent, const char *name = 0);

        /**
         * Destructor.
         */
        virtual ~KexiScriptTextEditor();
};

#endif
