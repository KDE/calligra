/* This file is part of the KDE project
   Copyright (C) 2006 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexidbutils.h"

#include <kpopupmenu.h>
#include <kiconloader.h>

#include <kexidb/queryschema.h>
#include <formeditor/widgetlibrary.h>
#include <kexiutils/utils.h>
#include "../kexiformpart.h"

QColor lighterGrayBackgroundColor(const QPalette& palette)
{
	return KexiUtils::blendedColors(palette.active().background(), palette.active().base(), 1, 2);
}

//static
bool KexiDBWidgetContextMenuExtender::updateContextMenuTitleForDataItem(QPopupMenu *menu, KexiDataItemInterface* iface)
{
	if (!menu)
		return false;
	QString title( iface->columnInfo() ? iface->columnInfo()->captionOrAliasOrName() : QString::null );

	if (title.isEmpty())
		return false;

	/*! @todo look at makeFirstCharacterUpperCaseInCaptions setting [bool]
	 (see doc/dev/settings.txt) */
	title = title[0].upper() + title.mid(1);

	const int id = menu->idAt(0);
	if (dynamic_cast<QWidget*>(iface)) {
		QPixmap icon(SmallIcon( KexiFormPart::library()->iconName(dynamic_cast<QWidget*>(iface)->className()) ));
		QMenuItem *item = menu->findItem(id);
		if (item && dynamic_cast<KPopupTitle *>(item->widget()))
			dynamic_cast<KPopupTitle *>(item->widget())->setTitle(title, &icon);
	}
	else {
		QMenuItem *item = menu->findItem(id);
		if (item && dynamic_cast<KPopupTitle *>(item->widget()))
			dynamic_cast<KPopupTitle *>(item->widget())->setTitle(title);
	}
	return true;
}

//-------

KexiDBWidgetContextMenuExtender::KexiDBWidgetContextMenuExtender( QObject* parent, KexiDataItemInterface* iface )
 : QObject(parent)
 , m_iface(iface)
 , m_contextMenuHasTitle(false)
{
}

KexiDBWidgetContextMenuExtender::~KexiDBWidgetContextMenuExtender()
{
}

void KexiDBWidgetContextMenuExtender::createTitle(QPopupMenu *menu)
{
	if (!menu)
		return;
	m_contextMenu = menu;
	KPopupTitle *titleItem = new KPopupTitle();
	const int id = m_contextMenu->insertItem(titleItem, -1, 0);
	m_contextMenu->setItemEnabled(id, false);
	m_contextMenuHasTitle = updateContextMenuTitleForDataItem(m_contextMenu, m_iface);
	if (!m_contextMenuHasTitle)
		m_contextMenu->removeItem(id);
	updatePopupMenuActions();
}

void KexiDBWidgetContextMenuExtender::updatePopupMenuActions()
{
	if (m_contextMenu) {
		enum { IdUndo, IdRedo, IdSep1, IdCut, IdCopy, IdPaste, IdClear, IdSep2, IdSelectAll }; //from qlineedit.h
		const bool readOnly = m_iface->isReadOnly();
		const int id = m_contextMenu->idAt(m_contextMenuHasTitle ? 1 : 0);
		m_contextMenu->setItemEnabled(id-(int)IdCut, !readOnly);
		m_contextMenu->setItemEnabled(id-(int)IdPaste, !readOnly);
		m_contextMenu->setItemEnabled(id-(int)IdClear, !readOnly);
	}
}

//------------------

KexiSubwidgetInterface::KexiSubwidgetInterface()
{
}

KexiSubwidgetInterface::~KexiSubwidgetInterface()
{
}
