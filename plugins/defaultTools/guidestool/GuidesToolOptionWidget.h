/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef GUIDESTOOLOPTIONWIDGET_H
#define GUIDESTOOLOPTIONWIDGET_H

#include <ui_GuidesToolOptionWidget.h>

#include <KoUnit.h>

#include <QWidget>

class GuidesToolOptionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GuidesToolOptionWidget(QWidget *parent = nullptr);
    ~GuidesToolOptionWidget() override;

    /// Sets horizontal guide lines
    void setHorizontalGuideLines(const QList<qreal> &lines);

    /// Sets vertical guide lines
    void setVerticalGuideLines(const QList<qreal> &lines);

    /// Returns the list of horizontal guide lines.
    QList<qreal> horizontalGuideLines() const;

    /// Returns the list of vertical guide lines.
    QList<qreal> verticalGuideLines() const;

    /// Returns the current selected lines orientation
    Qt::Orientation orientation() const;

    /// Sets the current selected lines orientation
    void setOrientation(Qt::Orientation orientation);

    /// Selects the given guide line
    void selectGuideLine(Qt::Orientation orientation, int index);

    /// Sets the unit to be displayed
    void setUnit(const KoUnit &unit);

Q_SIGNALS:
    /// Emitted whenever a specific guide line was selected
    void guideLineSelected(Qt::Orientation orientation, int index);
    /// Emitted whenever a guide line with the given orientation has changed
    void guideLinesChanged(Qt::Orientation orientation);

private Q_SLOTS:
    void updateList(int orientation);
    void updatePosition(int index);
    void positionChanged(qreal position);
    void removeLine();
    void addLine();

private:
    Ui_GuidesToolOptionWidget widget;
    QList<qreal> m_hGuides;
    QList<qreal> m_vGuides;
    KoUnit m_unit;
};

#endif // GUIDESTOOLOPTIONWIDGET_H
