/* This file is part of the KDE project

   Copyright (C) 2011 Thorsten Zachmann <zachmann@kde.org>
   Copyright (C) 2012 Mohammed Nafees <nafees.technocool@gmail.com>

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

#include "CompareView.h"

#include <QDebug>
#include <QKeyEvent>
#include <QLabel>
#include <QTabBar>
#include <QStackedWidget>


CompareView::CompareView(QWidget *parent)
: QWidget(parent)
{
    setMinimumSize(800, 600);
    init();
}

CompareView::CompareView(const QImage &image1, const QImage &image2, const QString &name1, const QString &name2, QWidget *parent)
: QWidget(parent)
, m_image1(image1)
, m_image2(image2)
{
    init();
    update(image1, image2, name1, name2, QImage());
}

CompareView::~CompareView()
{
}

void CompareView::init()
{
    m_layout = new QGridLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_tabBar = new QTabBar();
    m_tabBar->addTab("Image 1");
    m_tabBar->addTab("Image 2");
    m_layout->addWidget(m_tabBar, 0, 0, 1, 2);
    connect(m_tabBar, SIGNAL(currentChanged(int)), this, SLOT(currentChanged(int)));
    m_image1Label = new QLabel();
    m_image2Label = new QLabel();
    m_stack = new QStackedWidget();
    m_stack->addWidget(m_image1Label);
    m_stack->addWidget(m_image2Label);
    m_layout->addWidget(m_stack, 1, 0);

    m_diffLabel = new QLabel(this);
    m_layout->addWidget(m_diffLabel, 1, 1, Qt::AlignCenter);

    setLayout(m_layout);
}

void CompareView::update(const QImage &image1, const QImage &image2, const QString &name1, const QString &name2, const QImage &forcedDeltaView)
{
    m_image1 = image1;
    m_image2 = image2;

    if (forcedDeltaView.isNull()) {
        m_diff = difference(image1, image2);
    } else {
        m_diff = forcedDeltaView;
    }
    m_tabBar->setTabText(0, name1);
    m_tabBar->setTabText(1, name2);
    setLabelText();
    m_diffLabel->setAlignment(Qt::AlignCenter);
}

void CompareView::setLabelText()
{
    QSize stackSize(m_stack->size());
    QSize diffSize(m_diffLabel->size());
    // the maximum size of the images can be the width of the widget - 4 / 2 and the height of the stack 
    int minLength = qMin((size().width() - 4)/ 2, stackSize.height());
    QSize newSize(minLength, minLength);
    m_layout->setColumnMinimumWidth(0, minLength);
    m_layout->setColumnMinimumWidth(1, minLength);
    m_image1Label->setPixmap(QPixmap::fromImage(m_image1.scaled(newSize, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
    m_image2Label->setPixmap(QPixmap::fromImage(m_image2.scaled(newSize, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
    m_diffLabel->setPixmap(QPixmap::fromImage(m_diff.scaled(newSize, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
}

int lighten(int value)
{
    return 255 - (255 - value) / 2;
}

QImage CompareView::difference(const QImage &image1, const QImage &image2)
{
    QSize size(image1.size());
    if (size != image2.size()) {
        return QImage();
    }

    quint32 width = size.width();
    quint32 height = size.height();
    QImage result(width, height, QImage::Format_ARGB32);
    for (quint32 y = 0; y < height; ++y) {
        QRgb *scanLineRes = (QRgb *)result.scanLine(y);
        QRgb *scanLineIn1 = (QRgb *)image1.scanLine(y);
        QRgb *scanLineIn2 = (QRgb *)image2.scanLine(y);
        for (quint32 x = 0; x < width; ++x) {
            if (scanLineIn1[x] == scanLineIn2[x]) {
                scanLineRes[x] = qRgb(lighten(qRed(scanLineIn1[x])), lighten(qGreen(scanLineIn1[x])), lighten(qBlue(scanLineIn1[x])));
            }
            else {
                scanLineRes[x] = qRgb(255, 0, 0);
            }
        }
    }
    return result;
}

void CompareView::keyPressEvent(QKeyEvent * event)
{
    switch (event->key()) {
    case Qt::Key_Space:
        m_tabBar->setCurrentIndex((m_tabBar->currentIndex() + 1) % 2);
        m_stack->setCurrentIndex(m_tabBar->currentIndex());
        break;
    default:
        event->setAccepted(false);
        break;
    }
}

void CompareView::resizeEvent(QResizeEvent *event)
{
    setLabelText();
    QWidget::resizeEvent(event);
}

void CompareView::currentChanged(int currentIndex)
{
    m_tabBar->setCurrentIndex(currentIndex);
    m_stack->setCurrentIndex(currentIndex);
}

