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

#include "kexifilterwizardbase.h"
#include "kexifiltermanager.h"


KexiFilterWizardBase::KexiFilterWizardBase(KexiFilterManager *parent,const char *name,const QStringList &)
		:QObject(parent,name),m_filterManager(parent){
	
	Q_ASSERT(m_filterManager!=0);
}

KexiFilterWizardBase::~KexiFilterWizardBase() {
}

KexiFilterManager *KexiFilterWizardBase::filterManager() {
	return m_filterManager;
}

void KexiFilterWizardBase::setMode(unsigned long) { }

#include "kexifilterwizardbase.moc"
