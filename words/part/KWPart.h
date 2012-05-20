/* This file is part of the KDE project
   Copyright (C) 2012 C. Boemann <cbo@kogmbh.com>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef KWPART_H
#define KWPART_H

#include <KoPart.h>

#include "words_export.h"

class KoMainWindow;
class KWDocument;
class KComponentData;
class QGraphicsItem;

class WORDS_EXPORT KWPart : public KoPart
{
    Q_OBJECT

public:
    KWPart(QObject *parent);

    virtual ~KWPart();

    QGraphicsItem *createCanvasItem(); ///reimplemented

    void setDocument(KWDocument *document);

    /// reimplemented from super
    QList<KoPart::CustomDocumentWidgetItem> createCustomDocumentWidgets(QWidget *parent);

    void showStartUpWidget(KoMainWindow *parent, bool alwaysShow = false);

private slots:
     void showErrorAndDie();

protected:

    virtual KoView *createViewInstance(QWidget *parent); ///reimplemented

    KWDocument *m_document;
};

#endif
