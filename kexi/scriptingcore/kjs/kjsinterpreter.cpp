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
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 ***************************************************************************/

#include "kjsinterpreter.h"

//#include <kjsembed/kjsembedpart.h>
//#include <kjs/object.h>

using namespace Kross::Kjs;

namespace Kross { namespace Kjs {

    /// @internal
    class KjsInterpreterPrivate
    {
        public:
            //::KJSEmbed::KJSEmbedPart* kjspart;
    };

}}

KjsInterpreter::KjsInterpreter(Kross::Api::InterpreterInfo* info)
    : Kross::Api::Interpreter(info)
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

Kross::Api::Script* KjsInterpreter::createScript(Kross::Api::ScriptContainer* scriptcontainer)
{
    //application/x-javascript

    //TODO
    return 0;
}

