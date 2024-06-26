/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoTextEditingPlugin.h"

#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>

#include "TextDebug.h"

class Q_DECL_HIDDEN KoTextEditingPlugin::Private
{
public:
    QHash<QString, QAction *> actionCollection;
};

KoTextEditingPlugin::KoTextEditingPlugin()
    : d(new Private())
{
}

KoTextEditingPlugin::~KoTextEditingPlugin()
{
    delete d;
}

void KoTextEditingPlugin::selectWord(QTextCursor &cursor, int cursorPosition) const
{
    cursor.setPosition(cursorPosition);
    // Protect against trying to select with an invalid cursorPosition
    if (cursor.position() == 0) {
        errorText << "Invalid cursor position" << cursorPosition;
        return;
    }
    QTextBlock block = cursor.block();
    cursor.setPosition(block.position());
    cursorPosition -= block.position();
    const QString string = block.text();
    int pos = 0;
    bool space = false;
    QString::ConstIterator iter = string.begin();
    while (iter != string.end()) {
        if (iter->isSpace()) {
            if (space)
                ; // double spaces belong to the previous word
            else if (pos < cursorPosition)
                cursor.setPosition(pos + block.position() + 1); // +1 because we don't want to set it on the space itself
            else
                space = true;
        } else if (space)
            break;
        pos++;
        ++iter;
    }
    cursor.setPosition(pos + block.position(), QTextCursor::KeepAnchor);
}

QString KoTextEditingPlugin::paragraph(QTextDocument *document, int cursorPosition) const
{
    QTextBlock block = document->findBlock(cursorPosition);
    return block.text();
}

void KoTextEditingPlugin::addAction(const QString &name, QAction *action)
{
    d->actionCollection.insert(name, action);
}

void KoTextEditingPlugin::checkSection(QTextDocument *document, int startPosition, int endPosition)
{
    QTextBlock block = document->findBlock(startPosition);
    int pos = block.position();
    while (true) {
        if (!block.contains(startPosition - 1) && !block.contains(endPosition + 1)) // only parags that are completely in
            finishedParagraph(document, block.position());

        const QString text = block.text();
        bool space = true;
        QString::ConstIterator iter = text.begin();
        while (pos < endPosition && iter != text.end()) {
            bool isSpace = iter->isSpace();
            if (pos >= startPosition && space && !isSpace) // for each word, call finishedWord
                finishedWord(document, pos);
            else if (!isSpace && pos == startPosition)
                finishedWord(document, startPosition);
            space = isSpace;
            pos++;
            iter++;
        }

        if (!(block.isValid() && block.position() + block.length() < endPosition))
            break;
        block = block.next();
    }
}

QHash<QString, QAction *> KoTextEditingPlugin::actions() const
{
    return d->actionCollection;
}

void KoTextEditingPlugin::setCurrentCursorPosition(QTextDocument *document, int cursorPosition)
{
    Q_UNUSED(cursorPosition);
    Q_UNUSED(document);
}
