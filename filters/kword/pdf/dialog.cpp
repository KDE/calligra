/*
 * Copyright (c) 2002-2003 Nicolas HADACEK (hadacek@kde.org)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "dialog.h"
#include "dialog.moc"



#include <QRegExp>
#include <qapplication.h>
#include <q3grid.h>
#include <QLabel>
#include <QLayout>
#include <q3buttongroup.h>
#include <qradiobutton.h>

#include <QCheckBox>
//Added by qt3to4:
#include <Q3VBoxLayout>

#include <klocale.h>
#include <kdebug.h>
#include <klineedit.h>
#include <kvbox.h>


//-----------------------------------------------------------------------------
SelectionRange::SelectionRange(const QString &s)
{
    // fill
    Q3ValueVector<QPair<uint, uint> > r;
    QStringList list = QStringList::split(',', s);
    QRegExp range("^([0-9]+)\\-([0-9]+)$");
    QRegExp one("^[0-9]+$");
    for (QStringList::iterator it = list.begin(); it!=list.end(); ++it) {
        if ( one.exactMatch(*it) ) {
            uint p = (*it).toUInt();
            r.push_back( qMakePair(p, p) );
        } else if ( range.exactMatch(*it) ) {
            uint p1 = range.cap(1).toUInt();
            uint p2 = range.cap(2).toUInt();
            if ( p1>p2 ) continue;
            r.push_back( qMakePair(p1, p2) );
        }
    }

    // order
    QPair<uint, uint> tmp;
    for (uint i=1; i<r.size(); i++)
        if ( r[i].first<r[i-1].first )
            qSwap(r[i-1], r[i]);

    // coalesce
    for (uint i=0; i<r.size(); i++)
        if ( i!=0 && r[i].first<=tmp.second )
            tmp.second = qMax(tmp.second, r[i].second);
        else {
            _ranges.push_back(r[i]);
            tmp = r[i];
            kDebug(30516) << "selection range: (" << tmp.first << ","
                           << tmp.second << ") " << endl;
        }
}

uint SelectionRange::nbPages() const
{
    uint nb = 0;
    for (uint i=0; i<_ranges.size(); i++)
        nb += _ranges[i].second - _ranges[i].first + 1;
    return nb;
}

SelectionRangeIterator::SelectionRangeIterator(const SelectionRange &range)
    : _ranges(range._ranges)
{
    toFirst();
}

int SelectionRangeIterator::toFirst()
{
    if ( _ranges.size()==0 ) _current = -1;
    else {
        _index = 0;
        _current = _ranges[0].first;
    }
    return _current;
}

int SelectionRangeIterator::next()
{
    if ( _current==-1 ) return -1;
    if ( _current==int(_ranges[_index].second) ) {
        _index++;
        _current = (_index==_ranges.size() ? -1
                    : int(_ranges[_index].first));
    } else _current++;
    return _current;
}

//-----------------------------------------------------------------------------
namespace PDFImport
{

Dialog::Dialog(uint nbPages, bool isEncrypted, QWidget *widget)
    : KDialogBase(Plain, i18n("KWord's PDF Import Filter"), Ok|Cancel, Ok,
                  widget, "pdf_import_dialog"), _nbPages(nbPages)
{
    QApplication::restoreOverrideCursor();

    Q3VBoxLayout *top = new Q3VBoxLayout(plainPage(), KDialogBase::marginHint(),
                                       KDialogBase::spacingHint());

    // page selection
    Q3GroupBox *gbox = new Q3GroupBox(1, Qt::Horizontal,i18n("Page Selection"), plainPage());
    gbox->setInsideSpacing(KDialogBase::spacingHint());
    top->addWidget(gbox);
    _group = new Q3ButtonGroup;
    _allButton = new QRadioButton(i18n("All (%1 pages)").arg(nbPages), gbox);
    _allButton->setChecked(true);
    _group->insert(_allButton);
    KHBox *hbox = new KHBox(gbox);
    _rangeButton = new QRadioButton(i18n("Range:"), hbox);
    _group->insert(_rangeButton);
    _range = new KLineEdit(hbox);
    _range->setFocus();
    connect(_range, SIGNAL(textChanged(const QString &)),
            SLOT(rangeChanged(const QString &)));

    // options
    _images = new QCheckBox(i18n("Import images"), plainPage());
    _images->setChecked(true);
    top->addWidget(_images);
    _smart = new QCheckBox(i18n("\"Smart\" mode"), plainPage());
    _smart->setChecked(true);
    _smart->setWhatsThis(
                    i18n("Removes returns and hyphens at end of line. "
                         "Also tries to compute the paragraph alignment. "
                         "Note that the layout of some pages can "
                         "get messed up."));
    top->addWidget(_smart);

    // passwords
    gbox = new Q3GroupBox(1, Qt::Horizontal,i18n("Passwords"), plainPage());
    top->addWidget(gbox);
    Q3Grid *grid = new Q3Grid(2, gbox);
    grid->setSpacing(KDialogBase::spacingHint());
    (void)new QLabel(i18n("Owner:"), grid);
    _owner = new KLineEdit(grid);
    _owner->setEchoMode(QLineEdit::Password);
    (void)new QLabel(i18n("User:"), grid);
    _user = new KLineEdit(grid);
    _user->setEchoMode(QLineEdit::Password);
    grid->setEnabled(isEncrypted);
}

Dialog::~Dialog()
{
    delete _group;
    QApplication::setOverrideCursor(Qt::WaitCursor);
}

void Dialog::rangeChanged(const QString &)
{
    _rangeButton->setChecked(true);
}

Options Dialog::options() const
{
    Options o;
    o.range = SelectionRange( (_allButton->isChecked() ?
                             QString("1-%1").arg(_nbPages) : _range->text()) );
    o.ownerPassword = _owner->text();
    o.userPassword = _user->text();
    o.importImages = _images->isChecked();
    o.smart = _smart->isChecked();
    return o;
}

} // namespace
