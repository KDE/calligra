/* This file is part of the KDE project
Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

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

#ifndef KEXICREATEPROJECTPAGEENGINE_H
#define KEXICREATEPROJECTPAGEENGINE_H

#include "kexicreateprojectpage.h"

class KComboBox;
class KTextBrowser;
class KexiCreateProject;

class KEXIPRJWIZARD_EXPORT KexiCreateProjectPageEngine : public KexiCreateProjectPage
{
	Q_OBJECT
	
	public:
		KexiCreateProjectPageEngine(KexiCreateProject *parent, QPixmap *wpic, const char *name=0);
		~KexiCreateProjectPageEngine();

	protected:
		void		fill();
		void		fillSummary();

		KComboBox	*m_engine;
		KTextBrowser	*m_summary;
#ifdef KEXI_ADD_EXAMPLE_ENGINES
		QStringList m_example_engines;
		int m_first_example_engine_nr; //! inx of 1st engine that is just example
#endif
	protected slots:
		void		slotActivated(int idx);
};

#endif
