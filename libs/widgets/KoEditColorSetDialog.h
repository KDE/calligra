/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOEDITCOLORSET_H
#define KOEDITCOLORSET_H

#include <ui_KoEditColorSet.h>

#include <KoDialog.h>

#include "kowidgets_export.h"

class QGridLayout;
class QScrollArea;
class KoColorPatch;
class KoColorSet;

class KoEditColorSetWidget : public QWidget
{
    Q_OBJECT
public:
    KoEditColorSetWidget(const QList<KoColorSet *> &palettes, const QString &activePalette, QWidget *parent = nullptr);
    ~KoEditColorSetWidget() override;

    /**
     * Return the active color set. The caller takes ownership of that color set.
     */
    KoColorSet *activeColorSet();

private Q_SLOTS:
    void setActiveColorSet(int index);
    void setTextLabel(KoColorPatch *patch);
    void addColor();
    void removeColor();
    void open();
    void save();

private:
    Ui::KoEditColorSet widget;
    QList<KoColorSet *> m_colorSets;
    QGridLayout *m_gridLayout;
    QScrollArea *m_scrollArea;
    KoColorSet *m_activeColorSet;
    KoColorPatch *m_activePatch;
    uint m_initialColorSetCount;
    bool m_activeColorSetRequested;
};

/**
 * A dialog for editing palettes/color sets in an application. Example use of this dialog is in text color toolbar,
 * the toolbar brings a set of colors from one palette, and a button brings this dialog for editing palettes.
 * This dialog is able to:
 * - Set active palette from a combobox
 * - Add/remove color from a palette
 * - Open new palette from a gimp palette file (.gpl)
 * - Save changes to the file
 * @see KoColorSetWidget
 */
class KOWIDGETS_EXPORT KoEditColorSetDialog : public KoDialog
{
    Q_OBJECT

public:
    /**
     * Constructs a KoEditColorSetDialog.
     * @param palettes all available palettes that are going to be edited.
     * @param activePalette name of the palette which will be activated after this dialog is shown.
     * @param parent the parent widget
     */
    KoEditColorSetDialog(const QList<KoColorSet *> &palettes, const QString &activePalette, QWidget *parent = nullptr);

    /**
     * Returns the last active color set.
     * The caller takes ownership of that color set.
     * @return the last active KoColorSet in the dialog before the user press OK
     */
    KoColorSet *activeColorSet();

    /**
     * Destructor
     */
    ~KoEditColorSetDialog() override;

private:
    KoEditColorSetWidget *ui;
};

#endif
