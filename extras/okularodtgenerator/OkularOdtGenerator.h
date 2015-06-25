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

#include <okular/core/generator.h>
#include <okular/core/document.h>
#include <okular/core/version.h>

// add dummy macro, no longer exists, remove post 3.0
#ifndef OKULAR_IS_VERSION
#define OKULAR_IS_VERSION(a,b,c) 1
#endif

class KWDocument;

class OkularOdtGenerator : public Okular::Generator
{
public:
    OkularOdtGenerator( QObject *parent, const QVariantList &args );
    ~OkularOdtGenerator();

    bool loadDocument( const QString &fileName, QVector<Okular::Page*> &pages );

    bool canGeneratePixmap() const;
    void generatePixmap( Okular::PixmapRequest *request );

#if OKULAR_IS_VERSION(0, 20, 60)
    Okular::DocumentInfo generateDocumentInfo( const QSet<Okular::DocumentInfo::Key> &keys ) const;
#else
    const Okular::DocumentInfo* generateDocumentInfo();
#endif
    const Okular::DocumentSynopsis* generateDocumentSynopsis();

protected:
    bool doCloseDocument();

private:
    KWDocument* m_doc;

    Okular::DocumentInfo m_documentInfo;
    Okular::DocumentSynopsis m_documentSynopsis;

};

#endif
