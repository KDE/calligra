/* This file is part of the KDE project
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIPROJECTSELECTOR_H
#define KEXIPROJECTSELECTOR_H

#include "KexiProjectSelectorBase.h"
#include "KexiProjectSet.h"

#include <kdialogbase.h>
#include <qwidgetstack.h>

class KexiNewFileDBWidget;
class KexiProjectSelectorBase;
class KexiProjectSelectorWidgetPrivate;

/*! Widget that allows to select a kexi project (or database)
*/
class KEXICORE_EXPORT KexiProjectSelectorWidget : public KexiProjectSelectorBase
{
	Q_OBJECT

public:
	enum ConnType { FileBased=1, ServerBased=2 };
    
	KexiProjectSelectorWidget( const KexiProjectSet& prj_set, QWidget* parent = 0, const char* name = 0 );
    ~KexiProjectSelectorWidget();
	
	/*! \return data of selected project.
		Returns NULL if no selection has been made.
	*/
	const KexiProjectData* selectedProjectData() const;

signals:
	void projectDoubleClicked(const KexiProjectData*);

protected slots:
	void itemDoubleClicked(QListViewItem*);

protected:
	const KexiProjectSet *m_prj_set;
	
	KexiProjectSelectorWidgetPrivate *d;
	
//	friend class KexiProjectSelectorDialog;
};

#endif

