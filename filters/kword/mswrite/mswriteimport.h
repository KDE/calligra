/* This file is part of the KDE project
   Copyright (C) 2001-2003 Clarence Dang <dang@kde.org>
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
   IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
   OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
   IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
   THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MSWRITEIMPORT_H
#define MSWRITEIMPORT_H

#include <KoFilter.h>
//Added by qt3to4:
#include <QByteArray>

class WRIDevice;
class MSWrite::InternalParser;
class KWordGenerator;

class MSWriteImport : public KoFilter
{
    Q_OBJECT

private:
    WRIDevice *m_device;
    MSWrite::InternalParser *m_parser;
    KWordGenerator *m_generator;

public:
    MSWriteImport(QObject *parent, const QStringList &);
    virtual ~MSWriteImport();

    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to);

    void sigProgress(const int value) {
        emit KoFilter::sigProgress(value);
    }
};

#endif // MSWRITEIMPORT_H
