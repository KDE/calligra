/***************************************************************************
 * object.h
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

#ifndef KROSS_API_OBJECT_H
#define KROSS_API_OBJECT_H

#include <qstring.h>
#include <qvaluelist.h>
#include <qmap.h>
#include <qvariant.h>
//#include <qobject.h>
#include <ksharedptr.h>

#include "../main/krossconfig.h"
#include "exception.h"

namespace Kross { namespace Api {

    // Forward declaration.
    class List;

    /**
     * The common Object class all other object-classes are
     * inheritated from.
     *
     * The Object class is used as base class to spend common
     * functionality. It's similar to what we know in Python
     * as PyObject or in Qt as QObject.
     *
     * Inherited from e.g. \a Value, \a Module and \a Class.
     *
     * This class implementates reference counting for shared
     * objects. So, no need to take care of freeing objects.
     */
    class Object : public KShared
    {
        public:

            /// Shared pointer to implement reference-counting.
            typedef KSharedPtr<Object> Ptr;

            /**
             * Constructor.
             *
             * \param name The name this object has. Return
             *        it via \a getName() and set a new
             *        name via \a setName().
             * \param parent The parent Object or NULL if
             *        this object doesn't has an object.
             */
            explicit Object(const QString& name, Object::Ptr parent = 0);

            /**
             * Destructor.
             */
            virtual ~Object();

            /**
             * Return the name this object has.
             *
             * \return Name of this object.
             */
            const QString& getName() const;

            /**
             * Return the class name. This could be something
             * like "Kross::Api::Object" for this object. The
             * value is mainly used for display purposes.
             *
             * \return The name of this class.
             */
            virtual const QString getClassName() const = 0;

            /**
             * Return a detailed description about this object.
             * Each object should describe itself a bit about
             * what it is designed for, how to use it and
             * such stuff.
             *
             * \return The description of this class.
             */
            virtual const QString getDescription() const = 0;

            /**
             * Return the parent object or NULL if this object
             * doesn't has a parent.
             *
             * \return The parent-Object or NULL if this Object
             *         doesn't has a parent.
             */
            Object::Ptr getParent() const;

            /**
             * Returns if the defined child is avaible.
             *
             * \return true if child exists else false.
             */
            bool hasChild(const QString& name) const;

            /**
             * Return the defined child or NULL if there is
             * no such object with that name avaible.
             *
             * \param name The name of the Object to return.
             * \return The Object matching to the defined
             *         name or NULL if there is no such Object.
             */
            Object::Ptr getChild(const QString& name) const;

            /**
             * Return all children.
             *
             * \return A \a ObjectMap of children this Object has.
             */
            QMap<QString, Object::Ptr> getChildren() const;

            /**
             * Add a new Child.
             *
             * \param name The name of the new child. Each child
             *        should have an unique name compared to other
             *        children of this object to be direct
             *        accessible by this name.
             * \param object The Object to add.
             * \param replace Replace an already existing Object
             *        with same name (default is false)?
             * \return true if the Object was added successfully
             *         else (e.g. if there exists already another
             *         childobject with same name) false.
             */
            bool addChild(const QString& name, Object::Ptr object, bool replace = false);

            /**
             * Remove an existing child.
             *
             * \param name The name of the Object to remove.
             *        If there doesn't exists an Object with
             *        such name just nothing will be done.
             */
            void removeChild(const QString& name);

            /**
             * Remove all children.
             */
            void removeAllChildren();

            /**
             * Pass a call to the object. Objects like \a Class
             * are able to handle call's by just implementating
             * this function.
             *
             * \throws TypeException if the object or the name
             *         is not callable.
             * \param name Each call has a name that says what
             *        should be called. In the case of a \a Class
             *        the name is the functionname.
             * \param arguments The list of arguments passed to
             *        the call.
             * \return The call-result as \a Object::Ptr instance or
             *         NULL if the call has no result.
             */
            virtual Object::Ptr call(const QString& name, KSharedPtr<List> arguments);

            /**
             * Return a list of supported callable objects.
             *
             * \return List of supported calls.
             */
            virtual QStringList getCalls() { return QStringList(); }

            /**
             * Try to convert the \a Object instance to the
             * template class T.
             *
             * \throw TypeException if the cast failed.
             * \param object The Object to cast.
             * \return The to a instance from template type T
             *         casted Object.
             */
            template<class T> static T* fromObject(Object::Ptr object)
            {
                T* t = (T*) object.data();
                if(! t)
                    throw Kross::Api::TypeException(i18n("Object \"%1\" invalid.").arg(object->getClassName()));
                return t;
            }

        private:
            /// Name of this object.
            QString m_name;
            /// The parent object.
            Object::Ptr m_parent;
            /// A list of childobjects.
            QMap<QString, Object::Ptr> m_children;
    };

}}

#endif

