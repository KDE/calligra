/* This file is part of the KDE project
   Copyright (C)  2001 Montel Laurent <lmontel@mandrakesoft.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __KoInsertLink__
#define __KoInsertLink__

#include <kdialogbase.h>

class QLineEdit;
class KURLRequester;


class internetLinkPage : public QWidget
{
  Q_OBJECT
public:
  internetLinkPage( QWidget *parent = 0, char *name = 0 );
  QString linkName()const;
  QString hrefName();
    void setLinkName(const QString & _name);
    void setHrefName(const QString &_name);
private:
  QString createInternetLink();
  QLineEdit* m_linkName, *m_hrefName;
private slots:
  void textChanged ( const QString & );
signals:
  void textChanged();
};

class mailLinkPage : public QWidget
{
  Q_OBJECT
public:
  mailLinkPage( QWidget *parent = 0, char *name = 0 );
  QString linkName()const;
  QString hrefName();
    void setLinkName(const QString & _name);
    void setHrefName(const QString &_name);

private slots:
  void textChanged ( const QString & );
private:
  QString createMailLink();
  QLineEdit* m_linkName, *m_hrefName;
signals:
  void textChanged();
};

class fileLinkPage : public QWidget
{
  Q_OBJECT
public:
  fileLinkPage( QWidget *parent = 0, char *name = 0 );
  QString linkName()const;
    QString hrefName();
    void setLinkName(const QString & _name);
    void setHrefName(const QString &_name);

private slots:
  void textChanged ( const QString & );
    void slotSelectRecentFile( const QString & );
private:
  QString createFileLink();
  QLineEdit* m_linkName;
  KURLRequester* m_hrefName;
signals:
  void textChanged();
};

class KoInsertLinkDia : public KDialogBase
{
    Q_OBJECT
public:
    KoInsertLinkDia( QWidget *parent, const char *name = 0 );
    static bool createLinkDia(QString & linkName, QString & hrefName);

    //internal
    QString linkName()const;
    QString hrefName();
    void setHrefLinkName(const QString &_href, const QString &_link);
protected slots:
    virtual void slotOk();
    void slotTextChanged (  );

private:
    fileLinkPage *fileLink;
    mailLinkPage *mailLink;
    internetLinkPage *internetLink;
};

#endif
