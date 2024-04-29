/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Carlos Licea <carlos.licea@kdemail.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPAMASTERPAGEDOCKER_H
#define KOPAMASTERPAGEDOCKER_H

#include <QWidget>

class KoPAView;

class KoPAMasterPageDocker : public QWidget
{
    Q_OBJECT
public:
    explicit KoPAMasterPageDocker(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags())
    {
        Q_UNUSED(parent);
        Q_UNUSED(flags);
    }
    void setView(KoPAView *view)
    {
        Q_UNUSED(view);
    }
};

#endif // KOPAMASTERPAGEDOCKER_H
