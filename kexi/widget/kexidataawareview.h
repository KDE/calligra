/* This file is part of the KDE project
   Copyright (C) 2005-2006 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIDATAAWAREVIEW_H
#define KEXIDATAAWAREVIEW_H

#include <kexiviewbase.h>

class KexiDataAwareObjectInterface;
class KexiSharedActionClient;

/*! @short Provides a view displaying record-based data.

 The KexiDataAwareView is used to implement differently-looking views 
 for displaying record-based data in a consistent way:
 - tabular data views
 - form data view

 Action implementations like data editing and deleting are shared for different
 view types to keep even better consistency.
*/
class KEXIEXTWIDGETS_EXPORT KexiDataAwareView : public KexiViewBase
{
	Q_OBJECT

	public:
		KexiDataAwareView(KexiMainWindow *mainWin, QWidget *parent, const char *name = 0);

		QWidget* mainWidget();

		virtual QSize minimumSizeHint() const;
		virtual QSize sizeHint() const;
		KexiDataAwareObjectInterface* dataAwareObject() const { return m_dataAwareObject; }

	public slots:
		void deleteAllRows();
		void deleteCurrentRow();
		void deleteAndStartEditCurrentCell();
		void startEditOrToggleValue();
		bool acceptRowEdit();
		void cancelRowEdit();
		void sortAscending();
		void sortDescending();
		void copySelection();
		void cutSelection();
		void paste();
		void slotGoToFirstRow();
		void slotGoToPreviusRow();
		void slotGoToNextRow();
		void slotGoToLastRow();
		void slotGoToNewRow();

	protected slots:
//		void slotCellSelected(const QVariant& v); //!< @internal
		void slotCellSelected(int col, int row);
		void reloadActions();
		void slotUpdateRowActions(int row);
		void slotClosing(bool& cancel);

	protected:
		void init( QWidget* viewWidget, KexiSharedActionClient* actionClient,
			KexiDataAwareObjectInterface* dataAwareObject, 
		// temporary, for KexiFormView in design mode 
			bool noDataAware = false
		);
		void initActions();
		virtual void updateActions(bool activated);

		//KexiViewBase
		QWidget* m_internalView;
		KexiSharedActionClient* m_actionClient;
		KexiDataAwareObjectInterface* m_dataAwareObject;
};

#endif
