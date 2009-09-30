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
#include "SectionGroup.h"

class KActionCollection;
class KoUndoStack;
class QUndoCommand;
class ViewManager;
class SectionsIO;

class RootSection : public QObject, public SectionGroup {
    Q_OBJECT
  public:
    RootSection();
    ~RootSection();
    ViewManager* viewManager();
    SectionsIO* sectionsIO();
    void addCommand(Section* , QUndoCommand* command);
    void createActions(KActionCollection* );
    KoUndoStack* undoStack();
  signals:
    /// This signal is emited when a command is executed in the undo stack
    void commandExecuted();
  private:
    ViewManager* m_viewManager;
    KoUndoStack* m_undoStack;
    SectionsIO* m_sectionsSaver;
};

#endif
