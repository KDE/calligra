/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Pierre Stirnweiss <pstirnweiss@googlemail.com>
 * SPDX-FileCopyrightText: 2009-2010 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "FormattingPreview.h"

#include <KoCharacterStyle.h>
#include <KoPostscriptPaintDevice.h>
#include <KoStyleThumbnailer.h>
#include <KoZoomHandler.h>

#include <QBrush>
#include <QColor>
#include <QFont>
#include <QFontMetrics>
#include <QFrame>
#include <QPainter>
#include <QPen>
#include <QPointF>
#include <QRect>
#include <QRectF>
#include <QString>
#include <QTextLayout>
#include <QTextLine>
#include <QTextOption>

#include <math.h>

#include <KLocalizedString>
#include <QDebug>

FormattingPreview::FormattingPreview(QWidget *parent)
    : QFrame(parent)
    , m_sampleText(i18n("Font"))
    , m_characterStyle(nullptr)
    , m_paragraphStyle(nullptr)
    , m_thumbnailer(new KoStyleThumbnailer())
    , m_previewLayoutRequired(true)
{
    setFrameStyle(QFrame::Box | QFrame::Plain);
    setMinimumSize(500, 150);

    m_thumbnailer->setText(m_sampleText);
}

FormattingPreview::~FormattingPreview()
{
    delete m_thumbnailer;
    if (m_characterStyle) {
        delete m_characterStyle;
    }
    if (m_paragraphStyle) {
        delete m_paragraphStyle;
    }
}

void FormattingPreview::setText(const QString &sampleText)
{
    m_sampleText = sampleText;
    m_thumbnailer->setText(m_sampleText);

    m_previewLayoutRequired = true;

    update();
}

// Character properties
void FormattingPreview::setCharacterStyle(const KoCharacterStyle *style)
{
    if (m_characterStyle) {
        delete m_characterStyle;
    }

    m_characterStyle = style->clone();

    m_previewLayoutRequired = true;

    update();
}

void FormattingPreview::setParagraphStyle(const KoParagraphStyle *style)
{
    if (m_paragraphStyle) {
        delete m_paragraphStyle;
    }

    m_paragraphStyle = style->clone();

    m_previewLayoutRequired = true;

    update();
}

// Painting related methods

void FormattingPreview::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter *p = new QPainter(this);
    p->save();

    QRect rectang = contentsRect();
    p->fillRect(rectang, QBrush(QColor(Qt::white)));

    rectang.adjust(6, 6, -6, -6);
    p->drawImage(rectang, m_thumbnailer->thumbnail(m_characterStyle, m_paragraphStyle, rectang.size(), m_previewLayoutRequired, KoStyleThumbnailer::NoFlags));

    m_previewLayoutRequired = false;

    p->restore();
    delete p;
}
