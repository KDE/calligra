/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Alexander Dymo <cloudtemple@mskat.net>
   Copyright (C) 2008 Jarosław Staniek <staniek@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#include "combobox.h"
#include "EditorDataModel.h"

#include <QLayout>
#include <QMap>
#include <QVariant>
#include <QPainter>
#include <QHBoxLayout>

#include <KDebug>

#include "Property.h"

ComboBox::ComboBox(const KoProperty::Property* property, QWidget *parent)
        : KComboBox(parent)
        , m_setValueEnabled(true)
{
//    QHBoxLayout *l = new QHBoxLayout(this);
//    l->setMargin(0);
//    l->setSpacing(0);
//    m_edit = new KComboBox(this);
//    m_edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//    m_edit->setMinimumHeight(5);
    //setPlainWidgetStyle(m_edit);

//    l->addWidget(m_edit);
    m_extraValueAllowed = property->option("extraValueAllowed", false).toBool();

    setEditable( m_extraValueAllowed );
    setInsertPolicy(QComboBox::NoInsert);
//    m_edit->setMinimumSize(10, 0); // to allow the combo to be resized to a small size
    setAutoCompletion(true);
    setContextMenuPolicy(Qt::NoContextMenu);

    setProperty(property);
//    if (property->listData()) {
  //      fillValues(property);
    //}
//not needed for combo setLeavesTheSpaceForRevertButton(true);

//    setFocusWidget(m_edit);
    connect(this, SIGNAL(activated(int)), this, SLOT(slotValueChanged(int)));
    
    setFrame(0);
/*    QList<QWidget*> children( findChildren<QWidget*>() );
    foreach (QWidget* w, children) {
        kDebug() << w->objectName() << w->metaObject()->className();
        w->setStyleSheet(QString());
    }*/
    //QComboBoxPrivateContainer
}

ComboBox::~ComboBox()
{
}

QVariant ComboBox::value() const
{
    if (!m_property->listData()) {
        kopropertywarn << "propery listData not available!" << endl;
        return QVariant();
    }
    const int idx = currentIndex();
    if (idx < 0 || idx >= (int)m_property->listData()->keys.count() || m_property->listData()->names[idx] != currentText().trimmed()) {
        if (!m_extraValueAllowed || currentText().isEmpty())
            return QVariant();
        return QVariant(currentText().trimmed());//trimmed 4 safety
    }
    return QVariant(m_property->listData()->keys[idx]);
}

void ComboBox::setValue(const QVariant &value, bool emitChange)
{
    if (!m_property || !m_property->listData()) {
        kopropertywarn << "propery listData not available!" << endl;
        return;
    }
    if (!m_setValueEnabled)
        return;
    int idx = m_property->listData()->keys.indexOf(value.toString());
    kDebug(30007) << "**********" << idx << "" << value.toString();
    if (idx >= 0 && idx < count()) {
        setCurrentIndex(idx);
    } else {
        if (idx < 0) {
            if (m_extraValueAllowed) {
                setCurrentIndex(-1);
                setEditText(value.toString());
            }
            kopropertywarn << "NO SUCH KEY '" << value.toString()
            << "' (property '" << m_property->name() << "')" << endl;
        } else {
            QStringList list;
            for (int i = 0; i < count(); i++)
                list += itemText(i);
            kopropertywarn << "ComboBox::setValue(): NO SUCH INDEX WITHIN COMBOBOX: " << idx
            << " count=" << count() << " value='" << value.toString()
            << "' (property '" << m_property->name() << "')\nActual combobox contents: "
            << list << endl;
        }
        setItemText(currentIndex(), QString::null);
    }

    if (value.isNull())
        return;

//??    if (emitChange)
//??        emit valueChanged(this);
}

/*
void ComboBox::drawViewer(QPainter *p, const QColorGroup &cg, const QRect &r, const QVariant &value)
{
    QString txt;
    if (property()->listData()) {
        const int idx = property()->listData()->keys.indexOf(value);
        if (idx >= 0)
            txt = property()->listData()->names[ idx ];
        else if (m_edit->isEditable())
            txt = m_edit->currentText();
    } else if (m_edit->isEditable()) {
        txt = m_edit->currentText();
    }

//    Widget::drawViewer(p, cg, r, txt); //keyForValue(value));
// p->eraseRect(r);
// p->drawText(r, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, keyForValue(value));
}*/

void ComboBox::fillValues()
{
    clear();
    //m_edit->clearContents();

    if (!m_property)
        return;
    if (!m_property->listData()) {
        kopropertywarn << "property listData not available!" << endl;
        return;
    }
//    m_keys = m_property->listData()->keys;
    addItems(m_property->listData()->names);
    KCompletion *comp = completionObject();
    comp->insertItems(m_property->listData()->names);
    comp->setCompletionMode(KGlobalSettings::CompletionShell);
}

void ComboBox::setProperty( const KoProperty::Property *property )
{
//    const bool b = (property() == prop);
//    m_setValueEnabled = false; //setValue() couldn't be called before fillBox()
//    Widget::setProperty(prop);
//    m_setValueEnabled = true;
//    if (!b)
    m_property = property;
    fillValues();
//    if (prop)
//        setValue(prop->value(), false); //now the value can be set
}

void ComboBox::slotValueChanged(int)
{
//    emit valueChanged(this);
}

/*
void ComboBox::setReadOnlyInternal(bool readOnly)
{
    setVisibleFlag(!readOnly);
}*/


/*QString
ComboBox::keyForValue(const QVariant &value)
{
  const QMap<QString, QVariant> *list = property()->valueList();
  Property::ListData *list = property()->listData();

  if (!list)
    return QString();
  int idx = listData->keys.findIndex( value );


  QMap<QString, QVariant>::ConstIterator endIt = list->constEnd();
  for(QMap<QString, QVariant>::ConstIterator it = list->constBegin(); it != endIt; ++it) {
    if(it.data() == value)
      return it.key();
  }
  return QString();
}*/

//-----------------------

ComboBoxDelegate::ComboBoxDelegate()
{
    options.removeBorders = false;
}

QString ComboBoxDelegate::displayText( const KoProperty::Property* property ) const
{
    KoProperty::Property::ListData *listData = property->listData();
    if (!listData)
        return property->value().toString();
    if (property->value().isNull())
        return QString();
    const int idx = listData->keys.indexOf( property->value() );
    if (idx == -1)
        return QString();
    return property->listData()->names[ idx ];
}

QWidget* ComboBoxDelegate::createEditor( int type, QWidget *parent, 
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    const KoProperty::EditorDataModel *editorModel = dynamic_cast<const KoProperty::EditorDataModel*>(index.model());
    KoProperty::Property *property = editorModel->propertyForItem(index);
    return new ComboBox(property, parent);
}

/*void ComboBoxDelegate::paint( QPainter * painter, 
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
}*/


#include "combobox.moc"
