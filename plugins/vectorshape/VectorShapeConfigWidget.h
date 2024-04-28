/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2012 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef VECTORSHAPECONFIGWIDGET_H
#define VECTORSHAPECONFIGWIDGET_H

#include <KoShapeConfigWidgetBase.h>
#include <QWidget>

class VectorShape;
class KFileWidget;
class KJob;

class VectorShapeConfigWidget : public KoShapeConfigWidgetBase
{
    Q_OBJECT
public:
    VectorShapeConfigWidget();
    ~VectorShapeConfigWidget() override;

    /// reimplemented from KoShapeConfigWidgetBase
    void open(KoShape *shape) override;
    /// reimplemented from KoShapeConfigWidgetBase
    void save() override;
    /// reimplemented from KoShapeConfigWidgetBase
    bool showOnShapeCreate() override;
    /// reimplemented from KoShapeConfigWidgetBase
    bool showOnShapeSelect() override;

private:
    VectorShape *m_shape;
    KFileWidget *m_fileWidget;
};

class LoadWaiter : public QObject
{
    Q_OBJECT
public:
    explicit LoadWaiter(VectorShape *shape)
        : m_vectorShape(shape)
    {
    }

public Q_SLOTS:
    void setImageData(KJob *job);

private:
    VectorShape *m_vectorShape;
};

#endif // VECTORSHAPECONFIGWIDGET_H
