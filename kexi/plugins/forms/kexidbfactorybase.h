/* This file is part of the KDE project
   Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXIDBFACTORYBASE_H
#define KEXIDBFACTORYBASE_H

#include <formeditor/widgetfactory.h>

class KAction;

namespace KFormDesigner
{
class Form;
}

//! Kexi Base Factory
/*! Provides a number of default features for Kexi widget factories. */
class KEXIFORMUTILS_EXPORT KexiDBFactoryBase : public KFormDesigner::WidgetFactory
{
    Q_OBJECT

public:
    KexiDBFactoryBase(QObject *parent, const char *name);
    virtual ~KexiDBFactoryBase();

protected:
    virtual bool isPropertyVisibleInternal(const QByteArray& classname, QWidget *w,
                                           const QByteArray& property, bool isTopLevel);
};

#endif
