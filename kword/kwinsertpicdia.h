/* This file is part of the KDE project
   Copyright (C)  2001 David Faure <faure@kde.org>

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

#ifndef kwinsertpicdia_h
#define kwinsertpicdia_h

#include <kdialogbase.h>
class QCheckBox;
class KWInsertPicPreview;
class KFileDialog;

class KWInsertPicDia : public KDialogBase
{
    Q_OBJECT
public:
    KWInsertPicDia( QWidget *parent, const char *name = 0 );

    // IPD = InsertPicDia :)
    enum { IPD_IMAGE = 0, IPD_CLIPART };

    QString filename() const { return m_filename; }
    int type() const { return m_type; }
    bool makeInline() const;

    // For pixmaps only
    QSize pixmapSize() const;

protected slots:
    void slotChooseImage();
    void slotChooseClipart();

protected:
    QString selectPicture( KFileDialog & fd );

private:
    int m_type;
    QString m_filename;
    QCheckBox *m_cbInline;
    KWInsertPicPreview *m_preview;
};

#endif
