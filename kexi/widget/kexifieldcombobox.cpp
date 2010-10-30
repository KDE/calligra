/* This file is part of the KDE project
   Copyright (C) 2005-2006 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexifieldcombobox.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qcursor.h>
#include <qpoint.h>
#include <qapplication.h>
#include <qbitmap.h>
#include <qstyle.h>
#include <QPixmap>

#include <kdebug.h>
#include <kiconloader.h>

#include <kconfig.h>
#include <kglobalsettings.h>
#include <klocale.h>

#include <kexidb/tableschema.h>
#include <kexidb/queryschema.h>
#include <kexidb/utils.h>
#include <kexiutils/utils.h>
#include <kexidragobjects.h>
#include <kexiproject.h>
#include <kexi_global.h>
#include "KexiFieldListModel.h"

//! @internal
class KexiFieldComboBox::Private
{
public:
    Private()
            : table(true) {
    }
    ~Private() {
    }
    QPointer<KexiProject> prj;
    QPointer<KexiFieldListModel> model;
    
    QString tableOrQueryName;
    QString fieldOrExpression;

    bool table;
};

//------------------------

KexiFieldComboBox::KexiFieldComboBox(QWidget *parent)
        : KComboBox(true/*rw*/, parent)
        , d(new Private())
{
    setInsertPolicy(NoInsert);
    setCompletionMode(KGlobalSettings::CompletionPopupAuto);

    setMaxVisibleItems(16);
    connect(this, SIGNAL(activated(int)),
            this, SLOT(slotActivated(int)));
    connect(this, SIGNAL(returnPressed(const QString &)),
            this, SLOT(slotReturnPressed(const QString &)));

}

KexiFieldComboBox::~KexiFieldComboBox()
{
    delete d;
}

void KexiFieldComboBox::setProject(KexiProject *prj)
{
    if ((KexiProject*)d->prj == prj)
        return;
    d->prj = prj;
    setTableOrQuery(QString(), true);
}

KexiProject* KexiFieldComboBox::project() const
{
    return d->prj;
}

void KexiFieldComboBox::setTableOrQuery(const QString& name, bool table)
{

    d->tableOrQueryName = name;
    d->table = table;
    clear();

    if (d->tableOrQueryName.isEmpty() || !d->prj)
        return;

    KexiDB::TableOrQuerySchema tableOrQuery(d->prj->dbConnection(), d->tableOrQueryName.toLatin1(), d->table);
    if (!tableOrQuery.table() && !tableOrQuery.query())
        return;

    delete d->model;
    d->model = new KexiFieldListModel(this, KexiFieldListModel::ShowEmptyItem);
    
    d->model->setSchema(&tableOrQuery);
    setModel(d->model);

    //update selection
    setFieldOrExpression(d->fieldOrExpression);
}

QString KexiFieldComboBox::tableOrQueryName() const
{
    return d->tableOrQueryName;
}

bool KexiFieldComboBox::isTableAssigned() const
{
    return d->table;
}

void KexiFieldComboBox::setFieldOrExpression(const QString& string)
{
    kDebug() << string;
    const QString name(string);
    const int pos = name.indexOf('.');
    if (pos == -1) {
        d->fieldOrExpression = name;
    } else {
        QString objectName = name.left(pos);
        if (d->tableOrQueryName != objectName) {
            d->fieldOrExpression = name;
            setCurrentIndex(0);
            setEditText(name);
//! @todo show error
            kWarning() << "KexiFieldComboBox::setField(): invalid table/query name in '" << name << "'";
            return;
        }
        d->fieldOrExpression = name.mid(pos + 1);
    }

    const int index = findText(d->fieldOrExpression);
    if (index == -1) {
        setCurrentIndex(0);
        setEditText(d->fieldOrExpression);
//! @todo show 'the item doesn't match' info?
        return;
    }
    
    setCurrentIndex(index);
}

void KexiFieldComboBox::setFieldOrExpression(int index)
{
    kDebug() << index;
    index++; //skip 1st empty item
    if (index >= count()) {
        kWarning() << QString("KexiFieldComboBox::setFieldOrExpression(int index): index %1 "
                            "out of range (0..%2)").arg(index).arg(count() - 1);
        index = -1;
    }
    if (index <= 0) {
        setCurrentIndex(0);
        d->fieldOrExpression.clear();
    } else {
        setCurrentIndex(index);
        d->fieldOrExpression = itemData(currentIndex(), Qt::DisplayRole).toString();
    }
}

QString KexiFieldComboBox::fieldOrExpression() const
{
    kDebug() << d->fieldOrExpression;
    return d->fieldOrExpression;
}

int KexiFieldComboBox::indexOfField() const
{
    kDebug();
    KexiDB::TableOrQuerySchema tableOrQuery(d->prj->dbConnection(), d->tableOrQueryName.toLatin1(), d->table);
    if (!tableOrQuery.table() && !tableOrQuery.query())
        return -1;

    return currentIndex() > 0 ? (currentIndex() - 1) : -1;
}

QString KexiFieldComboBox::fieldOrExpressionCaption() const
{
    kDebug() << itemData(currentIndex()).toString();
    return itemData(currentIndex()).toString();
}

void KexiFieldComboBox::slotActivated(int i)
{
    d->fieldOrExpression = itemData(i, Qt::DisplayRole).toString();
    kDebug() << i << d->fieldOrExpression;
    setFieldOrExpression(d->fieldOrExpression);
    emit selected();
}

void KexiFieldComboBox::slotReturnPressed(const QString & text)
{
    //text is available: select item for this text:
    int index;
    if (text.isEmpty()) {
        index = 0;
    } else {
        index = findText(text, Qt::MatchExactly);
        if (index < 1)
            return;
    }
    setCurrentIndex(index);
    slotActivated(index);
}

void KexiFieldComboBox::focusOutEvent(QFocusEvent *e)
{
    KComboBox::focusOutEvent(e);
    // accept changes if the focus is moved
    if (!KexiUtils::hasParent(this, focusWidget())) {
        //(a check needed because drop-down listbox also causes a focusout)
        slotReturnPressed(currentText());
    }
}

#include "kexifieldcombobox.moc"
