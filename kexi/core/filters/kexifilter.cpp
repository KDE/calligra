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
#include "kexifilter.h"
#include "kexifilterwizardbase.h"
#include <kdebug.h>

KexiFilter::KexiFilter(KexiFilterWizardBase *parent, const char *name, const QStringList &):
	QObject(parent),m_filterWizard(KEXIFILTERWIZARDBASE(parent)) {

	Q_ASSERT(m_filterWizard!=0);
}

KexiFilter::~KexiFilter()
{
}

KexiFilterWizardBase *KexiFilter::filterWizard()
{
	return m_filterWizard;
}

void KexiFilter::widgetActivated(QWidget *current, QWidget *former) {}

#include "kexifilter.moc"
