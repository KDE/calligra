/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003   Daniel Molkentin <molkentin@kde.org>
   Copyright (C) 2003-2007 Jarosław Staniek <staniek@kde.org>

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

   Original Author:  Till Busch <till@bux.at>
   Original Project: buX (www.bux.at)
*/

#ifndef KEXITABLEVIEWCOLUMN_H
#define KEXITABLEVIEWCOLUMN_H

#include <QVariant>
#include <QVector>
#include <QList>

#include <kexidb/field.h>
#include <kexidb/queryschema.h>

#include <kexi_export.h>

namespace KexiUtils
{
class Validator;
}
class KexiTableViewData;


/*! Single column definition. */
class KEXIDATATABLE_EXPORT KexiTableViewColumn
{
public:
    typedef QList<KexiTableViewColumn*> List;
    typedef QList<KexiTableViewColumn*>::ConstIterator ListIterator;

    /*! Not db-aware ctor. if \a owner is true, the field \a will be owned by this column,
     so you shouldn't care about destroying this field. */
    KexiTableViewColumn(KexiDB::Field& f, bool owner = false);

    /*! Not db-aware, convenience ctor, like above. The field is created using specified parameters that are
     equal to these accepted by KexiDB::Field ctor. The column will be the owner
     of this automatically generated field.
     */
    KexiTableViewColumn(const QString& name, KexiDB::Field::Type ctype,
                        uint cconst = KexiDB::Field::NoConstraints,
                        uint options = KexiDB::Field::NoOptions,
                        uint length = 0, uint precision = 0,
                        QVariant defaultValue = QVariant(),
                        const QString& caption = QString(),
                        const QString& description = QString(),
                        uint width = 0);

    /*! Not db-aware, convenience ctor, simplified version of the above. */
    KexiTableViewColumn(const QString& name, KexiDB::Field::Type ctype, const QString& caption,
                        const QString& description = QString());

    //! Db-aware version.
    KexiTableViewColumn(const KexiDB::QuerySchema &query, KexiDB::QueryColumnInfo& aColumnInfo,
                        KexiDB::QueryColumnInfo* aVisibleLookupColumnInfo = 0);

    virtual ~KexiTableViewColumn();

    virtual bool acceptsFirstChar(const QChar& ch) const;

    /*! \return true if the column is read-only
     For db-aware column this can depend on whether the column
     is in parent table of this query. \sa setReadOnly() */
    bool isReadOnly() const;

//TODO: synchronize this with table view:
    //! forces readOnly flag to be set to \a ro
    inline void setReadOnly(bool ro) {
        m_readOnly = ro;
    }

    //! Column visibility. By default column is visible.
    inline bool isVisible() const {
        return m_columnInfo ? m_columnInfo->visible : m_visible;
    }

    //! Changes column visibility.
    inline void setVisible(bool v) {
        if (m_columnInfo)
            m_columnInfo->visible = v;
        m_visible = v;
    }

    /*! Sets icon for displaying in the caption area (header). */
    inline void setIcon(const QIconSet& icon) {
        m_icon = icon;
    }

    /*! \return bame of icon displayed in the caption area (header). */
    inline QIconSet icon() const {
        return m_icon;
    }

    /*! If \a visible is true, caption has to be displayed in the column's header,
     (or field's name if caption is empty. True by default. */
    inline void setHeaderTextVisible(bool visible) {
        m_headerTextVisible = visible;
    }

    /*! \return true if caption has to be displayed in the column's header,
     (or field's name if caption is empty. */
    inline bool isHeaderTextVisible() const {
        return m_headerTextVisible;
    }

    /*! \return whatever is available:
     - field's caption
     - or field's alias (from query)
     - or finally - field's name */
    inline QString captionAliasOrName() const {
        return m_captionAliasOrName;
    }

    /*! Assigns validator \a v for this column.
     If the validator has no parent object, it will be owned by the column,
     so you shouldn't care about destroying it. */
    void setValidator(KexiUtils::Validator* v);

    //! \return validator assigned for this column of 0 if there is no validator assigned.
    inline KexiUtils::Validator* validator() const {
        return m_validator;
    }

    /*! For not-db-aware data only:
     Sets related data \a data for this column, what defines simple one-field,
     one-to-many relationship between this column and the primary key in \a data.
     The relationship will be used to generate a popup editor instead of just regular editor.
     This assignment has no result if \a data has no primary key defined.
     \a data is owned, so is will be destroyed when needed. It is also destroyed
     when another data (or NULL) is set for the same column. */
    void setRelatedData(KexiTableViewData *data);

    /*! For not-db-aware data only:
     Related data \a data for this column, what defines simple one-field.
     NULL by default. \sa setRelatedData() */
    inline KexiTableViewData *relatedData() const {
        return m_relatedData;
    }

    /*! \return field for this column.
     For db-aware information is taken from columnInfo(). */
    inline KexiDB::Field* field() const {
        return m_field;
    }

    /*! Only usable if related data is set (ie. this is for combo boxes).
     Sets 'editable' flag for this column, what means a new value can be entered
     by hand. This is similar to QComboBox::setEditable(). */
    inline void setRelatedDataEditable(bool set) {
        m_relatedDataEditable = set;
    }

    /*! Only usable if related data is set (ie. this is for combo boxes).
     \return 'editable' flag for this column.
     False by default. @see setRelatedDataEditable(bool). */
    inline bool isRelatedDataEditable() const {
        return m_relatedDataEditable;
    }

    /*! A rich field information for db-aware data.
     For not-db-aware data it is always 0 (use field() instead). */
    inline KexiDB::QueryColumnInfo* columnInfo() const {
        return m_columnInfo;
    }

    /*! A rich field information for db-aware data. Specifies information for a column
     that should be visible instead of columnInfo. For example case see
     @ref KexiDB::QueryColumnInfo::Vector KexiDB::QuerySchema::fieldsExpanded(KexiDB::QuerySchema::FieldsExpandedOptions options = Default)

     For not-db-aware data it is always 0. */
    inline KexiDB::QueryColumnInfo* visibleLookupColumnInfo() const {
        return m_visibleLookupColumnInfo;
    }

    //! \retrun true if data is stored in DB, not only in memeory.
    inline bool isDBAware() const {
        return m_isDBAware;
    }


    /*  QString caption;
        int type; //!< one of KexiDB::Field::Type
        uint width;
    */
//  bool isNull() const;

    /*  virtual QString caption() const;
        virtual void setCaption(const QString& c);
      */
protected:
    //! special ctor that does not allocate d member;
    KexiTableViewColumn(bool);

    void init();

    //! used by KexiTableViewData::addColumn()
    void setData(KexiTableViewData* data);

    QString m_captionAliasOrName;

    QIconSet m_icon;

    KexiUtils::Validator* m_validator;

    KexiTableViewData* m_relatedData;
    uint m_relatedDataPKeyID;

    KexiDB::Field* m_field;

    //! @see columnInfo()
    KexiDB::QueryColumnInfo* m_columnInfo;

    //! @see visibleLookupColumnInfo()
    KexiDB::QueryColumnInfo* m_visibleLookupColumnInfo;

    bool m_isDBAware : 1; //!< true if data is stored in DB, not only in memeory
    bool m_readOnly : 1;
    bool m_fieldOwned : 1;
    bool m_visible : 1;
    bool m_relatedDataEditable : 1;
    bool m_headerTextVisible : 1;

    class Private;
    Private * const d;

    friend class KexiTableViewData;
};

#endif
