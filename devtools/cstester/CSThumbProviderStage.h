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
#ifndef CSTHUMBPROVIDERSTAGE_H
#define CSTHUMBPROVIDERSTAGE_H

#include "CSThumbProvider.h"

class KoPADocument;

class CSThumbProviderStage : public CSThumbProvider
{
public:
    explicit CSThumbProviderStage(KoPADocument *doc);
    ~CSThumbProviderStage() override;

    QVector<QImage> createThumbnails(const QSize &thumbSize) override;

private:
    KoPADocument *m_doc;
};

#endif /* CSTHUMBPROVIDERSTAGE_H */
