/* This file is part of the KDE project
   Copyright (C) 2005 Christian Nitschkowski <segfault_ii@web.de>
   Copyright (C) 2009 Jarosław Staniek <staniek@kde.org>

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

#include "kexidblabel.h"
#include <kexiutils/utils.h>
#include <formeditor/utils.h>

#include <KDbField>

#include <QPainter>
#include <qdrawutil.h>
#include <QPaintEvent>
#include <QLabel>
#include <QShowEvent>
#include <QResizeEvent>
#include <QDebug>

//! @internal
class KexiDBLabel::Private
{
public:
    Private()
            : resizeEvent(false) {
    }
    ~Private() {}
    QColor frameColor;
    bool resizeEvent;
};

//=========================================================

KexiDBLabel::KexiDBLabel(QWidget *parent, Qt::WFlags f)
        : QLabel(parent, f)
        , KexiDBTextWidgetInterface()
        , KexiFormDataItemInterface()
        , d(new Private())
{
    init();
}

KexiDBLabel::KexiDBLabel(const QString& text, QWidget *parent, Qt::WFlags f)
        : QLabel(parent, f)
        , KexiDBTextWidgetInterface()
        , KexiFormDataItemInterface()
        , d(new Private())
{
    init();
    setText(text);
}

KexiDBLabel::~KexiDBLabel()
{
    delete d;
}

void KexiDBLabel::init()
{
    KexiDataItemInterface::setHasFocusableWidget(false);
    d->frameColor = palette().color(foregroundRole());
    setIndent(2);
    setAutoFillBackground(true);
}


void KexiDBLabel::paintEvent(QPaintEvent* e)
{
    QPainter p(this);
    KexiDBTextWidgetInterface::paint(this, &p, text().isEmpty(), alignment(), false);
    p.end();

    QPalette origPal;
    if (editingMode()) {
        origPal = palette();
        QPalette pal(palette());
        pal.setBrush(QPalette::WindowText, Qt::transparent);
        setPalette(pal);
    }
    QLabel::paintEvent(e);
    if (editingMode()) {
        setPalette(origPal);
    }

    const bool hasFrame = frameWidth() >= 1 && frameShape() != QFrame::NoFrame;
    if (designMode() && !hasFrame) {
        p.begin(this);
        KFormDesigner::paintWidgetFrame(p, rect());
        p.end();
    }
}

void KexiDBLabel::setValueInternal(const QVariant& add, bool removeOld)
{
    if (removeOld)
        setText(add.toString());
    else
        setText(KexiDataItemInterface::originalValue().toString() + add.toString());
}

QVariant KexiDBLabel::value()
{
    return text();
}

void KexiDBLabel::setInvalidState(const QString& displayText)
{
    setText(displayText);
}

bool KexiDBLabel::valueIsNull()
{
    return text().isNull();
}

bool KexiDBLabel::valueIsEmpty()
{
    return text().isEmpty();
}

bool KexiDBLabel::isReadOnly() const
{
    return true;
}

void KexiDBLabel::setReadOnly(bool readOnly)
{
    Q_UNUSED(readOnly);
}

QWidget* KexiDBLabel::widget()
{
    return this;
}

bool KexiDBLabel::cursorAtStart()
{
    return false;
}

bool KexiDBLabel::cursorAtEnd()
{
    return false;
}

void KexiDBLabel::clear()
{
    setText(QString());
}

bool KexiDBLabel::setProperty(const char * name, const QVariant & value)
{
    const bool ret = QLabel::setProperty(name, value);
    return ret;
}

const QPixmap* KexiDBLabel::pixmap() const
{
    return QLabel::pixmap();
}

void KexiDBLabel::setColumnInfo(KDbQueryColumnInfo* cinfo)
{
    KexiFormDataItemInterface::setColumnInfo(cinfo);
    KexiDBTextWidgetInterface::setColumnInfo(cinfo, this);
}

void KexiDBLabel::resizeEvent(QResizeEvent* e)
{
    if (isVisible())
        d->resizeEvent = true;
    QLabel::resizeEvent(e);
}

void KexiDBLabel::setText(const QString& text)
{
    QLabel::setText(text);
    //This is necessary for KexiFormDataItemInterface
    valueChanged();
    repaint();
}

#define ClassName KexiDBLabel
#define SuperClassName QLabel
#include "kexiframeutils_p.cpp"
