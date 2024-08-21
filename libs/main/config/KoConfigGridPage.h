/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002 Laurent Montel <lmontel@mandrakesoft.com>
   SPDX-FileCopyrightText: 2006-2007 Jan Hambrecht <jaham@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QWidget>

#include "komain_export.h"

class KoUnit;
class KoDocument;
class KoGridData;

class KOMAIN_EXPORT KoConfigGridPage : public QWidget
{
    Q_OBJECT

public:
    explicit KoConfigGridPage(KoDocument *doc, char *name = nullptr);
    ~KoConfigGridPage() override;

    void apply();

public Q_SLOTS:
    void slotDefault();
    void slotUnitChanged(const KoUnit &unit);
    void spinBoxHSpacingChanged(qreal);
    void spinBoxVSpacingChanged(qreal);

private:
    void setValuesFromGrid(const KoGridData &grid);

    class Private;
    Private *const d;
};
