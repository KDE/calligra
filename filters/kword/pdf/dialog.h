/*
 * Copyright (c) 2002 Nicolas HADACEK (hadacek@kde.org)
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef DIALOG_H
#define DIALOG_H

#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qvaluevector.h>
#include <qpair.h>

#include <kdialogbase.h>
#include <klineedit.h>


//-----------------------------------------------------------------------------
class SelectionRange
{
public:
    SelectionRange(const QString &);

    uint nbPages() const;

private:
    QValueVector<QPair<uint, uint> > _ranges;

    friend class SelectionRangeIterator;
};

class SelectionRangeIterator
{
public:
    SelectionRangeIterator(const SelectionRange &);

    // -1 means at end
    int toFirst();
    int current() const { return _current; }
    int next();

private:
    uint _index;
    int  _current;
    const QValueVector<QPair<uint, uint> > &_ranges;
};

//-----------------------------------------------------------------------------
class PdfImportDialog : public KDialogBase
{
 Q_OBJECT
 public:
    PdfImportDialog(uint nbPages, bool isEncrypted, QWidget *parent);
    ~PdfImportDialog();

    SelectionRange range() const;
    QString ownerPassword() const { return _owner->text(); }
    QString userPassword() const { return _user->text(); }

 private slots:
    void rangeChanged(const QString &);

 private:
    uint          _nbPages;
    QRadioButton *_allButton, *_rangeButton;
    QButtonGroup *_group;
    KLineEdit    *_range, *_owner, *_user;
};

#endif
