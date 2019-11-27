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

#ifndef COMPAREVIEW_H
#define COMPAREVIEW_H

#include <QWidget>
#include <QImage>
#include <QGridLayout>

class QLabel;
class QTabBar;
class QStackedWidget;

class CompareView : public QWidget
{
    Q_OBJECT

public:
    explicit CompareView(QWidget *parent = 0);
    CompareView(const QImage &image1, const QImage &image2, const QString &name1, const QString &name2, QWidget *parent = 0);
    ~CompareView() override;

    void update(const QImage &image1, const QImage &image2, const QString &name1, const QString &name2, const QImage &forcedDeltaView);

    static QImage difference(const QImage &image1, const QImage &image2);

protected:
    void keyPressEvent(QKeyEvent * event) override;

    void resizeEvent(QResizeEvent *event) override;

protected Q_SLOTS:
    void currentChanged(int currentIndex);

private:
    void init();
    void setLabelText();

    QImage m_image1;
    QImage m_image2;
    QImage m_diff;

    // UI
    QGridLayout *m_layout;
    QTabBar *m_tabBar;
    QStackedWidget *m_stack;
    QLabel *m_image1Label;
    QLabel *m_image2Label;
    QLabel *m_diffLabel;
};

#endif /* COMPAREVIEW_H */

