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

#ifndef BRAINDUMPDOCUMENT_H
#define BRAINDUMPDOCUMENT_H

#include <KoPADocument.h>
#include <KComponentData>

class BrainDumpDocument : public KoPADocument
{
    Q_OBJECT

    public:
        BrainDumpDocument(QWidget* parentWidget, QObject* parent, bool singleViewMode = false);
        ~BrainDumpDocument();

        virtual KoOdf::DocumentType documentType() const;

    signals:
        /// Emitted when the gui needs to be updated.
        void updateGui();

    protected:
        /// Creates a BrainDumpView instance and returns it
        virtual KoView* createViewInstance(QWidget* parent);
        const char *odfTagName( bool withNamespace );
  private:
    KAboutData* m_aboutData;
    KComponentData* m_documentData;
};

#endif
