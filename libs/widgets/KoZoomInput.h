/* SPDX-FileCopyrightText: 2008 Peter Simonsson <peter.simonsson@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOZOOMINPUT_H
#define KOZOOMINPUT_H

#include <QStackedWidget>

class KoZoomInput : public QStackedWidget
{
    Q_OBJECT
public:
    explicit KoZoomInput(QWidget *parent = nullptr);
    ~KoZoomInput() override;

    void setZoomLevels(const QStringList &levels);
    void setCurrentZoomLevel(const QString &level);

    bool eventFilter(QObject *watched, QEvent *event) override;

Q_SIGNALS:
    void zoomLevelChanged(const QString &level);

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    class Private;
    Private *const d;
};

#endif // KOZOOMINPUT_H
