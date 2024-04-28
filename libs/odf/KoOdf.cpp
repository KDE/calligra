/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2010 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoOdf.h"

namespace KoOdf
{
struct DocumentData {
    const char *mimeType;
    const char *templateMimeType;
    const char *bodyContentElement;
};

const DocumentData s_documentData[] = {
    {"application/vnd.oasis.opendocument.text", "application/vnd.oasis.opendocument.text-template", "office:text"},
    {"application/vnd.oasis.opendocument.graphics", "application/vnd.oasis.opendocument.graphics-template", "office:drawing"},
    {"application/vnd.oasis.opendocument.presentation", "application/vnd.oasis.opendocument.presentation-template", "office:presentation"},
    {"application/vnd.oasis.opendocument.spreadsheet", "application/vnd.oasis.opendocument.spreadsheet-template", "office:spreadsheet"},
    {"application/vnd.oasis.opendocument.chart", "application/vnd.oasis.opendocument.chart-template", "office:chart"},
    {"application/vnd.oasis.opendocument.image", "application/vnd.oasis.opendocument.image-template", "office:image"},
    // TODO what is the element for a formula check if bodyContentElement is ok
    {"application/vnd.oasis.opendocument.formula", "application/vnd.oasis.opendocument.formula-template", "office:XXX"},
    {"application/x-openoffice-embed-source-xml;windows_formatname=\"Star Embed Source (XML)\"", "", "office:text"}};
//"application/vnd.oasis.opendocument.text-master"
//"application/vnd.oasis.opendocument.text-web"

const char *mimeType(DocumentType documentType)
{
    return s_documentData[documentType].mimeType;
}

const char *templateMimeType(DocumentType documentType)
{
    return s_documentData[documentType].templateMimeType;
}

const char *bodyContentElement(DocumentType documentType, bool withNamespace)
{
    return withNamespace ? s_documentData[documentType].bodyContentElement : s_documentData[documentType].bodyContentElement + 7;
}

}
