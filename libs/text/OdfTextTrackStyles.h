/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2012 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef CHANGEFOLLOWER_H
#define CHANGEFOLLOWER_H

#include "commands/ChangeStylesMacroCommand.h"

#include <KoStyleManager.h>

#include <QObject>
#include <QPointer>
#include <QSet>
#include <QTextDocument>

/**
 * OdfTextTrackStyles is used to update a list of qtextdocument with
 * any changes made in the style manager.
 *
 * It also creates undo commands and adds them to the undo stack
 *
 * Style changes affect a lot of qtextdocuments and we store the changes and apply
 * the changes to every qtextdocument, so every KoTextDocument has to
 * register their QTextDocument to us.
 *
 * We use the QObject principle of children getting deleted when the
 * parent gets deleted. Thus we die when the KoStyleManager dies.
 */
class OdfTextTrackStyles : public QObject
{
    Q_OBJECT

public:
    static OdfTextTrackStyles *instance(KoStyleManager *manager);

private:
    static QHash<QObject *, OdfTextTrackStyles *> instances;

    explicit OdfTextTrackStyles(KoStyleManager *manager);

    /// Destructor, called when the parent is deleted.
    ~OdfTextTrackStyles() override;

private Q_SLOTS:
    void beginEdit();
    void endEdit();
    void recordStyleChange(int id, const KoParagraphStyle *origStyle, const KoParagraphStyle *newStyle);
    void recordStyleChange(int id, const KoCharacterStyle *origStyle, const KoCharacterStyle *newStyle);
    void styleManagerDied(QObject *manager);
    void documentDied(QObject *manager);

public:
    void registerDocument(QTextDocument *qDoc);
    void unregisterDocument(QTextDocument *qDoc);

private:
    QList<QTextDocument *> m_documents;
    QPointer<KoStyleManager> m_styleManager;
    ChangeStylesMacroCommand *m_changeCommand;
};

#endif
