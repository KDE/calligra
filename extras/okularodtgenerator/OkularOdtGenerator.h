/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef OKULARODTGENERATOR_H
#define OKULARODTGENERATOR_H

#include "kookulargenerator_odt_export.h"

#include <okular/core/document.h>
#include <okular/core/generator.h>
#include <okular/core/version.h>

class KWDocument;

class KOOKULARGENERATOR_ODT_EXPORT OkularOdtGenerator : public Okular::Generator
{
    Q_OBJECT
    Q_INTERFACES(Okular::Generator)

public:
    OkularOdtGenerator(QObject *parent, const QVariantList &args);
    ~OkularOdtGenerator();

    bool loadDocument(const QString &fileName, QVector<Okular::Page *> &pages) override;

    bool canGeneratePixmap() const override;
    void generatePixmap(Okular::PixmapRequest *request) override;

    Okular::DocumentInfo generateDocumentInfo(const QSet<Okular::DocumentInfo::Key> &keys) const override;
    const Okular::DocumentSynopsis *generateDocumentSynopsis() override;

    bool canGenerateTextPage() const override;

protected:
    bool doCloseDocument() override;
#if OKULAR_VERSION < 0x010400
    Okular::TextPage *textPage(Okular::Page *page) override;
#else
    Okular::TextPage *textPage(Okular::Page *page);
    Okular::TextPage *textPage(Okular::TextRequest *request) override;
#endif

private:
    KWDocument *m_doc;

    Okular::DocumentInfo m_documentInfo;
    Okular::DocumentSynopsis m_documentSynopsis;
};

#endif
