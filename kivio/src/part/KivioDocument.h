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

#include <QList>

#include <KoDocument.h>

class QDomDocument;
class QIODevice;
class QPainter;
class QRect;

class KCommand;
class KCommandHistory;

class KoStore;
class KoOasisStyle;
class KoOasisContext;
class KoXmlWriter;
class KoView;

class KivioMasterPage;
class KivioPage;

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

    /// Reimplemented for command history support
    virtual void addShell(KoMainWindow* shell);

    /// Add @p command to the command history
    void addCommand(KCommand* command, bool execute);

    /* ---------Page handling--------- */
    /** Creates a master page and adds it to the document
      * @param title Title of the master page
      * @return Pointer to the new master page
      */
    KivioMasterPage* addMasterPage(const QString& title);

    /** Creates a page and adds it to the document
      * @param masterPage Master page for the page
      * @param title Title of the page
      * @return Pointer to the new page or null if the page couldn't be created
      */
    KivioPage* addPage(KivioMasterPage* masterPage, const QString& title);

    /** Returns the master page at @p index in the list
      * @param index Index in the list
      * @return Pointer to master page if @p index exists in the list else null
      */
    KivioMasterPage* masterPageByIndex(int index);

    /** Returns the page at @p index in the list
     * @param index Index in the list
     * @return Pointer to page if @p index exists in the list else null
     */
    KivioPage* pageByIndex(int index);

  public slots:
    /// This is called when the last saved state of the document has been restored
    void slotDocumentRestored();
    /// This is called whenever a command has been executed or unexecuted
    void slotCommandExecuted();

  protected:
    /// Creates a KivioView instance and returns it
    virtual KoView* createViewInstance(QWidget* parent, const char* name);

    /** Load OpenDoc master pages
      * @return true if loading was successful else false
      */
    bool loadMasterPages(const KoOasisContext& oasisContext);

  private:
    KCommandHistory* m_commandHistory;

    QList<KivioMasterPage*> m_masterPageList;
    QList<KivioPage*> m_pageList;
};

#endif
