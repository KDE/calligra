/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Shantanu Tushar <shantanu@kde.org>
 * SPDX-FileCopyrightText: 2013 Sujith Haridasan <sujith.h@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#ifndef CQIMAGEPROVIDER_H
#define CQIMAGEPROVIDER_H

#include <QDeclarativeImageProvider>
#include <QHash>

class CQImageProvider : public QDeclarativeImageProvider
{
public:
    static const char identificationString[];
    static CQImageProvider *s_imageProvider;

    CQImageProvider();
    virtual ~CQImageProvider();
    virtual QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

    void addImage(const QString &id, const QImage &image);
    bool containsId(const QString &id);
    void clearCache();

private:
    QHash<QString, QImage> m_images;
};

#endif // CQIMAGEPROVIDER_H
