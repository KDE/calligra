/* This file is part of the KDE project
   Copyright (C) 2000 Werner Trobin <wtrobin@mandrakesoft.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <gcommand.h>
//#include <kdebug.h>

GCommandHistory::GCommandHistory(KAction *undo, KAction *redo) :
    m_undo(undo), m_redo(redo), m_undoLimit(50), m_redoLimit(30) {
}

GCommandHistory::~GCommandHistory() {
}

void GCommandHistory::addCommand(GCommand */*command*/) {
    // TODO
}

void GCommandHistory::undo() {
    // TODO
}

void GCommandHistory::redo() {
    // TODO
}

void GCommandHistory::setUndoLimit(const int &limit) {

    if(limit>0 && limit!=m_undoLimit) {
	m_undoLimit=limit;
	clipCommands();
    }
}

void GCommandHistory::setRedoLimit(const int &limit) {

    if(limit>0 && limit!=m_redoLimit) {
	m_redoLimit=limit;
	clipCommands();
    }
}

void GCommandHistory::clipCommands() {
    // TODO
}
