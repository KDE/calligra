/**
 * SPDX-FileCopyrightText: 2006 C. Boemann (cbo@boemann.dk)
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOCOLORPATCH_H
#define KOCOLORPATCH_H

#include <QFrame>

#include "kowidgets_export.h"
#include <KoColor.h>

/**
 *  The small widget showing the selected color
 */
class KOWIDGETS_EXPORT KoColorPatch : public QFrame
{
    Q_OBJECT
public:
    explicit KoColorPatch(QWidget *parent);
    ~KoColorPatch() override;

    /**
     * Set the color of this color patch
     * @param c the new color
     */
    void setColor(const KoColor &c);

    /**
     * @return current color shown by this patch
     */
    KoColor color() const;

protected:
    void mousePressEvent(QMouseEvent *e) override; ///< reimplemented from QFrame
    void paintEvent(QPaintEvent *e) override; ///< reimplemented from QFrame
    QSize sizeHint() const override; ///< reimplemented from QFrame

Q_SIGNALS:

    /**
     * Emitted when the mouse is released.
     * @param widget a pointer to this widget
     */
    void triggered(KoColorPatch *widget);

private:
    KoColor m_color;
};

#endif
