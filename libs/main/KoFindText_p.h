/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2010 Arjen Hiemstra <ahiemstra@heimr.nl>
 * SPDX-FileCopyrightText: 2011 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KoFindText_p_h
#define KoFindText_p_h

#include "KoFindText.h"

#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlock>
#include <QTextLayout>
#include <QPalette>
#include <QStyle>
#include <QAbstractTextDocumentLayout>

#include <MainDebug.h>
#include <klocalizedstring.h>

#include <KoText.h>
#include <KoTextDocument.h>
#include <KoShape.h>
#include <KoShapeContainer.h>

#include "KoFindOptionSet.h"
#include "KoFindOption.h"
#include "KoDocument.h"

class Q_DECL_HIDDEN KoFindText::Private
{
public:
    Private(KoFindText* qq) : q(qq), selectionStart(-1), selectionEnd(-1) { }

    void updateSelections();
    void updateDocumentList();
    void documentDestroyed(QObject *document);
    void updateCurrentMatch(int position);
    static void initializeFormats();

    KoFindText *q;

    QList<QTextDocument*> documents;

    QTextCursor currentCursor;
    QTextCursor selection;
    QHash<QTextDocument*, QVector<QAbstractTextDocumentLayout::Selection> > selections;

    int selectionStart;
    int selectionEnd;

    static QTextCharFormat highlightFormat;
    static QTextCharFormat currentMatchFormat;
    static QTextCharFormat currentSelectionFormat;
    static QTextCharFormat replacedFormat;
    static bool formatsInitialized;

    QPair<QTextDocument*, int> currentMatch;
};

#endif
