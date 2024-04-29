/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2011 Silvio Heinrich <plassy@web.de>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef H_CROP_WIDGET_H
#define H_CROP_WIDGET_H

#include "SelectionRect.h"
#include <QImage>
#include <QWidget>

class PictureShape;

/**
 * This widget is a graphical user interface to the functionality
 * the SelectionRect class provides. It shows an image and the user can
 * move a rectangle freely to select a certain region of that image.
 */
class CropWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CropWidget(QWidget *parent = nullptr);

    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

    void setPictureShape(PictureShape *shape);
    void setCropRect(const QRectF &rect);
    void setKeepPictureProportion(bool keepProportion);
    void maximizeCroppedArea();

Q_SIGNALS:
    void sigCropRegionChanged(const QRectF &, bool undoLast);

private:
    void calcImageRect();
    void emitCropRegionChanged();
    QPointF toUniformCoord(const QPointF &coord) const;
    QPointF fromUniformCoord(const QPointF &coord) const;

private:
    PictureShape *m_pictureShape;
    QRectF m_imageRect;
    QRectF m_oldSelectionRect;
    SelectionRect m_selectionRect;
    bool m_isMousePressed;
    bool m_undoLast;
};

#endif // H_CROP_WIDGET_H
