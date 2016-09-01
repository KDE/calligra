/* This file is part of the KDE project
 * Copyright (C) 2016 Dag Andersen <danders@get2net>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#ifndef CommandLineParser_H
#define CommandLineParser_H

#include <QObject>
#include <QCommandLineParser>

class KPlatoWork_MainWindow;
class QDir;

class CommandLineParser : public QObject
{
    Q_OBJECT

public:
    CommandLineParser();
    ~CommandLineParser();

public:
    void handleCommandLine(const QDir &workingDirectory);

public Q_SLOTS:
    void handleActivateRequest(const QStringList &arguments, const QString &workingDirectory);

private:
    KPlatoWork_MainWindow *m_mainwindow;
    QCommandLineParser m_commandLineParser;
};


#endif // CommandLineParser_H

