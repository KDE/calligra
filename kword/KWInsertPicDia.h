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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef kwinsertpicdia_h
#define kwinsertpicdia_h

#include <kdialog.h>
class QCheckBox;
class KWInsertPicPreview;
class KFileDialog;
class KWDocument;

class KWInsertPicDia : public KDialog
{
    Q_OBJECT
public:
    KWInsertPicDia( QWidget *parent, bool _inline, bool _keepRatio, KWDocument *_doc, const char *name = 0 );

    bool makeInline() const;

    bool keepRatio() const;

    /**
     * @param _path the name of the old file
     * @param parent the parent widget
     * @return the loaded picture (Picture is null if cancelled.)
     */
    static KoPicture selectPictureDia( const QString & _path, QWidget* parent );

    KoPicture picture ( void ) const;

protected slots:
    void slotChooseImage();

protected:
    static KoPicture selectPicture( KFileDialog & fd, QWidget *parent );

private:

    bool m_bFirst;
    QCheckBox *m_cbInline, *m_cbKeepRatio;
    KWInsertPicPreview *m_preview;
    KWDocument *m_doc;
    KoPicture m_picture;
};

#endif
