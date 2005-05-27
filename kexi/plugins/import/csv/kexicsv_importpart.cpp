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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kexicsv_importpart.h"
#include "kexicsvdialog.h"
#include <core/keximainwindow.h>

#include <kgenericfactory.h>

KexiCSVImportPart::KexiCSVImportPart(QObject *parent, const char *name, const QStringList &args)
 : KexiInternalPart(parent, name, args)
{
}

KexiCSVImportPart::~KexiCSVImportPart()
{
}

QWidget *KexiCSVImportPart::createWidget(const char* /*widgetClass*/, KexiMainWindow* mainWin, 
 QWidget *parent, const char *objName, const QVariant& arg )
{
	KexiCSVDialog *dlg = new KexiCSVDialog( 
		arg=="file"? KexiCSVDialog::File : KexiCSVDialog::Clipboard, 
		mainWin, parent, objName );
	m_cancelled = dlg->cancelled();
	if (m_cancelled) {
		delete dlg;
		dlg = 0;
	}
	return dlg;
}

K_EXPORT_COMPONENT_FACTORY( kexihandler_csv_import, 
	KGenericFactory<KexiCSVImportPart>("kexihandler_csv_import") )
