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

#ifndef BRAINDUMPVIEW_H
#define BRAINDUMPVIEW_H

#include <KoPAView.h>

class KAction;

class Document;

class BrainDumpView : public KoPAView
{
  Q_OBJECT

  public:
    BrainDumpView(Document* document, QWidget* parent);
    ~BrainDumpView();

    /// Returns the document
    Document* document() const;

  protected slots:
    /// Called when the doc emits updateGui
    void updateGui();

  protected:
    /// Creates and initializes the GUI.
    void initializeGUI();
    /// Initializes all the actions
    void initializeActions();

  private:
    Document* m_document;
};

#endif
