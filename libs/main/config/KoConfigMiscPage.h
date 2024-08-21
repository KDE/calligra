/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002 Laurent Montel <lmontel@mandrakesoft.com>
   SPDX-FileCopyrightText: 2006-2007 Jan Hambrecht <jaham@gmx.net>
   SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QWidget>

#include "komain_export.h"

class KoUnit;
class KoDocument;
class KoDocumentResourceManager;

class KOMAIN_EXPORT KoConfigMiscPage : public QWidget
{
    Q_OBJECT

public:
    explicit KoConfigMiscPage(KoDocument *doc, KoDocumentResourceManager *documentResources, char *name = nullptr);
    ~KoConfigMiscPage() override;

    void apply();

Q_SIGNALS:
    void unitChanged(const KoUnit &unit);

public Q_SLOTS:
    void slotDefault();
    void slotUnitChanged(int);

private:
    class Private;
    std::unique_ptr<Private> const d;
};
