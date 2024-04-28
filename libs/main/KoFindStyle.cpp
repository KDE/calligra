/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoFindStyle.h"
#include "KoFindOption.h"
#include "KoFindOptionSet.h"

#include <KoCharacterStyle.h>
#include <KoParagraphStyle.h>
#include <KoTextDocument.h>

#include <QAbstractTextDocumentLayout>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>

Q_DECLARE_METATYPE(QTextDocument *)
Q_DECLARE_METATYPE(QTextCursor)

class Q_DECL_HIDDEN KoFindStyle::Private
{
public:
    QList<QTextDocument *> documents;
    QHash<QTextDocument *, QVector<QAbstractTextDocumentLayout::Selection>> selections;

    static QTextCharFormat highlightFormat;
    void updateSelections();
};

QTextCharFormat KoFindStyle::Private::highlightFormat;

KoFindStyle::KoFindStyle(QObject *parent)
    : KoFindBase(parent)
    , d(new Private)
{
    KoFindOptionSet *options = new KoFindOptionSet();
    options->addOption("paragraphStyle", "Paragraph Style", QString(), QVariant::fromValue<int>(0));
    options->addOption("characterStyle", "Character Style", QString(), QVariant::fromValue<int>(0));
    setOptions(options);

    d->highlightFormat.setBackground(Qt::yellow);
}

KoFindStyle::~KoFindStyle()
{
    delete d;
}

QList<QTextDocument *> KoFindStyle::documents() const
{
    return d->documents;
}

void KoFindStyle::setDocuments(const QList<QTextDocument *> &list)
{
    clearMatches();
    d->documents = list;
}

void KoFindStyle::clearMatches()
{
    d->selections.clear();
    foreach (QTextDocument *doc, d->documents) {
        d->selections.insert(doc, QVector<QAbstractTextDocumentLayout::Selection>());
    }
    d->updateSelections();
}

void KoFindStyle::replaceImplementation(const KoFindMatch & /*match*/, const QVariant & /*value*/)
{
}

void KoFindStyle::findImplementation(const QString & /*pattern*/, KoFindBase::KoFindMatchList &matchList)
{
    int charStyle = options()->option("characterStyle")->value().toInt();
    int parStyle = options()->option("paragraphStyle")->value().toInt();

    foreach (QTextDocument *document, d->documents) {
        QTextBlock block = document->firstBlock();
        QVector<QAbstractTextDocumentLayout::Selection> selections;
        while (block.isValid()) {
            if (block.blockFormat().intProperty(KoParagraphStyle::StyleId) == parStyle) {
                for (QTextBlock::iterator itr = block.begin(); itr != block.end(); ++itr) {
                    if (itr.fragment().charFormat().intProperty(KoCharacterStyle::StyleId) == charStyle) {
                        QTextCursor cursor(document);
                        cursor.setPosition(itr.fragment().position());
                        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, itr.fragment().length());
                        matchList.append(KoFindMatch(QVariant::fromValue(document), QVariant::fromValue(cursor)));

                        QAbstractTextDocumentLayout::Selection selection;
                        selection.cursor = cursor;
                        selection.format = d->highlightFormat;
                        selections.append(selection);
                    }
                }
            }
            block = block.next();
        }
        d->selections.insert(document, selections);
    }

    d->updateSelections();
}

void KoFindStyle::Private::updateSelections()
{
    QHash<QTextDocument *, QVector<QAbstractTextDocumentLayout::Selection>>::iterator itr;
    for (itr = selections.begin(); itr != selections.end(); ++itr) {
        KoTextDocument doc(itr.key());
        doc.setSelections(itr.value());
    }
}
