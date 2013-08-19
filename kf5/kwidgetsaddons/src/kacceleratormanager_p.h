/*  This file is part of the KDE project
    Copyright (C) 2002 Matthias H�zer-Klpfel <mhk@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/


#ifndef KACCELERATORMANAGER_PRIVATE_H
#define KACCELERATORMANAGER_PRIVATE_H


#include <QtCore/QString>
#include <QtCore/QObject>

#include <kwidgetsaddons_export.h>

class QStackedWidget;
class QMenuBar;
class QTabBar;
class QDockWidget;


/**
 * A string class handling accelerators.
 *
 * This class contains a string and knowledge about accelerators.
 * It keeps a list weights, telling how valuable each character
 * would be as an accelerator.
 *
 * @author Matthias H�zer-Klpfel <mhk@kde.org>
*/

class KAccelString
{
public:

  KAccelString() : m_pureText(), m_accel(-1) {}
  explicit KAccelString(const QString &input, int initalWeight=-1);

  void calculateWeights(int initialWeight);

  const QString &pure() const { return m_pureText; }
  QString accelerated() const;

  int accel() const { return m_accel; }
  void setAccel(int accel) { m_accel = accel; }

  int originalAccel() const { return m_orig_accel; }
  QString originalText() const { return m_origText; }

  QChar accelerator() const;

  int maxWeight(int &index, const QString &used) const;

  bool operator == (const KAccelString &c) const { return m_pureText == c.m_pureText && m_accel == c.m_accel && m_orig_accel == c.m_orig_accel; }


private:

  int stripAccelerator(QString &input);

  void dump();

  QString        m_pureText,  m_origText;
  int            m_accel, m_orig_accel;
  QVector<int> m_weight;

};


typedef QList<KAccelString> KAccelStringList;


/**
 * This class encapsulates the algorithm finding the 'best'
 * distribution of accelerators in a hierarchy of widgets.
 *
 * @author Matthias H�zer-Klpfel <mhk@kde.org>
*/

class KAccelManagerAlgorithm
{
public:

  enum {
    // Default control weight
    DEFAULT_WEIGHT = 50,
    // Additional weight for first character in string
    FIRST_CHARACTER_EXTRA_WEIGHT = 50,
    // Additional weight for the beginning of a word
    WORD_BEGINNING_EXTRA_WEIGHT = 50,
    // Additional weight for the dialog buttons (large, we basically never want these reassigned)
    DIALOG_BUTTON_EXTRA_WEIGHT = 300,
    // Additional weight for a 'wanted' accelerator
    WANTED_ACCEL_EXTRA_WEIGHT = 150,
    // Default weight for an 'action' widget (ie, pushbuttons)
    ACTION_ELEMENT_WEIGHT = 50,
    // Default weight for group boxes (lowest priority)
    GROUP_BOX_WEIGHT = -2000,
    // Default weight for checkable group boxes (low priority)
    CHECKABLE_GROUP_BOX_WEIGHT = 20,
    // Default weight for menu titles
    MENU_TITLE_WEIGHT = 250,
    // Additional weight for KDE standard accelerators
    STANDARD_ACCEL = 300
  };

  static void findAccelerators(KAccelStringList &result, QString &used);

};


/**
 * This class manages a popup menu. It will notice if entries have been
 * added or changed, and will recalculate the accelerators accordingly.
 *
 * This is necessary for dynamic menus like for example in kicker.
 *
 * @author Matthias H�zer-Klpfel <mhk@kde.org>
*/

class KPopupAccelManager : public QObject
{
  Q_OBJECT

public:

  static void manage(QMenu *popup);


protected:

  KPopupAccelManager(QMenu *popup);


private Q_SLOTS:

  void aboutToShow();


private:

  void calculateAccelerators();

  void findMenuEntries(KAccelStringList &list);
  void setMenuEntries(const KAccelStringList &list);

  QMenu       *m_popup;
  KAccelStringList m_entries;
  int              m_count;

};


class QWidgetStackAccelManager : public QObject
{
  Q_OBJECT

public:

  static void manage(QStackedWidget *popup);


protected:

  QWidgetStackAccelManager(QStackedWidget *popup);


private Q_SLOTS:

    void currentChanged(int child);
    bool eventFilter ( QObject * watched, QEvent * e );

private:

  void calculateAccelerators();

  QStackedWidget     *m_stack;
  KAccelStringList m_entries;

};

/*********************************************************************

 class KAcceleratorManagerPrivate - internal helper class

 This class does all the work to find accelerators for a hierarchy of
 widgets.

 *********************************************************************/


class KAcceleratorManagerPrivate
{
public:

    static void manage(QWidget *widget);
    static bool programmers_mode;
    // We export this function because KStandardAction uses it (sets list of standard action names).
    // KStandardAction calls this function only once when the first standard action is created.
    static void KWIDGETSADDONS_EXPORT setStandardActionNames(const QStringList &strList);
    static bool standardName(const QString &str);

    static bool checkChange(const KAccelString &as)  {
        QString t2 = as.accelerated();
        QString t1 = as.originalText();
        if (t1 != t2)
        {
            if (as.accel() == -1)  {
                removed_string  += QLatin1String("<tr><td>") + t1.toHtmlEscaped() + QLatin1String("</td></tr>");
            } else if (as.originalAccel() == -1) {
                added_string += QLatin1String("<tr><td>") + t2.toHtmlEscaped() + QLatin1String("</td></tr>");
            } else {
                changed_string += QLatin1String("<tr><td>") + t1.toHtmlEscaped() + QLatin1String("</td>");
                changed_string += QLatin1String("<td>") + t2.toHtmlEscaped() + QLatin1String("</td></tr>");
            }
            return true;
        }
        return false;
    }
    static QString changed_string;
    static QString added_string;
    static QString removed_string;
    static QMap<QWidget *, int> ignored_widgets;
    static QStringList standardNames;

private:
  class Item;
public:
  typedef QList<Item *> ItemList;

private:
  static void traverseChildren(QWidget *widget, Item *item);

  static void manageWidget(QWidget *widget, Item *item);
  static void manageMenuBar(QMenuBar *mbar, Item *item);
  static void manageTabBar(QTabBar *bar, Item *item);
  static void manageDockWidget(QDockWidget *dock, Item *item);

  static void calculateAccelerators(Item *item, QString &used);

  class Item
  {
  public:

    Item() : m_widget(0), m_children(0), m_index(-1) {}
    ~Item();

    void addChild(Item *item);

    QWidget       *m_widget;
    KAccelString  m_content;
    ItemList      *m_children;
    int           m_index;

  };
};


#endif // KACCELERATORMANAGER_PRIVATE_H
