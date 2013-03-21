/* This file is part of the KDE project
 * Copyright (C) 2008 Thomas Zander <zander@kde.org>
 * Copyright (C) 2011 C. Boemann <cbo@boemann.dk>
 * Copyright (C) 2011-2012 Pierre Stirnweiss <pstirnweiss@googlemail.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#ifndef MODEL_H
#define MODEL_H

#include "AbstractStylesModel.h"

#include <QAbstractListModel>

#include <QSize>

class KoStyleThumbnailer;

class KoStyleManager;
class KoParagraphStyle;
class KoCharacterStyle;

class QImage;
class QSignalMapper;

/** This class is used to provide widgets (like the @class StylesCombo) the styles available to the document being worked on. The @class StylesModel can be of two types: character styles or paragraph styles type. This allows the widget to ignore the type of style it is handling.
  * Character styles in ODF can be specified in two ways. First, a named character style, specifying character formatting properties. It is meant to be used on a couple of individual characters. Secondely, a paragraph style also specifies character formatting properties, which are to be considered the default for that particular paragraph.
  * For this reason, the @class Stylesmodel, when of the type @value characterStyle, do not list the paragraph style names. Only the specific named chracter styles are listed. Additionally, as the first item, a virtual style "As paragraph" is provided. Selecting this "style" will set the character properties as specified by the paragraph style currently applied to the selection.
  * This class requires that a @class KoStyleManager and a @class KoStyleThumbnailer be set. See below methods.
  *
  * The StylesModel re-implement the AbstractStylesModel interface. Several components assume the following properties:
  * - the StylesModel is a flat list of items (this also means that "parent" QModelIndexes are always invalid)
  * - the StylesModel has only one column
  * - there is no header in the model
  * - only the following methods are used when updating the underlying model's data: resetModel, insertRows, moveRows, removeRows
*/

class StylesModel : public AbstractStylesModel
{
    Q_OBJECT

public:
    explicit StylesModel(KoStyleManager *styleManager, AbstractStylesModel::Type modelType, QObject *parent = 0);
    ~StylesModel();

    /** Re-implemented from QAbstractItemModel. */

    virtual QModelIndex index(int row, int column=0, const QModelIndex &parent = QModelIndex()) const;

    virtual int rowCount(const QModelIndex &parent) const;

    virtual QVariant data(const QModelIndex &index, int role) const;

    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    virtual QModelIndex parent(const QModelIndex &child) const;

    virtual int columnCount(const QModelIndex &parent) const;

    /** *********************************** */
    /** Specific methods of the StylesModel */

    /** ************************* */
    /** Initialising of the model */

    /** Sets the @class KoStyleManager of the model. Setting this will populate the styles. It is required that a @param manager is set before using the model.
      * CAUTION: Populating the style will select the first inserted item. If this model is already set on a view, this might cause the view to emit an item selection changed signal.
    */
    void setStyleManager(KoStyleManager *manager);

    /** Sets the @class KoStyleThumbnailer of the model. It is required that a @param thumbnailer is set before using the model. */
    void setStyleThumbnailer(KoStyleThumbnailer *thumbnailer);


    /** *************** */
    /** Using the model */

    /** Return a @class QModelIndex for the specified @param style. */
    QModelIndex indexForParagraphStyle(const KoParagraphStyle &style) const;

    /** Return a @class QModelIndex for the specified @param style. */
    QModelIndex indexForCharacterStyle(const KoCharacterStyle &style) const;

    /** Returns a QImage which is a preview of the style specified by @param index of the given @param size.
      * If size isn't specified, the default size of the given @class KoStyleThumbnailer is used.
    */
    QImage stylePreview(const QModelIndex &index, QSize size = QSize());

    /** Specifies which paragraph style is currently the active one (on the current paragraph). This is used in order to properly preview the "As paragraph" virtual character style. */
    void setCurrentParagraphStyle(int styleId);

    /** Return the first index at list. */
    QModelIndex firstStyleIndex();

    /** Return style id list. */
    QList<int> StyleList();

    /** We call this when we want a clear style model. */
    void clearStyleModel();

    /** Returns the type of styles in the model */
    AbstractStylesModel::Type stylesType() const;

private slots:
    void removeParagraphStyle(KoParagraphStyle*);
    void removeCharacterStyle(KoCharacterStyle*);
    void updateName(int styleId);

public slots:
    void addParagraphStyle(KoParagraphStyle*);
    void addCharacterStyle(KoCharacterStyle*);


private:
    void updateParagraphStyles();
    void updateCharacterStyles();

protected:
    QList<int> m_styleList; // list of style IDs

private:
    KoStyleManager *m_styleManager;

    KoParagraphStyle *m_currentParagraphStyle;
    KoCharacterStyle *m_defaultCharacterStyle;

    QSignalMapper *m_styleMapper;
};

#endif
