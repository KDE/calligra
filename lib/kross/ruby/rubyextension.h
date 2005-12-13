/***************************************************************************
 * rubyinterpreter.cpp
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
#ifndef KROSS_RUBYRUBYEXTENSION_H
#define KROSS_RUBYRUBYEXTENSION_H

#include <ruby.h>

#include <api/class.h>
#include <api/dict.h>
#include <api/list.h>
#include <api/object.h>

namespace Kross {

namespace Ruby {

class RubyExtensionPrivate;
/**
@author Cyrille Berger
*/
class RubyExtension{
        friend class RubyScript;
        friend class RubyInterpreter;
    public:
        RubyExtension(Kross::Api::Object::Ptr object);
        ~RubyExtension();
    private:
        /**
         * This function will catch functions that are undefined.
         * 
         */
        static VALUE method_missing(int argc, VALUE *argv, VALUE self);
    private: //Converting functions
        /**
         * This function iterats through a ruby hash
         */
        static int RubyExtension::convertHash_i(VALUE key, VALUE value, VALUE vmap);
        /**
         * Converts a \a VALUE into a \a Kross::Api::Object.
         * \param object The ruby VALUE to convert.
         * \return The to a Kross::Api::Object converted Py::Object.
         */
        static Kross::Api::Object::Ptr toObject(VALUE value);
        /**
         * Converts a QString to a VALUE. If
         * the QString isNull() then a "" will
         * be returned.
         * \param s The QString to convert.
         * \return The converted QString.
         */
        static VALUE toVALUE(const QString& s);

        /**
         * Converts a QStringList to a VALUE.
         * \param list The QStringList to convert.
         * \return The converted QStringList.
         */
        static VALUE toVALUE(QStringList list);

        /**
         * Converts a QMap to a VALUE.
         * \param map The QMap to convert.
         * \return The converted QMap.
         */
        static VALUE toVALUE(QMap<QString, QVariant> map);

        /**
         * Converts a QValueList to a VALUE.
         * \param list The QValueList to convert.
         * \return The converted QValueList.
         */
        static VALUE toVALUE(QValueList<QVariant> list);
        /**
         * Converts a QVariant to a VALUE.
         * \param variant The QVariant to convert.
         * \return The converted QVariant.
         */
        static VALUE toVALUE(const QVariant& variant);

        /**
         * Converts a \a Kross::Api::Object to a VALUE.
         * \param object The Kross::Api::Object to convert.
         * \return The converted Kross::Api::Object.
         */
        static VALUE toVALUE(Kross::Api::Object::Ptr object);

        /**
         * Converts a \a Kross::Api::List into a VALUE.
         * \param list The Kross::Api::List to convert.
         * \return The converted Kross::Api::List.
         */
        static VALUE toVALUE(Kross::Api::List::Ptr list);
    private:
        RubyExtensionPrivate* d;
 };

}

}

#endif
