/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef DELETEANNOTATIONSCOMMAND_H
#define DELETEANNOTATIONSCOMMAND_H

#include <kundo2command.h>

#include <QList>

class QTextDocument;
class KoAnnotation;

class DeleteAnnotationsCommand : public KUndo2Command
{
public:
    DeleteAnnotationsCommand(const QList<KoAnnotation *> &annotations, QTextDocument *document, KUndo2Command *parent);
    ~DeleteAnnotationsCommand() override;

    void redo() override;
    void undo() override;

private:
    QList<KoAnnotation *> m_annotations;
    QTextDocument *m_document;
    bool m_deleteAnnotations;
};

#endif /* DELETEANNOTATIONSCOMMAND_H */
