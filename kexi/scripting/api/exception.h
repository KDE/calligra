/***************************************************************************
 * exception.h
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

#ifndef KROSS_API_EXCEPTION_H
#define KROSS_API_EXCEPTION_H

#include <qstring.h>
#include <klocale.h>

namespace Kross { namespace Api {

    /**
     * Common exception class used for throwing exceptions
     * in Kross.
     */
    class Exception
    {
        public:
            Exception(const QString& description)
                : m_description(description) {}
            virtual ~Exception() {}
            virtual const QString type() { return i18n("Exception"); }
            const QString& description() { return m_description; }
        private:
            QString m_description;
    };

    /**
     * From \a Exception inherited exception class to
     * represent any kind of attribute/argument error.
     */
    class AttributeException : public Exception
    {
        public:
            AttributeException(const QString& description)
                : Exception(description) {}
            virtual ~AttributeException() {}
            virtual const QString type() { return i18n("AttributeException"); }
    };

    /**
     * From \a Exception inherited exception class to
     * represent runtime or unexpected errors.
     */
    class RuntimeException : public Exception
    {
        public:
            RuntimeException(const QString& description)
                : Exception(description) {}
            virtual ~RuntimeException() {}
            virtual const QString type() { return i18n("RuntimeException"); }
    };

    /**
     * From \a Exception inherited exception class to
     * represent type or casting errors.
     */
    class TypeException : public Exception
    {
        public:
            TypeException(const QString& description)
                : Exception(description) {}
            virtual ~TypeException() {}
            virtual const QString type() { return i18n("TypeException"); }
    };

}}

#endif

