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

#include "Settings.h"

class Settings::Private
{
public:

    Private()
        : imageWidth(1024)
        , imageHeight(768)
        , imageResolution(300)
        , useClipBoard(false)
        , useWebCam(false)
    {}


    QString currentFile;
    int imageWidth;
    int imageHeight;
    int imageResolution;
    bool useClipBoard;
    bool useWebCam;
};

Settings::Settings( QObject* parent )
    : QObject( parent ), d( new Private )
{
}

Settings::~Settings()
{
    delete d;
}


QString Settings::currentFile() const
{
    return d->currentFile;
}

void Settings::setCurrentFile(const QString& fileName)
{
    d->currentFile = fileName;
    emit currentFileChanged();
}

int Settings::imageWidth() const
{
    return d->imageWidth;
}

void Settings::setImageWidth(int imageWidth)
{
    d->imageWidth = imageWidth;
}


int Settings::imageHeight() const
{
    return d->imageHeight;
}

void Settings::setImageHeight(int imageHeight)
{
    d->imageHeight = imageHeight;
}

int Settings::imageResolution() const
{
    return d->imageResolution;
}

void Settings::setImageResolution(int imageResolution)
{
    d->imageResolution = imageResolution;
}


bool Settings::useClipBoard() const
{
    return d->useClipBoard;
}

void Settings::setUseClipBoard(bool useClipBoard)
{
    d->useClipBoard = useClipBoard;
}


bool Settings::useWebCam() const
{
    return d->useWebCam;
}

void Settings::setUseWebCam(bool useWebCam)
{
    d->useWebCam = useWebCam;
}


#include "Settings.moc"
