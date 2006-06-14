/* This file is part of the KDE project
   Copyright (C) 2003 Robert JACOLIN <rjacolin@ifrance.com>
   Copyright (C) 2006 Fredrik Edemar <f_edemar@linux.se>

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

#ifndef __LATEXEXPORTADAPTOR_H__
#define __LATEXEXPORTADAPTOR_H__

#include <QtCore/QObject>
#include <dbus/qdbus.h>

class KWordLatexExportDia;

class LatexExportAdaptor: public QDBusAbstractAdaptor
{
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", "org.kde.koffice.filter.kword.latex")

	public:
    LatexExportAdaptor(KWordLatexExportDia* dia);
		
    ~LatexExportAdaptor();

public Q_SLOTS: // METHODS
	Q_SCRIPTABLE void useDefaultConfig();

	private:
		KWordLatexExportDia* _dialog;
};

#endif /* __LATEXEXPORTADAPTOR_H__ */
