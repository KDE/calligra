/* This file is part of the KDE project
   Copyright (C) 2000 Werner Trobin <trobin@kde.org>

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

// This class creates all the gobjects from XML via the instantiate
// method from GObject. (prototype pattern) There is only *one*
// GObjectFactory for the whole application (singleton)

#ifndef gobjectfactory_h
#define gobjectfactory_h

#include <qdict.h>

class GObject;
class QDomElement;


class GObjectFactory {

public:
    static GObjectFactory *self();

    void registerPrototype(const QString &classname, const GObject *prototype);
    void unregisterPrototype(const QString &classname);

    GObject *create(const QString &classname);  // create a new object
    GObject *create(const QDomElement &element); // create & initialize!

private:
    GObjectFactory();

    QDict<const GObject> m_registry;
    static GObjectFactory *m_self;
};

#endif // gobjectfactory_h
