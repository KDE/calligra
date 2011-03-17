/*
 * This file is part of Maemo 5 Office UI for KOffice
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Sugnan Prabhu S <sugnan.prabhu@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "KoDocumentRdfBase.h"

//#include <KoDataCenterBase.h>
//#include <kconfig.h>

#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <Soprano/Soprano>
#include <QTextBlockUserData>
#include <QTreeWidgetItem>

class QDomDocument;
class KoStore;
class KoXmlWriter;
class KoDocument;
class KoCanvasBase;
class KoTextEditor;
class RdfInfoDialog;
class KoTextInlineRdf;
class KoTextDocument;
class KWDocument;

class FoDocumentRdf : public KoDocumentRdfBase
{
    Q_OBJECT

public:

    FoDocumentRdf(KoDocument *parent , KoTextEditor *m_editor);

    ~FoDocumentRdf();

    virtual Soprano::Model *model() const;

    bool loadRdf(KoStore *store, const Soprano::Parser *parser, const QString &fileName);

    bool loadOasis(KoStore *store);


    void freshenBNodes(Soprano::Model *m);

    void findStatements(QTextCursor &cursor, int depth = 1);

    Soprano::Statement toStatement(KoTextInlineRdf *inlineRdf) const;

    void addStatements(Soprano::Model *model, const QString &xmlid);

    void expandStatementsReferencingSubject(Soprano::Model *model);

    void expandStatementsToIncludeOtherPredicates(Soprano::Model *model);

    void expandStatements(Soprano::Model *model);

    void expandStatementsToIncludeRdfListsRecurse(Soprano::Model *model,
            QList<Soprano::Statement> &addList,
            const Soprano::Node &n);

    Soprano::Node inlineRdfContext() const;

private:
    /// reimplemented
    virtual bool completeLoading(KoStore *store);

    /// reimplemented
    virtual bool completeSaving(KoStore *store, KoXmlWriter *manifestWriter, KoShapeSavingContext *context);

void removeStatementsIfTheyExist(Soprano::Model *m, const QList<Soprano::Statement> &removeList);
    Soprano::Model *m_model;

    KoDocument *m_doc;
    KoTextEditor *m_editor;

    QList<QString> subject;
    QList<QString> predicate;
    QList<QString> object;


public:
    RdfInfoDialog *rdfInfoDialog;
    bool inRdfMode;

public slots:
    void highlightRdf();
};

