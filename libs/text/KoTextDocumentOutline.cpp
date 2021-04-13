/*
    This file is part of Calligra

    SPDX-FileCopyrightText: 2021 Pierre Ducroquet <pinaraf@pinaraf.info>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "KoTextDocumentOutline.h"

#include <QTextDocument>

KoTextDocumentOutline::KoTextDocumentOutline(QTextDocument *parent)
    : QObject(parent), document(parent)
{
}

KoTextDocumentOutline::~KoTextDocumentOutline()
{
}

QList<QTextCursor> KoTextDocumentOutline::outline() const
{
    return m_paragraphs;
}

void KoTextDocumentOutline::add(const QTextCursor &cursor)
{
    if (m_loading) {
        // TODO: sorting !
        m_pendingPositions.append(cursor.position());
    } else {
        // TODO: sorting !
        m_paragraphs.append(cursor);
    }
}

void KoTextDocumentOutline::remove(const QTextCursor &cursor)
{
    if (m_loading) {
        m_pendingPositions.removeAll(cursor.position());
    } else {
        m_paragraphs.removeAll(cursor);
    }
}

void KoTextDocumentOutline::setLoading()
{
    m_loading = true;
}

void KoTextDocumentOutline::finalizeLoading()
{
    if (m_loading) {
        m_loading = false;
        for (auto position: m_pendingPositions) {
            QTextCursor cursor(document);
            cursor.setPosition(position);
            cursor.setKeepPositionOnInsert(true);
            m_paragraphs.append(cursor);
        }
    }
}
