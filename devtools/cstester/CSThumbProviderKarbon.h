/*
 * This file is part of Calligra
 *
 * Copyright (C) 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
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
