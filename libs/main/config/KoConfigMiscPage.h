/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002 Laurent Montel <lmontel@mandrakesoft.com>
   SPDX-FileCopyrightText: 2006-2007 Jan Hambrecht <jaham@gmx.net>
   SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOCONFIGMISCPAGE_H
#define KOCONFIGMISCPAGE_H

#include <KoVBox.h>

#include "komain_export.h"

class KoUnit;
class KoDocument;
class KoDocumentResourceManager;

class KOMAIN_EXPORT KoConfigMiscPage : public KoVBox
{
    Q_OBJECT

public:
    explicit KoConfigMiscPage(KoDocument *doc, KoDocumentResourceManager *documentResources, char *name = 0L);
    ~KoConfigMiscPage() override;

    void apply();

Q_SIGNALS:
    void unitChanged(const KoUnit &unit);

public Q_SLOTS:
    void slotDefault();
    void slotUnitChanged(int);

private:
    class Private;
    Private *const d;
};

#endif // KOCONFIGMISCPAGE_H
