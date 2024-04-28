/* This file is part of the Calligra project, made within the KDE community.

   SPDX-FileCopyrightText: 2012 Friedrich W. H. Kossebau <kossebau@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef XFIGIMPORTFILTER_H
#define XFIGIMPORTFILTER_H

// Calligra
#include <KoFilter.h>

typedef class QList<QVariant> QVariantList;

class XFigImportFilter : public KoFilter
{
    Q_OBJECT

public:
    XFigImportFilter(QObject *parent, const QVariantList &);
    ~XFigImportFilter() override;

public: // KoFilter API
    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;
};

#endif
