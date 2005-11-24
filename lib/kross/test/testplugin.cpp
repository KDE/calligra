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
            << Kross::Api::Argument("Kross::Api::Variant::String")
    );
    addFunction("func3", &TestPluginObject::func3,
        Kross::Api::ArgumentList()
            << Kross::Api::Argument("Kross::Api::Variant::String")
            << Kross::Api::Argument("Kross::Api::Variant::String", new Kross::Api::Variant("func3string2_default"))
    );
    addFunction("func4", &TestPluginObject::func4,
        Kross::Api::ArgumentList()
            << Kross::Api::Argument("Kross::Api::Variant::String")
            << Kross::Api::Argument("Kross::Api::Variant::String")
            << Kross::Api::Argument("Kross::Api::Variant::String", new Kross::Api::Variant("func4string3_defaultconst"), false)
    );
    addFunction("func5", &TestPluginObject::func5,
        Kross::Api::ArgumentList()
            << Kross::Api::Argument("Kross::Api::Variant::String", new Kross::Api::Variant("func5string1_defaultconst"), false)
            << Kross::Api::Argument("Kross::Api::Variant::String", new Kross::Api::Variant("func5string2_default"))
            << Kross::Api::Argument("Kross::Api::Variant::String", new Kross::Api::Variant("func5string3_default"))
    );

    addFunction("overloadedFunc", &TestPluginObject::overloadedFunc2,
        Kross::Api::ArgumentList() 
            << Kross::Api::Argument("Kross::Api::Variant::String")
    );
    addFunction("overloadedFunc", &TestPluginObject::overloadedFunc3,
        Kross::Api::ArgumentList()
            << Kross::Api::Argument("Kross::Api::Variant::Integer")
    );
    addFunction("overloadedFunc", &TestPluginObject::overloadedFunc4,
        Kross::Api::ArgumentList() 
            << Kross::Api::Argument("Kross::Api::Variant::String")
            << Kross::Api::Argument("Kross::Api::Variant::String")
    );
    addFunction("overloadedFunc", &TestPluginObject::overloadedFunc5,
        Kross::Api::ArgumentList() 
            << Kross::Api::Argument("Kross::Api::Variant::String")
            << Kross::Api::Argument("Kross::Api::Variant::Integer")
    );

    // This function will never be called cause we arn't able to differ
    // between a Kross::Api::Variant::Integer and a 
    // Kross::Api::Variant::Bool. There doesn't seem to be an easy
    // solution to work around this problem. So, take care to differ
    // only between string, list, dict and numeric...
    addFunction("overloadedFunc", &TestPluginObject::overloadedFunc6,
        Kross::Api::ArgumentList() 
            << Kross::Api::Argument("Kross::Api::Variant::Bool")
    );

    addFunction("overloadedFunc", &TestPluginObject::overloadedFunc7,
        Kross::Api::ArgumentList() 
            << Kross::Api::Argument("Kross::Api::Variant::StringList")
    );

    addFunction("overloadedFunc", &TestPluginObject::overloadedFunc1);
}

TestPluginObject::~TestPluginObject()
{
}

const QString TestPluginObject::getClassName() const
{
    return "TestPluginObject";
}

Kross::Api::Object::Ptr TestPluginObject::func1(Kross::Api::List::Ptr args)
{
    kdDebug() << "CALLED => TestPluginObject::func1 args=" << args->toString() << endl;
    return 0;
}

Kross::Api::Object::Ptr TestPluginObject::func2(Kross::Api::List::Ptr args)
{
    kdDebug() << "CALLED => TestPluginObject::func2 args=" << args->toString() << endl;
    return new Kross::Api::Variant("func2returnvalue");
}

Kross::Api::Object::Ptr TestPluginObject::func3(Kross::Api::List::Ptr args)
{
    kdDebug() << "CALLED => TestPluginObject::func3 args=" << args->toString() << endl;
    return new Kross::Api::Variant("func3returnvalue");
}

Kross::Api::Object::Ptr TestPluginObject::func4(Kross::Api::List::Ptr args)
{
    kdDebug() << "CALLED => TestPluginObject::func4 args=" << args->toString() << endl;
    return 0;
}

Kross::Api::Object::Ptr TestPluginObject::func5(Kross::Api::List::Ptr args)
{
    kdDebug() << "CALLED => TestPluginObject::func5 args=" << args->toString() << endl;
    return 0;
}

Kross::Api::Object::Ptr TestPluginObject::overloadedFunc1(Kross::Api::List::Ptr)
{
    kdDebug() << "CALLED => TestPluginObject::overloadedFunc1()" << endl;
    return 0;
}

Kross::Api::Object::Ptr TestPluginObject::overloadedFunc2(Kross::Api::List::Ptr)
{
    kdDebug() << "CALLED => TestPluginObject::overloadedFunc2(Kross::Api::Variant::String)" << endl;
    return 0;
}

Kross::Api::Object::Ptr TestPluginObject::overloadedFunc3(Kross::Api::List::Ptr)
{
    kdDebug() << "CALLED => TestPluginObject::overloadedFunc3(Kross::Api::Variant::Integer)" << endl;
    return 0;
}

Kross::Api::Object::Ptr TestPluginObject::overloadedFunc4(Kross::Api::List::Ptr)
{
    kdDebug() << "CALLED => TestPluginObject::overloadedFunc4(Kross::Api::Variant::String, Kross::Api::Variant::String)" << endl;
    return 0;
}

Kross::Api::Object::Ptr TestPluginObject::overloadedFunc5(Kross::Api::List::Ptr)
{
    kdDebug() << "CALLED => TestPluginObject::overloadedFunc5(Kross::Api::Variant::String, Kross::Api::Variant::Integer)" << endl;
    return 0;
}

Kross::Api::Object::Ptr TestPluginObject::overloadedFunc6(Kross::Api::List::Ptr)
{
    kdDebug() << "CALLED => TestPluginObject::overloadedFunc5(Kross::Api::Variant::Bool)" << endl;
    return 0;
}

Kross::Api::Object::Ptr TestPluginObject::overloadedFunc7(Kross::Api::List::Ptr)
{
    kdDebug() << "CALLED => TestPluginObject::overloadedFunc5(Kross::Api::Variant::StringList)" << endl;
    return 0;
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

