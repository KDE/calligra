/* This file is part of the KDE project

   Copyright (C) 2011 Thorsten Zachmann <zachmann@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB. If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef CSTCOMPAREVIEW_H
#define CSTCOMPAREVIEW_H

#include <QWidget>
#include <QStringList>
#include <QDir>

#ifdef HAS_POPPLER
#include <poppler-qt5.h>
#endif

class CompareView;
class QLabel;

class CSTCompareView : public QWidget
{
public:
    explicit CSTCompareView(QWidget *parent = 0);
    ~CSTCompareView() override;

    bool open(const QString &inDir1, const QString &inDir2, const QString &pdfDir, const QString &resultFile);

protected:
    void keyPressEvent(QKeyEvent * event) override;

private:
    int updateResult(int index);
    void updateImage(int index);

#ifdef HAS_POPPLER
    bool m_showPdf;
    int m_pdfDelta;
    Poppler::Document *m_pdfDocument;
#endif

    QStringList m_result;
    int m_currentIndex;
    QStringList m_data;
    int m_dataIndex;

    QLabel *m_current;
    QLabel *m_currentPage;
    QLabel *m_changesAndDocument;
    CompareView *m_compareView;

    QDir m_dir1;
    QDir m_dir2;
    QDir m_pdfDir;
};

#endif /* CSTCOMPAREVIEW_H */
