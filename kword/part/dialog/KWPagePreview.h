/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2006 Gary Cramblitt <garycramblitt@comcast.net>

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

#ifndef KWPAGEPREVIEW_H
#define KWPAGEPREVIEW_H

// Qt includes.
#include <QGroupBox>

// KOffice includes.
#include <KoPageLayout.h>

class KWPagePreview : public QGroupBox
{
    Q_OBJECT

public:

    /**
     *  Constructor.
     *  @param parent The parent widget.
     *  @param layout Layout to draw.
     */
    KWPagePreview( QWidget* parent, const KoPageLayout& layout );

    /**
     *  Set page layout.
     *  @param layout Layout to draw.
     */
    void setPageLayout( const KoPageLayout& layout );
    /**
     *  Set page columns.
     *  @param columns Number of columns.
     */
    void setPageColumns( const KoColumns& columns );

protected:

    // paint page
    virtual void paintEvent ( QPaintEvent * event );
    virtual void resizeEvent ( QResizeEvent * event );
    void updateZoomedSize();

    double m_pageHeight;
    double m_pageWidth;
    double m_textFrameX;
    double m_textFrameY;
    double m_textFrameWidth;
    double m_textFrameHeight;
    int m_columns;
    KoPageLayout m_layout;
};

#endif      // KWPAGEPREVIEW_H
