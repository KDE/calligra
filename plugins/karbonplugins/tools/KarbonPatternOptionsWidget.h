/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KARBONPATTERNOPTIONSWIDGET_H
#define KARBONPATTERNOPTIONSWIDGET_H

#include <KoPatternBackground.h>

#include <QWidget>

class KarbonPatternOptionsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KarbonPatternOptionsWidget(QWidget *parent = nullptr);
    ~KarbonPatternOptionsWidget() override;

    /// Sets the pattern repeat
    void setRepeat(KoPatternBackground::PatternRepeat repeat);

    /// Return the pattern repeat
    KoPatternBackground::PatternRepeat repeat() const;

    /// Returns the pattern reference point identifier
    KoPatternBackground::ReferencePoint referencePoint() const;

    /// Sets the pattern reference point
    void setReferencePoint(KoPatternBackground::ReferencePoint referencePoint);

    /// Returns reference point offset in percent of the size to fill
    QPointF referencePointOffset() const;

    /// Sets the reference point offset in percent of the size to fill
    void setReferencePointOffset(const QPointF &offset);

    /// Returns tile repeat offset in percent of the size to fill
    QPointF tileRepeatOffset() const;

    /// Sets the tile repeat offset in percent of the size to fill
    void setTileRepeatOffset(const QPointF &offset);

    /// Returns the pattern size
    QSize patternSize() const;

    /// Sets the pattern size
    void setPatternSize(const QSize &size);

Q_SIGNALS:
    /// is emitted whenever an option has changed
    void patternChanged();
private Q_SLOTS:
    void updateControls();

private:
    class Private;
    Private *const d;
};

#endif // KARBONPATTERNOPTIONSWIDGET_H
