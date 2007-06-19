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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef TABSTOPEDIT_DIALOG_H
#define TABSTOPEDIT_DIALOG_H

#include <kdialog.h>

#include <kexi_export.h>

class Q3ListViewItem;
class QCheckBox;
class KPushButton;

namespace KFormDesigner {

class Form;
class ObjectTreeView;

//! A dialog to edit Form tab stops
/*! The user can change the order by dragging list items or using buttons at the right.
  The tab stops can be arranged automatically (see \ref Form::autoAssignTabStops()). */
class KFORMEDITOR_EXPORT TabStopDialog : public KDialog
{
	Q_OBJECT

	public:
		TabStopDialog(QWidget *parent);
		virtual ~TabStopDialog();

	public slots:
		int exec(KFormDesigner::Form *form);
		void moveItemUp();
		void moveItemDown();
		void updateButtons(Q3ListViewItem*);
		void slotRadioClicked(bool isOn);

		bool autoTabStops() const;

	protected:
		ObjectTreeView   *m_treeview;
		KPushButton *m_btnUp, *m_btnDown;
		QCheckBox *m_check;
};

}

#endif
