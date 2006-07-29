/* This file is part of the KDE project
   Copyright (C) 2004 Peter Simonsson <psn@linux.se>,

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
   Boston, MA 02110-1301, USA.
*/
#ifndef KIVIOTARGETTOOLFACTORY_H
#define KIVIOTARGETTOOLFACTORY_H

#include <qstringlist.h>
#include <qobject.h>

#include <klibloader.h>

class KInstance;

class KivioTargetToolFactory : public KLibFactory
{
  Q_OBJECT
  public:
    KivioTargetToolFactory(QObject *parent = 0, const char *name = 0);
    ~KivioTargetToolFactory();

    virtual QObject* createObject( QObject* parent = 0, const char* name = 0,
                            const char* classname = "QObject", const QStringList &args = QStringList() );
  
    static KInstance* global();
  
  private:
    static KInstance* s_global;
};

#endif
