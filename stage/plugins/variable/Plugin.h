/*
* This file is part of the KDE project
*
* SPDX-FileCopyrightText: 2009 Nokia Corporation and /or its subsidiary(-ies).
*
* Contact: Amit Aggarwal <amitcs06@gmail.com>
*            <amit.5.aggarwal@nokia.com>
*
SPDX-License-Identifier: LGPL-2.1-or-later
*
*/

#ifndef PLUGIN_H
#define PLUGIN_H

#include <QObject>

#include <QVariantList>

class Plugin : public QObject
{
    Q_OBJECT
public:
    Plugin(QObject *parent, const QVariantList &);
};

#endif
