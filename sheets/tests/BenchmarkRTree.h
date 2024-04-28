/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
   MA  02110-1301  USA
*/
#ifndef CALLIGRA_SHEETS_RTREE_BENCHMARK_H
#define CALLIGRA_SHEETS_RTREE_BENCHMARK_H

#include "engine/RTree.h"
#include <QObject>

namespace Calligra
{
namespace Sheets
{

class RTreeBenchmark : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void init();
    void cleanup();

    void testInsertionPerformance();
    void testRowInsertionPerformance();
    void testColumnInsertionPerformance();
    void testRowDeletionPerformance();
    void testColumnDeletionPerformance();
    void testLookupPerformance();

private:
    RTree<double> m_tree;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_RTREE_BENCHMARK_H
