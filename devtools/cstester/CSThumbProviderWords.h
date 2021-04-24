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
