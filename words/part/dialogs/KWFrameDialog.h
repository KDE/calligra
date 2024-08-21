/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2011 KoGmbh <cbo@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KWFRAMEDIALOG_H
#define KWFRAMEDIALOG_H

#include <KoShapeConfigFactoryBase.h>

#include <QCheckBox>
#include <QList>
#include <kpagedialog.h>

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
    KWFrameDialog(const QList<KoShape *> &selectedShapes, KWDocument *document, KWCanvas *canvas = nullptr);
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
        On, ///< on
        Off, ///< off
        TriState ///< Both on and off
    };
    /// constructor
    GuiHelper()
        : m_state(Unset)
    {
    }
    /// Add a new state
    void addState(State state)
    {
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
    void updateCheckBox(QCheckBox *checkbox, bool hide)
    {
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
