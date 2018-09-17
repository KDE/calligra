/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _ROOT_SECTION_H_
#define _ROOT_SECTION_H_

#include <QObject>
#include <QMap>

#include "SectionGroup.h"

class KActionCollection;
class KUndo2Stack;
class KUndo2Command;
class ViewManager;
class SectionsIO;

class RootSection : public QObject, public SectionGroup
{
    Q_OBJECT
public:
    RootSection();
    ~RootSection();
    ViewManager* viewManager();
    SectionsIO* sectionsIO();
    void addCommand(Section* , KUndo2Command* command);
    void createActions(KActionCollection*);
    KUndo2Stack* undoStack(); // TODO remove when it is again possible to hide the undo stack
    void setCurrentSection(Section*);  // TODO when the command stack is hidden again, remove
Q_SIGNALS:
    /// This signal is emitted when a command is executed in the undo stack
    void commandExecuted();
private Q_SLOTS:
    void undoIndexChanged(int idx);
private:
    KUndo2Stack* m_undoStack;
    ViewManager* m_viewManager;
    SectionsIO* m_sectionsSaver;
    QMap<const KUndo2Command*, Section* > m_commandsMap;
    Section* m_currentSection; // TODO when the command stack is hidden again, remove
};

#endif
