/* This file is part of the KDE project
   Copyright (C)  2006 Peter Simonsson <peter.simonsson@gmail.com>

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

#include <KoDocument.h>

class QDomDocument;
class QIODevice;
class QPainter;
class QRect;

class KCommand;
class KCommandHistory;

class KoStore;
class KoOasisStyle;
class KoXmlWriter;
class KoView;

class KivioDocument : public KoDocument
{
  Q_OBJECT

  public:
    KivioDocument(QWidget* parentWidget, QObject* parent, bool singleViewMode = false);
    ~KivioDocument();

    virtual void paintContent(QPainter &painter, const QRect &rect, bool transparent = false,
                              double zoomX = 1.0, double zoomY = 1.0);

    virtual bool loadXML(QIODevice* device, const QDomDocument& doc);

    virtual bool loadOasis(const QDomDocument& doc, KoOasisStyles& oasisStyles,
                           const QDomDocument& settings, KoStore* store);

    virtual bool saveOasis(KoStore* store, KoXmlWriter* manifestWriter);

    virtual void addShell(KoMainWindow* shell);

    /// Add @p command to the command history
    void addCommand(KCommand* command, bool execute);

  public slots:
    void slotDocumentRestored();
    void slotCommandExecuted();

  protected:
    virtual KoView* createViewInstance(QWidget* parent, const char* name);

  private:
    KCommandHistory* m_commandHistory;
};

#endif
