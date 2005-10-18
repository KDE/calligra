/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexicustompropertyfactory_p.h"

#include <kdebug.h>

#include <koproperty/property.h>

using namespace KoProperty;

KexiImagePropertyEdit::KexiImagePropertyEdit(
	Property *property, QWidget *parent, const char *name)
 : PixmapEdit(property, parent, name)
{
}

KexiImagePropertyEdit::~KexiImagePropertyEdit()
{
}

void KexiImagePropertyEdit::selectPixmap()
{
	PixmapEdit::selectPixmap();
#if 0 //will be reenabled for new image collection
	if(!m_manager->activeForm() || !property())
		return;

	ObjectTreeItem *item = m_manager->activeForm()->objectTree()->lookup(m_manager->activeForm()->selectedWidget()->name());
	QString name = item ? item->pixmapName(property()->name()) : "";
	PixmapCollectionChooser dialog( m_manager->activeForm()->pixmapCollection(), name, topLevelWidget() );
	if(dialog.exec() == QDialog::Accepted) {
		setValue(dialog.pixmap(), true);
		item->setPixmapName(property()->name(), dialog.pixmapName());
	}
#endif
}

QVariant KexiImagePropertyEdit::value() const
{
	return QVariant();
}

void KexiImagePropertyEdit::setValue(const QVariant &value, bool emitChange)
{
}

#include "kexicustompropertyfactory_p.moc"
