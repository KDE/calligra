/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2011 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef COMPAREVIEW_H
#define COMPAREVIEW_H

#include <QGridLayout>
#include <QImage>
#include <QWidget>

class QLabel;
class QTabBar;
class QStackedWidget;

class CompareView : public QWidget
{
    Q_OBJECT

public:
    explicit CompareView(QWidget *parent = nullptr);
    CompareView(const QImage &image1, const QImage &image2, const QString &name1, const QString &name2, QWidget *parent = nullptr);
    ~CompareView() override;

    void update(const QImage &image1, const QImage &image2, const QString &name1, const QString &name2, const QImage &forcedDeltaView);

    static QImage difference(const QImage &image1, const QImage &image2);

protected:
    void keyPressEvent(QKeyEvent *event) override;

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
