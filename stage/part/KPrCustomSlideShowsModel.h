/* This file is part of the KDE project
* Copyright (C) 2011 Paul Mendez <paulestebanms@gmail.com>
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

#ifndef KPRCUSTOMSLIDESHOWSMODEL_H
#define KPRCUSTOMSLIDESHOWSMODEL_H

#include <QAbstractListModel>
#include <QSize>

class KPrCustomSlideShows;
class KoPAPageBase;
class KPrDocument;

#include "stage_export.h"

/**
 * Class meant to hold the model for Custom Slide Shows View
 * This model lets display slides as thumbnails in a List view, using standard QT
 * view/model framework. It supports add, remove and move slides of a custom slide show
 * and add, remove or rename slide Shows
 */
class STAGE_TEST_EXPORT KPrCustomSlideShowsModel : public QAbstractListModel
{
    Q_OBJECT
public:

    //List the actions that you could perform with slides
    //within a custom show.
    enum CustomShowActions
    {
        SlidesAdd,       ///< add slides
        SlidesDelete,    ///< delete slides
        SlidesMove       ///< move slides
    };

    explicit KPrCustomSlideShowsModel(KPrDocument *document, QObject *parent = 0);

    ~KPrCustomSlideShowsModel() override;

    QVariant data(const QModelIndex &index, int role) const override;

    int rowCount(const QModelIndex &parent) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent) const override;

    QStringList mimeTypes() const override;

    QMimeData *mimeData(const QModelIndexList &indexes) const override;

    Qt::DropActions supportedDropActions() const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;

    /**
     * @brief Set the current KPrCustomSlideShows
     *
     * @param customShows the KPrCustomSlideShows class
     */
    void setCustomSlideShows(KPrCustomSlideShows *customShows);

    /**
     * @brief Returns the active slide show name
     *
     * @return the active slide show name
     */
    QString activeCustomSlideShow() const;

    /**
     * @brief Set the active custom slide show by name
     *
     * @param name of the custom slide show
     */
    void setActiveSlideShow(const QString &name);

    /**
     * @brief Set the active custom slide show by index
     *
     * @param index of the custom slide show within the KPrCustomSlideShows class
     */
    void setActiveSlideShow(int index);

    /**
     * Setter of the size with a rect
     *
     * @param size which is a QRect
     */
    void setIconSize(const QSize &size);

    /**
     * @brief Returns the current slide show names list
     *
     * @return the current slide show names list
     */
    QStringList customShowsNamesList() const;

    /**
     * @brief setter of the current document
     *
     * @param document the document
     */
    void setDocument(KPrDocument *document);

    /**
     * @brief Deletes all the occurrences of a given list of slides indexes from current custom SlideShows
     *
     * @param slideIndexes list of slide indexes to be removed
     */
    void removeSlidesByIndexes(const QModelIndexList &slideIndexes);

    /**
     * @brief Add a given list of slides to the current custom slide show
     *
     * @param pages list of slides to be added.
     * @param row the insertion will be performed before the given row
     */
    void addSlides(const QList<KoPAPageBase *> &pages, const int &row);

    /**
     * @brief Manage all edit actions on a custom slide show
     *
     * @param action action to be performed according to CustomShowsAction Enum
     * @param slides list of slides to be added or moved
     * @param indexes list of indexes of the slides to be deleted
     * @param beginRow destination row for copy and move actions
     * @return true if the action success
     */
    bool doCustomSlideShowAction(const CustomShowActions &action, const QList<KoPAPageBase *> &slides, QList<int> indexes, int beginRow = 0);

    /**
     * @brief Insert a new Custom SlideShow
     *
     * @param name The name of the new SlideShow
     */
    void addNewCustomShow(const QString &name);

    /**
     * @brief Replaces a custom SlideShow name with the given SlideShow name
     *
     * @param oldName The name of the SlideShow to be renamed
     * @param newName The new name of the SlideShow
     */
    void renameCustomShow(const QString &oldName, const QString &newName);

    /**
     * @brief Delete a Custom SlideShow by its name
     *
     * @param name The name of the SlideShow to be removed
     */
    void removeCustomShow(const QString &name);

    /**
     * @brief Change active custom slide show and notify the change
     * If name exists change active custom slide show to name otherwise
     * switch to the default slide set
     *
     * @param name of the Custom Slide Show
     */
    void updateCustomSlideShowsList(const QString &name);

    QStringList namesByPage(KoPAPageBase *page);

Q_SIGNALS:
    /** Notify a change of the active custom slide show */
    void customSlideShowsChanged();

    /** Notify a need of page selection after coping or moving pages */
    void selectPages(int start, int number);

private Q_SLOTS:
    /** emit signals indicating a change in the model layout or items */
    void updateModel();

private:
    QList<KoPAPageBase *> decodeSlidesList(const QByteArray &encoded);

    KPrCustomSlideShows *m_customSlideShows;
    QString m_activeCustomSlideShowName;
    QSize m_iconSize;
    KPrDocument *m_document;

};

#endif // KPRCUSTOMSLIDESHOWSMODEL_H
