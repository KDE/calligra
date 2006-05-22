/*
 *  Copyright (c) 2006 Cyrille Berger <cberger@cberger.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KIS_PDF_IMPORT_WIDGET_H
#define KIS_PDF_IMPORT_WIDGET_H

#include <pdfimportwidgetbase.h>

namespace Poppler {
class Document;
}

class KisPDFImportWidget : public PDFImportWidgetBase
{
    Q_OBJECT
    public:
        KisPDFImportWidget(Poppler::Document* pdfDoc, QWidget * parent, const char * name = "");

        ~KisPDFImportWidget();
    public:
        inline QValueList<int>  pages() { return m_pages; }
    private slots:
        void selectAllPages(bool v);
        void selectFirstPage(bool v);
        void selectSelectionOfPages(bool v);
        void updateSelectionOfPages();
        void updateWidth();
        void updateHeight();
        void updateHRes();
        void updateHVer();
        void updateMaxCanvasSize();
    private:
        Poppler::Document* m_pdfDoc;
        QValueList<int> m_pages;
        double m_maxWidthInch, m_maxHeightInch;
};

#endif
