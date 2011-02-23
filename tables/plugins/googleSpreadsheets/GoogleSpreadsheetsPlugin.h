/* This file is part of the KDE project
   Copyright (C) 2010-2011 Christoph Goerlich <chgoerlich@gmx.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

//TODO
/*
    * make a general Calligra Plugin
    * add support for Text Documents, Presentations and Drawings
*/

#ifndef GOOGLESPREADSHEETSPLUGIN_H
#define GOOGLESPREADSHEETSPLUGIN_H

#include <kparts/plugin.h>


class KoView;
class QByteArray;


class GoogleSpreadsheetsPlugin : public KParts::Plugin
{
    Q_OBJECT
    
public:
    explicit GoogleSpreadsheetsPlugin(QObject* parent, const QStringList& list);
    virtual ~GoogleSpreadsheetsPlugin();
    KoView *parentView() const;
    void enableMenuActions();
    
    void startProgress(int maximum = 0, int minimum = 0, QString msg = QString());
    void setProgressMaximum(int maximum);
    int progressMaximum() const;
    void endProgress();
    
    void setStatusMessage(QString msg);
    void addToRecentUrl();
    
    void openExportedDocument(QByteArray& data, QString& docTitle);
    void disconnectKoDocsSignals();
    
public Q_SLOTS:
    void activate();
    void setProgress(int value);
    void slotModified(bool b);
    
    void slotCompleted(bool forceUpdate = false);

private:
    class Private;
    Private * const d;
};

#endif //GOOGLESPREADSHEETSOLUGIN_H
