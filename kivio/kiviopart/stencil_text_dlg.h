/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#ifndef STENCIL_TEXT_DLG_H
#define STENCIL_TEXT_DLG_H

#include <kdialogbase.h>
#include <koffice_export.h>
class QTextEdit;

class KIVIO_EXPORT KivioStencilTextDlg : public KDialogBase
{
  Q_OBJECT
  protected:
    QTextEdit *m_text;

  public:
    KivioStencilTextDlg( QWidget *, const QString & );
    virtual ~KivioStencilTextDlg();

    QString text();
};

#endif

