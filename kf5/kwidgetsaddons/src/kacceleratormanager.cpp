/*  This file is part of the KDE project
    Copyright (C) 2002 Matthias Hölzer-Klüpfel <mhk@kde.org>

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

#include "kacceleratormanager.h"

#include <QApplication>
#include <QMainWindow>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMenuBar>
#include <QtCore/QMetaClassInfo>
#include <QtCore/QObject>
#include <QList>
#include <QPushButton>
#include <QRadioButton>
#include <QDoubleSpinBox>
#include <QTabBar>
#include <QTextEdit>
#include <QWidget>
#include <QStackedWidget>
#include <QDockWidget>
#include <QTextDocument>
#include <QDebug>

#include "kacceleratormanager_p.h"


/*********************************************************************

 class Item - helper class containing widget information

 This class stores information about the widgets the need accelerators,
 as well as about their relationship.

 *********************************************************************/


bool KAcceleratorManagerPrivate::programmers_mode = false;
QString KAcceleratorManagerPrivate::changed_string;
QString KAcceleratorManagerPrivate::added_string;
QString KAcceleratorManagerPrivate::removed_string;
QMap<QWidget*, int> KAcceleratorManagerPrivate::ignored_widgets;
QStringList KAcceleratorManagerPrivate::standardNames;

void KAcceleratorManagerPrivate::setStandardActionNames(const QStringList &list)
{
    standardNames = list;
}

bool KAcceleratorManagerPrivate::standardName(const QString &str)
{
    return standardNames.contains(str);
}

KAcceleratorManagerPrivate::Item::~Item()
{
    if (m_children)
        while (!m_children->isEmpty())
            delete m_children->takeFirst();

    delete m_children;
}


void KAcceleratorManagerPrivate::Item::addChild(Item *item)
{
    if (!m_children) {
        m_children = new ItemList;
    }

    m_children->append(item);
}

void KAcceleratorManagerPrivate::manage(QWidget *widget)
{
    if (!widget)
    {
        qDebug() << "null pointer given to manage";
        return;
    }

    if (KAcceleratorManagerPrivate::ignored_widgets.contains(widget)) {
        return;
    }

    if (qobject_cast<QMenu*>(widget))
    {
        // create a popup accel manager that can deal with dynamic menus
        KPopupAccelManager::manage(static_cast<QMenu*>(widget));
        return;
    }

    Item *root = new Item;

    manageWidget(widget, root);

    QString used;
    calculateAccelerators(root, used);
    delete root;
}


void KAcceleratorManagerPrivate::calculateAccelerators(Item *item, QString &used)
{
    if (!item->m_children)
        return;

    // collect the contents
    KAccelStringList contents;
    Q_FOREACH (Item *it, *item->m_children)
    {
        contents << it->m_content;
    }

    // find the right accelerators
    KAccelManagerAlgorithm::findAccelerators(contents, used);

    // write them back into the widgets
    int cnt = -1;
    Q_FOREACH(Item *it, *item->m_children)
    {
        cnt++;

        QDockWidget *dock = qobject_cast<QDockWidget*>(it->m_widget);
        if (dock)
        {
            if (checkChange(contents[cnt]))
                dock->setWindowTitle(contents[cnt].accelerated());
            continue;
        }
        QTabBar *tabBar = qobject_cast<QTabBar*>(it->m_widget);
        if (tabBar)
        {
            if (checkChange(contents[cnt]))
                tabBar->setTabText(it->m_index, contents[cnt].accelerated());
            continue;
        }
        QMenuBar *menuBar = qobject_cast<QMenuBar*>(it->m_widget);
        if (menuBar)
        {
            if (it->m_index >= 0)
            {
                QAction *maction = menuBar->actions()[it->m_index];
                if (maction)
                {
                    checkChange(contents[cnt]);
                    maction->setText(contents[cnt].accelerated());
                }
                continue;
            }
        }

        // we possibly reserved an accel, but we won't set it as it looks silly
        QGroupBox *groupBox = qobject_cast<QGroupBox*>(it->m_widget);
        if (groupBox && !groupBox->isCheckable())
            continue;

        int tprop = it->m_widget->metaObject()->indexOfProperty("text");
        if (tprop != -1)  {
            if (checkChange(contents[cnt]))
                it->m_widget->setProperty("text", contents[cnt].accelerated());
        } else {
            tprop = it->m_widget->metaObject()->indexOfProperty("title");
            if (tprop != -1 && checkChange(contents[cnt]))
                it->m_widget->setProperty("title", contents[cnt].accelerated());
        }
    }

    // calculate the accelerators for the children
    Q_FOREACH(Item *it, *item->m_children)
    {
        if (it->m_widget && it->m_widget->isVisibleTo( item->m_widget ) )
            calculateAccelerators(it, used);
    }
}


void KAcceleratorManagerPrivate::traverseChildren(QWidget *widget, Item *item)
{
  QList<QWidget*> childList = widget->findChildren<QWidget*>();
  Q_FOREACH ( QWidget *w , childList ) {
    // Ignore unless we have the direct parent
    if(qobject_cast<QWidget *>(w->parent()) != widget) continue;

    if ( !w->isVisibleTo( widget ) || (w->isTopLevel() && qobject_cast<QMenu*>(w) == NULL) )
        continue;

    if ( KAcceleratorManagerPrivate::ignored_widgets.contains( w ) )
        continue;

    manageWidget(w, item);
  }
}

void KAcceleratorManagerPrivate::manageWidget(QWidget *w, Item *item)
{
  // first treat the special cases

  QTabBar *tabBar = qobject_cast<QTabBar*>(w);
  if (tabBar)
  {
      manageTabBar(tabBar, item);
      return;
  }

  QStackedWidget *wds = qobject_cast<QStackedWidget*>( w );
  if ( wds )
  {
      QWidgetStackAccelManager::manage( wds );
      // return;
  }

  QDockWidget *dock = qobject_cast<QDockWidget*>( w );
  if ( dock )
  {
      //QWidgetStackAccelManager::manage( wds );
      manageDockWidget(dock, item);
  }


  QMenu *popupMenu = qobject_cast<QMenu*>(w);
  if (popupMenu)
  {
      // create a popup accel manager that can deal with dynamic menus
      KPopupAccelManager::manage(popupMenu);
      return;
  }

  QStackedWidget *wdst = qobject_cast<QStackedWidget*>( w );
  if ( wdst )
  {
      QWidgetStackAccelManager::manage( wdst );
      // return;
  }

  QMenuBar *menuBar = qobject_cast<QMenuBar*>(w);
  if (menuBar)
  {
      manageMenuBar(menuBar, item);
      return;
  }

  if (qobject_cast<QComboBox*>(w) || qobject_cast<QLineEdit*>(w) ||
      w->inherits("Q3TextEdit") ||
      qobject_cast<QTextEdit*>(w) ||
      qobject_cast<QAbstractSpinBox*>(w) || w->inherits( "KMultiTabBar" ) )
      return;

  if ( w->inherits("KUrlRequester") ) {
    traverseChildren(w, item);
    return;
  }

  // now treat 'ordinary' widgets
  QLabel *label =  qobject_cast<QLabel*>(w);
  if ( label  ) {
      if ( !label->buddy() )
          return;
      else {
          if ( label->textFormat() == Qt::RichText ||
               ( label->textFormat() == Qt::AutoText &&
                 Qt::mightBeRichText( label->text() ) ) )
              return;
      }
  }

  if (w->focusPolicy() != Qt::NoFocus || label || qobject_cast<QGroupBox*>(w) || qobject_cast<QRadioButton*>( w ))
  {
    QString content;
    QVariant variant;
    int tprop = w->metaObject()->indexOfProperty("text");
    if (tprop != -1)  {
        QMetaProperty p = w->metaObject()->property( tprop );
        if ( p.isValid() && p.isWritable() )
            variant = p.read (w);
        else
            tprop = -1;
    }

    if (tprop == -1)  {
        tprop = w->metaObject()->indexOfProperty("title");
        if (tprop != -1)  {
            QMetaProperty p = w->metaObject()->property( tprop );
            if ( p.isValid() && p.isWritable() )
                variant = p.read (w);
        }
    }

    if (variant.isValid())
        content = variant.toString();

    if (!content.isEmpty())
    {
        Item *i = new Item;
        i->m_widget = w;

        // put some more weight on the usual action elements
        int weight = KAccelManagerAlgorithm::DEFAULT_WEIGHT;
        if (qobject_cast<QPushButton*>(w) || qobject_cast<QCheckBox*>(w) || qobject_cast<QRadioButton*>(w) || qobject_cast<QLabel*>(w))
            weight = KAccelManagerAlgorithm::ACTION_ELEMENT_WEIGHT;

        // don't put weight on non-checkable group boxes,
        // as usually the contents are more important
        QGroupBox *groupBox = qobject_cast<QGroupBox*>(w);
        if (groupBox)
        {
            if (groupBox->isCheckable())
                weight = KAccelManagerAlgorithm::CHECKABLE_GROUP_BOX_WEIGHT;
            else
                weight = KAccelManagerAlgorithm::GROUP_BOX_WEIGHT;
        }

        i->m_content = KAccelString(content, weight);
        item->addChild(i);
    }
  }
  traverseChildren(w, item);
}

void KAcceleratorManagerPrivate::manageTabBar(QTabBar *bar, Item *item)
{
  // ignore QTabBar for QDockWidgets, because QDockWidget on its title change
  // also updates its tabbar entry, so on the next run of KCheckAccelerators
  // this looks like a conflict and triggers a new reset of the shortcuts -> endless loop
  QWidget* parentWidget = bar->parentWidget();
  if( parentWidget )
  {
    QMainWindow* mainWindow = qobject_cast<QMainWindow*>(parentWidget);
    // TODO: find better hints that this is a QTabBar for QDockWidgets
    if( mainWindow ) // && (mainWindow->layout()->indexOf(bar) != -1)) QMainWindowLayout lacks proper support
      return;
  }

  for (int i=0; i<bar->count(); i++)
  {
    QString content = bar->tabText(i);
    if (content.isEmpty())
      continue;

    Item *it = new Item;
    item->addChild(it);
    it->m_widget = bar;
    it->m_index = i;
    it->m_content = KAccelString(content);
  }
}

void KAcceleratorManagerPrivate::manageDockWidget(QDockWidget *dock, Item *item)
{
    // As of Qt 4.4.3 setting a shortcut to a QDockWidget has no effect,
    // because a QDockWidget does not grab it, even while displaying an underscore
    // in the title for the given shortcut letter.
    // Still it is useful to set the shortcut, because if QDockWidgets are tabbed,
    // the tab automatically gets the same text as the QDockWidget title, including the shortcut.
    // And for the QTabBar the shortcut does work, it gets grabbed as usual.
    // Having the QDockWidget without a shortcut and resetting the tab text with a title including
    // the shortcut does not work, the tab text is instantly reverted to the QDockWidget title
    // (see also manageTabBar()).
    // All in all QDockWidgets and shortcuts are a little broken for now.
    QString content = dock->windowTitle();
    if (content.isEmpty())
        return;

    Item *it = new Item;
    item->addChild(it);
    it->m_widget = dock;
    it->m_content = KAccelString(content, KAccelManagerAlgorithm::STANDARD_ACCEL);
}


void KAcceleratorManagerPrivate::manageMenuBar(QMenuBar *mbar, Item *item)
{
    QAction *maction;
    QString s;

    for (int i=0; i<mbar->actions().count(); ++i)
    {
        maction = mbar->actions()[i];
        if (!maction)
            continue;

        // nothing to do for separators
        if (maction->isSeparator())
            continue;

        s = maction->text();
        if (!s.isEmpty())
        {
            Item *it = new Item;
            item->addChild(it);
            it->m_content =
                KAccelString(s,
                             // menu titles are important, so raise the weight
                             KAccelManagerAlgorithm::MENU_TITLE_WEIGHT);

            it->m_widget = mbar;
            it->m_index = i;
        }

        // have a look at the popup as well, if present
        if (maction->menu())
            KPopupAccelManager::manage(maction->menu());
    }
}


/*********************************************************************

 class KAcceleratorManager - main entry point

 This class is just here to provide a clean public API...

 *********************************************************************/

void KAcceleratorManager::manage(QWidget *widget, bool programmers_mode)
{
    KAcceleratorManagerPrivate::changed_string.clear();
    KAcceleratorManagerPrivate::added_string.clear();
    KAcceleratorManagerPrivate::removed_string.clear();
    KAcceleratorManagerPrivate::programmers_mode = programmers_mode;
    KAcceleratorManagerPrivate::manage(widget);
}

void KAcceleratorManager::last_manage(QString &added,  QString &changed, QString &removed)
{
    added = KAcceleratorManagerPrivate::added_string;
    changed = KAcceleratorManagerPrivate::changed_string;
    removed = KAcceleratorManagerPrivate::removed_string;
}


/*********************************************************************

 class KAccelString - a string with weighted characters

 *********************************************************************/

KAccelString::KAccelString(const QString &input, int initialWeight)
  : m_pureText(input), m_weight()
{
    m_orig_accel = m_pureText.indexOf(QStringLiteral("(!)&"));
    if (m_orig_accel != -1)
	m_pureText.remove(m_orig_accel, 4);

    m_orig_accel = m_pureText.indexOf(QStringLiteral("(&&)"));
    if (m_orig_accel != -1)
        m_pureText.replace(m_orig_accel, 4, QStringLiteral("&"));

    m_origText = m_pureText;

    if (m_pureText.contains(QLatin1Char('\t')))
        m_pureText = m_pureText.left(m_pureText.indexOf(QLatin1Char('\t')));

    m_orig_accel = m_accel = stripAccelerator(m_pureText);

    if (initialWeight == -1)
        initialWeight = KAccelManagerAlgorithm::DEFAULT_WEIGHT;

    calculateWeights(initialWeight);

    // dump();
}


QString KAccelString::accelerated() const
{
  QString result = m_origText;
  if (result.isEmpty())
      return result;

  if (KAcceleratorManagerPrivate::programmers_mode)
  {
    if (m_accel != m_orig_accel) {
      int oa = m_orig_accel;

      if (m_accel >= 0) {
              result.insert(m_accel, QStringLiteral("(!)&"));
              if (m_accel < m_orig_accel)
                  oa += 4;
      }
      if (m_orig_accel >= 0)
          result.replace(oa, 1, QStringLiteral("(&&)"));
    }
  } else {
      if (m_accel >= 0 && m_orig_accel != m_accel) {
          if (m_orig_accel != -1)
              result.remove(m_orig_accel, 1);
          result.insert(m_accel, QStringLiteral("&"));
      }
  }
  return result;
}


QChar KAccelString::accelerator() const
{
  if ((m_accel < 0) || (m_accel > (int)m_pureText.length()))
    return QChar();

  return m_pureText[m_accel].toLower();
}


void KAccelString::calculateWeights(int initialWeight)
{
  m_weight.resize(m_pureText.length());

  int pos = 0;
  bool start_character = true;

  while (pos<m_pureText.length())
  {
    QChar c = m_pureText[pos];

    int weight = initialWeight+1;

    // add special weight to first character
    if (pos == 0)
      weight += KAccelManagerAlgorithm::FIRST_CHARACTER_EXTRA_WEIGHT;

    // add weight to word beginnings
    if (start_character)
    {
      weight += KAccelManagerAlgorithm::WORD_BEGINNING_EXTRA_WEIGHT;
      start_character = false;
    }

    // add decreasing weight to left characters
    if (pos < 50)
      weight += (50-pos);

    // try to preserve the wanted accelarators
    if ((int)pos == accel()) {
        weight += KAccelManagerAlgorithm::WANTED_ACCEL_EXTRA_WEIGHT;
        // qDebug() << "wanted " << m_pureText << " " << KAcceleratorManagerPrivate::standardName(m_origText);
        if (KAcceleratorManagerPrivate::standardName(m_origText))  {
            weight += KAccelManagerAlgorithm::STANDARD_ACCEL;
        }
    }

    // skip non typeable characters
    if (!c.isLetterOrNumber())
    {
      weight = 0;
      start_character = true;
    }

    m_weight[pos] = weight;

    ++pos;
  }
}


int KAccelString::stripAccelerator(QString &text)
{
  // Note: this code is derived from QAccel::shortcutKey
  int p = 0;

  while (p >= 0)
  {
    p = text.indexOf(QLatin1Char('&'), p)+1;

    if (p <= 0 || p >= (int)text.length())
      break;

    if (text[p] != QLatin1Char('&'))
    {
      QChar c = text[p];
      if (c.isPrint())
      {
        text.remove(p-1,1);
        return p-1;
      }
    }

    p++;
  }

  return -1;
}


int KAccelString::maxWeight(int &index, const QString &used) const
{
  int max = 0;
  index = -1;

  for (int pos=0; pos<m_pureText.length(); ++pos)
    if (used.indexOf(m_pureText[pos], 0, Qt::CaseInsensitive) == -1 && m_pureText[pos].toLatin1() != 0)
      if (m_weight[pos] > max)
      {
        max = m_weight[pos];
        index = pos;
      }

  return max;
}


void KAccelString::dump()
{
  QString s;
  for (int i=0; i<m_weight.count(); ++i)
    s += QStringLiteral("%1(%2) ").arg(pure()[i]).arg(m_weight[i]);
  qDebug() << "s " << s;
}


/*********************************************************************

 findAccelerators - the algorithm determining the new accelerators

 The algorithm is very crude:

   * each character in each widget text is assigned a weight
   * the character with the highest weight over all is picked
   * that widget is removed from the list
   * the weights are recalculated
   * the process is repeated until no more accelerators can be found

 The algorithm has some advantages:

   * it favors 'nice' accelerators (first characters in a word, etc.)
   * it is quite fast, O(N²)
   * it is easy to understand :-)

 The disadvantages:

   * it does not try to find as many accelerators as possible

 TODO:

 * The result is always correct, but not necessarily optimal. Perhaps
   it would be a good idea to add another algorithm with higher complexity
   that gets used when this one fails, i.e. leaves widgets without
   accelerators.

 * The weights probably need some tweaking so they make more sense.

 *********************************************************************/

void KAccelManagerAlgorithm::findAccelerators(KAccelStringList &result, QString &used)
{
  KAccelStringList accel_strings = result;

  // initially remove all accelerators
  for (KAccelStringList::Iterator it = result.begin(); it != result.end(); ++it) {
    (*it).setAccel(-1);
  }

  // pick the highest bids
  for (int cnt=0; cnt<accel_strings.count(); ++cnt)
  {
    int max = 0, index = -1, accel = -1;

    // find maximum weight
    for (int i=0; i<accel_strings.count(); ++i)
    {
      int a;
      int m = accel_strings[i].maxWeight(a, used);
      if (m>max)
      {
        max = m;
        index = i;
        accel = a;
      }
    }

    // stop if no more accelerators can be found
    if (index < 0)
      return;

    // insert the accelerator
    if (accel >= 0)
    {
      result[index].setAccel(accel);
      used.append(result[index].accelerator());
    }

    // make sure we don't visit this one again
    accel_strings[index] = KAccelString();
  }
}


/*********************************************************************

 class KPopupAccelManager - managing QPopupMenu widgets dynamically

 *********************************************************************/

KPopupAccelManager::KPopupAccelManager(QMenu *popup)
  : QObject(popup), m_popup(popup), m_count(-1)
{
    aboutToShow(); // do one check and then connect to show
    connect(popup, SIGNAL(aboutToShow()), SLOT(aboutToShow()));
}


void KPopupAccelManager::aboutToShow()
{
  // Note: we try to be smart and avoid recalculating the accelerators
  // whenever possible. Unfortunately, there is no way to know if an
 // item has been added or removed, so we can not do much more than
  // to compare the items each time the menu is shown :-(

  if (m_count != (int)m_popup->actions().count())
  {
    findMenuEntries(m_entries);
    calculateAccelerators();
    m_count = m_popup->actions().count();
  }
  else
  {
    KAccelStringList entries;
    findMenuEntries(entries);
    if (entries != m_entries)
    {
      m_entries = entries;
      calculateAccelerators();
    }
  }
}


void KPopupAccelManager::calculateAccelerators()
{
  // find the new accelerators
  QString used;
  KAccelManagerAlgorithm::findAccelerators(m_entries, used);

  // change the menu entries
  setMenuEntries(m_entries);
}


void KPopupAccelManager::findMenuEntries(KAccelStringList &list)
{
  QString s;

  list.clear();

  // read out the menu entries
  Q_FOREACH (QAction *maction, m_popup->actions())
  {
    if (maction->isSeparator())
      continue;

    s = maction->text();

    // in full menus, look at entries with global accelerators last
    int weight = 50;
    if (s.contains(QLatin1Char('\t')))
        weight = 0;

    list.append(KAccelString(s, weight));

    // have a look at the popup as well, if present
    if (maction->menu())
        KPopupAccelManager::manage(maction->menu());
  }
}


void KPopupAccelManager::setMenuEntries(const KAccelStringList &list)
{
  uint cnt = 0;
  Q_FOREACH (QAction *maction, m_popup->actions())
  {
    if (maction->isSeparator())
      continue;

    if (KAcceleratorManagerPrivate::checkChange(list[cnt]))
        maction->setText(list[cnt].accelerated());
    cnt++;
  }
}


void KPopupAccelManager::manage(QMenu *popup)
{
  // don't add more than one manager to a popup
  if (popup->findChild<KPopupAccelManager*>(QString()) == 0 )
    new KPopupAccelManager(popup);
}

void QWidgetStackAccelManager::manage( QStackedWidget *stack )
{
    if ( stack->findChild<QWidgetStackAccelManager*>(QString()) == 0 )
        new QWidgetStackAccelManager( stack );
}

QWidgetStackAccelManager::QWidgetStackAccelManager(QStackedWidget *stack)
  : QObject(stack), m_stack(stack)
{
    currentChanged(stack->currentIndex()); // do one check and then connect to show
    connect(stack, SIGNAL(currentChanged(int)), SLOT(currentChanged(int)));
}

bool QWidgetStackAccelManager::eventFilter ( QObject * watched, QEvent * e )
{
    if ( e->type() == QEvent::Show && qApp->activeWindow() ) {
        KAcceleratorManager::manage( qApp->activeWindow() );
        watched->removeEventFilter( this );
    }
    return false;
}

void QWidgetStackAccelManager::currentChanged(int child)
{
    if (child < 0 || child >= static_cast<QStackedWidget*>(parent())->count())
    {
        // NOTE: QStackedWidget emits currentChanged(-1) when it is emptied
        return;
    }

    static_cast<QStackedWidget*>(parent())->widget(child)->installEventFilter( this );
}

void KAcceleratorManager::setNoAccel( QWidget *widget )
{
    KAcceleratorManagerPrivate::ignored_widgets[widget] = 1;
}

#include "moc_kacceleratormanager_p.cpp"
