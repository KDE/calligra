/* This file is part of the KDE project
   Copyright (C) 2002 Joseph Wenninger <jowenn@kde.org>

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

#ifndef KEXIADDPARAMDIALOG_H
#define KEXIADDPARAMDIALOG_H

#include <kdialogbase.h>

class KexiAddParamWidget;

class KEXI_HAND_QUERY_EXPORT KexiAddParamDialog : public KDialogBase
{
	Q_OBJECT
	
	public:
		KexiAddParamDialog(QWidget *parent);
		virtual ~KexiAddParamDialog();
		QString parameterName();
		int parameterType();
	private:
		KexiAddParamWidget *m_wid;
}; 

#endif
