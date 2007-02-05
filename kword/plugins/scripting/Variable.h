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
// koffice
#include <KoInlineObjectFactory.h>
#include <KoVariable.h>
#include <KoProperties.h>
// kdelibs/kross
#include <kross/core/action.h>

namespace Scripting {

    class Variable : public QObject, public KoVariable
    {
            Q_OBJECT
        public:
            explicit Variable(const KoProperties* properties)
                : QObject(), KoVariable()
            {
                setProperties(properties);
            }

            virtual ~Variable() {}

            //virtual void setProperties(const KoProperties *props) { Q_UNUSED(props); }
            //virtual QWidget *createOptionsWidget() { return 0; }
    };

    class VariableFactory : public KoInlineObjectFactory
    {
        public:
            VariableFactory(QObject* parent, Kross::Action* action)
                : KoInlineObjectFactory(parent, action->objectName())
            {
                KoInlineObjectTemplate var;
                var.id = action->objectName();
                var.name = action->text();
                KoProperties *props = new KoProperties();
                //props->setProperty("id", DateVariable::Fixed);
                //props->setProperty("definition", "dd/MM/yy");
                var.properties = props;
                addTemplate(var);
            }

            virtual ~VariableFactory() {}

            KoInlineObject *createInlineObject(const KoProperties* properties) const {
                return new Variable(properties);
            }

            virtual ObjectType type() const {
                return TextVariable;
            }
    };

}

#endif
