/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef OKULARODPGENERATOR_H
#define OKULARODPGENERATOR_H

#include "kookulargenerator_odp_export.h"

#include <okular/core/document.h>
#include <okular/core/generator.h>
#include <okular/core/version.h>

class KoPADocument;

class KOOKULARGENERATOR_ODP_EXPORT OkularOdpGenerator : public Okular::Generator
{
    Q_OBJECT
    Q_INTERFACES(Okular::Generator)

public:
    OkularOdpGenerator(QObject *parent, const QVariantList &args);
    ~OkularOdpGenerator();

    bool loadDocument(const QString &fileName, QVector<Okular::Page *> &pages) override;

    bool canGeneratePixmap() const override;
    void generatePixmap(Okular::PixmapRequest *request) override;

    Okular::DocumentInfo generateDocumentInfo(const QSet<Okular::DocumentInfo::Key> &keys) const override;

protected:
    bool doCloseDocument() override;

private:
    const KoPADocument *m_doc;

    Okular::DocumentInfo m_documentInfo;
};

#endif
