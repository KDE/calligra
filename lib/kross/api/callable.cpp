/***************************************************************************
 * callable.cpp
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

#include "callable.h"
#include "variant.h"
#include "dict.h"

#include <kdebug.h>

using namespace Kross::Api;

Callable::Callable(const QString& name, Object::Ptr parent, ArgumentList arglist)
    : Object(name, parent)
    , m_arglist(arglist)
{
}

Callable::~Callable()
{
}

const QString Callable::getClassName() const
{
    return "Kross::Api::Callable";
}

Object::Ptr Callable::call(const QString& name, List::Ptr arguments)
{
#ifdef KROSS_API_CALLABLE_CALL_DEBUG
    kdDebug() << QString("Kross::Api::Callable::call() name=%1 getName()=%2 arguments=%3").arg(name).arg(getName()).arg(arguments ? arguments->toString() : QString("")) << endl;
#endif

    if(name == "get") {
        //checkArguments( ArgumentList() << Argument("Kross::Api::Variant::String") );
        return getChild(arguments);
    }
    else if(name == "has") {
        //checkArguments( ArgumentList() << Argument("Kross::Api::Variant::String") );
        return hasChild(arguments);
    }
    else if(name == "call") {
        //checkArguments( ArgumentList() << Argument("Kross::Api::Variant::String") << Argument("Kross::Api::List", new List( QValueList<Object::Ptr>() )) );
        return callChild(arguments);
    }
    else if(name == "list") {
        //checkArguments( ArgumentList() << Argument("Kross::Api::Variant::String") << Argument("Kross::Api::List", new List( QValueList<Object::Ptr>() )) );
        return getChildrenList(arguments);
    }
    else if(name == "dict") {
        //checkArguments( ArgumentList() << Argument("Kross::Api::Variant::String") << Argument("Kross::Api::List", new List( QValueList<Object::Ptr>() )) );
        return getChildrenDict(arguments);
    }

    return Object::call(name, arguments);
}

bool Callable::validArguments(List::Ptr arguments)
{
    try {
        checkArguments(arguments);
        return true;
    }
    catch(Exception::Ptr&) {} // just ignore the exception
    return false;
}

void Callable::checkArguments(List::Ptr arguments)
{
#ifdef KROSS_API_CALLABLE_CHECKARG_DEBUG
    kdDebug() << QString("Kross::Api::Callable::checkArguments() getName()=%1 arguments=%2")
                 .arg(getName()).arg(arguments ? arguments->toString() : QString::null) << endl;
#endif

    QValueList<Object::Ptr>& arglist = arguments->getValue();
    uint fmax = m_arglist.getMaxParams();
    uint fmin = m_arglist.getMinParams();

    // check the number of parameters passed.
    if(arglist.size() < fmin)
        throw Exception::Ptr( new Exception(QString("Too few parameters for callable object '%1'.").arg(getName())) );
    if(arglist.size() > fmax)
        throw Exception::Ptr( new Exception(QString("Too many parameters for callable object '%1'.").arg(getName())) );

    // check type of passed parameters.
    QValueList<Argument>& farglist = m_arglist;
    QValueList<Argument>::Iterator it = farglist.begin();
    QValueList<Object::Ptr>::Iterator argit = arglist.begin();
    for(; it != farglist.end(); ++it) {
        bool argend = ( argit == arglist.end() );

        if( ! (*it).isVisible() ) {
            // if the argument isn't visibled, we always use the default argument.
            Object::Ptr o = (*it).getObject();
            if(! o)
                throw Exception::Ptr( new Exception(QString("Callable object '%1' is hidden, but has no default value.").arg(getName())) );
            if(argend)
                arglist.append(o);
            else
                arglist.insert(argit, o);
        }
        else {
            // the argument is visibled and therefore the passed arguments may
            // define the value.

            if(argend) {
                // no argument defined, use the default value.
                Object::Ptr o = (*it).getObject();
                if(! o)
                    throw Exception::Ptr( new Exception(QString("Callable object '%1' was not passed, but has also no default value.").arg(getName())) );
                arglist.append( o );
            }
            else {

                // Check if the type of the passed argument matches to what we 
                // expect. The given argument could have just the same type like 
                // the expected argument or could be a specialization of it.
                Object::Ptr o = (*argit);
                QString fcn = (*it).getClassName(); // expected argument
                QString ocn = o->getClassName(); // given argument
                bool ok = ocn.startsWith(fcn);
                if(! ok) {
                    if(ocn.startsWith("Kross::Api::Variant")) {
                        ocn = Variant::getVariantType(o);
                        ok = (
                            ocn == fcn ||
                            (
                                (fcn == "Kross::Api::Variant::Integer" || fcn == "Kross::Api::Variant::UInt") &&
                                (ocn == "Kross::Api::Variant::Integer" || ocn == "Kross::Api::Variant::UInt")
                            )
                        );
                    }
                    if(! ok)
                        throw Exception::Ptr( new Exception(QString("Callable object '%1' expected parameter of type '%2', but got '%3'").arg(getName()).arg(fcn).arg(ocn)) );
                }

            }
        }

        if(! argend)
            ++argit;
    }

/*
    // check type of passed parameters.
    QValueList<Argument>& farglist = m_arglist;
    for(uint i = 0; i < fmax; i++) {
        if(i >= arglist.count()) { // handle default arguments
kdDebug()<<"##############> ("<<farglist[i].getObject()->getName()<<") "<<farglist[i].getObject()->toString()<<endl;
            arglist.append( farglist[i].getObject() );
            continue;
        }
kdDebug()<<"==> "<<arguments->toString()<<endl;

        Object::Ptr o = arguments->item(i);
        QString fcn = farglist[i].getClassName(); // expected argument
        QString ocn = o->getClassName(); // given argument

        // Check if the type of the passed argument matches to what we 
        // expect. The given argument could have just the same type like 
        // the expected argument or could be a specialization of it.
        bool ok = ocn.startsWith(fcn);
        if(! ok) {
            if(ocn.startsWith("Kross::Api::Variant")) {
                ocn = Variant::getVariantType(o);
//FIXME: we need a more typesafe way to handle QVariant::type's
                if(ocn == fcn ||
                    (
                        (fcn == "Kross::Api::Variant::Integer" || fcn == "Kross::Api::Variant::UInt") &&
                        (ocn == "Kross::Api::Variant::Integer" || ocn == "Kross::Api::Variant::UInt")
                    )
                ) {
                    ok = true;
                }
            }
            if(! ok)
                throw Exception::Ptr( new Exception(QString("Callable object '%1' expected parameter of type '%2', but got '%3'").arg(getName()).arg(fcn).arg(ocn)) );
        }
    }
*/
}

Object::Ptr Callable::hasChild(List::Ptr args)
{
    //kdDebug() << QString("Kross::Api::Callable::hasChild() getName()=%1").arg(getName()) << endl;
    return new Variant( Object::hasChild( Variant::toString(args->item(0)) ),
                        "Kross::Api::Callable::hasChild::Bool" );
}

Object::Ptr Callable::getChild(List::Ptr args)
{
    QString s = Variant::toString(args->item(0));
    //kdDebug() << QString("Kross::Api::Callable::getChild() getName()=%1 childName=%2").arg(getName()).arg(s) << endl;
    Object::Ptr obj = Object::getChild(s);
    if(! obj)
        throw Exception::Ptr( new Exception(QString("The object '%1' has no child object '%2'").arg(getName()).arg(s)) );
    return obj;
}

Object::Ptr Callable::getChildrenList(List::Ptr)
{
    QStringList list;
    QMap<QString, Object::Ptr> children = getChildren();
    QMap<QString, Object::Ptr>::Iterator it( children.begin() );
    for(; it != children.end(); ++it)
        list.append( it.key() );
    return new Variant(list);
}

Object::Ptr Callable::getChildrenDict(List::Ptr)
{
    //kdDebug()<<"Kross::Api::Callable::getChildrenDict()"<<endl;
    return new Dict(Object::getChildren(), "Kross::Api::Callable::getChildrenDict::Dict");
}

Object::Ptr Callable::callChild(List::Ptr args)
{
    //kdDebug() << QString("Kross::Api::Callable::callChild() getName()=%1").arg(getName()) << endl;
    return Object::call(Variant::toString(args->item(0)), args);
}
