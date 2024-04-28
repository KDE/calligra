/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002 Laurent Montel <lmontel@mandrakesoft.com>
   SPDX-FileCopyrightText: 2006-2007 Jan Hambrecht <jaham@gmx.net>
   SPDX-FileCopyrightText: 2012 C. Boemann <cbo@boemann.dk>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOCONFIGAUTHORPAGE_H
#define KOCONFIGAUTHORPAGE_H

#include <QWidget>

#include "kowidgets_export.h"

class KOWIDGETS_EXPORT KoConfigAuthorPage : public QWidget
{
    Q_OBJECT

public:
    KoConfigAuthorPage();
    ~KoConfigAuthorPage() override;

    void apply();

private Q_SLOTS:
    void profileChanged(int i);
    void addUser();
    void deleteUser();

private:
    class Private;
    Private *const d;
};

#endif // KOCONFIGAUTHORPAGE_H
