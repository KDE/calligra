/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Joseph Wenninger <jowenn@kde.org>

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

#include "kexidataprovider.h"

const char *KexiDataProvider::Parameter::typeNames[]={"UNKNOWN","TEXT","FLOAT","INTEGER","DATE",
			"TIME","DATETIME"};

const char *KexiDataProvider::Parameter::typeDescription[]={I18N_NOOP("unknown"),I18N_NOOP("Text"),
			I18N_NOOP("Float (eg 1.3)"),
                        I18N_NOOP("Integer (eg 10)"),I18N_NOOP("Date (eg 01-Apr-02)"),
                        I18N_NOOP("Time (eg 13:01)"),I18N_NOOP("DateTime (eg 01-Apr-02 13:01)")};

const int KexiDataProvider::Parameter::maxType=6;

KexiDataProvider::KexiDataProvider(){;}
KexiDataProvider::~KexiDataProvider(){;}

