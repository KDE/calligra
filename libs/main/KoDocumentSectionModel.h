/*
  SPDX-FileCopyrightText: 2006 Gábor Lehel <illissius@gmail.com>
  SPDX-FileCopyrightText: 2011 José Luis Vergara <pentalis@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KO_DOCUMENT_SECTION_MODEL_H
#define KO_DOCUMENT_SECTION_MODEL_H

#include <QAbstractItemModel>
#include <QIcon>
#include <QList>
#include <QString>
#include <QVariant>

/**
 * Model class for use with KoDocumentSectionView. The document
 * sections that an application uses need to inherit from this class
 * to be displayable in the KoDocumentSectionView. That would be,
 * for instance, pages in Words or Stage, layers in Karbon,
 * sheets in Calligra Sheets.
 *
 * The KoDocumentSectionView will display a thumbnail and a row of
 * icon properties for every document section.
 *
 * KoDocumentSectionModel is separate from the flake hierarchy:
 * documents sections are things like layers, sheets or pages, i.e., a
 * shape can cover more than one section, and a section can contain
 * many shapes or part of shapes.)
 *
 * See also the Qt documentation for QAbstractItemModel. This class
 * only extends that interface to provide a name and set of toggle
 * properties (like visible, locked, selected -- let your imagination
 * run riot).
 *
 * See for an example implementation KisLayer.
 *
 * Your class also needs to implement the following pure abstract
 * virtuals from QAbstractItemModel:
@code

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

    virtual QModelIndex parent(const QModelIndex &index) const;

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
@endcode

 */
class KoDocumentSectionModel : public QAbstractItemModel
{
public:
    explicit KoDocumentSectionModel(QObject *parent = nullptr)
        : QAbstractItemModel(parent)
    {
    }

    /// Extensions to Qt::ItemDataRole.
    enum ItemDataRole {
        /// Whether the section is the active one
        ActiveRole = Qt::UserRole + 1,

        /// A list of properties the part has.
        PropertiesRole,

        /// The aspect ratio of the section as a floating point value: width divided by height.
        AspectRatioRole,

        /// Use to communicate a progress report to the section delegate on an action (a value of -1 or a QVariant() disable the progress bar
        ProgressRole,

        /// Special activation role which is emitted when the user Atl-clicks on a section
        /// The item is first activated with ActiveRole, then a separate AlternateActiveRole comes
        AlternateActiveRole,

        /// This is to ensure that we can extend the data role in the future, since it's not possible to add a role after BeginThumbnailRole (due to "Hack")
        ReservedRole = Qt::UserRole + 99,

        /**
         * For values of BeginThumbnailRole or higher, a thumbnail of the layer of which neither dimension
         * is larger than (int) value - (int) BeginThumbnailRole.
         * This is a hack to work around the fact that Interview doesn't have a nice way to
         * request thumbnails of arbitrary size.
         */
        BeginThumbnailRole
    };

    /**
     *  describes a single property of a document section
     *
     * FIXME: using a QList instead of QMap and not having an untranslated identifier,
     * either enum or string, forces applications to rely on the order of properties
     * or to compare the translated strings. This makes it hard to robustly extend the
     * properties of document section items.
     */
    struct Property {
        /** i18n-ed name, suitable for displaying */
        QString name;

        /** Whether the property is a boolean (e.g. locked, visible) which can be toggled directly from the widget itself. */
        bool isMutable;

        /** Provide these if the property isMutable. */
        QIcon onIcon;
        QIcon offIcon;

        /** If the property isMutable, provide a boolean. Otherwise, a string suitable for displaying. */
        QVariant state;

        /** If the property is mutable, specifies whether it can be put into stasis. When a property
        is in stasis, a new state is created, and the old one is stored in stateInStasis. When
        stasis ends, the old value is restored and the new one discarded */
        bool canHaveStasis;

        /** If the property isMutable and canHaveStasis, indicate whether it is in stasis or not */
        bool isInStasis;

        /** If the property isMutable and canHaveStasis, provide this value to store the property's
        state while in stasis */
        bool stateInStasis;

        /// Default constructor. Use if you want to assign the members manually.
        Property()
            : isMutable(false)
        {
        }

        /// Constructor for a mutable property.
        Property(const QString &n, const QIcon &on, const QIcon &off, bool isOn)
            : name(n)
            , isMutable(true)
            , onIcon(on)
            , offIcon(off)
            , state(isOn)
            , canHaveStasis(false)
        {
        }

        /** Constructor for a mutable property accepting stasis */
        Property(const QString &n, const QIcon &on, const QIcon &off, bool isOn, bool isInStasis, bool stateInStasis)
            : name(n)
            , isMutable(true)
            , onIcon(on)
            , offIcon(off)
            , state(isOn)
            , canHaveStasis(true)
            , isInStasis(isInStasis)
            , stateInStasis(stateInStasis)
        {
        }

        /// Constructor for a nonmutable property.
        Property(const QString &n, const QString &s)
            : name(n)
            , isMutable(false)
            , state(s)
        {
        }
    };

    /** Return this type for PropertiesRole. */
    typedef QList<Property> PropertyList;
};

Q_DECLARE_METATYPE(KoDocumentSectionModel::PropertyList)

#endif
