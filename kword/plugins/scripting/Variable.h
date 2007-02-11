/* This file is part of the KOffice project
 * Copyright (C) 2007 Sebastian Sauer <mail@dipe.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef SCRIPTING_VARIABLE_H
#define SCRIPTING_VARIABLE_H

// qt
#include <QObject>
//kde
#include <kdebug.h>
// koffice
#include <KoInlineObjectRegistry.h>
#include <KoInlineObjectFactory.h>
#include <KoVariable.h>
#include <KoProperties.h>
// kdelibs/kross
#include <kross/core/action.h>

namespace Scripting {

    class Variable : public KoVariable
    {
        public:
            explicit Variable(Kross::Action* action, const KoProperties* props = 0)
                : KoVariable(), m_action(action), m_props(props)
            {
            }

            virtual ~Variable() {}

            Kross::Action* action() const { return m_action; }

            virtual void setProperties(const KoProperties* props)
            {
                Q_ASSERT(props);
                const QString function = m_props->property("getValue").toString();
                kDebug(32010) << "Scripting::Variable::setProperties: function=" << function << endl;
                if( ! function.isEmpty() ) {
                    QVariantList args;
                    args << m_props->property("id").toString();
                    QVariant result = m_action->callFunction(function, args);
                    kDebug(32010) << "Scripting::Variable::setProperties: Calling function.name=" << function << " result.value=" << result.toString() << " result.type=" << result.typeName() << endl;
                    if( result.isValid() ) {
                        setValue( result.toString() );
                        return;
                    }
                }
                QString value = props->property("value").toString();
                setValue(value);
            }

            virtual QWidget* createOptionsWidget()
            {
                kDebug(32010) << "Scripting::Variable::createOptionsWidget" << endl;
                QWidget* resultwidget = 0;
                //m_action->setProperty("PropertyName", m_action->objectName());
                const QString function = m_props->property("createOptionsWidget").toString();
                if( ! function.isEmpty() ) {
                    QVariantList args;
                    args << m_props->property("id").toString();
                    QVariant result = m_action->callFunction(function, args);
                    kDebug(32010) << "Scripting::Variable::createOptionsWidget: Calling function.name=" << function << " result.value=" << result.toString() << " result.type=" << result.typeName() << endl;
                    if( result.isValid() ) {
                        if( qVariantCanConvert< QWidget* >(result) )
                            resultwidget = qvariant_cast< QWidget* >(result);
                        else if( qVariantCanConvert< QObject* >(result) )
                            resultwidget = dynamic_cast< QWidget* >( qvariant_cast< QObject* >(result) );
                    }
                }

                //m_action->trigger();
                //QString widgetname = m_action->property("OptionsWidget");
                if( value().isNull() && m_props )
                    setProperties(m_props);

                return resultwidget;
            }
        private:
            Kross::Action* m_action;
            const KoProperties* m_props;
    };

    class VariableFactory : public KoInlineObjectFactory
    {
        protected:
            explicit VariableFactory(Kross::Action* action)
                : KoInlineObjectFactory(action, action->objectName()), m_action(action)
            {
                KoInlineObjectTemplate var;
                var.id = action->objectName();
                var.name = action->text();
                KoProperties *props = new KoProperties();
                props->setProperty("id", action->objectName());
                foreach(QString propname, action->propertyNames()) {
                    const QString value = action->property(propname);
                    props->setProperty(propname, value);
                }
                var.properties = props;
                addTemplate(var);
            }

        public:
            virtual ~VariableFactory() {}

            Kross::Action* action() const { return m_action; }

            virtual KoInlineObject *createInlineObject(const KoProperties* props) const
            {
                Q_ASSERT(props);
                return new Variable(m_action, props);
            }

            virtual ObjectType type() const
            {
                return TextVariable;
            }

            static VariableFactory* create(Kross::Action* action)
            {
                Q_ASSERT(action);
                if( action->objectName().isEmpty() ) {
                    kDebug(32010) << "Scripting::VariableFactory::create: Action has empty objectName" << endl;
                    return 0;
                }
                if( KoInlineObjectRegistry::instance()->exists(action->objectName()) ) {
                    kDebug(32010) << "Scripting::VariableFactory::create: Action \"" << action->objectName() << "\" already exist" << endl;
                    return 0;
                }
                VariableFactory* factory = new VariableFactory(action);
                KoInlineObjectRegistry::instance()->add(factory);
                return factory;
            }

        private:
            Kross::Action* m_action;
    };

}

#endif
