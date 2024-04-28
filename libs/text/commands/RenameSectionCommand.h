// This file is part of the KDE project
// SPDX-FileCopyrightText: 2014-2015 Denis Kuplaykov <dener.kup@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef RENAMESECTIONCOMMAND_H
#define RENAMESECTIONCOMMAND_H

#include <QString>

#include <kundo2qstack.h>

class QTextDocument;

class KoSection;
class KoSectionModel;

class RenameSectionCommand : public KUndo2Command
{
public:
    RenameSectionCommand(KoSection *section, const QString &newName, QTextDocument *document);
    ~RenameSectionCommand() override;

    void undo() override;
    void redo() override;

    bool mergeWith(const KUndo2Command *other) override;
    int id() const override;

private:
    KoSectionModel *m_sectionModel; ///< Pointer to document's KoSectionModel
    KoSection *m_section; ///< Section to rename
    QString m_newName; ///< New section name
    QString m_oldName; ///< Old section name (needed to undo)
    bool m_first; ///< Checks first call of redo
};

#endif // RENAMESECTIONCOMMAND_H
