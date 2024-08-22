/*
 * This file is part of Calligra
 *
 * SPDX-FileCopyrightText: 2011 Nokia Corporation and /or its subsidiary(-ies).
 *
 * Contact: Thorsten Zachmann thorsten.zachmann@nokia.com
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */
#ifndef CSTHUMBPROVIDER_H
#define CSTHUMBPROVIDER_H

#include <QVector>

class QImage;
class QSize;

class CSThumbProvider
{
public:
    CSThumbProvider() = default;
    virtual ~CSThumbProvider() = default;

    virtual QVector<QImage> createThumbnails(const QSize &thumbSize) = 0;
};

#endif /* CSTHUMBPROVIDER_H */
