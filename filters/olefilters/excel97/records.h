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

#ifndef RECORDS_H
#define RECORDS_H

struct record {
  Q_UINT16 opcode;
  bool (XMLTree::*func)(Q_UINT16 size, QDataStream& body);
};

const record biff[] = {
  { 0x0022, &XMLTree::_1904 },
  { 0x0221, &XMLTree::_array },
  { 0x0040, &XMLTree::_backup },
  { 0x0201, &XMLTree::_blank },
  { 0x0809, &XMLTree::_bof },
  { 0x00da, &XMLTree::_bookbool },
  { 0x0205, &XMLTree::_boolerr },
  { 0x0029, &XMLTree::_bottommargin },
  { 0x0085, &XMLTree::_boundsheet },
  { 0x01b1, &XMLTree::_cf },
  { 0x01b0, &XMLTree::_condfmt },
  { 0X0042, &XMLTree::_codepage }, 
  { 0x007d, &XMLTree::_colinfo },
  { 0x003c, &XMLTree::_cont },
  { 0x008c, &XMLTree::_country },
  { 0x005a, &XMLTree::_crn },
  { 0x00d7, &XMLTree::_dbcell },
  { 0x0225, &XMLTree::_defaultrowheight },
  { 0x0055, &XMLTree::_defcolwidth },
  { 0x0200, &XMLTree::_dimensions },
  { 0x000a, &XMLTree::_eof },
  { 0x0016, &XMLTree::_externcount },
  { 0x0223, &XMLTree::_externname },
  { 0x0017, &XMLTree::_externsheet },
  { 0x00ff, &XMLTree::_extsst },
  { 0x002f, &XMLTree::_filepass },
  { 0x005b, &XMLTree::_filesharing },
  { 0x01a5, &XMLTree::_filesharing2 },
  { 0x0031, &XMLTree::_font },
  { 0x0015, &XMLTree::_footer },
  { 0x041e, &XMLTree::_format },
  { 0x0006, &XMLTree::_formula },
  { 0x00ab, &XMLTree::_gcw },
  { 0x0080, &XMLTree::_guts },
  { 0x0083, &XMLTree::_hcenter },
  { 0x0014, &XMLTree::_header },
  { 0x01b8, &XMLTree::_hlink },
  { 0x001b, &XMLTree::_horizontalpagebreaks },
  { 0x007f, &XMLTree::_imdata },
  { 0x00fd, &XMLTree::_labelsst },
  { 0x0026, &XMLTree::_leftmargin },
  { 0x00be, &XMLTree::_mulblank },
  { 0x00bd, &XMLTree::_mulrk },
  { 0x0218, &XMLTree::_name },
  { 0x001c, &XMLTree::_note },
  { 0x0203, &XMLTree::_number },
  { 0x0041, &XMLTree::_pane },
  { 0x00dc, &XMLTree::_paramqry },
  { 0x0013, &XMLTree::_password },
  { 0x0012, &XMLTree::_protect },
  { 0x01ad, &XMLTree::_qsi },
  { 0x00b9, &XMLTree::_recipname },
  { 0x0027, &XMLTree::_rightmargin },
  { 0x027e, &XMLTree::_rk },
  { 0x0208, &XMLTree::_row },
  { 0x00a0, &XMLTree::_scl },
  { 0x00a1, &XMLTree::_setup },
  { 0x00bc, &XMLTree::_shrfmla },
  { 0x0090, &XMLTree::_sort },
  { 0x00fc, &XMLTree::_sst },
  { 0x0099, &XMLTree::_standardwidth },
  { 0x0207, &XMLTree::_string },
  { 0x013d, &XMLTree::_tabid },
  { 0x00ea, &XMLTree::_tabidconf },
  { 0x0236, &XMLTree::_table },
  { 0x0028, &XMLTree::_topmargin },
  { 0x0084, &XMLTree::_vcenter },
  { 0x001a, &XMLTree::_verticalpagebreaks },
  { 0x003d, &XMLTree::_window1 },
  { 0x023e, &XMLTree::_window2 },
  { 0x005C, &XMLTree::_writeaccess },
  { 0x0086, &XMLTree::_writeprot },
  { 0x0081, &XMLTree::_wsbool },
  { 0x00e0, &XMLTree::_xf },
  { 0x0000, NULL }
};

#endif
