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

#ifndef _KEXIFILTERWIZARDBASE_
#define _KEXIFILTERWIZARDBASE_

#include <qobject.h>
#include <kurl.h>

class KexiFilterManager;


#define KEXIFILTERMANAGER(obj) (static_cast<KexiFilterManager*>(obj->qt_cast("KexiFilterManager")))

/* this class has to delete itself after usage */
class KEXIFILTER_EXPORT KexiFilterWizardBase:public QObject {

Q_OBJECT
public:
	KexiFilterWizardBase(KexiFilterManager *parent, const char *name, const QStringList &);
	virtual ~KexiFilterWizardBase();
	virtual void exec(const QString& dialogfilter, const QMap<QString,QString> mimePluginMap,unsigned long importTypes,bool modal)=0;
	KexiFilterManager *filterManager();
	/* standard modes. Extended modes will later be plugin based, if needed at all */
	virtual void setMode(unsigned long);
private:
	KexiFilterManager *m_filterManager;
};


#endif
