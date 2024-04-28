/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005-2006 Peter Simonsson <psn@linux.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOTEMPLATESPANE
#define KOTEMPLATESPANE

#include "KoDetailsPane.h"

class KoTemplate;
class KoTemplateGroup;

class KoTemplatesPanePrivate;

/**
 * This widget is the right-side part of the template opening widget.
 * The parent widget is initial widget in the document space of each Calligra component.
 * This widget shows a list of templates and can show their details or open it.
 */
class KoTemplatesPane : public KoDetailsPane
{
    Q_OBJECT
public:
    /**
     * Constructor.
     * @param parent the parent widget
     * @param header string used as header text in the listview
     * @param group the group of templates this widget will show.
     * @param defaultTemplate pointer to the default template. Used to select a
     * template when none has been selected before.
     */
    KoTemplatesPane(QWidget *parent, const QString &header, KoTemplateGroup *group, KoTemplate *defaultTemplate);
    ~KoTemplatesPane() override;

    /// Returns true if a template in this group was the last one selected
    bool isSelected();

public Q_SLOTS:
    void changeAlwaysUseTemplate(KoTemplatesPane *sender, const QString &alwaysUse);

Q_SIGNALS:
    /// Emitted when the always use checkbox is selected
    void alwaysUseChanged(KoTemplatesPane *sender, const QString &alwaysUse);

protected Q_SLOTS:
    void selectionChanged(const QModelIndex &index) override;

    void openFile() override;
    void openFile(const QModelIndex &index) override;
    void alwaysUseClicked();

private:
    KoTemplatesPanePrivate *const d;
};

#endif
