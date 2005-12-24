/***************************************************************************
 * testplugin.cpp
 * This file is part of the KDE project
 * copyright (C)2005 by Sebastian Sauer (mail@dipe.org)
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

#include "testplugin.h"

#include <kdebug.h>

/************************************************************************
 * TestPluginObject
 */

TestPluginObject::TestPluginObject(const QString& name)
    : Kross::Api::Class<TestPluginObject>(name)
{
    addFunction("func1", &TestPluginObject::func1);
    addFunction("func2", &TestPluginObject::func2,
        Kross::Api::ArgumentList()
            << Kross::Api::Argument("Kross::Api::Variant::String")
            << Kross::Api::Argument("Kross::Api::Variant::String") );
    addFunction("func3", &TestPluginObject::func3,
        Kross::Api::ArgumentList()
            << Kross::Api::Argument("Kross::Api::Variant::String")
            << Kross::Api::Argument("Kross::Api::Variant::String") );
    addFunction("func4", &TestPluginObject::func4,
        Kross::Api::ArgumentList()
            << Kross::Api::Argument("Kross::Api::Variant::String")
            << Kross::Api::Argument("Kross::Api::Variant::String")
            << Kross::Api::Argument("Kross::Api::Variant::String") );
    addFunction("func5", &TestPluginObject::func5,
        Kross::Api::ArgumentList()
            << Kross::Api::Argument("Kross::Api::Variant::String")
            << Kross::Api::Argument("Kross::Api::Variant::String")
            << Kross::Api::Argument("Kross::Api::Variant::String") );
    addFunction("func6", &TestPluginObject::func6,
        Kross::Api::ArgumentList()
            << Kross::Api::Argument("Kross::Api::List")
            << Kross::Api::Argument("Kross::Api::Variant") );
    addFunction("func7", &TestPluginObject::func7,
        Kross::Api::ArgumentList()
            << Kross::Api::Argument("Kross::Api::Variant")
            << Kross::Api::Argument("Kross::Api::Variant") );
    addFunction("func8", &TestPluginObject::func8,
        Kross::Api::ArgumentList()
            << Kross::Api::Argument("Kross::Api::Variant")
            << Kross::Api::Argument("Kross::Api::Variant") );
    addFunction("func9", &TestPluginObject::func9,
        Kross::Api::ArgumentList()
            << Kross::Api::Argument("Kross::Api::Variant")
            << Kross::Api::Argument("Kross::Api::Variant") );

}

TestPluginObject::~TestPluginObject()
{
}

const QString TestPluginObject::getClassName() const
{
    return "TestPluginObject";
}

Kross::Api::Object::Ptr TestPluginObject::func1(Kross::Api::List::Ptr /*args*/)
{
    //kdDebug() << "CALLED => TestPluginObject::func1 args=" << args->toString() << endl;
    return 0;
}

Kross::Api::Object::Ptr TestPluginObject::func2(Kross::Api::List::Ptr /*args*/)
{
    //kdDebug() << "CALLED => TestPluginObject::func2 args=" << args->toString() << endl;
    return new Kross::Api::Variant("func2returnvalue");
}

Kross::Api::Object::Ptr TestPluginObject::func3(Kross::Api::List::Ptr /*args*/)
{
    //kdDebug() << "CALLED => TestPluginObject::func3 args=" << args->toString() << endl;
    return new Kross::Api::Variant("func3returnvalue");
}

Kross::Api::Object::Ptr TestPluginObject::func4(Kross::Api::List::Ptr /*args*/)
{
    //kdDebug() << "CALLED => TestPluginObject::func4 args=" << args->toString() << endl;
    return new Kross::Api::Variant("func4returnvalue");
}

Kross::Api::Object::Ptr TestPluginObject::func5(Kross::Api::List::Ptr /*args*/)
{
    //kdDebug() << "CALLED => TestPluginObject::func5 args=" << args->toString() << endl;
    return new Kross::Api::Variant("func5returnvalue");
}

Kross::Api::Object::Ptr TestPluginObject::func6(Kross::Api::List::Ptr args)
{
    Kross::Api::List* list = Kross::Api::Object::fromObject< Kross::Api::List >( args->item(0) );
    //kdDebug() << "CALLED => TestPluginObject::func6 args=" << list->toString() << endl;
    return new Kross::Api::Variant("func6returnvalue");
}

Kross::Api::Object::Ptr TestPluginObject::func7(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant("func7returnvalue");
}

Kross::Api::Object::Ptr TestPluginObject::func8(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant("func8returnvalue");
}

Kross::Api::Object::Ptr TestPluginObject::func9(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant("func9returnvalue");
}

/************************************************************************
 * TestPluginModule
 */

TestPluginModule::TestPluginModule(const QString& name)
    : Kross::Api::Module(name)
{
    addChild( new TestPluginObject("testpluginobject1") );
    //addChild( new TestPluginObject("testpluginobject2") );
}

TestPluginModule::~TestPluginModule()
{
}

const QString TestPluginModule::getClassName() const
{
    return "TestPluginModule";
}

