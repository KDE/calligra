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

#ifndef _KEXIFILTER_
#define _KEXIFILTER_

#include <qobject.h>
#include <kurl.h>
#include <qptrlist.h>

class KexiFilterWizardBase;


#define KEXIFILTERWIZARDBASE(obj) (static_cast<KexiFilterWizardBase*>(obj->qt_cast("KexiFilterWizardBase")))

class KEXIFILTER_EXPORT KexiFilter: public QObject {

	Q_OBJECT

public:
	KexiFilter(KexiFilterWizardBase *parent, const char *name, const QStringList &);
	virtual ~KexiFilter();
	KexiFilterWizardBase *filterWizard();

	virtual bool prepareImport(unsigned long type, const KURL& url=KURL())=0;

	virtual unsigned long supportedTypes()=0;

        virtual QPtrList<QWidget> sourceWidgets(QWidget *parent)=0;
        virtual bool pageChanging(QWidget* from, QWidget *to)=0;
	
        void setPageBefore(QWidget *);
        void setPageAfter(QWidget *);
        QWidget *pageBefore();
        QWidget *pageAfter();

private:
	KexiFilterWizardBase *m_filterWizard;
	class KexiFilterPrivate *d;
};


#endif
