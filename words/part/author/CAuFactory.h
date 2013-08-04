/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>

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

#ifndef CAUFACTORY_H
#define CAUFACTORY_H

#include <kpluginfactory.h>
#include <kcomponentdata.h>
#include "words_export.h"

class KAboutData;

/**
 * Factory for the Author application.
 * Will use the Author library to create a new instance of the Author doc.
 */
class WORDS_EXPORT CAuFactory : public KPluginFactory
{
    Q_OBJECT
//    Q_PLUGIN_METADATA(IID KPluginFactory_iid FILE "authorpart.json")
//    Q_INTERFACES(KPluginFactory)

public:
    /**
     * Constructor
     * @param parent the parent QObject
     */
    explicit CAuFactory(QObject *parent = 0);
    ~CAuFactory();

    /// overwritten method from superclass
    virtual QObject* create(const char* iface, QWidget* parentWidget, QObject *parent, const QVariantList& args, const QString& keyword);

    /// Return an instance
    static const KComponentData &componentData();

    /// Creates a new KAboutData instance
    static KAboutData *aboutData();

private:
    static KComponentData *s_instance;
    static KAboutData *s_aboutData;
};

#endif
