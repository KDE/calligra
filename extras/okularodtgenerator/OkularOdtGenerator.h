/* This file is part of the KDE project
   Copyright (C) 2012 Sven Langkamp <sven.langkamp@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef OKULARODTGENERATOR_H
#define OKULARODTGENERATOR_H

#include "kookulargenerator_odt_export.h"

#include <okular/core/generator.h>
#include <okular/core/document.h>
#include <okular/core/version.h>

class KWDocument;

class KOOKULARGENERATOR_ODT_EXPORT OkularOdtGenerator : public Okular::Generator
{
    Q_OBJECT
    Q_INTERFACES( Okular::Generator )

public:
    OkularOdtGenerator( QObject *parent, const QVariantList &args );
    ~OkularOdtGenerator();

    bool loadDocument( const QString &fileName, QVector<Okular::Page*> &pages ) override;

    bool canGeneratePixmap() const override;
    void generatePixmap( Okular::PixmapRequest *request ) override;

    Okular::DocumentInfo generateDocumentInfo( const QSet<Okular::DocumentInfo::Key> &keys ) const override;
    const Okular::DocumentSynopsis* generateDocumentSynopsis() override;

    bool canGenerateTextPage() const override;

protected:
    bool doCloseDocument() override;
    Okular::TextPage* textPage( Okular::Page *page );

private:
    KWDocument* m_doc;

    Okular::DocumentInfo m_documentInfo;
    Okular::DocumentSynopsis m_documentSynopsis;

};

#endif
