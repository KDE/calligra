/*
   This file is part of the KDE project
   Copyright (C) 2001 Ewald Snel <ewald@rambo.its.tudelft.nl>
   Copyright (C) 2001 Tomasz Grobelny <grotk@poczta.onet.pl>
   Copyright (C) 2003, 2004 Nicolas GOUTTE <goutte@kde.org>
   Copyright (C) 2011 Boudewijn Rempt <boud@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#ifndef __RTFIMPORT_H__
#define __RTFIMPORT_H__

#include <KoFilter.h>

#include <QVariantList>

class RTFImport : public KoFilter
{
    Q_OBJECT

public:

    RTFImport(QObject* parent, const QVariantList&);

    /**
     * Convert document from RTF to Words format.
     * @param from the mimetype for RTF
     * @param to the mimetype for Words
     * @return true if the document was successfully converted
     */
    KoFilter::ConversionStatus convert(const QByteArray& from, const QByteArray& to) override;
};



#endif
