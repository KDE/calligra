/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 2003 Benjamin C Meyer (ben+kdelibs at meyerhome dot net)
 *  Copyright (C) 2003 Waldo Bastian <bastian@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#include "kconfigdialogmanager.h"

#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QMetaObject>
#include <QMetaProperty>
#include <QTimer>
#include <QRadioButton>
#include <QDebug>

#include <kconfigskeleton.h>

typedef QHash<QString, QByteArray> MyHash;
Q_GLOBAL_STATIC(MyHash, s_propertyMap)
Q_GLOBAL_STATIC(MyHash, s_changedMap)

class KConfigDialogManager::Private {

public:
  Private(KConfigDialogManager *q) : q(q), insideGroupBox(false) { }

public:
  KConfigDialogManager *q;

  /**
  * KConfigSkeleton object used to store settings
   */
  KCoreConfigSkeleton *m_conf;

  /**
  * Dialog being managed
   */
  QWidget *m_dialog;

  QHash<QString, QWidget *> knownWidget;
  QHash<QString, QWidget *> buddyWidget;
  bool insideGroupBox : 1;
  bool trackChanges : 1;
};

KConfigDialogManager::KConfigDialogManager(QWidget *parent, KCoreConfigSkeleton *conf)
 : QObject(parent), d(new Private(this))
{
  d->m_conf = conf;
  d->m_dialog = parent;
  init(true);
}

KConfigDialogManager::KConfigDialogManager(QWidget *parent, KConfigSkeleton *conf)
 : QObject(parent), d(new Private(this))
{
  d->m_conf = conf;
  d->m_dialog = parent;
  init(true);
}

KConfigDialogManager::~KConfigDialogManager()
{
  delete d;
}

void KConfigDialogManager::initMaps()
{
  if ( s_propertyMap()->isEmpty() ) {
    s_propertyMap()->insert( "KButtonGroup", "current" );
    s_propertyMap()->insert( "KColorButton", "color" );
    s_propertyMap()->insert( "KColorCombo", "color" );
    //s_propertyMap()->insert( "KUrlRequester", "url" );
    //s_propertyMap()->insert( "KUrlComboRequester", "url" );
  }

  if( s_changedMap()->isEmpty() )
  {
    // QT
    s_changedMap()->insert("QCheckBox", SIGNAL(stateChanged(int)));
    s_changedMap()->insert("QPushButton", SIGNAL(clicked(bool)));
    s_changedMap()->insert("QRadioButton", SIGNAL(toggled(bool)));
    // We can only store one thing, so you can't have
    // a ButtonGroup that is checkable.
//    s_changedMap()->insert("QButtonGroup", SIGNAL(buttonClicked(int)));
    s_changedMap()->insert("QGroupBox", SIGNAL(toggled(bool)));
    s_changedMap()->insert("QComboBox", SIGNAL(activated(int)));
    //qsqlproperty map doesn't store the text, but the value!
    //s_changedMap()->insert("QComboBox", SIGNAL(textChanged(QString)));
    s_changedMap()->insert("QDateEdit", SIGNAL(dateChanged(QDate)));
    s_changedMap()->insert("QTimeEdit", SIGNAL(timeChanged(QTime)));
    s_changedMap()->insert("QDateTimeEdit", SIGNAL(dateTimeChanged(QDateTime)));
    s_changedMap()->insert("QDial", SIGNAL(valueChanged(int)));
    s_changedMap()->insert("QDoubleSpinBox", SIGNAL(valueChanged(double)));
    s_changedMap()->insert("QLineEdit", SIGNAL(textChanged(QString)));
    s_changedMap()->insert("QSlider", SIGNAL(valueChanged(int)));
    s_changedMap()->insert("QSpinBox", SIGNAL(valueChanged(int)));
    s_changedMap()->insert("QTextEdit", SIGNAL(textChanged()));
    s_changedMap()->insert("QTextBrowser", SIGNAL(sourceChanged(QString)));
    s_changedMap()->insert("QPlainTextEdit", SIGNAL(textChanged()));
    s_changedMap()->insert("QTabWidget", SIGNAL(currentChanged(int)));

    // KDE
    s_changedMap()->insert( "KComboBox", SIGNAL(activated(int)));
    s_changedMap()->insert( "KFontComboBox", SIGNAL(activated(int)));
    s_changedMap()->insert( "KFontRequester", SIGNAL(fontSelected(QFont)));
    s_changedMap()->insert( "KFontChooser",  SIGNAL(fontSelected(QFont)));
    s_changedMap()->insert( "KHistoryCombo", SIGNAL(activated(int)));
    s_changedMap()->insert( "KColorCombo", SIGNAL(activated(QColor)));

    s_changedMap()->insert( "KColorButton", SIGNAL(changed(QColor)));
    s_changedMap()->insert( "KDatePicker", SIGNAL(dateSelected(QDate)));
    s_changedMap()->insert( "KDateWidget", SIGNAL(changed(QDate)));
    s_changedMap()->insert( "KDateTimeWidget", SIGNAL(valueChanged(QDateTime)));
    s_changedMap()->insert( "KEditListWidget", SIGNAL(changed()));
    s_changedMap()->insert( "KListWidget", SIGNAL(itemSelectionChanged()));
    s_changedMap()->insert( "KLineEdit", SIGNAL(textChanged(QString)));
    s_changedMap()->insert( "KPasswordEdit", SIGNAL(textChanged(QString)));
    s_changedMap()->insert( "KRestrictedLine", SIGNAL(textChanged(QString)));
    s_changedMap()->insert( "KTextEdit", SIGNAL(textChanged()));
    s_changedMap()->insert( "KUrlRequester",  SIGNAL(textChanged(QString)));
    s_changedMap()->insert( "KUrlComboRequester",  SIGNAL(textChanged(QString)));
    s_changedMap()->insert( "KUrlComboBox",  SIGNAL(urlActivated(QUrl)));
    s_changedMap()->insert( "KButtonGroup", SIGNAL(changed(int)));
  }
}

QHash<QString, QByteArray> *KConfigDialogManager::propertyMap()
{
  initMaps();
  return s_propertyMap();
}

QHash<QString, QByteArray> *KConfigDialogManager::changedMap()
{
  initMaps();
  return s_changedMap();
}

void KConfigDialogManager::init(bool trackChanges)
{
  initMaps();
  d->trackChanges = trackChanges;

  // Go through all of the children of the widgets and find all known widgets
  (void) parseChildren(d->m_dialog, trackChanges);
}

void KConfigDialogManager::addWidget(QWidget *widget)
{
  (void) parseChildren(widget, true);
}

void KConfigDialogManager::setupWidget(QWidget *widget, KConfigSkeletonItem *item)
{
  QVariant minValue = item->minValue();
  if (minValue.isValid())
  {
    // Only q3datetimeedit is using this property we can remove it if we stop supporting Qt3Support
    if (widget->metaObject()->indexOfProperty("minValue") != -1)
       widget->setProperty("minValue", minValue);
    if (widget->metaObject()->indexOfProperty("minimum") != -1)
       widget->setProperty("minimum", minValue);
  }
  QVariant maxValue = item->maxValue();
  if (maxValue.isValid())
  {
    // Only q3datetimeedit is using that property we can remove it if we stop supporting Qt3Support
    if (widget->metaObject()->indexOfProperty("maxValue") != -1)
       widget->setProperty("maxValue", maxValue);
    if (widget->metaObject()->indexOfProperty("maximum") != -1)
       widget->setProperty("maximum", maxValue);
  }

  if (widget->whatsThis().isEmpty())
  {
    QString whatsThis = item->whatsThis();
    if ( !whatsThis.isEmpty() )
    {
      widget->setWhatsThis(whatsThis );
    }
  }

  if (widget->toolTip().isEmpty())
  {
    QString toolTip = item->toolTip();
    if ( !toolTip.isEmpty() )
    {
      widget->setToolTip(toolTip);
    }
  }

  if(!item->isEqual( property(widget) ))
    setProperty( widget, item->property() );
}

bool KConfigDialogManager::parseChildren(const QWidget *widget, bool trackChanges)
{
  bool valueChanged = false;
  const QList<QObject*> listOfChildren = widget->children();
  if(listOfChildren.count()==0) //?? XXX
    return valueChanged;

  foreach ( QObject *object, listOfChildren )
  {
    if(!object->isWidgetType())
      continue; // Skip non-widgets

    QWidget *childWidget = static_cast<QWidget *>(object);

    QString widgetName = childWidget->objectName();
    bool bParseChildren = true;
    bool bSaveInsideGroupBox = d->insideGroupBox;

    if (widgetName.startsWith(QLatin1String("kcfg_")))
    {
      // This is one of our widgets!
      QString configId = widgetName.mid(5);
      KConfigSkeletonItem *item = d->m_conf->findItem(configId);
      if (item)
      {
        d->knownWidget.insert(configId, childWidget);

        setupWidget(childWidget, item);

        if ( d->trackChanges ) {
          QHash<QString, QByteArray>::const_iterator changedIt = s_changedMap()->constFind(childWidget->metaObject()->className());

          if (changedIt == s_changedMap()->constEnd())
          {
		   // If the class name of the widget wasn't in the monitored widgets map, then look for
		   // it again using the super class name. This fixes a problem with using QtRuby/Korundum
		   // widgets with KConfigXT where 'Qt::Widget' wasn't being seen a the real deal, even
		   // though it was a 'QWidget'.
            if ( childWidget->metaObject()->superClass() )
              changedIt = s_changedMap()->constFind(childWidget->metaObject()->superClass()->className());
            else
              changedIt = s_changedMap()->constFind(0);
          }

          if (changedIt == s_changedMap()->constEnd())
          {
            qWarning() << "Don't know how to monitor widget '" << childWidget->metaObject()->className() << "' for changes!";
          }
          else
          {
            connect(childWidget, *changedIt,
                  this, SIGNAL(widgetModified()));

            QComboBox *cb = qobject_cast<QComboBox *>(childWidget);
            if (cb && cb->isEditable())
              connect(cb, SIGNAL(editTextChanged(QString)),
                    this, SIGNAL(widgetModified()));
	  }
        }
        QGroupBox *gb = qobject_cast<QGroupBox *>(childWidget);
        if (!gb)
          bParseChildren = false;
        else
          d->insideGroupBox = true;
      }
      else
      {
        qWarning() << "A widget named '" << widgetName << "' was found but there is no setting named '" << configId << "'";
      }
    }
    else if (QLabel *label = qobject_cast<QLabel*>(childWidget))
    {
      QWidget *buddy = label->buddy();
      if (!buddy)
        continue;
      QString buddyName = buddy->objectName();
      if (buddyName.startsWith(QLatin1String("kcfg_")))
      {
        // This is one of our widgets!
        QString configId = buddyName.mid(5);
        d->buddyWidget.insert(configId, childWidget);
      }
    }
//kf5: commented out to reduce debug output
// #ifndef NDEBUG
//     else if (!widgetName.isEmpty() && d->trackChanges)
//     {
//       QHash<QString, QByteArray>::const_iterator changedIt = s_changedMap()->constFind(childWidget->metaObject()->className());
//       if (changedIt != s_changedMap()->constEnd())
//       {
//         if ((!d->insideGroupBox || !qobject_cast<QRadioButton*>(childWidget)) &&
//             !qobject_cast<QGroupBox*>(childWidget) &&!qobject_cast<QTabWidget*>(childWidget) )
//           qDebug() << "Widget '" << widgetName << "' (" << childWidget->metaObject()->className() << ") remains unmanaged.";
//       }
//     }
// #endif

    if(bParseChildren)
    {
      // this widget is not known as something we can store.
      // Maybe we can store one of its children.
      valueChanged |= parseChildren(childWidget, trackChanges);
    }
    d->insideGroupBox = bSaveInsideGroupBox;
  }
  return valueChanged;
}

void KConfigDialogManager::updateWidgets()
{
  bool changed = false;
  bool bSignalsBlocked = signalsBlocked();
  blockSignals(true);

  QWidget *widget;
  QHashIterator<QString, QWidget *> it( d->knownWidget );
  while(it.hasNext()) {
     it.next();
     widget = it.value();

     KConfigSkeletonItem *item = d->m_conf->findItem(it.key());
     if (!item)
     {
        qWarning() << "The setting '" << it.key() << "' has disappeared!";
        continue;
     }

     if(!item->isEqual( property(widget) ))
     {
        setProperty( widget, item->property() );
//        qDebug() << "The setting '" << it.key() << "' [" << widget->className() << "] has changed";
        changed = true;
     }
     if (item->isImmutable())
     {
        widget->setEnabled(false);
        QWidget *buddy = d->buddyWidget.value(it.key(), 0);
        if (buddy)
           buddy->setEnabled(false);
     }
  }
  blockSignals(bSignalsBlocked);

  if (changed)
    QTimer::singleShot(0, this, SIGNAL(widgetModified()));
}

void KConfigDialogManager::updateWidgetsDefault()
{
  bool bUseDefaults = d->m_conf->useDefaults(true);
  updateWidgets();
  d->m_conf->useDefaults(bUseDefaults);
}

void KConfigDialogManager::updateSettings()
{
    bool changed = false;

    QWidget *widget;
    QHashIterator<QString, QWidget *> it( d->knownWidget );
    while(it.hasNext()) {
        it.next();
        widget = it.value();

        KConfigSkeletonItem *item = d->m_conf->findItem(it.key());
        if (!item) {
            qWarning() << "The setting '" << it.key() << "' has disappeared!";
            continue;
        }

        QVariant fromWidget = property(widget);
        if(!item->isEqual( fromWidget )) {
            item->setProperty( fromWidget );
            changed = true;
        }
    }
    if (changed)
    {
        d->m_conf->writeConfig();
        emit settingsChanged();
    }
}

QByteArray KConfigDialogManager::getUserProperty(const QWidget *widget) const
{
  if (!s_propertyMap()->contains(widget->metaObject()->className())) {
    const QMetaObject *metaObject = widget->metaObject();
    const QMetaProperty user = metaObject->userProperty();
    if ( user.isValid() ) {
        s_propertyMap()->insert( widget->metaObject()->className(), user.name() );
        //qDebug() << "class name: '" << widget->metaObject()->className()
        //<< " 's USER property: " << metaProperty.name() << endl;
    }
    else {
        return QByteArray(); //no USER property
    }
  }
  const QComboBox *cb = qobject_cast<const QComboBox *>(widget);
  if (cb) {
    const char *qcomboUserPropertyName = cb->QComboBox::metaObject()->userProperty().name();
    const int qcomboUserPropertyIndex = qcomboUserPropertyName ? cb->QComboBox::metaObject()->indexOfProperty(qcomboUserPropertyName) : -1;
    const char *widgetUserPropertyName = widget->metaObject()->userProperty().name();
    const int widgetUserPropertyIndex = widgetUserPropertyName ? cb->metaObject()->indexOfProperty(widgetUserPropertyName) : -1;

    if (qcomboUserPropertyIndex == widgetUserPropertyIndex) {
        return QByteArray(); // use the q/kcombobox special code
    }
  }

  return s_propertyMap()->value( widget->metaObject()->className() );
}

QByteArray KConfigDialogManager::getCustomProperty(const QWidget *widget) const
{
    QVariant prop(widget->property("kcfg_property"));
    if (prop.isValid()) {
        if (!prop.canConvert(QVariant::ByteArray)) {
            qWarning() << "kcfg_property on" << widget->metaObject()->className()
                          << "is not of type ByteArray";
        } else {
            return prop.toByteArray();
        }
    }
    return QByteArray();
}

void KConfigDialogManager::setProperty(QWidget *w, const QVariant &v)
{
/*  QButtonGroup *bg = qobject_cast<QButtonGroup *>(w);
  if (bg)
  {
    QAbstractButton *b = bg->button(v.toInt());
    if (b)
        b->setDown(true);
    return;
  }*/

    QByteArray userproperty = getCustomProperty(w);
    if (userproperty.isEmpty()) {
        userproperty = getUserProperty(w);
    }
    if (userproperty.isEmpty()) {
        QComboBox *cb = qobject_cast<QComboBox *>(w);
        if (cb) {
            if (cb->isEditable()) {
                int i = cb->findText(v.toString());
                if (i != -1) {
                    cb->setCurrentIndex(i);
                } else {
                    cb->setEditText(v.toString());
                }
            } else {
                cb->setCurrentIndex(v.toInt());
            }
            return;
        }
    }
    if (userproperty.isEmpty()) {
        qWarning() << w->metaObject()->className() << " widget not handled!";
        return;
    }

    w->setProperty(userproperty, v);
}

QVariant KConfigDialogManager::property(QWidget *w) const
{
/*  QButtonGroup *bg = qobject_cast<QButtonGroup *>(w);
  if (bg && bg->checkedButton())
    return QVariant(bg->id(bg->checkedButton()));*/

    QByteArray userproperty = getCustomProperty(w);
    if (userproperty.isEmpty()) {
        userproperty = getUserProperty(w);
    }
    if (userproperty.isEmpty()) {
        QComboBox *cb = qobject_cast<QComboBox *>(w);
        if (cb) {
            if (cb->isEditable()) {
                return QVariant(cb->currentText());
            } else {
                return QVariant(cb->currentIndex());
            }
        }
    }
    if (userproperty.isEmpty()) {
        qWarning() << w->metaObject()->className() << " widget not handled!";
        return QVariant();
    }

    return w->property(userproperty);
}

bool KConfigDialogManager::hasChanged() const
{
    QWidget *widget;
    QHashIterator<QString, QWidget *> it( d->knownWidget) ;
    while(it.hasNext()) {
        it.next();
        widget = it.value();

        KConfigSkeletonItem *item = d->m_conf->findItem(it.key());
        if (!item) {
            qWarning() << "The setting '" << it.key() << "' has disappeared!";
            continue;
        }

        if(!item->isEqual( property(widget) )) {
            // qDebug() << "Widget for '" << it.key() << "' has changed.";
            return true;
        }
    }
    return false;
}

bool KConfigDialogManager::isDefault() const
{
  bool bUseDefaults = d->m_conf->useDefaults(true);
  bool result = !hasChanged();
  d->m_conf->useDefaults(bUseDefaults);
  return result;
}


