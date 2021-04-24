/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef MUSIC_CORE_STAFFSYSTEM_H
#define MUSIC_CORE_STAFFSYSTEM_H

#include <QObject>
#include <QList>

namespace MusicCore {

class Sheet;
class Clef;
class Staff;

class StaffSystem : public QObject
{
    Q_OBJECT
public:
    explicit StaffSystem(Sheet *sheet);
    ~StaffSystem() override;

    qreal top() const;
    qreal height() const;
    void setHeight(qreal height);
    int firstBar() const;
    qreal indent() const;
    void setIndent(qreal indent);
    qreal lineWidth() const;
    void setLineWidth(qreal width);
    QList<Clef*> clefs() const;
    Clef* clef(Staff* staff) const;
    void setClefs(QList<Clef*> clefs);
public Q_SLOTS:
    void setTop(qreal top);
    void setFirstBar(int bar);
Q_SIGNALS:
    void topChanged(qreal top);
    void firstBarChanged(int bar);
private:
    class Private;
    Private * const d;
};

} // namespace MusicCore

#endif // MUSIC_CORE_STAFFSYSTEM_H
