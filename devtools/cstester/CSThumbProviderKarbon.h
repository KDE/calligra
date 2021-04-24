/*
 * This file is part of Calligra
 *
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */

#ifndef CSTHUMBPROVIDERKARBON_H
#define CSTHUMBPROVIDERKARBON_H

#include "CSThumbProvider.h"

class KarbonDocument;

class CSThumbProviderKarbon : public CSThumbProvider
{
public:
    explicit CSThumbProviderKarbon(KarbonDocument *doc);
    ~CSThumbProviderKarbon() override;

    QVector<QImage> createThumbnails(const QSize &thumbSize) override;

private:
    KarbonDocument *m_doc;
};

#endif // CSTHUMBPROVIDERKARBON_H
