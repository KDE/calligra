/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>

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

#ifndef KEXIDSSOURCE_H
#define KEXIDSSOURCE_H

#include <qwidget.h>

class KexiMainWindow;
class KexiDataSourceWizard;
class KexiDataSourceFields;

/**
 * this page is part of @ref KexiDataSourceWizard
 * it provides a combo box to choose (@ref KexiDataSourceCombo)
 * the datasource and fields that should be used in the produced
 * document (@ref KexiDataSourceFields)
 */
class KexiDSSource : public QWidget
{
	Q_OBJECT

	public:
		KexiDSSource(KexiMainWindow *win, KexiDataSourceWizard *parent);
		~KexiDSSource();

	protected slots:
		void fieldsChanged();

	private:
		KexiDataSourceWizard *m_wiz;
		KexiDataSourceFields *m_fields;
};

#endif

