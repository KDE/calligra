/*
 *  Copyright (c) 2005 Cyrille Berger <cberger@cberger.net>
 *  Copyright (c) 2006 Isaac Clerencia <isaac@warp.es>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Library General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "kspreadcoremodule.h"

#include <kdebug.h>

//#include <api/variant.h>
#include <api/qtobject.h>
#include <main/manager.h>

#include "krs_color.h"
#include "krs_doc.h"

extern "C"
{
    /**
     * Exported an loadable function as entry point to use
     * the \a KexiAppModule.
     */
    Kross::Api::Object* init_module(Kross::Api::Manager* manager)
    {
        return new Kross::KSpreadCore::KSpreadCoreModule(manager);
    }
}


using namespace Kross::KSpreadCore;

KSpreadCoreFactory::KSpreadCoreFactory() : Kross::Api::Event<KSpreadCoreFactory>("KSpreadCoreFactory")
{
    addFunction("newRGBColor", &KSpreadCoreFactory::newRGBColor);
}

Kross::Api::Object::Ptr KSpreadCoreFactory::newRGBColor(Kross::Api::List::Ptr args)
{
    return Kross::Api::Object::Ptr(new Color(Kross::Api::Variant::toUInt(args->item(0)), Kross::Api::Variant::toUInt(args->item(1)), Kross::Api::Variant::toUInt(args->item(2)), QColor::Rgb));
}


KSpreadCoreModule::KSpreadCoreModule(Kross::Api::Manager* manager)
    : Kross::Api::Module("kspreadcore") , m_manager(manager), m_factory(new KSpreadCoreFactory())
{
    QMap<QString, Object::Ptr> children = manager->getChildren();
    kDebug() << " there are " << children.size() << endl;
    for(QMap<QString, Object::Ptr>::const_iterator it = children.begin(); it != children.end(); it++)
    {
        kDebug() << it.key() << " " << it.data() << endl;
    }

    // Wrap doc
    Kross::Api::Object::Ptr kspreaddocument = manager->getChild("KSpreadDocument");
    if(kspreaddocument) {
        Kross::Api::QtObject* kspreaddocumentqt = dynamic_cast< Kross::Api::QtObject* >( kspreaddocument.data() );
        if(kspreaddocumentqt) {
            KSpread::Doc* document = dynamic_cast< ::KSpread::Doc* >( kspreaddocumentqt->getObject() );
            if(document) {
                addChild( new Doc(document) );
            } else {
                throw Kross::Api::Exception::Ptr( new Kross::Api::Exception("There was no 'KSpreadDocument' published.") );
            }
         }
    }
}

KSpreadCoreModule::~KSpreadCoreModule()
{
    delete m_factory;
}


const QString KSpreadCoreModule::getClassName() const
{
    return "Kross::KSpreadCore::KSpreadCoreModule";
}

Kross::Api::Object::Ptr KSpreadCoreModule::call(const QString& name, Kross::Api::List::Ptr arguments)
{
    kDebug() << "KSpreadCoreModule::call" << name << endl;
    if( m_factory->isAFunction(name))
    {
        return m_factory->call(name, arguments);
    } else {
        return Kross::Api::Module::call(name, arguments);
    }
}
