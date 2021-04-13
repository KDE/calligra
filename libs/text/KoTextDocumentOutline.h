/*
    This file is part of Calligra

    SPDX-FileCopyrightText: 2021 Pierre Ducroquet <pinaraf@pinaraf.info>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KOTEXTDOCUMENTOUTLINE_H
#define KOTEXTDOCUMENTOUTLINE_H

#include <QObject>
#include <QTextCursor>

#include "kotext_export.h"

/**
 * A container to register all the paragraphs that make the
 * outline of the document.
 * This comes in handy in layouting, generating ToC, possibly
 * navigation widgets…
 */
class KOTEXT_EXPORT KoTextDocumentOutline : public QObject
{
Q_OBJECT
public:
/// Constructor
    explicit KoTextDocumentOutline(QTextDocument *parent = nullptr);
    ~KoTextDocumentOutline() override;

    QList<QTextCursor> outline() const;
    void add(const QTextCursor &cursor);
    void remove(const QTextCursor &cursor);

    /// Same work-around against QTBUG-92153 as for KoTextRangeManager
    void setLoading();
    void finalizeLoading();

private:
    QTextDocument *document;
    bool m_loading;
    QList<int> m_pendingPositions;
    QList<QTextCursor> m_paragraphs;
};

#endif // KOTEXTDOCUMENTOUTLINE_H
