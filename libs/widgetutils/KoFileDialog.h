/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2013-2014 Yue Liu <yue.liu@mail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOFILEDIALOG_H
#define KOFILEDIALOG_H

#include "kowidgetutils_export.h"

#include <QFileDialog>
#include <QList>
#include <QString>
#include <QStringList>
#include <QUrl>

/**
 * Wrapper around QFileDialog providing native file dialogs
 * on KDE/Gnome/Windows/OSX/etc.
 */
class KOWIDGETUTILS_EXPORT KoFileDialog : public QObject
{
    Q_OBJECT

public:
    enum DialogType { OpenFile, OpenFiles, OpenDirectory, ImportFile, ImportFiles, ImportDirectory, SaveFile };

    /**
     * @brief constructor
     * @param parent The parent of the file dialog
     * @param dialogType usage of the file dialog
     * @param dialogName the name for the file dialog. This will be used to open
     * the filedialog in the last open location, instead the specified directory.
     *
     * @return The name of the entry user selected in the file dialog
     *
     */
    KoFileDialog(QWidget *parent, KoFileDialog::DialogType type, const QString &dialogName);

    ~KoFileDialog() override;

    void setCaption(const QString &caption);

    /**
     * @brief setDefaultDir set the default directory to defaultDir
     *
     * @param defaultDir a path to a file or directory
     */
    void setDefaultDir(const QString &defaultDir, bool override = false);

    /**
     * @brief setOverrideDir override both the default dir and the saved dir found by dialogName
     * @param overrideDir a path to a file or directory
     */
    void setOverrideDir(const QString &overrideDir);

    /**
     * @brief setImageFilters sets the name filters for the file dialog to all
     * image formats Qt's QImageReader supports.
     */
    void setImageFilters();

    void setNameFilter(const QString &filter);

    /**
     * @brief setNameFilters set a list of description/extension pairs.
     *
     * These are not registered mimetypes. In contrast with Qt's filedialog namefilters,
     * you can only have _one_ pair per line. I.e.
     *
     * Gif Image (*gif)
     * Tiff Image (*tif)
     *
     * And NOT Images (*gif *tif)
     *
     * @param filterList
     * @param defaultFilter
     */
    void setNameFilters(const QStringList &filterList, QString defaultFilter = QString());
    void setMimeTypeFilters(const QStringList &filterList, QString defaultFilter = QString());
    void setHideNameFilterDetailsOption();

    QStringList nameFilters() const;

    QStringList filenames();
    QString filename();

    /**
     * @brief selectedNameFilter returns the name filter the user selected, either
     *    directory or by clicking on it.
     * @return
     */
    QString selectedNameFilter() const;

    QString selectedMimeType() const;

private Q_SLOTS:

    void filterSelected(const QString &filter);

private:
    /**
     * @brief splitNameFilter take a single line of a QDialog name filter and split it
     *   into several lines. This is needed because a single line name filter can contain
     *   more than one mimetype, making it impossible to figure out the correct extension.
     *
     *   The methods takes care of some duplicated extensions, like jpeg and jpg.
     * @param nameFilter the namefilter to be split
     * @param mimeList a pointer to the list with mimes that shouldn't be added.
     * @return a stringlist of all name filters.
     */
    static QStringList splitNameFilter(const QString &nameFilter, QStringList *mimeList);

    void createFileDialog();

    QString getUsedDir(const QString &dialogName);
    void saveUsedDir(const QString &fileName, const QString &dialogName);

    const QStringList getFilterStringListFromMime(const QStringList &mimeList, bool withAllSupportedEntry = false);

    class Private;
    Private *const d;
};

#endif /* KOFILEDIALOG_H */
