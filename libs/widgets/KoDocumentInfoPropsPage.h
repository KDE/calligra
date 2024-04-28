/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2000 Simon Hausmann <hausmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef _KODOCUMENTINFOPROPSPAGE_H_
#define _KODOCUMENTINFOPROPSPAGE_H_

#include "kowidgets_export.h"
#include <KPropertiesDialogPlugin>

class KOWIDGETS_EXPORT KoDocumentInfoPropsPage : public KPropertiesDialogPlugin
{
    Q_OBJECT

public:
    explicit KoDocumentInfoPropsPage(KPropertiesDialog *props, const QVariantList & = QVariantList());
    ~KoDocumentInfoPropsPage() override;

    void applyChanges() override;

private:
    class KoDocumentInfoPropsPagePrivate;
    KoDocumentInfoPropsPagePrivate *const d;
};

#endif
