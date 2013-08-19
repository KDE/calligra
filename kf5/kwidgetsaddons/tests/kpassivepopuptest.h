#ifndef KPASSIVEPOPUPTEST_H
#define KPASSIVEPOPUPTEST_H

#include <QtCore/QObject>
#include <QSystemTrayIcon>

class Test : public QObject
{
  Q_OBJECT

public:
  Test() : QObject() {}
  ~Test() {}

public Q_SLOTS:
  void showIt();
  void showIt2();
  void showIt3();
  void showIt4();
  void showIt5();
  void showIt6(QSystemTrayIcon::ActivationReason);
};


#endif // KPASSIVEPOPUPTEST_H


