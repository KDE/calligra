/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Jeremy Lugagne <lugagne.jeremy@gmail.com>
 * SPDX-FileCopyrightText: 2011 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPathShapeMarkerCommand.h"
#include "KoMarker.h"
#include "KoPathShape.h"

#include <KLocalizedString>

KoPathShapeMarkerCommand::KoPathShapeMarkerCommand(const QList<KoPathShape *> &shapes,
                                                   KoMarker *marker,
                                                   KoMarkerData::MarkerPosition position,
                                                   KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_shapes(shapes)
    , m_marker(marker)
    , m_position(position)
{
    setText(kundo2_i18n("Set marker"));

    // save old markers
    foreach (KoPathShape *shape, m_shapes) {
        m_oldMarkers.append(shape->marker(position));
    }
}

KoPathShapeMarkerCommand::~KoPathShapeMarkerCommand() = default;

void KoPathShapeMarkerCommand::redo()
{
    KUndo2Command::redo();
    foreach (KoPathShape *shape, m_shapes) {
        shape->setMarker(m_marker, m_position);
        shape->update();
    }
}

void KoPathShapeMarkerCommand::undo()
{
    KUndo2Command::undo();
    QList<KoMarker *>::ConstIterator markerIt = m_oldMarkers.constBegin();
    foreach (KoPathShape *shape, m_shapes) {
        shape->setMarker(*markerIt, m_position);
        shape->update();
        ++markerIt;
    }
}
