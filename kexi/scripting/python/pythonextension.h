/***************************************************************************
 * pythonextension.h
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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 ***************************************************************************/

#ifndef KROSS_PYTHON_EXTENSION_H
#define KROSS_PYTHON_EXTENSION_H

#include "pythonconfig.h"
#include "../api/object.h"
#include "../api/list.h"
#include "../api/class.h"

#include <qstring.h>
#include <qstringlist.h>
#include <qvaluelist.h>
#include <qvaluevector.h>
#include <qmap.h>
#include <qvariant.h>

namespace Kross { namespace Python {

    // Forward declaration.
    class PythonScript;

    /**
     * The PythonExtension is a wrapper-object to let C++ and
     * Python interact together.
     * Instances of this class are used everytime if we send
     * or got something to/from python.
     */
    class PythonExtension : public Py::PythonExtension<PythonExtension>
    {
            friend class PythonScript;
            friend class PythonObject;

        public:

            /**
             * Constructor.
             *
             * \param object The \a Kross::Api::Object object
             *        this instance is the wrapper for.
             */
            explicit PythonExtension(Kross::Api::Object::Ptr object);

            /**
             * Destructor.
             */
            virtual ~PythonExtension();

            /**
             * Overloaded method to handle attribute calls
             * from within python.
             *
             * \param name The name of the attribute that
             *        should be handled.
             * \return An \a Py::Object that could be
             *         a value or a callable object. Python
             *         will decide what to do with the
             *         returnvalue.
             */
            virtual Py::Object getattr(const char* name);

            /**
             * Overloaded method to handle method calls
             * from within python. This function behaves
             * similar as the \a getattr but should return
             * callable objects or None if there is no
             * method with such name.
             *
             * \param name The name of the callable object
             *        that should be returned.
             */
            virtual Py::Object getattr_methods(const char* name);

            /**
             * Return the \a Kross::Api::Object this
             * PythonExtension wraps.
             */
            Kross::Api::Object::Ptr getObject();

            //virtual Py::Object repr() { return Py::String(m_object->getName().latin1()); }
            //virtual Py::Object str() { return Py::String(m_object->getName().latin1()); }
            //virtual int print(FILE *, int) {}

        private:
            /// The \a Kross::Api::Object this PythonExtension wraps.
            Kross::Api::Object::Ptr m_object;
            /// Internal value used by our dirty hack to handle calls more flexible.
            QString m_methodname;

            /**
             * Converts a \a Py::Tuple into a \a Kross::Api::List.
             *
             * \param tuple The Py::Tuple to convert.
             * \return The to a Kross::Api::List converted Py::Tuple.
             */
            static Kross::Api::List::Ptr toObject(const Py::Tuple& tuple);

            /**
             * Converts a \a Py::Object into a \a Kross::Api::Object.
             *
             * \param object The Py::Object to convert.
             * \return The to a Kross::Api::Object converted Py::Object.
             */
            static Kross::Api::Object::Ptr toObject(const Py::Object& object);

            /**
             * Converts a QString to a Py::Object. If
             * the QString isNull() then Py::None() will
             * be returned.
             *
             * \param s The QString to convert.
             * \return The to a Py::String converted QString.
             */
            static Py::Object toPyObject(const QString& s);

            /**
             * Converts a QStringList to a Py::List.
             *
             * \param list The QStringList to convert.
             * \return The to a Py::List converted QStringList.
             */
            static Py::List toPyObject(QStringList list);

            /**
             * Converts a QMap to a Py::Dict.
             *
             * \param map The QMap to convert.
             * \return The to a Py::Dict converted QMap.
             */
            static Py::Dict toPyObject(QMap<QString, QVariant> map);

            /**
             * Converts a QValueList to a Py::List.
             *
             * \param list The QValueList to convert.
             * \return The to a Py::List converted QValueList.
             */
            static Py::List toPyObject(QValueList<QVariant> list);

            /**
             * Converts a QVariant to a Py::Object.
             *
             * \param variant The QVariant to convert.
             * \return The to a Py::Object converted QVariant.
             */
            static Py::Object toPyObject(const QVariant& variant);

            /**
             * Converts a \a Kross::Api::Object to a Py::Object.
             *
             * \param object The Kross::Api::Object to convert.
             * \return The to a Py::Object converted Kross::Api::Object.
             */
            static Py::Object toPyObject(Kross::Api::Object::Ptr object);

            /**
             * Converts a \a Kross::Api::List into a Py::Tuple.
             *
             * \param list The Kross::Api::List to convert.
             * \return The to a Py::Tuple converted Kross::Api::List.
             */
            static Py::Tuple toPyTuple(Kross::Api::List::Ptr list);

            /**
             * Callback function called from within python. This
             * function acts as call redirector. The \a m_dynamic_methods
             * and \a m_static_methods are used to forward the calling
             * to the correct method or throw an \a Py::Exception if
             * something went wrong.
             *
             * \param args The methodarguments.
             * \return Returnvalue of the methodcall.
             */
            Py::Object _call_(const Py::Tuple&);
    };

}}

#endif
