/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOODF_H
#define KOODF_H

#include "koodf_export.h"

namespace KoOdf
{
    enum DocumentType {
        Text,
        Graphics,
        Presentation,
        Spreadsheet,
        Chart,
        Image,
        Formula,
        OpenOfficeClipboard
    };

    /**
     * Get the mime type
     *
     * @param documentType the document type
     * @return the mime type used for the given document type
     */
    KOODF_EXPORT const char * mimeType(DocumentType documentType);

    /**
     * Get the mime type
     *
     * @param documentType the document type
     * @return the mime type used for templates of the given document type
     */
    KOODF_EXPORT const char * templateMimeType(DocumentType documentType);

    /**
     * Get the mime type
     *
     * @param documentType the document type
     * @param withNamespace if true the namespace before the element is also returned
     *                      if false only the element is returned
     * @return the body element name for the given document type
     */
    KOODF_EXPORT const char * bodyContentElement(DocumentType documentType, bool withNamespace);
}

#endif /* KOODF_H */
