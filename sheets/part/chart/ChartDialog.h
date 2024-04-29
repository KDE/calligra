/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_CHART_DIALOG
#define CALLIGRA_SHEETS_CHART_DIALOG

#include <KoShapeConfigFactoryBase.h>

#include <kpagedialog.h>

#include <QCheckBox>
#include <QList>

namespace KoChart
{
class ChartShape;
}

namespace Calligra
{
namespace Sheets
{
class Map;

/// A dialog for showing and altering frame properties
class ChartDialog : public KPageDialog
{
    Q_OBJECT
public:
    /**
     * Constructor.
     * @param selectedCharts all charts that this dialog will show for user modification
     * @param parent a parent widget for the purpose of centering the dialog
     */
    explicit ChartDialog(const QList<KoChart::ChartShape *> &selectedCharts, QWidget *parent = nullptr);
    ~ChartDialog() override;

    /**
     * Create a list of factories that will be able to create widgets to configure shapes.
     * @param map the parent document these panels will work for.
     */
    static QList<KoShapeConfigFactoryBase *> panels(Map *map);

private:
};

/// A simple class useful for finding out if a series of data object will cause a
/// normal or a tri-state checkbox. For example.
class GuiHelper
{
public:
    /// the different states
    enum State {
        Unset, ///< start value
        On, ///< on
        Off, ///< off
        TriState ///< Both on and off
    };
    /// constructor
    GuiHelper()
        : m_state(Unset)
    {
    }
    /// Add a new state
    void addState(State state)
    {
        if (m_state == Unset)
            m_state = state;
        else if (m_state != state)
            m_state = TriState;
    }

    /**
     * Based on all the added states initialize the checkbox.
     * @param checkbox the checkbox to set.
     * @param hide if true the checkbox will be hidden when there was no 'addState' called
     */
    void updateCheckBox(QCheckBox *checkbox, bool hide)
    {
        if (m_state == Unset) {
            if (hide)
                checkbox->setVisible(false);
            checkbox->setEnabled(false);
            checkbox->setTristate(true);
            checkbox->setCheckState(Qt::PartiallyChecked);
        } else if (m_state == TriState) {
            checkbox->setTristate(true);
            checkbox->setCheckState(Qt::PartiallyChecked);
        } else {
            checkbox->setCheckState(m_state == On ? Qt::Checked : Qt::Unchecked);
        }
    }

    State m_state; ///< the current state
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_CHART_DIALOG
