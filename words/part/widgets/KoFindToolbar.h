/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2010 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOFINDTOOLBAR_H
#define KOFINDTOOLBAR_H

#include <QWidget>

class KActionCollection;
class KoFindBase;
/**
 * \brief A search toolbar widget.
 *
 * This class implements a search toolbar containing an autocomplete text
 * field, previous/next buttons and an options selector. It uses the methods
 * provided by KoFindBase to find and navigate search matches.
 */
class KoFindToolbar : public QWidget
{
    Q_OBJECT
public:
    /**
     * Constructor.
     *
     * \param find The KoFindBase instance this toolbar operates on.
     * \param ac An action collection that will get several find-related actions added to it.
     * \param parent The parent QWidget
     * \param f Qt window flags
     */
    explicit KoFindToolbar(KoFindBase *find, KActionCollection *ac, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~KoFindToolbar() override;

public Q_SLOTS:
    /**
     * Set focus to this widget, show it and hide replace related features.
     */
    void activateSearch();

    /**
     * Set focus to this widget, show it including replace related features.
     */
    void activateReplace();

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void matchFound())
    Q_PRIVATE_SLOT(d, void noMatchFound())
    Q_PRIVATE_SLOT(d, void searchWrapped(bool direction))
    Q_PRIVATE_SLOT(d, void addToHistory())
    Q_PRIVATE_SLOT(d, void find(const QString &pattern))
    Q_PRIVATE_SLOT(d, void optionChanged())
    Q_PRIVATE_SLOT(d, void replace())
    Q_PRIVATE_SLOT(d, void replaceAll())
    Q_PRIVATE_SLOT(d, void inputTimeout())
    Q_PRIVATE_SLOT(d, void returnPressed())
};

#endif // KOFINDTOOLBAR_H
