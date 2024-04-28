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
#ifndef CSTHUMBPROVIDERTABLES_H
#define CSTHUMBPROVIDERTABLES_H

#include "CSThumbProvider.h"

namespace Calligra
{
namespace Sheets
{
class Doc;
}
}

class CSThumbProviderTables : public CSThumbProvider
{
public:
    explicit CSThumbProviderTables(Calligra::Sheets::Doc *doc);
    ~CSThumbProviderTables() override;

    QVector<QImage> createThumbnails(const QSize &thumbSize) override;

private:
    Calligra::Sheets::Doc *m_doc;
};

#endif /* CSTHUMBPROVIDERTABLES_H */
