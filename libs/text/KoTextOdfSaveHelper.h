/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOTEXTODFSAVEHELPER_H
#define KOTEXTODFSAVEHELPER_H

#include <KoDragOdfSaveHelper.h>
#ifdef SHOULD_BUILD_RDF
#include <QSharedPointer>
#include <Soprano/Soprano>
#endif
#include "kotext_export.h"

class QTextDocument;
class KoStyleManager;

class KOTEXT_EXPORT KoTextOdfSaveHelper : public KoDragOdfSaveHelper
{
public:
    KoTextOdfSaveHelper(const QTextDocument *document, int from, int to);
    ~KoTextOdfSaveHelper() override;

    /// reimplemented
    bool writeBody() override;

    KoShapeSavingContext *context(KoXmlWriter *bodyWriter, KoGenStyles &mainStyles, KoEmbeddedDocumentSaver &embeddedSaver) override;

#ifdef SHOULD_BUILD_RDF
    /**
     * The Rdf Model ownership is not taken, you must still delete it,
     * and you need to ensure that it lives longer than this object
     * unless you reset the model to 0.
     */
    void setRdfModel(QSharedPointer<Soprano::Model> m);
    QSharedPointer<Soprano::Model> rdfModel() const;
#endif

    KoStyleManager *styleManager() const;

private:
    struct Private;
    Private *const d;
};

#endif /* KOTEXTODFSAVEHELPER_H */
