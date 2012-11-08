/* This file is part of the KDE project
   Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2012 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexidbtextedit.h"
#include "kexidblineedit.h"
#include <db/queryschema.h>
#include <kexiutils/utils.h>

#include <kapplication.h>
#include <kstdaccel.h>
#include <kdebug.h>

#include <QPaintEvent>
#include <QPainter>
#include <QLabel>

class DataSourceLabel : public QLabel
{
public:
    DataSourceLabel(QWidget *parent) : QLabel(parent)
    {
    }
protected:
    void paintEvent(QPaintEvent *pe)
    {
        QLabel::paintEvent(pe);
        QPainter p(this);
        int leftMargin, topMargin, rightMargin, bottomMargin;
        getContentsMargins(&leftMargin, &topMargin, &rightMargin, &bottomMargin);
        QRect r( rect() );
        r.setX(r.x() + leftMargin);
        r.setY(r.y() + topMargin);
        r.setRight(r.right() - rightMargin);
        r.setBottom(r.bottom() - bottomMargin);
        QPixmap dataSourceTagIcon;
        int x;
        if (layoutDirection() == Qt::LeftToRight) {
            dataSourceTagIcon = KexiFormUtils::dataSourceTagIcon();
            x = r.left() - 1;
        }
        else {
            dataSourceTagIcon = KexiFormUtils::dataSourceRTLTagIcon();
            x = r.right() - dataSourceTagIcon.width() - 5;
        }
        p.drawPixmap(
            x, r.top() + (r.height() - dataSourceTagIcon.height()) / 2,
            dataSourceTagIcon
        );
    }
};

// --------------

KexiDBTextEdit::KexiDBTextEdit(QWidget *parent)
        : KTextEdit(parent)
        , KexiDBTextWidgetInterface()
        , KexiFormDataItemInterface()
        , m_menuExtender(this, this)
        , m_slotTextChanged_enabled(true)
        , m_dataSourceLabel(0)
        , m_length(0)
        , m_paletteChangeEvent_enabled(true)
{
    QFont tmpFont;
    tmpFont.setPointSize(KGlobalSettings::smallestReadableFont().pointSize());
    setMinimumHeight(QFontMetrics(tmpFont).height() + 6);
    connect(this, SIGNAL(textChanged()), this, SLOT(slotTextChanged()));
//hmm disabled again because this makes the widget disappear entirely
//    setAutoFillBackground(true); // otherwise we get transparent background...
//    installEventFilter(this);
    setBackgroundRole(QPalette::Base);
}

KexiDBTextEdit::~KexiDBTextEdit()
{
}

void KexiDBTextEdit::setInvalidState(const QString& displayText)
{
    setReadOnly(true);
//! @todo move this to KexiDataItemInterface::setInvalidStateInternal() ?
    if (focusPolicy() & Qt::TabFocus)
        setFocusPolicy(Qt::ClickFocus);
    KTextEdit::setPlainText(displayText);
}

void KexiDBTextEdit::setValueInternal(const QVariant& add, bool removeOld)
{
//! @todo how about rich text?
    if (m_columnInfo && m_columnInfo->field->type() == KexiDB::Field::Boolean) {
//! @todo temporary solution for booleans!
        KTextEdit::setHtml(add.toBool() ? "1" : "0");
    } else {
        QString t;
        if (removeOld) {
            t = add.toString();
        }
        else {
            t = m_origValue.toString() + add.toString();
        }

        if (acceptRichText()) {
            KTextEdit::setHtml(t);
        }
        else {
            KTextEdit::setPlainText(t);
        }
    }
}

QVariant KexiDBTextEdit::value()
{
    return acceptRichText() ? toHtml() : toPlainText();
}

void KexiDBTextEdit::slotTextChanged()
{
    if (!m_slotTextChanged_enabled)
        return;

    if (m_length > 0) {
        QString t;
        if (acceptRichText()) {
            t = toHtml();
        }
        else {
            t = toPlainText();
        }
        if (t.length() > (int)m_length) {
            m_slotTextChanged_enabled = false;
            if (acceptRichText()) {
#warning todo setHtml(t.left(m_length));
            }
            else {
                setPlainText(t.left(m_length));
            }
            m_slotTextChanged_enabled = true;
            moveCursorToEnd();
        }
    }

    signalValueChanged();
}

bool KexiDBTextEdit::valueIsNull()
{
    return (acceptRichText() ? toHtml() : toPlainText()).isNull();
}

bool KexiDBTextEdit::valueIsEmpty()
{
    return (acceptRichText() ? toHtml() : toPlainText()).isEmpty();
}

bool KexiDBTextEdit::isReadOnly() const
{
    return KTextEdit::isReadOnly();
}

void KexiDBTextEdit::setReadOnly(bool readOnly)
{
    KTextEdit::setReadOnly(readOnly);
#ifdef __GNUC__
#warning TODO KexiDBTextEdit::setReadOnly() - bg color
#else
#pragma WARNING( TODO KexiDBTextEdit::setReadOnly() - bg color )
#endif
#if 0//TODO
    QPalette p = palette();
    QColor c(readOnly
             ? KexiFormUtils::lighterGrayBackgroundColor(kapp->palette()) : p.color(QPalette::Normal, QColorGroup::Base));
    setPaper(c);
    p.setColor(QColorGroup::Base, c);
    p.setColor(QColorGroup::Background, c);
    setPalette(p);
#endif
}

/* Qt4
void KexiDBTextEdit::setText( const QString & text, const QString & context )
{
  KTextEdit::setText(text, context);
}*/

QWidget* KexiDBTextEdit::widget()
{
    return this;
}

bool KexiDBTextEdit::cursorAtStart()
{
    return textCursor().atStart();
}

bool KexiDBTextEdit::cursorAtEnd()
{
    return textCursor().atEnd();
}

void KexiDBTextEdit::clear()
{
    document()->clear();
}

void KexiDBTextEdit::setColumnInfo(KexiDB::QueryColumnInfo* cinfo)
{
    KexiFormDataItemInterface::setColumnInfo(cinfo);
    if (!cinfo) {
        m_length = 0;
        return;
    }

    if (cinfo->field->type() == KexiDB::Field::Text) {
        if (!designMode()) {
            if (cinfo->field->maxLength() > 0) {
                m_length = cinfo->field->maxLength();
            }
        }
    }

    KexiDBTextWidgetInterface::setColumnInfo(m_columnInfo, this);
}

void KexiDBTextEdit::paintEvent(QPaintEvent *pe)
{
    KTextEdit::paintEvent(pe);
    QPainter p(viewport());
    //! @todo how about rich text?
    KexiDBTextWidgetInterface::paint(this, &p, toPlainText().isEmpty(), alignment(), hasFocus());
}

void KexiDBTextEdit::contextMenuEvent(QContextMenuEvent *e)
{
    QMenu *menu = createStandardContextMenu();
    m_menuExtender.exec(menu, e->globalPos());
    delete menu;
}

void KexiDBTextEdit::undo()
{
    cancelEditor();
}

void KexiDBTextEdit::setDisplayDefaultValue(QWidget* widget, bool displayDefaultValue)
{
    KexiFormDataItemInterface::setDisplayDefaultValue(widget, displayDefaultValue);
    // initialize display parameters for default / entered value
    KexiDisplayUtils::DisplayParameters * const params
    = displayDefaultValue ? m_displayParametersForDefaultValue : m_displayParametersForEnteredValue;
    QPalette pal(palette());
    pal.setColor(QPalette::Active, QColorGroup::Text, params->textColor);
    setPalette(pal);
    setFont(params->font);
//! @todo support rich text...
    /* m_slotTextChanged_enabled = false;
        //for rich text...
        const QString origText( text() );
        KTextEdit::setText(QString());
        setCurrentFont(params->font);
        setColor(params->textColor);
        KTextEdit::setText(origText);
      m_slotTextChanged_enabled = true;*/
}

void KexiDBTextEdit::moveCursorToEnd()
{
    moveCursor(QTextCursor::End);
}

void KexiDBTextEdit::moveCursorToStart()
{
    moveCursor(QTextCursor::Start);
}

void KexiDBTextEdit::selectAll()
{
    KTextEdit::selectAll();
}

void KexiDBTextEdit::keyPressEvent(QKeyEvent *ke)
{
    // for instance, Windows uses Ctrl+Tab for moving between tabs, so do not steal this shortcut
    if (KStandardShortcut::tabNext().contains(QKeySequence(ke->key() | ke->modifiers()))
            || KStandardShortcut::tabPrev().contains(QKeySequence(ke->key() | ke->modifiers()))) {
        ke->ignore();
        return;
    }
    KTextEdit::keyPressEvent(ke);
}

bool KexiDBTextEdit::event(QEvent *e)
{
    bool res = KTextEdit::event(e);

    if (e->type() == QEvent::LayoutDirectionChange) {
        if (m_dataSourceLabel) {
            m_dataSourceLabel->setLayoutDirection( layoutDirection() );
        }
        updateTextForDataSource();
    }
    else if (e->type() == QEvent::Resize) {
        if (m_dataSourceLabel) {
            m_dataSourceLabel->setFixedWidth(width());
        }
    }
    return res;
}

void KexiDBTextEdit::updateTextForDataSource()
{
    if (!designMode()) {
        if (m_dataSourceLabel) {
            m_dataSourceLabel->hide();
        }
        return;
    }
    setPlainText(QString());
    if (!m_dataSourceLabel && !dataSource().isEmpty()) {
        createDataSourceLabel();
    }
    if (m_dataSourceLabel) {
        m_dataSourceLabel->setText(dataSource());
        m_dataSourceLabel->setIndent( KexiFormUtils::dataSourceTagIcon().width()
            + (layoutDirection() == Qt::LeftToRight ? 0 : 7) );
        m_dataSourceLabel->setVisible(!dataSource().isEmpty());
    }
}

void KexiDBTextEdit::setDataSource(const QString &ds)
{
    KexiFormDataItemInterface::setDataSource(ds);
    updateTextForDataSource();
}

void KexiDBTextEdit::setDataSourcePartClass(const QString &partClass)
{
    KexiFormDataItemInterface::setDataSourcePartClass(partClass);
    updateTextForDataSource();
}

void KexiDBTextEdit::createDataSourceLabel()
{
    if (m_dataSourceLabel)
        return;
    m_dataSourceLabel = new DataSourceLabel(viewport());
    m_dataSourceLabel->hide();
    m_dataSourceLabel->move(0, 0);
    int leftMargin, topMargin, rightMargin, bottomMargin;
    getContentsMargins(&leftMargin, &topMargin, &rightMargin, &bottomMargin);
    m_dataSourceLabel->setContentsMargins(leftMargin, topMargin, rightMargin, bottomMargin);
}

void KexiDBTextEdit::selectAllOnFocusIfNeeded()
{
//    moveCursorToEnd();
//    selectAll();
}

void KexiDBTextEdit::updatePalette()
{
    m_paletteChangeEvent_enabled = false;
    setPalette(isReadOnly() ?
               KexiUtils::paletteForReadOnly(m_originalPalette)
              : m_originalPalette);
    m_paletteChangeEvent_enabled = true;
}

void KexiDBTextEdit::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::PaletteChange && m_paletteChangeEvent_enabled) {
        m_originalPalette = palette();
        updatePalette();
    }
    KTextEdit::changeEvent(e);
}

#include "kexidbtextedit.moc"
