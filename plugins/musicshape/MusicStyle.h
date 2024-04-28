/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef MUSIC_STYLE_H
#define MUSIC_STYLE_H

#include "core/Clef.h"
#include "core/Global.h"

#include <QPainter>
#include <QPen>

/**
 * This class contains various methods that define how music is rendered. Currently all hardcoded
 * implementations, but in the future this class would become pure virtual, with various implementations.
 */
class MusicStyle
{
public:
    MusicStyle();
    virtual ~MusicStyle();
    virtual QPen staffLinePen(const QColor &color = Qt::black);
    virtual QPen stemPen(const QColor &color = Qt::black);
    virtual QPen noteDotPen(const QColor &color = Qt::black);
    virtual qreal beamLineWidth();
    virtual void renderNoteHead(QPainter &painter, qreal x, qreal y, MusicCore::Duration duration, const QColor &color = Qt::black);
    virtual void renderRest(QPainter &painter, qreal x, qreal y, MusicCore::Duration duration, const QColor &color = Qt::black);
    virtual void renderClef(QPainter &painter, qreal x, qreal y, MusicCore::Clef::ClefShape shape, const QColor &color = Qt::black);
    virtual void renderAccidental(QPainter &painter, qreal x, qreal y, int accidental, const QColor &color = Qt::black);
    virtual void renderTimeSignatureNumber(QPainter &painter, qreal x, qreal y, qreal w, int number, const QColor &color = Qt::black);
    virtual void renderNoteFlags(QPainter &painter, qreal x, qreal y, MusicCore::Duration duration, bool stemsUp, const QColor &color = Qt::black);
    /**
     * Render text either as text or as path, as specified by the textAsPath value
     */
    virtual void renderText(QPainter &painter, qreal x, qreal y, const QString &text);
    /**
     * Whether to render text as paths. Default value is false (render text as text)
     */
    virtual void setTextAsPath(bool drawTextAsPath);
    /**
     * Whether to render text as paths
     */
    virtual bool textAsPath() const;

private:
    QPen m_staffLinePen, m_stemPen, m_noteDotPen;
    QFont m_font;
    bool m_textAsPath;
};

#endif // MUSIC_STYLE_H
