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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef _KEXI_TABLE_IMPORTER_
#define _KEXI_TABLE_IMPORTER_

#include <qobject.h>
#include <qstringlist.h>
#include "filters/kexifilterwizardbase.h"

class QWidget;

class KexiImportWizardPrivate;
class KexiFilter;

class KexiImportWizard: public KexiFilterWizardBase
{
	Q_OBJECT
public:
	KexiImportWizard(QObject *parent, const char *name=0, const QStringList &list=QStringList());
	~KexiImportWizard();
	virtual void exec(const QString& dialogfilter, const QMap<QString,QString> mimePluginMap,unsigned long importTypes,bool modal);
	virtual void setMode(unsigned long mode);

	virtual QWidget* openWidget(QWidget *parent,const QString& dialogFilter)=0;
	virtual KexiFilter *openPageLeft(QWidget *widget, const QMap<QString,QString> mimePluginMap)=0;
private:
	KexiImportWizardPrivate *d;
signals:
};

#endif
