/* This file is part of the KDE project
 * Copyright (C) 2012 Smit Patel <smitpatel24@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KOBRIDGESERVER_H
#define KOBRIDGESERVER_H

#include "kotext_export.h"

#include <QObject>
#include <qjson/parser.h>

class QLocalServer;
class QLocalSocket;
class QDir;
class KoTextEditor;
class QSignalMapper;

class KOTEXT_EXPORT KoBridgeServer : public QObject
{
    Q_OBJECT
public:
    explicit KoBridgeServer(KoTextEditor *editor, QObject *parent = 0);
    ~KoBridgeServer();
    void initServer();

    static const QString pipeIn;
    
public slots:
    void handleNewEngine();
    void handle(QObject *o);
private:
    QLocalServer *m_server;
    KoTextEditor *m_editor;
    QSignalMapper *m_mapper;
    QJson::Parser m_parser;
    bool m_ok;
};

#endif // KOBRIDGESERVER_H
