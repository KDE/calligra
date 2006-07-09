/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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

#ifndef __MAP_H__
#define __MAP_H__

#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>

#include <koffice_export.h>

class KoStore;
class KoOasisLoadingContext;
class KoOasisStyles;

class QDomElement;
class QDomDocument;
class KoXmlWriter;
class KoGenStyles;
class KoOasisSettings;

namespace KSpread
{
class Changes;
class Map;
class Doc;
class Sheet;
class GenValidationStyles;
class Style;
class MapAdaptor;

/**
 * A map is a simple container for all sheets. Usually a complete map
 * is saved in one file.
 */
class KSPREAD_EXPORT Map : public QObject
{
Q_OBJECT
public:
  /**
   * Created an empty map.
   */
  Map(Doc* doc, const char* name = 0);

  /**
   * This deletes all sheets contained in this map.
   */
  virtual ~Map();

  Doc* doc() const;

  /**
   * \ingroup OpenDocument
   */
  void saveOasisSettings( KoXmlWriter &settingsWriter );

  /**
   * \ingroup OpenDocument
   */
  void loadOasisSettings( KoOasisSettings &settings );

  /**
   * \ingroup OpenDocument
   */
  bool saveOasis( KoXmlWriter & xmlWriter, KoGenStyles & mainStyles,
                  KoStore *store, KoXmlWriter* manifestWriter,
                  int &_indexObj, int &_partIndexObj );

  /**
   * \ingroup OpenDocument
   */
  bool loadOasis( const QDomElement& mymap, KoOasisLoadingContext& oasisContext );

  /**
   * \ingroup NativeFormat
   */
  bool loadXML( const QDomElement& mymap );

  /**
   * \ingroup NativeFormat
   */
  QDomElement save( QDomDocument& doc );


  bool loadChildren( KoStore* _store );

  bool saveChildren( KoStore* _store );

  void password( QByteArray & passwd ) const { passwd = m_strPassword; }
  bool isProtected() const { return !m_strPassword.isNull(); }
  void setProtected( QByteArray const & passwd );
  bool checkPassword( QByteArray const & passwd ) const { return ( passwd == m_strPassword ); }

  /**
   * The sheet named @p _from is being moved to the sheet @p _to.
   * If @p  _before is true @p _from is inserted before (after otherwise)
   * @p  _to.
   */
  void moveSheet( const QString & _from, const QString & _to, bool _before = true );

  /**
   * Searches for a sheet named @p name .
   * @return a pointer to the searched sheet
   * @return @c 0 if nothing was found
   */
  Sheet* findSheet( const QString& name );

  /**
   * @return a pointer to the next sheet to @p sheet
   */
  Sheet* nextSheet( Sheet* sheet );

  /**
   * @return a pointer to the previous sheet to @p sheet
   */
  Sheet* previousSheet( Sheet* );

  Sheet* initialActiveSheet()const { return m_initialActiveSheet; }
  int initialMarkerColumn() const { return m_initialMarkerColumn; }
  int initialMarkerRow()    const { return m_initialMarkerRow; }
  double initialXOffset()   const { return m_initialXOffset; }
  double initialYOffset()   const { return m_initialYOffset; }


  /**
   * Creates a new sheet.
   * The sheet is not added to the map nor added to the GUI.
   * @return a pointer to a new Sheet
   */
  Sheet* createSheet();

  /**
   * Adds @p sheet to this map.
   * The sheet becomes the active sheet.
  */
  void addSheet( Sheet* sheet );

  /**
   * Creates a new sheet.
   * Adds a new sheet to this map.
   * @return a pointer to the new sheet
   */
  Sheet* addNewSheet();

  /**
   * @return a pointer to the sheet at index @p index in this map
   * @return @c 0 if the index exceeds the list boundaries
   */
  Sheet* sheet( int index ) const { return m_lstSheets.value( index );  }

  /**
   * @return the list of sheets in this map
   */
  QList<Sheet*>& sheetList() { return m_lstSheets; }

  /**
   * @return amount of sheets in this map
   */
  int count() const { return m_lstSheets.count(); }

  void update();

  virtual MapAdaptor* dbusObject();

  void takeSheet( Sheet * sheet );
  void insertSheet( Sheet * sheet );

  QStringList visibleSheets() const;
  QStringList hiddenSheets() const;

    static bool respectCase;

signals:

  /**
   * Emitted if a new table is added to the document.
   */
  void sig_addSheet( Sheet *_table );
private:

  Doc* m_doc;

  /**
   * List of all sheets in this map. The list has autodelete turned on.
   */
  QList<Sheet*> m_lstSheets;
  QList<Sheet*> m_lstDeletedSheets;

  /**
   * Password to protect the map from being changed.
   */
  QByteArray m_strPassword;
  /**
   * Set from the XML
   */
  Sheet * m_initialActiveSheet;
  int m_initialMarkerColumn;
  int m_initialMarkerRow;
  double m_initialXOffset;
  double m_initialYOffset;

  // used to give every Sheet a unique default name.
  int tableId;

  MapAdaptor* m_dbus;
};

} // namespace KSpread

#endif
