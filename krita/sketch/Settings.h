/* This file is part of the KDE project
 * Copyright (C) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QString>

class Settings : public QObject
{

    Q_OBJECT
    Q_PROPERTY(QString currentFile READ currentFile WRITE setCurrentFile NOTIFY currentFileChanged)

    Q_PROPERTY(int imageWidth READ imageWidth WRITE setImageWidth)
    Q_PROPERTY(int imageHeight READ imageHeight WRITE setImageHeight)
    Q_PROPERTY(int imageResolution READ imageResolution WRITE setImageResolution)
    Q_PROPERTY(bool useClipBoard READ useClipBoard WRITE setUseClipBoard)
    Q_PROPERTY(bool useWebCam READ useWebCam WRITE setUseWebCam)

public:
    explicit Settings( QObject* parent = 0 );
    virtual ~Settings();

public Q_SLOTS:

    QString currentFile() const;
    void setCurrentFile(const QString &fileName);

    int imageWidth() const;
    void setImageWidth(int imageWidth);

    int imageHeight() const;
    void setImageHeight(int imageHeight);

    int imageResolution() const;
    void setImageResolution(int imageResolution);

    bool useClipBoard() const;
    void setUseClipBoard(bool useClipBoard);

    bool useWebCam() const;
    void setUseWebCam(bool useWebCam);

Q_SIGNALS:

    void currentFileChanged();

private:
    class Private;
    Private* const d;
};

#endif // SETTINGS_H
