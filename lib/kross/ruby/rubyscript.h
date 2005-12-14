/***************************************************************************
 * rubyscript.h
 * This file is part of the KDE project
 * copyright (C)2005 by Cyrille Berger (cberger@cberger.net)
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

#ifndef KROSS_RUBYRUBYSCRIPT_H
#define KROSS_RUBYRUBYSCRIPT_H

#include <api/script.h>

namespace Kross {

namespace Ruby {

class RubyScriptPrivate;
    /**
     * Handle ruby scripts. This class implements
     * \a Kross::Api::Script for ruby.
     */
class RubyScript : public Kross::Api::Script
{
    public:
        RubyScript(Kross::Api::Interpreter* interpreter, Kross::Api::ScriptContainer* scriptcontainer);
        ~RubyScript();
        /**
         * Return a list of callable functionnames this
         * script spends.
         */
        virtual const QStringList& getFunctionNames();

        /**
         * Execute the script.
         */
        virtual Kross::Api::Object::Ptr execute();

        /**
         * Call a function.
         */
        virtual Kross::Api::Object::Ptr callFunction(const QString& name, Kross::Api::List::Ptr args);

        /**
         * Return a list of class types this script supports.
         */
        virtual const QStringList& getClassNames();

        /**
         * Create and return a new class instance.
         */
        virtual Kross::Api::Object::Ptr classInstance(const QString& name);
    private:
        void compile();
        inline void selectScript();
    private:
        RubyScriptPrivate* d;
};

}

}

#endif
