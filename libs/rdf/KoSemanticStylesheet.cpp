/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "KoSemanticStylesheet.h"

#include "KoChangeTrackerDisabledRAII.h"
#include "KoDocumentRdf.h"
// main
#include <KoDocument.h>
#include <KoTextDocument.h>
#include <KoTextEditor.h>
// KF5
#include <kdebug.h>
// Qt
#include <QCoreApplication>

class KoSemanticStylesheetPrivate
{
public:
    QString m_uuid;
    QString m_name;
    QString m_templateString;
    QString m_type;
    bool m_isMutable;

    KoSemanticStylesheetPrivate(const QString &uuid,
                                const QString &name,
                                const QString &templateString,
                                const QString &type = QLatin1String("System"),
                                bool isMutable = false)
        : m_uuid(uuid)
        , m_name(name)
        , m_templateString(templateString)
        , m_type(type)
        , m_isMutable(isMutable)
    {
    }
};

KoSemanticStylesheet::KoSemanticStylesheet(const QString &uuid, const QString &name, const QString &templateString, const QString &type, bool isMutable)
    : QObject(QCoreApplication::instance())
    , d(new KoSemanticStylesheetPrivate(uuid, name, templateString, type, isMutable))
{
}

KoSemanticStylesheet::~KoSemanticStylesheet()
{
    delete d;
}

QString KoSemanticStylesheet::uuid() const
{
    return d->m_uuid;
}

QString KoSemanticStylesheet::name() const
{
    return d->m_name;
}

QString KoSemanticStylesheet::templateString() const
{
    return d->m_templateString;
}

QString KoSemanticStylesheet::type() const
{
    return d->m_type;
}

bool KoSemanticStylesheet::isMutable() const
{
    return d->m_isMutable;
}

void KoSemanticStylesheet::name(const QString &v)
{
    if (d->m_isMutable) {
        Q_EMIT nameChanging(hKoSemanticStylesheet(this), d->m_name, v);
        d->m_name = v;
    }
}

void KoSemanticStylesheet::templateString(const QString &v)
{
    if (d->m_isMutable) {
        d->m_templateString = v;
    }
}

void KoSemanticStylesheet::format(hKoRdfSemanticItem obj, KoTextEditor *editor, const QString &xmlid)
{
    Q_ASSERT(obj);
    Q_ASSERT(editor);
    kDebug(30015) << "formatting obj:" << obj << " name:" << obj->name();
    kDebug(30015) << "xmlid:" << xmlid << " editor:" << editor << " sheet-name:" << name();
    const KoDocumentRdf *rdf = obj->documentRdf();
    Q_ASSERT(rdf);
    Q_ASSERT(editor);
    QPair<int, int> p;
    if (xmlid.size()) {
        p = rdf->findExtent(xmlid);
    } else {
        p = rdf->findExtent(editor);
    }
    int startpos = p.first + 1;
    int endpos = p.second;
    if (!endpos) {
        kDebug(30015) << "format() invalid range, skipping! start:" << startpos << " end:" << endpos;
        return;
    }
    KoTextDocument ktd(editor->document());
    KoChangeTrackerDisabledRAII disableChangeTracker(ktd.changeTracker());
    editor->beginEditBlock();
    editor->setPosition(startpos, QTextCursor::MoveAnchor);
    editor->movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, endpos - startpos);
    QString oldText = editor->selectedText();
    if (editor->hasSelection()) {
        editor->deleteChar(); // deletes the selection
    }
    editor->setPosition(startpos, QTextCursor::MoveAnchor);
    kDebug(30015) << "formatting start:" << startpos << " end:" << endpos;
    kDebug(30015) << "semantic item oldText:" << oldText;
    QString data = templateString();
    QMap<QString, QString> m;
    m["%NAME%"] = obj->name();
    obj->setupStylesheetReplacementMapping(m);

    for (QMap<QString, QString>::iterator mi = m.begin(); mi != m.end(); ++mi) {
        QString k = mi.key();
        QString v = mi.value();
        data.replace(k, v);
    }
    // make sure there is something in the replacement other than commas and spaces
    QString tmpstring = data;
    tmpstring.remove(' ');
    tmpstring.remove(',');
    if (!tmpstring.size()) {
        kDebug(30015) << "stylesheet results in empty data, using name() instead";
        data = name();
    }
    kDebug(30015) << "Updating with new formatting:" << data;
    editor->insertText(data);
    editor->setPosition(startpos, QTextCursor::MoveAnchor);
    editor->endEditBlock();
}

QString KoSemanticStylesheet::stylesheetTypeSystem()
{
    return QLatin1String("System");
}

QString KoSemanticStylesheet::stylesheetTypeUser()
{
    return QLatin1String("User");
}
