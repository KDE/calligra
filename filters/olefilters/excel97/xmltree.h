/* This file is part of the KDE project

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef XMLTREE_H
#define XMLTREE_H

#include <qdom.h>
#include <qarray.h>
#include <qqueue.h>
#include <qobject.h>
#include <qintdict.h>
#include <klocale.h>

const int BIFF_5_7 = 0x0500;
const int BIFF_8 = 0x0600;

class XMLTree:public QObject
{
public:
  XMLTree();
  ~XMLTree();

  const QDomDocument * const part();

  bool _1904(Q_UINT16 size, QDataStream& body);
  bool _array(Q_UINT16, QDataStream&);
  bool _backup(Q_UINT16, QDataStream&);
  bool _blank(Q_UINT16 size, QDataStream& body);
  bool _bof(Q_UINT16 size, QDataStream& body);
  bool _bookbool(Q_UINT16, QDataStream&);
  bool _boolerr(Q_UINT16, QDataStream&);
  bool _bottommargin(Q_UINT16 size, QDataStream& body);
  bool _boundsheet(Q_UINT16 size, QDataStream& body);
  bool _cf(Q_UINT16, QDataStream&);
  bool _condfmt(Q_UINT16, QDataStream&);
  bool _codepage(Q_UINT16, QDataStream&);
  bool _colinfo(Q_UINT16 size, QDataStream& body);
  bool _country(Q_UINT16, QDataStream&);
  bool _crn(Q_UINT16, QDataStream&);
  bool _dbcell(Q_UINT16, QDataStream&);
  bool _defaultrowheight(Q_UINT16, QDataStream&);
  bool _defcolwidth(Q_UINT16, QDataStream&);
  bool _dimensions(Q_UINT16, QDataStream&);
  bool _eof(Q_UINT16, QDataStream&);
  bool _externcount(Q_UINT16, QDataStream&);
  bool _externname(Q_UINT16, QDataStream&);
  bool _externsheet(Q_UINT16, QDataStream&);
  bool _extsst(Q_UINT16, QDataStream&);
  bool _filepass(Q_UINT16, QDataStream&);
  bool _filesharing(Q_UINT16, QDataStream&);
  bool _filesharing2(Q_UINT16, QDataStream&);
  bool _font(Q_UINT16 size, QDataStream& body);
  bool _footer(Q_UINT16 size, QDataStream& body);
  bool _format(Q_UINT16 size, QDataStream& body);
  bool _formula(Q_UINT16 size, QDataStream& body);
  bool _gcw(Q_UINT16, QDataStream&);
  bool _guts(Q_UINT16, QDataStream&);
  bool _hcenter(Q_UINT16, QDataStream&);
  bool _header(Q_UINT16 size, QDataStream& body);
  bool _hlink(Q_UINT16, QDataStream&);
  bool _horizontalpagebreaks(Q_UINT16, QDataStream&);
  bool _imdata(Q_UINT16, QDataStream&);
  bool _label(Q_UINT16 size, QDataStream& body);
  bool _labelsst(Q_UINT16 size, QDataStream& body);
  bool _leftmargin(Q_UINT16 size, QDataStream& body);
  bool _mulblank(Q_UINT16 size, QDataStream& body);
  bool _mulrk(Q_UINT16 size, QDataStream& body);
  bool _name(Q_UINT16, QDataStream&);
  bool _note(Q_UINT16, QDataStream&);
  bool _number(Q_UINT16 size, QDataStream& body);
  bool _pane(Q_UINT16, QDataStream&);
  bool _paramqry(Q_UINT16, QDataStream&);
  bool _password(Q_UINT16, QDataStream&);
  bool _protect(Q_UINT16, QDataStream&);
  bool _qsi(Q_UINT16, QDataStream&);
  bool _recipname(Q_UINT16, QDataStream&);
  bool _rightmargin(Q_UINT16 size, QDataStream& body);
  bool _rk(Q_UINT16 size, QDataStream& body);
  bool _row(Q_UINT16 size, QDataStream& body);
  bool _scl(Q_UINT16, QDataStream&);
  bool _setup(Q_UINT16, QDataStream&);
  bool _shrfmla(Q_UINT16, QDataStream&);
  bool _sort(Q_UINT16, QDataStream&);
  bool _sst(Q_UINT16 size, QDataStream& body);
  bool _standardwidth(Q_UINT16, QDataStream&);
  bool _string(Q_UINT16, QDataStream&);
  bool _tabid(Q_UINT16, QDataStream&);
  bool _tabidconf(Q_UINT16, QDataStream&);
  bool _table(Q_UINT16, QDataStream&);
  bool _topmargin(Q_UINT16 size, QDataStream& body);
  bool _vcenter(Q_UINT16, QDataStream&);
  bool _verticalpagebreaks(Q_UINT16, QDataStream&);
  bool _window1(Q_UINT16, QDataStream&);
  bool _window2(Q_UINT16, QDataStream&);
  bool _writeaccess(Q_UINT16 size, QDataStream& body);
  bool _writeprot(Q_UINT16, QDataStream&);
  bool _wsbool(Q_UINT16, QDataStream&);
  bool _xf(Q_UINT16 size, QDataStream& body);

private:
  const QString getFormula(Q_UINT16 row, Q_UINT16 column, QDataStream& rgce);
  const QDomElement getFormat(Q_UINT16 xf);
  void getFont(Q_UINT16 xf, QDomElement &f, Q_UINT16 fontid);
  void getPen(Q_UINT16 xf, QDomElement &f, Q_UINT16 fontid);
  void getDate(int date, int& year, int& month, int& day);

  struct format_rec {
    Q_UINT8 cch;
    char* rgch;
  };

  struct font_rec {
    Q_UINT16 dyHeight,
      grbit,
      icv,
      bls,
      sss;
    Q_UINT8 uls,
      bFamily,
      bCharSet,
      reserved,
      cch;
    QString rgch;
  };

  struct xf_rec {
    Q_UINT16 ifnt,
      ifmt,
      attr,
      align,
      indent,
      borderStyle,
      sideBColor;
    Q_UINT32 topBColor;
    Q_UINT16 cellColor;
  };

  Q_UINT16 biff, date1904;

  QDomDocument *root;

  QIntDict<xf_rec> xfs;
  QIntDict<QString> sst;
  QIntDict<font_rec> fonts;
  QIntDict<format_rec> formats;

  QQueue<QDomElement> tables;

  KLocale m_locale;

  QDomElement doc, paper, map, borders, *table;

  // To avoid static variables:
  int fontCount, footerCount, headerCount, xfCount;
};

#endif
