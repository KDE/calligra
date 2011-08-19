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

#include "CSTCompareView.h"

#include "CompareView.h"

#include <QLabel>
#include <QDebug>
#include <QGridLayout>
#include <QKeyEvent>

CSTCompareView::CSTCompareView(QWidget *parent)
: QWidget(parent)
, m_currentIndex(0)
{
    QGridLayout *layout = new QGridLayout(this);
    m_current = new QLabel(this);
    layout->addWidget(m_current, 0, 0);
    m_currentPage = new QLabel(this);
    layout->addWidget(m_currentPage, 1, 0);
    m_compareView = new CompareView(this);
    layout->addWidget(m_compareView, 2, 0);
    setLayout(layout);
}

CSTCompareView::~CSTCompareView()
{
}

bool CSTCompareView::open(const QString &inDir1, const QString &inDir2, const QString &resultFile)
{
    m_dir1 = QDir(inDir1);
    m_dir2 = QDir(inDir2);

    if (!m_dir1.exists()) {
        qWarning() << "dir" << inDir1 << "does not exist!";
        return false;
    }

    if (!m_dir2.exists()) {
        qWarning() << "dir" << inDir1 << "does not exist!";
        return false;
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
    case Qt::Key_B:
        if (m_dataIndex > 1) {
            --m_dataIndex;
        }
        break;
    case Qt::Key_N:
        if (m_dataIndex < m_data.size() - 1) {
            ++m_dataIndex;
        }
        break;
    case Qt::Key_Up:
        if (m_currentIndex > 0) {
            --m_currentIndex;
            m_dataIndex = updateResult(m_currentIndex);
        }
        break;
    case Qt::Key_Down:
        if (m_currentIndex < m_result.size() - 1) {
            ++m_currentIndex;
            m_dataIndex = updateResult(m_currentIndex);
        }
        break;
    default:
        event->setAccepted(false);
        break;
    }
    updateImage(m_dataIndex);
}

int CSTCompareView::updateResult(int index)
{
    QString result(m_result[index]);

    m_data = result.split(' ');
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

    m_currentPage->setText(QString("Page: %1\t%2/%3").arg(m_data[index]).arg(index).arg(m_data.size() - 1));

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

    m_compareView->update(image1, image2, m_dir1.dirName(), m_dir2.dirName());
}
