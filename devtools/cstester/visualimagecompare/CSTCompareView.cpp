/* This file is part of the KDE project

   Copyright (C) 2011 Thorsten Zachmann <zachmann@kde.org>
   Copyright (C) 2013 Mohammed Nafees   <nafees.technocool@gmail.com>

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

#include "CSTCompareView.h"

#include "CompareView.h"

#include <QLabel>
#include <QDebug>
#include <QGridLayout>
#include <QKeyEvent>

#include <algorithm>

CSTCompareView::CSTCompareView(QWidget *parent)
: QWidget(parent)
#ifdef HAS_POPPLER
, m_showPdf(false)
, m_pdfDelta(0)
, m_pdfDocument(0)
#endif
, m_currentIndex(0)
{
    QGridLayout *layout = new QGridLayout(this);
    m_current = new QLabel(this);
    m_current->setWordWrap(true);
    m_current->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Minimum);
    layout->addWidget(m_current, 0, 0, 1, 2);
    m_currentPage = new QLabel(this);
    layout->addWidget(m_currentPage, 1, 0);
    m_changesAndDocument = new QLabel(this);
    layout->addWidget(m_changesAndDocument, 1, 1, Qt::AlignRight);
    m_compareView = new CompareView(this);
    layout->addWidget(m_compareView, 2, 0, 1, 2);
    setLayout(layout);
}

CSTCompareView::~CSTCompareView()
{
}

bool CSTCompareView::open(const QString &inDir1, const QString &inDir2, const QString &pdfDir, const QString &resultFile)
{
    m_dir1 = QDir(inDir1);
    m_dir2 = QDir(inDir2);
    m_pdfDir = QDir(pdfDir);

    if (!m_dir1.exists()) {
        qWarning() << "dir" << inDir1 << "does not exist!";
        return false;
    }

    if (!m_dir2.exists()) {
        qWarning() << "dir" << inDir1 << "does not exist!";
        return false;
    }

    if (!m_pdfDir.exists()) {
        qWarning() << "pdf directory" << pdfDir << "does not exist, no pdf for you my friend";
    }

    QFile file(resultFile);
    if (!file.open(QFile::ReadOnly)) {
        qWarning() << "Open result file" << resultFile << "failed!";
        return false;
    }

    qint64 lineLength = -1;
    do
    {
        char buf[10000];
        lineLength = file.readLine(buf, sizeof(buf));
        if (lineLength != -1) {
            QString line = QString::fromUtf8(buf).trimmed();
            if (!line.isEmpty())
                m_result.append(line);
        }
    } while (lineLength != -1);

    file.close();

    if (m_result.isEmpty()) {
        qWarning() << "No results found";
    }

    m_dataIndex = updateResult(0);
    if (m_dataIndex > 0) {
        updateImage(m_dataIndex);
    }
    return true;
}

void CSTCompareView::keyPressEvent(QKeyEvent * event)
{
    switch (event->key()) {
    case Qt::Key_PageUp:
    case Qt::Key_B:
        if (m_dataIndex > 1) {
            --m_dataIndex;
        }
        break;
    case Qt::Key_PageDown:
    case Qt::Key_N:
        if (m_dataIndex < m_data.size() - 1) {
            ++m_dataIndex;
        }
        break;
    case Qt::Key_Up:
        if (m_currentIndex > 0) {
            --m_currentIndex;
            m_dataIndex = updateResult(m_currentIndex);
#ifdef HAS_POPPLER
            m_pdfDelta = 0;
            if (m_pdfDocument) {
                delete(m_pdfDocument);
                m_pdfDocument = 0;
            }
#endif
        }
        break;
    case Qt::Key_Down:
        if (m_currentIndex < m_result.size() - 1) {
            ++m_currentIndex;
            m_dataIndex = updateResult(m_currentIndex);
#ifdef HAS_POPPLER
            m_pdfDelta = 0;
            if (m_pdfDocument) {
                delete(m_pdfDocument);
                m_pdfDocument = 0;
            }
#endif
        }
        break;
#ifdef HAS_POPPLER
    case Qt::Key_P:
        if (m_pdfDir.exists())
            m_showPdf = !m_showPdf;
        break;
    case Qt::Key_Plus:
        m_pdfDelta++;
        break;
    case Qt::Key_Minus:
        m_pdfDelta--;
        break;
#endif
    default:
        event->setAccepted(false);
        break;
    }
    updateImage(m_dataIndex);
}

int CSTCompareView::updateResult(int index)
{
    if (index < 0 || index >= m_result.count())
        return 0;

    QString result(m_result[index]);

    QStringList list = result.split(' ');
    QStringList filename;
    QList<int> pageNumbers;
    for(int i = list.count() - 1 ; i >= 0; --i) {
        bool ok;
        int n = list[i].toInt(&ok);
        if (i >= 1 && ok) {
            if (!pageNumbers.contains(n))
                pageNumbers.append(n);
        } else {
            for(int j = 0; j <= i; ++j)
                filename.append(list[j]);
            break;
        }
    }

    m_data.clear();
    m_data.append(filename.join(" "));
    std::sort(pageNumbers.begin(), pageNumbers.end());
    foreach(int n, pageNumbers)
        m_data.append(QString::number(n));

    if (m_data.size()) {
        m_current->setText(m_data[0]);
    }

    return m_data.size() > 1 ? 1 : 0;
}

void CSTCompareView::updateImage(int index)
{
    if (m_data.size() <= 1) {
        return;
    }

    QString currentPageText = QString("Page: %1\t%2/%3").arg(m_data[index]).arg(index).arg(m_data.size() - 1);

    QString subpath = m_data[0] + QString(".check/thumb_%2.png").arg(m_data[index]);

    QString filename1 = QString("%1/%2").arg(m_dir1.path()).arg(subpath);
    QString filename2 = QString("%1/%2").arg(m_dir2.path()).arg(subpath);

    qDebug() << "Subpath" << subpath << filename1 << filename2 << index;

    QImage image1;
    QImage image2;

    if (!image1.load(filename1)) {
        qWarning() << "loading image" << filename1 << "failed!";
    }

    if (!image2.load(filename2)) {
        qWarning() << "loading image" << filename2 << "failed!";
    }

    QImage pdfView;
#ifdef HAS_POPPLER
    if (m_showPdf) {
        if (!m_pdfDocument) {
            if (m_pdfDir.exists() && m_pdfDir.exists(m_data[0] + ".pdf")) {
                m_pdfDocument = Poppler::Document::load(m_pdfDir.filePath(m_data[0] + ".pdf"));
                if (m_pdfDocument) {
                    m_pdfDocument->setRenderHint(Poppler::Document::Antialiasing);
                    m_pdfDocument->setRenderHint(Poppler::Document::TextAntialiasing);
                }
            }
        }

        if (m_pdfDocument) {
            Poppler::Page *page = m_pdfDocument->page(m_data[index].toInt() - 1 + m_pdfDelta);
            if (page) {
                QSizeF pageSize = page->pageSizeF();
                qreal scale = qMin(72.0 * (image1.width() / pageSize.width()), 72.0 * (image1.height() / pageSize.height()));
                pdfView = page->renderToImage(scale, scale);
            } else {
                currentPageText += " - unable to load page from PDF";
            }
            if (m_pdfDelta) {
                currentPageText += QString(" - PDF delta : %1").arg(m_pdfDelta);
            }
        } else {
            currentPageText += " - unable to load PDF";
        }
    }
#endif

    m_changesAndDocument->setText(QString("Document: %1/%2").arg(m_currentIndex+1).arg(m_result.size()));

    m_currentPage->setText(currentPageText);

    m_compareView->update(image1, image2, m_dir1.dirName(), m_dir2.dirName(), pdfView);
}
