/*
 * This file is part of Calligra
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Thorsten Zachmann thorsten.zachmann@nokia.com
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
#ifndef CSTHUMBPROVIDERWORDS_H
#define CSTHUMBPROVIDERWORDS_H

#include "CSThumbProvider.h"

class KWDocument;

class CSThumbProviderWords : public CSThumbProvider
{
public:
    explicit CSThumbProviderWords(KWDocument *doc);
    ~CSThumbProviderWords() override;

    QVector<QImage> createThumbnails(const QSize &thumbSize) override;

private:
    KWDocument *m_doc;
};

#endif /* CSTHUMBPROVIDERWORDS_H */
