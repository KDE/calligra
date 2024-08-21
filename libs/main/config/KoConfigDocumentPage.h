/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002 Laurent Montel <lmontel@mandrakesoft.com>
   SPDX-FileCopyrightText: 2006-2007 Jan Hambrecht <jaham@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QWidget>

#include "komain_export.h"

class KoDocument;

class KOMAIN_EXPORT KoConfigDocumentPage : public QWidget
{
    Q_OBJECT

public:
    explicit KoConfigDocumentPage(KoDocument *doc, char *name = nullptr);
    ~KoConfigDocumentPage() override;

    void apply();

public Q_SLOTS:
    void slotDefault();

private:
    class Private;
    std::unique_ptr<Private> const d;
};
