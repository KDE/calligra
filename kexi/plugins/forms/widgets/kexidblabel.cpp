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

#include <QBitmap>
#include <QPainter>
#include <qdrawutil.h>
#include <QApplication>
#include <QTimer>
#include <QPaintEvent>
#include <QLabel>
#include <QShowEvent>
#include <QResizeEvent>

#include <kdebug.h>

#include <db/field.h>
#include <kexiutils/utils.h>
#include <formeditor/utils.h>

#define SHADOW_OFFSET_X 3
#define SHADOW_OFFSET_Y 3
#define SHADOW_FACTOR 16.0
#define SHADOW_OPACITY 50.0
#define SHADOW_AXIS_FACTOR 2.0
#define SHADOW_DIAGONAL_FACTOR 1.0
#define SHADOW_THICKNESS 1

//=========================================================

//! @internal
class KexiDBLabel::Private
{
public:
    Private()
            : timer(0)
//  , autonumberDisplayParameters(0)
            , pixmapDirty(true)
            , shadowEnabled(false)
            , resizeEvent(false) {
    }
    ~Private() {}
    QPixmap shadowPixmap;
    QPoint shadowPosition;
//    KexiDBInternalLabel* internalLabel;
    QTimer* timer;
    QColor frameColor;
    bool pixmapDirty : 1;
    bool shadowEnabled : 1;
    bool resizeEvent : 1;
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
//2.0    d->internalLabel = new KexiDBInternalLabel(this);
//2.0    d->internalLabel->hide();
    d->frameColor = palette().color(foregroundRole());

//2.0    setAlignment(d->internalLabel->alignment());
    setIndent(2);
}


void KexiDBLabel::paintEvent(QPaintEvent* e)
{
    QPainter p(this);
    if (d->shadowEnabled) {
        /*!
        If required, update the pixmap-cache.
        */
        if (d->pixmapDirty) {
//2.0            updatePixmapLater();
        }
    }
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
    if (d->shadowEnabled) {
        if (0 == qstrcmp("indent", name) || 0 == qstrcmp("font", name) || 0 == qstrcmp("margin", name)
                || 0 == qstrcmp("frameShadow", name) || 0 == qstrcmp("frameShape", name)
                || 0 == qstrcmp("frameStyle", name) || 0 == qstrcmp("midLineWidth", name)
                || 0 == qstrcmp("lineWidth", name)) {
//2.0            d->internalLabel->setProperty(name, value);
//2.0            updatePixmap();
        }
    }
    return ret;
}

void KexiDBLabel::setColumnInfo(KexiDB::QueryColumnInfo* cinfo)
{
    KexiFormDataItemInterface::setColumnInfo(cinfo);
    KexiDBTextWidgetInterface::setColumnInfo(cinfo, this);
}

void KexiDBLabel::setShadowEnabled(bool state)
{
    d->shadowEnabled = state;
    d->pixmapDirty = true;
//2.0    if (state)
//2.0        d->internalLabel->updateFrame();
    repaint();
}

void KexiDBLabel::resizeEvent(QResizeEvent* e)
{
    if (isVisible())
        d->resizeEvent = true;
    d->pixmapDirty = true;
    QLabel::resizeEvent(e);
}

void KexiDBLabel::fontChange(const QFont& font)
{
    d->pixmapDirty = true;
//2.0    d->internalLabel->setFont(font);
    QLabel::fontChange(font);
}

void KexiDBLabel::styleChange(QStyle& style)
{
    d->pixmapDirty = true;
    QLabel::styleChange(style);
}

void KexiDBLabel::enabledChange(bool enabled)
{
    d->pixmapDirty = true;
//2.0    d->internalLabel->setEnabled(enabled);
    QLabel::enabledChange(enabled);
}

void KexiDBLabel::paletteChange(const QPalette& oldPal)
{
    Q_UNUSED(oldPal);
    d->pixmapDirty = true;
//2.0    d->internalLabel->setPalette(palette());
}

/*const QColor & KexiDBLabel::paletteForegroundColor () const
{
  return d->foregroundColor;
}

void KexiDBLabel::setPaletteForegroundColor ( const QColor& color )
{
  d->foregroundColor = color;
}*/

#ifdef __GNUC__
#warning TODO
#else
#pragma WARNING( TODO  )
#endif
/*TODO
void KexiDBLabel::frameChanged()
{
  d->pixmapDirty = true;
  d->internalLabel->updateFrame();
  QFrame::frameChanged();
}*/

void KexiDBLabel::showEvent(QShowEvent* e)
{
    d->pixmapDirty = true;
    QLabel::showEvent(e);
}

void KexiDBLabel::setText(const QString& text)
{
    d->pixmapDirty = true;
    QLabel::setText(text);
    //This is necessary for KexiFormDataItemInterface
    valueChanged();
    repaint();
}

bool KexiDBLabel::shadowEnabled() const
{
    return d->shadowEnabled;
}

#define ClassName KexiDBLabel
#define SuperClassName QLabel
#include "kexiframeutils_p.cpp"
#include "kexidblabel.moc"
