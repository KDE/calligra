/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qlayout.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qlabel.h>


#include "propertyeditoritem.h"
#include "eventeditoritem.h"
#include "eventeditoreditor.h"
#include "eventdetail.h"
#include "eventeditor.h"
#include "eventbuffer.h"

EventEditorEditor::EventEditorEditor(QWidget *parent, PropertyEditorItem *i, const char *name)
 : PropertyEditorEditor(parent, QVariant::BitArray, QVariant(i->value()), name)
{
	QPushButton *btnCreate = new QPushButton("...", this);
	connect(btnCreate, SIGNAL(clicked()), this, SLOT(slotShowDetails()));

	m_item = static_cast<EventEditorItem *>(i);
	m_editor = m_item->parent();

	m_ed = 0;
	m_faked = false;

	setWidget(btnCreate);
}

void
EventEditorEditor::slotShowDetails()
{
	m_ed = new EventDetail(this);

	//preparing EventDetailDialog (a bit hacky)
	connect(m_ed->handler, SIGNAL(activated(const QString &)), this,
	 SLOT(slotHandlerChanged(const QString &)));
	m_ed->event->setText(QString("<qt><b>") + m_item->sender()->name() + "." + m_item->event());

	Fakes f1 = m_editor->fakes();
	for(Fakes::Iterator it = f1.begin(); it != f1.end(); ++it)
	{
		m_ed->handler->insertItem(it.key());
	}

	int accepted = m_ed->exec();
	if(accepted == QDialog::Accepted)
	{
		m_result = QVariant(m_ed->handler->currentText() + "." + m_ed->function->currentText());
		if(m_faked && m_editor->fakes().contains(m_ed->handler->currentText()))
		{
			FakeHandler *f = m_editor->fakes().find(m_ed->handler->currentText()).data();
			f->callConnect(m_item->sender(), m_item->event(), m_ed->function->currentText());
//			EventBufferItem *eb = new EventBufferItem(m_item->sender()->name(),
//			 m_ed->handler->currentText(), m_item->event(), m_ed->function->currentText());
//			m_item->parent()->buffer()->insertEvent(eb);
		}
	}
}

void
EventEditorEditor::slotHandlerChanged(const QString &h)
{
	if(!m_ed)
		return;

	if(m_editor->fakes().contains(h))
	{
		FakeHandler *f = m_editor->fakes().find(h).data();
		m_ed->function->insertStringList(f->handlers());

		m_faked = true;
	}
}

QVariant
EventEditorEditor::getValue()
{
	return m_result;
}

EventEditorEditor::~EventEditorEditor()
{
}

#include "eventeditoreditor.moc"
