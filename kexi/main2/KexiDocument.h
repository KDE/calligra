/* This file is part of the KDE project

   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003-2006 Jaroslaw Staniek <js@iidea.pl>
   Copyright (C) 2005-2006 Sebastian Sauer <mail@dipe.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KEXIDOCUMENT_H
#define KEXIDOCUMENT_H

#include <QList>
#include <QMap>
#include <QRect>
#include <QString>

//#include <kglobalsettings.h>

#include <KoDocument.h>
//#include <KoXmlReader.h>
#include <KoZoomHandler.h>

//#include "Global.h"

//class QDomDocument;
//class QObject;
//class QPainter;

// class DCOPObject;
//class KCommand;
//class KCommandHistory;
//class KCompletion;
//class K3SpellConfig;
//class KoGenStyles;
//class KoOasisSettings;
//class KoPicture;
//class KoPictureCollection;
//class KoPictureKey;
class KoStore;
class KoXmlWriter;

class KDE_EXPORT KexiDocument : public KoDocument, public KoZoomHandler
{
        Q_OBJECT
    public:

        /**
        * Creates a new document.
        * @param parentWidget the parent widget
        * @param parent the parent object
        * @param singleViewMode enables single view mode, if @c true
        */
        explicit KexiDocument(QWidget *parentWidget = 0, QObject* parent = 0, bool singleViewMode = false);

        /**
        * Destroys the document.
        */
        ~KexiDocument();

        /**
        * Main painting method.
        * Implements the abstract \a KoDocument::paintContent method.
        */
        virtual void paintContent(QPainter& painter, const QRect& rect, bool transparent = false, double zoomX = 1.0, double zoomY = 1.0);

        /**
        * Main loading method.
        * Implements the abstract \a KoDocument::loadXML method.
        */
        virtual bool loadXML(QIODevice*, const KoXmlDocument& doc);

        /**
        * Main loading method.
        * Implements the abstract \a KoDocument::loadOasis method.
        */
        virtual bool loadOasis(const KoXmlDocument& doc, KoOasisStyles& oasisStyles, const KoXmlDocument& settings, KoStore*);

        /**
        * Main saving method.
        * Implements the abstract \a KoDocument::saveOasis method.
        */
        virtual bool saveOasis(KoStore* store, KoXmlWriter* manifestWriter);

    protected:

        /**
        * Create and return a new \a KexiView instance.
        * Implement abstract \a KoDocument::createViewInstance method.
        */
        virtual KoView* createViewInstance(QWidget* parent);

    private:
        class Private;
        Private* const d;
};

#endif
