/*
 *  SPDX-FileCopyrightText: 2014 Dmitry Kazakov <dimula73@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef __KO_COLOR_DISPLAY_RENDERER_INTERFACE_H
#define __KO_COLOR_DISPLAY_RENDERER_INTERFACE_H

#include <QColor>
#include <QObject>
#include <QSharedPointer>

#include "KoColor.h"

class KoChannelInfo;

/**
 * A special interface class provided by pigment to let widgets render
 * a KoColor on screen using custom profiling provided by the user.
 *
 * If you want to provide your own rendering of the KoColor on screen,
 * reimplement this class and provide its instance to a supporting
 * widget.
 */
class PIGMENTCMS_EXPORT KoColorDisplayRendererInterface : public QObject
{
    Q_OBJECT

public:
    KoColorDisplayRendererInterface();
    ~KoColorDisplayRendererInterface() override;

    /**
     * Convert the color \p c to a custom QColor that will be
     * displayed by the widget on screen. Please note, that the
     * reverse conversion may simply not exist.
     */
    virtual QColor toQColor(const KoColor &c) const = 0;

    /**
     * This tries to approximate a rendered QColor into the KoColor
     * of the painting color space. Please note, that in most of the
     * cases the exact reverse transformation does not exist, so the
     * resulting color will be only a rough approximation. Never try
     * to do a round trip like that:
     *
     * // r will never be equal to c!
     * r = approximateFromRenderedQColor(toQColor(c));
     */
    virtual KoColor approximateFromRenderedQColor(const QColor &c) const = 0;

    virtual KoColor fromHsv(int h, int s, int v, int a = 255) const = 0;
    virtual void getHsv(const KoColor &srcColor, int *h, int *s, int *v, int *a = nullptr) const = 0;

    /**
     * \return the minimum value of a floating point channel that can
     *         be seen on screen
     */
    virtual qreal minVisibleFloatValue(const KoChannelInfo *chaninfo) const = 0;

    /**
     * \return the maximum value of a floating point channel that can
     *         be seen on screen. In normal situation it is 1.0. When
     *         the user changes exposure the value varies.
     */
    virtual qreal maxVisibleFloatValue(const KoChannelInfo *chaninfo) const = 0;

Q_SIGNALS:
    void displayConfigurationChanged();

private:
    Q_DISABLE_COPY(KoColorDisplayRendererInterface)
};

/**
 * The default conversion class that just calls KoColor::toQColor()
 * conversion implementation which effectively renders the color into
 * sRGB color space.
 */
class PIGMENTCMS_EXPORT KoDumbColorDisplayRenderer : public KoColorDisplayRendererInterface
{
    Q_OBJECT
public:
    QColor toQColor(const KoColor &c) const override;
    KoColor approximateFromRenderedQColor(const QColor &c) const override;
    KoColor fromHsv(int h, int s, int v, int a = 255) const override;
    void getHsv(const KoColor &srcColor, int *h, int *s, int *v, int *a = nullptr) const override;

    qreal minVisibleFloatValue(const KoChannelInfo *chaninfo) const override;
    qreal maxVisibleFloatValue(const KoChannelInfo *chaninfo) const override;

    static KoColorDisplayRendererInterface *instance();
};

#endif /* __KO_COLOR_DISPLAY_RENDERER_INTERFACE_H */
