/* This file is part of the KDE project
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>

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

#ifndef _KEXI_FILE_IMPORTER_
#define _KEXI_FILE_IMPORTER_

#include <qobject.h>
#include <qstringlist.h>
#include <kexiimportwizard.h>
#include <filters/kexifilter.h>
#include <filters/kexifilterwizardbase.h>

class KexiFileImportWizard: public KexiImportWizard
{
	Q_OBJECT
public:
	KexiFileImportWizard(QObject *parent, const char *name=0, const QStringList &list=QStringList());
	~KexiFileImportWizard();
	virtual QWidget* openWidget(QWidget *parent,const QString& dialogFilter);
	virtual KexiFilter *openPageLeft(QWidget *widget, const QMap<QString,QString> mimePluginMap);
};

#endif
