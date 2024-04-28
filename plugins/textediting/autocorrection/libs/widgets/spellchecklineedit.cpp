/*
 * SPDX-FileCopyrightText: 2011-2022 Laurent Montel <montel@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "spellchecklineedit.h"
#include <QKeyEvent>
#include <QMimeData>
#include <QStyle>
#include <QStyleOptionFrame>

SpellCheckLineEdit::SpellCheckLineEdit(QWidget *parent, const QString &configFile)
    : KRichTextEdit(parent)
{
    // setSpellCheckingConfigFileName(configFile);
    // setSearchSupport(false);
    // setAllowTabSupport(false);
    setAcceptRichText(false);
    setTabChangesFocus(true);
    // widget may not be resized vertically
    setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed));
    setLineWrapMode(NoWrap);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setCheckSpellingEnabled(true);
    document()->adjustSize();

    document()->setDocumentMargin(2);
}

SpellCheckLineEdit::~SpellCheckLineEdit() = default;

void SpellCheckLineEdit::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return || e->key() == Qt::Key_Down) {
        Q_EMIT focusDown();
        return;
    } else if (e->key() == Qt::Key_Up) {
        Q_EMIT focusUp();
        return;
    }
    KRichTextEdit::keyPressEvent(e);
}

QSize SpellCheckLineEdit::sizeHint() const
{
    QFontMetrics fm(font());

    const int h = document()->size().toSize().height() - fm.descent() + 2 * frameWidth();

    QStyleOptionFrame opt;
    opt.initFrom(this);
    opt.rect = QRect(0, 0, 100, h);
    opt.lineWidth = lineWidth();
    opt.midLineWidth = 0;
    opt.state |= QStyle::State_Sunken;

    QSize s = style()->sizeFromContents(QStyle::CT_LineEdit, &opt, QSize(100, h), this);

    return s;
}

QSize SpellCheckLineEdit::minimumSizeHint() const
{
    return sizeHint();
}

void SpellCheckLineEdit::insertFromMimeData(const QMimeData *source)
{
    if (!source) {
        return;
    }

    setFocus();

    // Copy text from the clipboard (paste)
    QString pasteText = source->text();

    // is there any text in the clipboard?
    if (!pasteText.isEmpty()) {
        // replace \r with \n to make xterm pastes happy
        pasteText.replace(QLatin1Char('\r'), QLatin1Char('\n'));
        // remove blank lines
        while (pasteText.contains(QLatin1String("\n\n"))) {
            pasteText.replace(QLatin1String("\n\n"), QLatin1String("\n"));
        }

        static const QRegularExpression reTopSpace(QStringLiteral("^ *\n"));
        while (pasteText.contains(reTopSpace)) {
            pasteText.remove(reTopSpace);
        }

        static const QRegularExpression reBottomSpace(QStringLiteral("\n *$"));
        while (pasteText.contains(reBottomSpace)) {
            pasteText.remove(reBottomSpace);
        }

        // does the text contain at least one newline character?
        pasteText.replace(QLatin1Char('\n'), QLatin1Char(' '));

        insertPlainText(pasteText);
        ensureCursorVisible();
        return;
    } else {
        KRichTextEdit::insertFromMimeData(source);
    }
}
