/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2013-2014 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ODTREADERBACKEND_H
#define ODTREADERBACKEND_H

// Calligra
#include <KoFilter.h>
#include <KoXmlStreamReader.h>

// this library
#include "OdfReaderBackend.h"
#include "OdfReaderInternals.h"
#include "OdtReader.h"
#include "koodfreader_export.h"

class OdfReaderContext;

/** @brief A default backend for the OdtReader class.
 *
 * This class defines an interface and the default behaviour for the
 * backend to the ODT reader (@see OdtReader). When the
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
class KOODFREADER_EXPORT OdtReaderBackend : public OdfReaderBackend
{
public:
    explicit OdtReaderBackend();
    ~OdtReaderBackend() override;

    // ----------------------------------------------------------------
    // ODT document level functions

    DECLARE_BACKEND_FUNCTION(OfficeText);

private:
    class Private;
    Private *const d;
};

#endif // ODTREADERBACKEND_H
