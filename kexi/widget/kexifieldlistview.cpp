/* This file is part of the KDE project
   Copyright (C) 2003-2005 Jarosław Staniek <staniek@kde.org>

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

#include "kexifieldlistview.h"

#include <q3header.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qcursor.h>
#include <qpoint.h>
#include <qapplication.h>
#include <qbitmap.h>
#include <qstyle.h>
//Added by qt3to4:
#include <QPixmap>

#include <kdebug.h>
#include <kiconloader.h>
#include <kdeversion.h>
#include <kconfig.h>
#include <kglobalsettings.h>
#include <klocale.h>

#include <kexidb/tableschema.h>
#include <kexidb/queryschema.h>
#include <kexidb/utils.h>
#include <kexidragobjects.h>
#include <kexiutils/utils.h>

KexiFieldListView::KexiFieldListView(QWidget *parent, int options)
        : K3ListView(parent)
        , m_schema(0)
        , m_keyIcon(SmallIcon("key"))
        , m_noIcon(KexiUtils::emptyIcon(KIconLoader::Small))
        , m_options(options)
        , m_allColumnsItem(0)
{
    setAcceptDrops(true);
    viewport()->setAcceptDrops(true);
    setDropVisualizer(false);
    setDropHighlighter(true);
    setAllColumnsShowFocus(true);
    addColumn(i18n("Field Name"));
    if (m_options & ShowDataTypes)
        addColumn(i18n("Data Type"));
    if (m_options & AllowMultiSelection)
        setSelectionMode(Q3ListView::Extended);
    setResizeMode(Q3ListView::LastColumn);
// header()->hide();
    setSorting(-1, true); // disable sorting
    setDragEnabled(true);

    connect(this, SIGNAL(doubleClicked(Q3ListViewItem*, const QPoint &, int)),
            this, SLOT(slotDoubleClicked(Q3ListViewItem*)));
}

KexiFieldListView::~KexiFieldListView()
{
    delete m_schema;
}

void KexiFieldListView::setSchema(KexiDB::TableOrQuerySchema* schema)
{
    if (schema && m_schema == schema)
        return;
    m_allColumnsItem = 0;
    clear();
    delete m_schema;
    m_schema = schema;
    if (!m_schema)
        return;

    int order = 0;
    bool hasPKeys = true; //t->hasPrimaryKeys();
    K3ListViewItem *item = 0;
    KexiDB::QueryColumnInfo::Vector columns = m_schema->columns(true /*unique*/);
    const int count = columns.count();
    for (int i = -1; i < count; i++) {
        KexiDB::QueryColumnInfo *colinfo = 0;
        if (i == -1) {
            if (!(m_options & ShowAsterisk))
                continue;
            item = new K3ListViewItem(this, item, i18n("* (All Columns)"));
            m_allColumnsItem = item;
        } else {
            colinfo = columns[i];
            item = new K3ListViewItem(this, item, colinfo->aliasOrName());
            if (m_options & ShowDataTypes)
                item->setText(1, colinfo->field->typeName());
        }
        if (colinfo && (colinfo->field->isPrimaryKey() || colinfo->field->isUniqueKey()))
            item->setPixmap(0, m_keyIcon);
        else if (hasPKeys) {
            item->setPixmap(0, m_noIcon);
        }
        order++;
    }

    setCurrentItem(firstChild());
}

#if 0
QSize KexiFieldListView::sizeHint()
{
    QFontMetrics fm(font());

    kDebug() << m_table->name() << " cw=" << columnWidth(1) + fm.width("i") << ", " << fm.width(m_table->name() + "  ");

    QSize s(
        qMax(columnWidth(1) + fm.width("i"), fm.width(m_table->name() + "  ")),
        childCount()*firstChild()->totalHeight() + 4);
// QSize s( columnWidth(1), childCount()*firstChild()->totalHeight() + 3*firstChild()->totalHeight()/10);
    return s;
}

void KexiFieldListView::setReadOnly(bool b)
{
    setAcceptDrops(!b);
    viewport()->setAcceptDrops(!b);
}
#endif

Q3DragObject* KexiFieldListView::dragObject()
{
    if (!schema())
        return 0;
    const QStringList selectedFields(selectedFieldNames());
    return new KexiFieldDrag(m_schema->table() ? "kexi/table" : "kexi/query",
                             m_schema->name(), selectedFields, this, "KexiFieldDrag");
    /* if (selectedItem()) {
        KexiFieldDrag *drag = new KexiFieldDrag("kexi/table", m_schema->name(),
          selectedItem()->text(1), this, "KexiFieldDrag");
          return drag;
      }*/
}

QStringList KexiFieldListView::selectedFieldNames() const
{
    if (!schema())
        return QStringList();
    QStringList selectedFields;
    for (Q3ListViewItem *item = firstChild(); item; item = item->nextSibling()) {
        if (item->isSelected()) {
//! @todo what about query fields/aliases? it.current()->text(0) can be not enough
            if (item == m_allColumnsItem && m_allColumnsItem)
                selectedFields.append("*");
            else
                selectedFields.append(item->text(0));
        }
    }
    return selectedFields;
}

void KexiFieldListView::slotDoubleClicked(Q3ListViewItem* item)
{
    if (schema() && item) {
        //! @todo what about query fields/aliases? it.current()->text(0) can be not enough
        emit fieldDoubleClicked(schema()->table() ? "kexi/table" : "kexi/query",
                                schema()->name(), item->text(0));
    }
}

#include "kexifieldlistview.moc"
