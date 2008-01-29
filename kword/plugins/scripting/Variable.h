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
#include <KoVariableRegistry.h>
#include <KoVariableFactory.h>
#include <KoVariable.h>
#include <KoProperties.h>
// kdelibs/kross
#include <kross/core/action.h>

namespace Scripting {

    /**
    * The Variable class implements functionality to deal with KWord variables within
    * documents.
    */
    class Variable : public QObject, public KoVariable
    {
            Q_OBJECT
        public:
            explicit Variable(Kross::Action* action, const KoProperties* props = 0)
                : KoVariable(), m_action(action), m_props(props), m_optionswidget(0) {}
            virtual ~Variable() { setOptionsWidget(); }
            Kross::Action* action() const { return m_action; }

            virtual void setProperties(const KoProperties* props)
            {
                kDebug(32010) <<"Scripting::Variable::setProperties";
                //Q_ASSERT(props);
                //QString value = props->property("value").toString();
                //setValue(value);

                m_props = props;
                emit propertiesUpdated();
#if 0
                Q_ASSERT(props);
                const QString function = m_props->property("getValue").toString();
                kDebug(32010) <<"Scripting::Variable::setProperties: function=" << function;
                if( ! function.isEmpty() ) {
                    QVariantList args;
                    args << m_props->property("id").toString();
                    QVariant result = m_action->callFunction(function, args);
                    kDebug(32010) <<"Scripting::Variable::setProperties: Calling function.name=" << function <<" result.value=" << result.toString() <<" result.type=" << result.typeName();
                    if( result.isValid() ) {
                        setValue( result.toString() );
                        return;
                    }
                }
                QString value = props->property("value").toString();
                setValue(value);
#endif
            }

            virtual QWidget* createOptionsWidget()
            {
                kDebug(32010) <<"Scripting::Variable::createOptionsWidget";
                emit optionsWidgetRequest();
                return m_optionswidget;

#if 0
                QWidget* resultwidget = 0;
                //m_action->setProperty("PropertyName", m_action->objectName());
                const QString function = m_props->property("createOptionsWidget").toString();
                if( ! function.isEmpty() ) {
                    QVariantList args;
                    args << m_props->property("id").toString();
                    QVariant result = m_action->callFunction(function, args);
                    kDebug(32010) <<"Scripting::Variable::createOptionsWidget: Calling function.name=" << function <<" result.value=" << result.toString() <<" result.type=" << result.typeName();
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
#endif
            }

        public Q_SLOTS:

            /**
            * Return the values the variable has as string.
            */
            QString value() const {
                return KoVariable::value();
            }

            /**
            * Set the value the variable has to the defined string \p value .
            */
            void setValue(const QString& value) {
                KoVariable::setValue(value);
            }

            /**
            * Set the options-widget that should be used.
            *
            * This widget will be shown if the user likes to insert a new
            * instance of the variable and it allows a variable to provide
            * additional optional options.
            */
            void setOptionsWidget(QWidget* optionswidget = 0) {
                //delete m_optionswidget;
                m_optionswidget = optionswidget;
            }

        Q_SIGNALS:

            /**
            * This signal got emitted if properties got updated.
            *
            * Such properties are additional optional options a variable
            * defines and they could be changed by the user. If such a
            * property changed, this signal got emitted to be able to
            * react on it - e.g. update the value the variable has cause
            * such a property changed.
            */
            void propertiesUpdated();

            /**
            * This signal got emitted if KWord requestes the options-widget.
            *
            * That way you are able to use the setOptionsWidget() method
            * to create register a new options-widget on demand.
            */
            void optionsWidgetRequest();

        private:
            Kross::Action* m_action;
            const KoProperties* m_props;
            QWidget* m_optionswidget;
    };

    /** \internal implementation of a factory for \a Variable instances used
    by KWord to create variables on demand. */
    class VariableFactory : public KoVariableFactory
    {
        protected:
            explicit VariableFactory(Kross::Action* action)
                : KoVariableFactory(action->objectName()), m_action(action)
            {
                KoVariableTemplate var;
                var.id = action->objectName();
                var.name = action->text();
                KoProperties *props = new KoProperties();
                props->setProperty("id", action->objectName());
                /*
                foreach(QString propname, action->propertyNames()) {
                    const QString value = action->property(propname);
                    props->setProperty(propname, value);
                }
                */
                var.properties = props;
                addTemplate(var);
            }

        public:
            virtual ~VariableFactory() {}

            Kross::Action* action() const { return m_action; }

            virtual KoVariable *createVariable(const KoProperties* props) const
            {
                Q_ASSERT(props);
                return new Variable(m_action, props);
            }

            virtual KoVariable *createVariable() const
            {
                //TODO
                return 0;
            }

            static VariableFactory* create(Kross::Action* action)
            {
                Q_ASSERT(action);
                if( action->objectName().isEmpty() ) {
                    kDebug(32010) <<"Scripting::VariableFactory::create: Action has empty objectName";
                    return 0;
                }
                if( KoVariableRegistry::instance()->contains(action->objectName()) ) {
                    kDebug(32010) <<"Scripting::VariableFactory::create: Action \"" << action->objectName() <<"\" already exist";
                    return 0;
                }
                VariableFactory* factory = new VariableFactory(action);
                KoVariableRegistry::instance()->add(factory);
                return factory;
            }

        private:
            Kross::Action* m_action;
    };

}

#endif
