/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2011 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CSTCOMPAREVIEW_H
#define CSTCOMPAREVIEW_H

#include <QDir>
#include <QStringList>
#include <QWidget>

#ifdef HAS_POPPLER
#include <poppler-qt6.h>
#endif

class CompareView;
class QLabel;

class CSTCompareView : public QWidget
{
public:
    explicit CSTCompareView(QWidget *parent = nullptr);
    ~CSTCompareView() override;

    bool open(const QString &inDir1, const QString &inDir2, const QString &pdfDir, const QString &resultFile);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    int updateResult(int index);
    void updateImage(int index);

#ifdef HAS_POPPLER
    bool m_showPdf;
    int m_pdfDelta;
    std::unique_ptr<Poppler::Document> m_pdfDocument;
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
