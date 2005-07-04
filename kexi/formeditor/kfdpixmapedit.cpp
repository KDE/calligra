/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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

#include "kfdpixmapedit.h"

#include <kdebug.h>

#include <koproperty/property.h>
#include "pixmapcollection.h"
#include "formmanager.h"
#include "form.h"
#include "objecttree.h"

using namespace KFormDesigner;

KFDPixmapEdit::KFDPixmapEdit(FormManager *manager, KoProperty::Property *property, QWidget *parent, const char *name)
 : KoProperty::PixmapEdit(property, parent, name)
{
	m_manager = manager;
}

KFDPixmapEdit::~KFDPixmapEdit()
{}

void
KFDPixmapEdit::selectPixmap()
{
	if(!m_manager->activeForm() || !property())
		return;

	ObjectTreeItem *item = m_manager->activeForm()->objectTree()->lookup(m_manager->activeForm()->selectedWidget()->name());
	QString name = item ? item->pixmapName(property()->name()) : "";
	PixmapCollectionChooser dialog( m_manager->activeForm()->pixmapCollection(), name, topLevelWidget() );
	if(dialog.exec() == QDialog::Accepted) {
		setValue(dialog.pixmap(), true);
		item->setPixmapName(property()->name(), dialog.pixmapName());
	}
}
