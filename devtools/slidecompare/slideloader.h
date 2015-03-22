/* This file is part of the KDE project
   Copyright (C) 2010 KO GmbH <jos.van.den.oever@kogmbh.com>

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
 * Boston, MA 02110-1301, USA.
*/
#ifndef SLIDELOADER_H
#define SLIDELOADER_H

#include <QObject>
#include <QSize>
#include <QPixmap>

class SlideLoader : public QObject {
private:
Q_OBJECT
public:
    explicit SlideLoader(QObject* parent = 0) :QObject(parent) {}
    virtual int numberOfSlides() = 0;
    virtual QSize slideSize() = 0;
    virtual int slideVersion(int position) = 0;
    virtual QPixmap loadSlide(int number, const QSize& maxsize) = 0;
Q_SIGNALS:
    void slidesChanged();
};

#endif
