/* This file is part of the KDE project
   Copyright (C)  2006 Peter Simonsson <peter.simonsson@gmail.com>
   Copyright (C)  2007 Thorsten Zachmann <zachmann@kde.okde.org>
   Copyright (C) 2010 Boudewijn Rempt <boud@valdyas.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KIVIODOCUMENT_H
#define KIVIODOCUMENT_H

#include <KoPADocument.h>

class KivioDocument : public KoPADocument
{
    Q_OBJECT

    public:
        KivioDocument(QWidget* parentWidget, QObject* parent, bool singleViewMode = false);
        ~KivioDocument();

        virtual KoOdf::DocumentType documentType() const;

    signals:
        /// Emitted when the gui needs to be updated.
        void updateGui();

    protected:
        /// Creates a KivioView instance and returns it
        virtual KoView* createViewInstance(QWidget* parent);
        virtual QGraphicsItem *createCanvasItem();
        const char *odfTagName( bool withNamespace );
};

#endif
