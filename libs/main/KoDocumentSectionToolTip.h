/*
  SPDX-FileCopyrightText: 2006 Gábor Lehel <illissius@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KO_DOCUMENT_SECTION_TOOLTIP_H
#define KO_DOCUMENT_SECTION_TOOLTIP_H

#include "KoItemToolTip.h"

class KoDocumentSectionModel;

/**
 * A default tooltip for a KoDocumentSection that shows a thumbnail
 * image and the list of properties associated with the document model.
 */
class KoDocumentSectionToolTip: public KoItemToolTip
{
    Q_OBJECT

public:
    KoDocumentSectionToolTip();
    ~KoDocumentSectionToolTip() override;

protected:
    QTextDocument *createDocument(const QModelIndex &index) override;

private:
    typedef KoDocumentSectionModel Model;
};

#endif
