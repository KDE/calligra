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
#include <kaction.h>
#include <kdebug.h>
#include <klocale.h>


GCommandHistory::GCommandHistory(KAction *undo, KAction *redo) :
    m_present(0L), m_undo(undo), m_redo(redo), m_undoLimit(50),
    m_redoLimit(30), m_first(false) {

    m_undo->setEnabled(false);
    m_undo->setText(i18n("No Undo Possible"));
    m_redo->setEnabled(false);
    m_redo->setText(i18n("No Redo Possible"));
    m_commands.setAutoDelete(true);
}

GCommandHistory::~GCommandHistory() {
}

void GCommandHistory::addCommand(GCommand *command) {

    if(command==0L)
	return;

    int index;
    if(m_present!=0L && (index=m_commands.findRef(m_present))!=-1) {
	m_commands.insert(index+1, command);
	m_present=command;
	m_first=false;
	m_undo->setEnabled(true);
	m_undo->setText(i18n("Und&o: %1").arg(m_present->name()));
	if(m_commands.next()!=0) {
	    GCommand *tmp=m_commands.current();
	    m_redo->setEnabled(true);
	    m_redo->setText(i18n("Re&do: %1").arg(tmp->name()));
	}
	else {
	    if(m_redo->isEnabled()) {
		m_redo->setEnabled(false);
		m_redo->setText(i18n("No Redo Possible"));
	    }
	}	
	clipCommands();
    }
    else { // either this is the first time we add a Command or something has gone wrong
	kdDebug(37001) << "Initializing the Command History" << endl;
	m_commands.clear();
	m_commands.append(command);
	m_present=command;
	m_undo->setEnabled(true);
	m_undo->setText(i18n("Und&o: %1").arg(m_present->name()));
	m_redo->setEnabled(false);
	m_redo->setText(i18n("No Redo Possible"));
	m_first=true;
    }
}

void GCommandHistory::undo() {

    m_present->unexecute();
    m_redo->setEnabled(true);
    m_redo->setText(i18n("Re&do: %1").arg(m_present->name()));
    if(m_commands.findRef(m_present)!=-1 && m_commands.prev()!=0) {
	m_present=m_commands.current();
	m_undo->setEnabled(true);
	m_undo->setText(i18n("Und&o: %1").arg(m_present->name()));
    }
    else {
	m_undo->setEnabled(false);
	m_undo->setText(i18n("No Undo Possible"));
	m_first=true;
    }
}

void GCommandHistory::redo() {

    if(m_first) {
	m_present->execute();
	m_undo->setEnabled(true);
	m_undo->setText(i18n("Und&o: %1").arg(m_present->name()));
	m_first=false;
	m_commands.first();
	if(m_commands.next()!=0) {
	    GCommand *tmp=m_commands.current();
	    m_redo->setEnabled(true);
	    m_redo->setText(i18n("Re&do: %1").arg(tmp->name()));
	}
	else {
	    if(m_redo->isEnabled()) {
		m_redo->setEnabled(false);
		m_redo->setText(i18n("No Redo Possible"));
	    }
	}
    }

    if(m_commands.findRef(m_present)!=-1 && m_commands.next()!=0) {
	m_present=m_commands.current();
	m_present->execute();
	m_undo->setEnabled(true);
	m_undo->setText(i18n("Und&o: %1").arg(m_present->name()));	
    }

    if(m_commands.next()!=0) {
	    GCommand *tmp=m_commands.current();
	    m_redo->setEnabled(true);
	    m_redo->setText(i18n("Re&do: %1").arg(tmp->name()));
    }
    else {
	if(m_redo->isEnabled()) {
	    m_redo->setEnabled(false);
	    m_redo->setText(i18n("No Redo Possible"));
	}
    }
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

    int count=m_commands.count();
    if(count<m_undoLimit && count<m_redoLimit)
	return;

    int index=m_commands.findRef(m_present);
    if(index>=m_undoLimit) {
	for(int i=0; i<=(index-m_undoLimit); ++i)
	    m_commands.removeFirst();
	index=m_commands.findRef(m_present); // calculate the new
	count=m_commands.count();            // values (redo-branch :)
    }
    if((index+m_redoLimit)<count) {
	for(int i=0; i<(count-(index+m_redoLimit)); ++i)
	    m_commands.removeLast();
    }
}
