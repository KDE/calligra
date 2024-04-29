/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Jeremy Lugagne <lugagne.jeremy@gmail.com>
 * SPDX-FileCopyrightText: 2011 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KoPathShapeMarkerCommand_H
#define KoPathShapeMarkerCommand_H

#include "flake_export.h"

#include "KoMarkerData.h"
#include <QList>
#include <kundo2command.h>

class KoPathShape;
class KoMarker;

/// The undo / redo command for setting the shape marker
class FLAKE_EXPORT KoPathShapeMarkerCommand : public KUndo2Command
{
public:
    /**
     * Command to set a new shape marker.
     * @param shapes a set of all the shapes that should get the new marker.
     * @param marker the new marker, the same for all given shapes
     * @param position the position - start or end - of the marker on the shape
     * @param parent the parent command used for macro commands
     */
    KoPathShapeMarkerCommand(const QList<KoPathShape *> &shapes, KoMarker *marker, KoMarkerData::MarkerPosition position, KUndo2Command *parent = nullptr);

    ~KoPathShapeMarkerCommand() override;
    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    QList<KoPathShape *> m_shapes; ///< the shapes to set marker for
    QList<KoMarker *> m_oldMarkers; ///< the old markers, one for each shape
    KoMarker *m_marker; ///< the new marker to set
    KoMarkerData::MarkerPosition m_position;
};

#endif // KoPathShapeMarkerCommand_H
