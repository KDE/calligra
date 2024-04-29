/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "Part.h"
#include "Sheet.h"
#include "Staff.h"
#include "Voice.h"
#include <QList>

namespace MusicCore
{

class Part::Private
{
public:
    QString name;
    QString shortName;
    QList<Staff *> staves;
    QList<Voice *> voices;
};

Part::Part(Sheet *sheet, const QString &name)
    : QObject(sheet)
    , d(new Private)
{
    d->name = name;
}

Part::~Part()
{
    delete d;
}

Sheet *Part::sheet()
{
    return qobject_cast<Sheet *>(parent());
}

QString Part::name() const
{
    return d->name;
}

void Part::setName(const QString &name)
{
    if (d->name == name)
        return;
    d->name = name;
    Q_EMIT nameChanged(name);
    if (d->shortName.isNull())
        Q_EMIT shortNameChanged(name);
}

QString Part::shortName(bool useFull) const
{
    if (d->shortName.isNull() && useFull) {
        return d->name;
    } else {
        return d->shortName;
    }
}

void Part::setShortName(const QString &name)
{
    if (d->shortName == name)
        return;
    d->shortName = name;
    Q_EMIT shortNameChanged(shortName());
}

int Part::staffCount() const
{
    return d->staves.size();
}

Staff *Part::staff(int index)
{
    Q_ASSERT(index >= 0 && index < staffCount());
    return d->staves[index];
}

Staff *Part::addStaff()
{
    Staff *staff = new Staff(this);
    d->staves.append(staff);
    return staff;
}

void Part::addStaff(Staff *staff)
{
    Q_ASSERT(staff);
    d->staves.append(staff);
}

Staff *Part::insertStaff(int before)
{
    Q_ASSERT(before >= 0 && before <= staffCount());
    Staff *staff = new Staff(this);
    d->staves.insert(before, staff);
    return staff;
}

int Part::indexOfStaff(Staff *staff)
{
    Q_ASSERT(staff);
    return d->staves.indexOf(staff);
}

void Part::removeStaff(Staff *staff, bool deleteStaff)
{
    Q_ASSERT(staff);
    d->staves.removeAll(staff);
    if (deleteStaff)
        delete staff;
}

int Part::voiceCount() const
{
    return d->voices.size();
}

Voice *Part::voice(int index)
{
    Q_ASSERT(index >= 0 && index < voiceCount());
    return d->voices[index];
}

Voice *Part::addVoice()
{
    Voice *voice = new Voice(this);
    d->voices.append(voice);
    return voice;
}

int Part::indexOfVoice(Voice *voice)
{
    Q_ASSERT(voice);
    return d->voices.indexOf(voice);
}

} // namespace
