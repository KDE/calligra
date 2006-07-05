/* This file is part of the KDE project
 * Copyright (C) 2006 Thomas Zander <zander@kde.org>
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

#include <KoShapeConfigFactory.h>

#include <kpagedialog.h>
#include <QList>
#include <QCheckBox>

class KWFrameConnectSelector;
class KWFrameGeometry;
class KWFrameRunaroundProperties;
class KWGeneralFrameProperties;
class KWFrame;
class KWFrameSet;
class KWDocument;
class FrameConfigSharedState;

class KWFrameDialog : public KPageDialog {
    Q_OBJECT
public:
    KWFrameDialog (const QList<KWFrame*> &selectedFrames, KWDocument *document, QWidget *parent=0);
    ~KWFrameDialog();

    static QList<KoShapeConfigFactory *> panels(KWDocument *document);

private slots:
    void okClicked();

private:
    KWFrameConnectSelector *m_frameConnectSelector;
    KWFrameGeometry *m_frameGeometry;
    KWFrameRunaroundProperties *m_frameRunaroundProperties;
    KWGeneralFrameProperties *m_generalFrameProperties;
    FrameConfigSharedState *m_state;
};

class GuiHelper {
public:
    enum State {
        Unset,
        On,
        Off,
        TriState
    };
    GuiHelper() : m_state(Unset) { }
    void addState(State state) {
        if(m_state == Unset)
            m_state = state;
        else if(m_state != state)
            m_state = TriState;
    }

    void updateCheckBox(QCheckBox *checkbox, bool hide) {
        if(m_state == Unset) {
            if(hide)
                checkbox->setVisible(false);
            checkbox->setEnabled(false);
            checkbox->setTristate(true);
            checkbox->setCheckState(Qt::PartiallyChecked);
        } else if(m_state == TriState) {
            checkbox->setTristate(true);
            checkbox->setCheckState(Qt::PartiallyChecked);
        } else {
            checkbox->setCheckState(m_state == On ? Qt::Checked : Qt::Unchecked);
        }
    }

    State m_state;
};

#endif
