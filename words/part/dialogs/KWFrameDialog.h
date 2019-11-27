/* This file is part of the KDE project
 * Copyright (C) 2006-2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2011 KoGmbh <cbo@kogmbh.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KWFRAMEDIALOG_H
#define KWFRAMEDIALOG_H

#include <KoShapeConfigFactoryBase.h>

#include <kpagedialog.h>
#include <QList>
#include <QCheckBox>

class KWFrameConnectSelector;
class KWRunAroundProperties;
class KWAnchoringProperties;
class KWFrame;
class KWDocument;
class FrameConfigSharedState;
class KWCanvas;

/// A dialog for showing and altering frame properties
class KWFrameDialog : public KPageDialog
{
    Q_OBJECT
public:
    /**
     * Constructor.
     * @param selectedShapes all shapes that this dialog will show for user modification
     * @param document the parent document where the frames belong to
     * @param canvas the canvas for centering the dialog and providing undobuffer
     */
    KWFrameDialog(const QList<KoShape *> &selectedShapes, KWDocument *document, KWCanvas *canvas = 0);
    ~KWFrameDialog() override;

    /**
     * Create a list of factories that will be able to create widgets to configure shapes.
     * @param document the parent document these panels will work for.
     */
    static QList<KoShapeConfigFactoryBase *> panels(KWDocument *document);

private Q_SLOTS:
    void okClicked();
    void cancelClicked();

private:
    KWFrameConnectSelector *m_frameConnectSelector;
    KWRunAroundProperties *m_runAroundProperties;
    KWAnchoringProperties *m_anchoringProperties;
    FrameConfigSharedState *m_state;
    KWCanvas *m_canvas;
};

/// A simple class useful for finding out if a series of data object will cause a
/// normal or a tri-state checkbox. For example.
class GuiHelper
{
public:
    /// the different states
    enum State {
        Unset, ///< start value
        On,     ///< on
        Off,    ///< off
        TriState ///< Both on and off
    };
    /// constructor
    GuiHelper() : m_state(Unset) { }
    /// Add a new state
    void addState(State state) {
        if (m_state == Unset)
            m_state = state;
        else if (m_state != state)
            m_state = TriState;
    }

    /**
     * Based on all the added states initialize the checkbox.
     * @param checkbox the checkbox to set.
     * @param hide if true the checkbox will be hidden when there was no 'addState' called
     */
    void updateCheckBox(QCheckBox *checkbox, bool hide) {
        if (m_state == Unset) {
            if (hide)
                checkbox->setVisible(false);
            checkbox->setEnabled(false);
            checkbox->setTristate(true);
            checkbox->setCheckState(Qt::PartiallyChecked);
        } else if (m_state == TriState) {
            checkbox->setTristate(true);
            checkbox->setCheckState(Qt::PartiallyChecked);
        } else {
            checkbox->setCheckState(m_state == On ? Qt::Checked : Qt::Unchecked);
        }
    }

    State m_state; ///< the current state
};

#endif
