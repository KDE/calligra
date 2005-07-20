/***************************************************************************
 * kjsinterpreter.cpp
 * This file is part of the KDE project
 * copyright (C)2004-2005 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 ***************************************************************************/

#include "kjsinterpreter.h"

//#include <kjsembed/kjsembedpart.h>
//#include <kjs/object.h>

using namespace Kross::Kjs;

namespace Kross { namespace Kjs {

    //! @internal
    class KjsInterpreterPrivate
    {
        public:
            //::KJSEmbed::KJSEmbedPart* kjspart;
    };

}}

KjsInterpreter::KjsInterpreter(Kross::Api::Manager* manager, const QString& interpretername)
    : Kross::Api::Interpreter(manager, interpretername)
{
    d = new KjsInterpreterPrivate();
    //d->kjspart = new ::KJSEmbed::KJSEmbedPart(0L, "console", this, "krosskjs");
    //d->kjspart = new ::KJSEmbed::KJSEmbedPart(0L, "console", 0, "krosskjs");
}

KjsInterpreter::~KjsInterpreter()
{
    //delete d->kjspart;
    delete d;
}

const QStringList KjsInterpreter::mimeTypes()
{
    return QStringList() << "application/x-javascript";
}

Kross::Api::Script* KjsInterpreter::createScript(Kross::Api::ScriptContainer* scriptcontainer)
{
    //TODO
    return 0;
}

