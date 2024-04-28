/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Ko GmbH <cbo@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef INDEXGENERATORMANAGER_H
#define INDEXGENERATORMANAGER_H

#include <QHash>
#include <QMetaType>
#include <QObject>
#include <QTimer>

class QTextDocument;
class KoTextDocumentLayout;
class KoTableOfContentsGeneratorInfo;
class ToCGenerator;

class IndexGeneratorManager : public QObject
{
    Q_OBJECT
private:
    explicit IndexGeneratorManager(QTextDocument *document);

public:
    ~IndexGeneratorManager() override;

    static IndexGeneratorManager *instance(QTextDocument *document);

    bool generate();

public Q_SLOTS:
    void requestGeneration();
    void startDoneTimer();

private Q_SLOTS:
    void layoutDone();
    void timeout();

private:
    enum State {
        Resting, // We are not doing anything, and don't need to either
        FirstRunNeeded, // We would like to update the indexes, with dummy pg nums
        FirstRun, // Updating indexes, so prevent layout and ignore documentChanged()
        FirstRunLayouting, // KoTextDocumentLayout is layouting so sit still
        SecondRunNeeded, // Would like to update the indexes, getting pg nums right
        SecondRun, // Updating indexes, so prevent layout and ignore documentChanged()
        SecondRunLayouting // KoTextDocumentLayout is layouting so sit still
    };
    QTextDocument *m_document;
    KoTextDocumentLayout *m_documentLayout;
    QHash<KoTableOfContentsGeneratorInfo *, ToCGenerator *> m_generators;
    State m_state;
    QTimer m_updateTimer;
    QTimer m_doneTimer;
};

Q_DECLARE_METATYPE(IndexGeneratorManager *)

#endif
