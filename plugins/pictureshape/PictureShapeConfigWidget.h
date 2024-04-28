/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef PICTURESHAPECONFIGWIDGET_H
#define PICTURESHAPECONFIGWIDGET_H

#include <KoShapeConfigWidgetBase.h>
#include <QWidget>

class PictureShape;
class KFileWidget;
class KJob;

class PictureShapeConfigWidget : public KoShapeConfigWidgetBase
{
    Q_OBJECT
public:
    PictureShapeConfigWidget();
    ~PictureShapeConfigWidget() override;

    /// reimplemented from KoShapeConfigWidgetBase
    void open(KoShape *shape) override;
    /// reimplemented from KoShapeConfigWidgetBase
    void save() override;
    /// reimplemented from KoShapeConfigWidgetBase
    bool showOnShapeCreate() override;
    /// reimplemented from KoShapeConfigWidgetBase
    bool showOnShapeSelect() override;

private:
    void slotAccept();

private:
    PictureShape *m_shape;
    KFileWidget *m_fileWidget;
};

class PictureShapeLoadWaiter : public QObject
{
    Q_OBJECT
public:
    explicit PictureShapeLoadWaiter(PictureShape *shape)
        : m_pictureShape(shape)
    {
    }

public Q_SLOTS:
    void setImageData(KJob *job);

private:
    PictureShape *m_pictureShape;
};

#endif // PICTURESHAPECONFIGWIDGET_H
