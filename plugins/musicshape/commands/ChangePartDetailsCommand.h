/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef CHANGEPARTDETAILSCOMMAND_H
#define CHANGEPARTDETAILSCOMMAND_H

#include <QList>
#include <QPair>
#include <kundo2command.h>

namespace MusicCore
{
class Part;
class Staff;
class VoiceElement;
class Note;
}
class MusicShape;

class ChangePartDetailsCommand : public KUndo2Command
{
public:
    ChangePartDetailsCommand(MusicShape *shape, MusicCore::Part *part, const QString &name, const QString &abbreviation, int staffCount);
    void redo() override;
    void undo() override;

private:
    MusicShape *m_shape;
    MusicCore::Part *m_part;
    QString m_oldName, m_newName;
    QString m_oldAbbr, m_newAbbr;
    int m_oldStaffCount, m_newStaffCount;
    QList<MusicCore::Staff *> m_staves;
    QList<QPair<MusicCore::VoiceElement *, MusicCore::Staff *>> m_elements;
    QList<QPair<MusicCore::Note *, MusicCore::Staff *>> m_notes;
};

#endif // CHANGEPARTDETAILSCOMMAND_H
