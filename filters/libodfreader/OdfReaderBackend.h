/* This file is part of the KDE project

   Copyright (C) 2013-2014 Inge Wallin       <inge@lysator.liu.se>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef ODFREADERBACKEND_H
#define ODFREADERBACKEND_H

// Calligra
#include <KoXmlStreamReader.h>
#include <KoFilter.h>

// this library
#include "koodfreader_export.h"
#include "OdfReader.h"


class QByteArray;
class QSizeF;
class QStringList;
class KoStore;
class OdfReaderContext;


/** @brief A default backend for the OdfReader class.
 *
 * This class defines an interface and the default behaviour for the
 * backend to the ODF reader (@see OdfReader). When the
 * reader is called upon to traverse a certain XML tree, there will
 * be two parameters to the root traverse function: a pointer to a
 * backend object and a pointer to a context object.
 *
 * The reader will traverse (read) the XML tree and for every element
 * it comes across it will call a specific function in the backend and
 * every call will pass the pointer to the context object.
 *
 * Each supported XML tag has a corresponding callback function. This
 * callback function will be called twice: once when the tag is first
 * encountered anc once when the tag is closed.  This means that an
 * element with no child elements will be called twice in succession.
 *
 * The callback function receives as parameter a reference to the XML
 * stream reader. From this parameter it can be deduced if the call is
 * for a start element or an end element and also access the
 * attributes of the element.
 *
 * This class defines a virtual function for every supported
 * element. It also implements a default behaviour for every element
 * which is to ignore the parameters and do nothing.
 *
 * When this class is used e.g. in a filter it is recommended to
 * inherit this class and only reimplement those functions that are
 * actually needed.
 */
class KOODFREADER_EXPORT OdfReaderBackend
{
 public:
    explicit OdfReaderBackend();
    virtual ~OdfReaderBackend();

    // ----------------------------------------------------------------
    // ODF document level functions

    DECLARE_BACKEND_FUNCTION(OfficeDocumentcontent);
    DECLARE_BACKEND_FUNCTION(OfficeBody);

 private:
    class Private;
    Private * const d;
};


#endif // ODFREADERBACKEND_H
