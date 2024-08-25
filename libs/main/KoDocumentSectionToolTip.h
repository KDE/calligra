/*
  SPDX-FileCopyrightText: 2006 Gábor Lehel <illissius@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "KoItemToolTip.h"

class KoDocumentSectionModel;

/**
 * A default tooltip for a KoDocumentSection that shows a thumbnail
 * image and the list of properties associated with the document model.
 */
class KoDocumentSectionToolTip : public KoItemToolTip
{
public:
    KoDocumentSectionToolTip();
    ~KoDocumentSectionToolTip() override;

protected:
    [[nodiscard]] QString createDocument(const QModelIndex &index) const override;

private:
    using Model = KoDocumentSectionModel;
};
