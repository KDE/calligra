/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005 Peter Simonsson <psn@linux.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KOOPENPANE_H
#define KOOPENPANE_H

#include <QList>
#include <QWidget>

class KoOpenPanePrivate;
class QPixmap;
class KoTemplatesPane;
class KoDetailsPane;
class QUrl;
class QTreeWidgetItem;
class QString;

/// \internal
class KoOpenPane : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructor
     * @param parent the parent widget
     * @param mimeFilter the MIME type filter that should be selected on creation.
     * @param templatesResourcePath the path for template resources.
     */
    KoOpenPane(QWidget *parent, const QStringList &mimeFilter, const QString &templatesResourcePath = QString());
    ~KoOpenPane() override;

    QTreeWidgetItem *addPane(const QString &title, const QString &iconName, QWidget *widget, int sortWeight);
    QTreeWidgetItem *addPane(const QString &title, const QPixmap &icon, QWidget *widget, int sortWeight);

    /**
     * If the application has a way to create a document not based on a template, but on user
     * provided settings, the widget showing these gets set here.
     * @see KoDocument::createCustomDocumentWidget()
     * @param widget the widget.
     * @param title the title shown in the sidebar
     * @param icon the icon shown in the sidebar
     */
    void addCustomDocumentWidget(QWidget *widget, const QString &title = QString(), const QString &icon = QString());

protected Q_SLOTS:
    void updateSelectedWidget();
    void itemClicked(QTreeWidgetItem *item);

    /// Saves the splitter sizes for KoDetailsPaneBase based panes
    void saveSplitterSizes(KoDetailsPane *sender, const QList<int> &sizes);

private Q_SLOTS:
    /// when clicked "Open Existing Document" button
    void openFileDialog();

Q_SIGNALS:
    void openExistingFile(const QUrl &);
    void openTemplate(const QUrl &);

    /// Emitted when the always use template has changed
    void alwaysUseChanged(KoTemplatesPane *sender, const QString &alwaysUse);

    /// Emitted when one of the detail panes have changed it's splitter
    void splitterResized(KoDetailsPane *sender, const QList<int> &sizes);

protected:
    void initRecentDocs();
    /**
     * Populate the list with all templates the user can choose.
     * @param templatesResourcePath the template-type (group) that should be selected on creation.
     */
    void initTemplates(const QString &templatesResourcePath);

    // QWidget overrides
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    QStringList m_mimeFilter;

    KoOpenPanePrivate *const d;
};

#endif // KOOPENPANE_H
