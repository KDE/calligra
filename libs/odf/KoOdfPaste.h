/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOODFPASTE_H
#define KOODFPASTE_H

#include "KoOdf.h"
#include "koodf_export.h"
#include "KoXmlReaderForward.h"

class QMimeData;
class QByteArray;
class KoOdfReadStore;

/**
 * This is a helper class to help you paste odf snippets.
 */
class KOODF_EXPORT KoOdfPaste
{
public:
    KoOdfPaste();
    virtual ~KoOdfPaste();

    bool paste(KoOdf::DocumentType documentType, const QMimeData *data);
    /**
     * This is an overloaded member function, provided for convenience. It differs
     * from the above function only in what argument(s) it accepts.
     */
    bool paste(KoOdf::DocumentType documentType, const QByteArray &data);

protected:
    virtual bool process(const KoXmlElement &body, KoOdfReadStore &odfStore) = 0;
};

#endif /* KOODFPASTE_H */
