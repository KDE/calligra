/* This file is part of the KDE project
   Copyright (C) 2003 Norbert Andres, nandres@web.de

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


#ifndef __kspread_style_manager__
#define __kspread_style_manager__

#include <qmap.h>
#include <koffice_export.h>
class KSpreadCustomStyle;
class KSpreadStyleDlg;
class KSpreadView;

class QDomElement;
class QDomDocument;
class QStringList;

class KoGenStyles;
class KoOasisStyles;

class KSPREAD_EXPORT KSpreadStyleManager
{
 public:
  KSpreadStyleManager();
  ~KSpreadStyleManager();

  QDomElement save( QDomDocument & doc );
  bool loadXML( QDomElement const & styles );

    void saveOasis( KoGenStyles &mainStyles );
    void loadOasisStyleTemplate(  KoOasisStyles& oasisStyles );

  KSpreadCustomStyle * defaultStyle() const { return m_defaultStyle; }
  KSpreadCustomStyle * style( QString const & name ) const;

  bool checkCircle( QString const & name, QString const & parent );
  bool validateStyleName( QString const & name, KSpreadCustomStyle * style );
  void changeName( QString const & oldName, QString const & newName );

  void takeStyle( KSpreadCustomStyle * style );
  void createBuiltinStyles();

  QStringList styleNames() const;
  int count() const { return m_styles.count(); }

 private:
  friend class KSpreadStyleDlg;
  friend class KSpreadView;
  class Styles : public QMap<QString, KSpreadCustomStyle *> {};

  KSpreadCustomStyle * m_defaultStyle;
  Styles               m_styles; // builtin and custom made styles
};

#endif
