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

class XMLTree:public QObject
{
public:
  XMLTree();
  ~XMLTree();

  const QDomDocument * const part();

  bool invokeHandler(Q_UINT16 opcode, Q_UINT32 bytes, QDataStream &operands);

private:
  // Debug support.

  static const int s_area = 30511;

  static const int BIFF_5_7 = 0x0500;
  static const int BIFF_8 = 0x0600;

  const QString getFormula(Q_UINT16 row, Q_UINT16 column, QDataStream& rgce);
  const QDomElement getFormat(Q_UINT16 xf);
  void getFont(Q_UINT16 xf, QDomElement &f, Q_UINT16 fontid);
  void getPen(Q_UINT16 xf, QDomElement &f, Q_UINT16 fontid);
  void getDate(int date, int& year, int& month, int& day);
  void getTime( double time, int &hour,int  &min, int &second);

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
  unsigned m_streamDepth;
  
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

  bool _1904(Q_UINT32 size, QDataStream &body);
  bool _array(Q_UINT32 size, QDataStream &body);
  bool _backup(Q_UINT32 size, QDataStream &body);
  bool _blank(Q_UINT32 size, QDataStream &body);
  bool _bof(Q_UINT32 size, QDataStream &body);
  bool _bookbool(Q_UINT32 size, QDataStream &body);
  bool _boolerr(Q_UINT32 size, QDataStream &body);
  bool _bottommargin(Q_UINT32 size, QDataStream &body);
  bool _boundsheet(Q_UINT32 size, QDataStream &body);
  bool _cf(Q_UINT32 size, QDataStream &body);
  bool _condfmt(Q_UINT32 size, QDataStream &body);
  bool _codepage(Q_UINT32 size, QDataStream &body);
  bool _colinfo(Q_UINT32 size, QDataStream &body);
  bool _country(Q_UINT32 size, QDataStream &body);
  bool _crn(Q_UINT32 size, QDataStream &body);
  bool _dbcell(Q_UINT32 size, QDataStream &body);
  bool _defaultrowheight(Q_UINT32 size, QDataStream &body);
  bool _defcolwidth(Q_UINT32 size, QDataStream &body);
  bool _dimensions(Q_UINT32 size, QDataStream &body);
  bool _eof(Q_UINT32 size, QDataStream &body);
  bool _externcount(Q_UINT32 size, QDataStream &body);
  bool _externname(Q_UINT32 size, QDataStream &body);
  bool _externsheet(Q_UINT32 size, QDataStream &body);
  bool _extsst(Q_UINT32 size, QDataStream &body);
  bool _filepass(Q_UINT32 size, QDataStream &body);
  bool _filesharing(Q_UINT32 size, QDataStream &body);
  bool _filesharing2(Q_UINT32 size, QDataStream &body);
  bool _font(Q_UINT32 size, QDataStream &body);
  bool _footer(Q_UINT32 size, QDataStream &body);
  bool _format(Q_UINT32 size, QDataStream &body);
  bool _formula(Q_UINT32 size, QDataStream &body);
  bool _gcw(Q_UINT32 size, QDataStream &body);
  bool _guts(Q_UINT32 size, QDataStream &body);
  bool _hcenter(Q_UINT32 size, QDataStream &body);
  bool _header(Q_UINT32 size, QDataStream &body);
  bool _hlink(Q_UINT32 size, QDataStream &body);
  bool _horizontalpagebreaks(Q_UINT32 size, QDataStream &body);
  bool _imdata(Q_UINT32 size, QDataStream &body);
  bool _label(Q_UINT32 size, QDataStream &body);
  bool _labelsst(Q_UINT32 size, QDataStream &body);
  bool _leftmargin(Q_UINT32 size, QDataStream &body);
  bool _mergecell(Q_UINT32 size, QDataStream &body);
  bool _mulblank(Q_UINT32 size, QDataStream &body);
  bool _mulrk(Q_UINT32 size, QDataStream &body);
  bool _name(Q_UINT32 size, QDataStream &body);
  bool _note(Q_UINT32 size, QDataStream &body);
  bool _number(Q_UINT32 size, QDataStream &body);
  bool _pane(Q_UINT32 size, QDataStream &body);
  bool _paramqry(Q_UINT32 size, QDataStream &body);
  bool _password(Q_UINT32 size, QDataStream &body);
  bool _protect(Q_UINT32 size, QDataStream &body);
  bool _qsi(Q_UINT32 size, QDataStream &body);
  bool _recipname(Q_UINT32 size, QDataStream &body);
  bool _rightmargin(Q_UINT32 size, QDataStream &body);
  bool _rk(Q_UINT32 size, QDataStream &body);
  bool _row(Q_UINT32 size, QDataStream &body);
  bool _scl(Q_UINT32 size, QDataStream &body);
  bool _setup(Q_UINT32 size, QDataStream &body);
  bool _shrfmla(Q_UINT32 size, QDataStream &body);
  bool _sort(Q_UINT32 size, QDataStream &body);
  bool _sst(Q_UINT32 size, QDataStream &body);
  bool _standardwidth(Q_UINT32 size, QDataStream &body);
  bool _string(Q_UINT32 size, QDataStream &body);
  bool _tabid(Q_UINT32 size, QDataStream &body);
  bool _tabidconf(Q_UINT32 size, QDataStream &body);
  bool _table(Q_UINT32 size, QDataStream &body);
  bool _topmargin(Q_UINT32 size, QDataStream &body);
  bool _vcenter(Q_UINT32 size, QDataStream &body);
  bool _verticalpagebreaks(Q_UINT32 size, QDataStream &body);
  bool _window1(Q_UINT32 size, QDataStream &body);
  bool _window2(Q_UINT32 size, QDataStream &body);
  bool _writeaccess(Q_UINT32 size, QDataStream &body);
  bool _writeprot(Q_UINT32 size, QDataStream &body);
  bool _wsbool(Q_UINT32 size, QDataStream &body);
  bool _xf(Q_UINT32 size, QDataStream &body);
};

#endif
