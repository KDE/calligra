/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <jos.van.den.oever@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KPRESENTERSLIDELOADER_H
#define KPRESENTERSLIDELOADER_H

#include "slideloader.h"

class KoPADocument;

class KPresenterSlideLoader : public SlideLoader
{
private:
    const KoPADocument *m_doc;
    int version;

public:
    explicit KPresenterSlideLoader(QObject *parent = nullptr);
    ~KPresenterSlideLoader();
    int numberOfSlides();
    QSize slideSize();
    int slideVersion(int /*position*/)
    {
        // version is independent of position for this loader
        return version;
    }
    QPixmap loadSlide(int number, const QSize &maxsize);
    void open(const QString &path);
    void close();
};

#endif
