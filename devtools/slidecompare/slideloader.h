/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <jos.van.den.oever@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef SLIDELOADER_H
#define SLIDELOADER_H

#include <QObject>
#include <QPixmap>
#include <QSize>

class SlideLoader : public QObject
{
private:
    Q_OBJECT
public:
    explicit SlideLoader(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
    virtual int numberOfSlides() = 0;
    virtual QSize slideSize() = 0;
    virtual int slideVersion(int position) = 0;
    virtual QPixmap loadSlide(int number, const QSize &maxsize) = 0;
Q_SIGNALS:
    void slidesChanged();
};

#endif
