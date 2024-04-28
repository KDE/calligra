/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef VIDEOSHAPECONFIGWIDGET_H
#define VIDEOSHAPECONFIGWIDGET_H

#include <KoShapeConfigWidgetBase.h>

#include <QUrl>
#include <QWidget>

class VideoShape;
class SelectVideoWidget;

class VideoShapeConfigWidget : public KoShapeConfigWidgetBase
{
    Q_OBJECT
public:
    VideoShapeConfigWidget();
    ~VideoShapeConfigWidget();

    /// reimplemented from KoShapeConfigWidgetBase
    void open(KoShape *shape) override;
    /// reimplemented from KoShapeConfigWidgetBase
    void save() override;
    /// reimplemented from KoShapeConfigWidgetBase
    bool showOnShapeCreate() override;
    /// reimplemented from KoShapeConfigWidgetBase
    bool showOnShapeSelect() override;

private:
    VideoShape *m_shape;
    SelectVideoWidget *m_fileSelectionWidget;
};

#endif // VIDEOSHAPECONFIGWIDGET_H
