/*
 *  SPDX-FileCopyrightText: 2008 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef _KO_TRIANGLE_COLOR_SELECTOR_H_
#define _KO_TRIANGLE_COLOR_SELECTOR_H_

#include <QWidget>

#include "kowidgets_export.h"

class KoColor;
class KoColorDisplayRendererInterface;

class KOWIDGETS_EXPORT KoTriangleColorSelector : public QWidget
{
    Q_OBJECT
public:
    explicit KoTriangleColorSelector(QWidget *parent);
    explicit KoTriangleColorSelector(const KoColorDisplayRendererInterface *displayRenderer, QWidget *parent);
    ~KoTriangleColorSelector() override;

protected: // events
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

public:
    int hue() const;
    int value() const;
    int saturation() const;
    KoColor color() const;

public Q_SLOTS:
    void setHue(int h);
    void setValue(int v);
    void setSaturation(int s);
    void setHSV(int h, int s, int v);

    void setColor(const KoColor &);

Q_SIGNALS:
    void colorChanged(const KoColor &);

private Q_SLOTS:
    void configurationChanged();

private:
    void tellColorChanged();
    void generateTriangle();
    void generateWheel();
    void updateTriangleCircleParameters();
    void selectColorAt(int x, int y, bool checkInWheel = true);

private:
    struct Private;
    Private *const d;
};

#endif
