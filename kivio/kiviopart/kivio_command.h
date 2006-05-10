/* This file is part of the KDE project
   Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef kiviocommand_h
#define kiviocommand_h

#include <kcommand.h>
#include <QFont>
#include <QColor>
#include <qvaluelist.h>
#include <KoRect.h>
#include <KoPageLayout.h>
#include <koffice_export.h>
class KivioPage;
class KivioLayer;
class KivioStencil;
class KivioGroupStencil;
struct KoPageLayout;

class KivioChangePageNameCommand : public KNamedCommand
{
public:
    KivioChangePageNameCommand( const QString &_name,  const QString & _oldPageName, const QString & _newPageName, KivioPage *_page );
    ~KivioChangePageNameCommand();

    virtual void execute();
    virtual void unexecute();

protected:
    QString oldPageName;
    QString newPageName;
    KivioPage * m_page;
};

class KivioHidePageCommand : public KNamedCommand
{
public:
    KivioHidePageCommand( const QString &_name, KivioPage *_page );
    ~KivioHidePageCommand();

    virtual void execute();
    virtual void unexecute();

protected:
    KivioPage * m_page;
};

class KivioShowPageCommand : public KivioHidePageCommand
{
public:
    KivioShowPageCommand( const QString &_name, KivioPage *_page );
    ~KivioShowPageCommand() {}

    void execute() { KivioHidePageCommand::unexecute(); }
    void unexecute() { KivioHidePageCommand::execute(); }
};

class KivioAddPageCommand : public KNamedCommand
{
public:
    KivioAddPageCommand( const QString &_name, KivioPage *_page );
    ~KivioAddPageCommand();

    virtual void execute();
    virtual void unexecute();

protected:
    KivioPage * m_page;
};

class KivioRemovePageCommand : public KNamedCommand
{
public:
    KivioRemovePageCommand( const QString &_name, KivioPage *_page );
    ~KivioRemovePageCommand();

    virtual void execute();
    virtual void unexecute();

protected:
    KivioPage * m_page;
};

class KivioAddStencilCommand : public KNamedCommand
{
public:
    KivioAddStencilCommand( const QString &_name, KivioPage *_page,  KivioLayer * _layer, KivioStencil *_stencil  );
    ~KivioAddStencilCommand();

    virtual void execute();
    virtual void unexecute();

protected:
    KivioPage * m_page;
    KivioLayer * m_layer;
    KivioStencil *m_stencil;
};


class KivioRemoveStencilCommand : public KivioAddStencilCommand
{
public:
    KivioRemoveStencilCommand(const QString &_name, KivioPage *_page,  KivioLayer * _layer, KivioStencil *_stencil );
    ~KivioRemoveStencilCommand() {}

    void execute() { KivioAddStencilCommand::unexecute(); }
    void unexecute() { KivioAddStencilCommand::execute(); }
};

class KIVIO_EXPORT KivioChangeStencilTextCommand : public KNamedCommand
{
public:
  KivioChangeStencilTextCommand( const QString &_name, KivioStencil *_stencil, const QString & _oldText, const QString & _newText, KivioPage *_page, const QString& textBoxName = QString::null);
    ~KivioChangeStencilTextCommand();
    virtual void execute();
    virtual void unexecute();
protected:
    KivioStencil *m_stencil;
    QString oldText;
    QString newText;
    KivioPage *m_page;
    QString m_textBoxName;
};

class KivioAddLayerCommand : public KNamedCommand
{
public:
    KivioAddLayerCommand( const QString &_name, KivioPage *_page, KivioLayer * _layer,int _pos );
    ~KivioAddLayerCommand();

    virtual void execute();
    virtual void unexecute();

protected:
    KivioPage * m_page;
    KivioLayer *m_layer;
    int layerPos;
};

class KivioRemoveLayerCommand : public KivioAddLayerCommand
{
public:
    KivioRemoveLayerCommand( const QString &_name, KivioPage *_page, KivioLayer * _layer, int _pos );
    ~KivioRemoveLayerCommand() {}

    void execute() { KivioAddLayerCommand::unexecute(); }
    void unexecute() { KivioAddLayerCommand::execute(); }
};

class KivioRenameLayerCommand : public KNamedCommand
{
public:
    KivioRenameLayerCommand( const QString &_name, KivioLayer * _layer, const QString & _oldName, const QString & _newName);
    ~KivioRenameLayerCommand();
    virtual void execute();
    virtual void unexecute();

protected:
    KivioLayer *m_layer;
    QString oldName;
    QString newName;

};

class KIVIO_EXPORT  KivioResizeStencilCommand : public KNamedCommand
{
public:
    KivioResizeStencilCommand(const QString &_name, KivioStencil *_stencil, KoRect _initSize, KoRect _endSize, KivioPage *_page );
    ~KivioResizeStencilCommand();

    virtual void execute();
    virtual void unexecute();

protected:
    KivioStencil *m_stencil;
    KoRect initSize;
    KoRect endSize;
    KivioPage *m_page;
};

class KIVIO_EXPORT KivioMoveStencilCommand : public KNamedCommand
{
public:
    KivioMoveStencilCommand(const QString &_name, KivioStencil *_stencil, KoRect _initSize, KoRect _endSize, KivioPage *_page );
    ~KivioMoveStencilCommand();

    virtual void execute();
    virtual void unexecute();

protected:
    KivioStencil *m_stencil;
    KoRect initSize;
    KoRect endSize;
    KivioPage *m_page;
};

class KivioChangeLayoutCommand : public KNamedCommand
{
public:
    KivioChangeLayoutCommand(const QString &_name, KivioPage *_page, KoPageLayout _oldLayout, KoPageLayout _newLayout);
    ~KivioChangeLayoutCommand();

    virtual void execute();
    virtual void unexecute();

protected:
    KivioPage *m_page;
    KoPageLayout oldLayout;
    KoPageLayout newLayout;
};


class KivioChangeStencilHAlignmentCommand : public KNamedCommand
{
  public:
    KivioChangeStencilHAlignmentCommand(const QString& _name, KivioPage* _page, KivioStencil* _stencil,
                                        int _oldAlign, int _newAlign, const QString& textBoxName = QString::null);
    ~KivioChangeStencilHAlignmentCommand();

    virtual void execute();
    virtual void unexecute();

  protected:
    KivioPage *m_page;
    KivioStencil *m_stencil;
    int oldAlign;
    int newAlign;
    QString m_textBoxName;
};

class KivioChangeStencilVAlignmentCommand : public KNamedCommand
{
  public:
    KivioChangeStencilVAlignmentCommand(const QString& _name, KivioPage* _page, KivioStencil* _stencil,
                                        int _oldAlign, int _newAlign, const QString& textBoxName = QString::null);
    ~KivioChangeStencilVAlignmentCommand();

    virtual void execute();
    virtual void unexecute();

  protected:
    KivioPage *m_page;
    KivioStencil *m_stencil;
    int oldAlign;
    int newAlign;
    QString m_textBoxName;
};


class KivioChangeStencilFontCommand : public KNamedCommand
{
  public:
    KivioChangeStencilFontCommand(const QString& _name, KivioPage* _page, KivioStencil* _stencil,
                                  const QFont& _oldFont,  const QFont& _newFont,
                                  const QString& textBoxName = QString::null);
    ~KivioChangeStencilFontCommand();

    virtual void execute();
    virtual void unexecute();

  protected:
    KivioPage *m_page;
    KivioStencil *m_stencil;
    QFont oldFont;
    QFont newFont;
    QString m_textBoxName;
};

class KivioChangeStencilColorCommand : public KNamedCommand
{
  public:
    enum ColorType { CT_TEXTCOLOR, CT_FGCOLOR, CT_BGCOLOR };
    KivioChangeStencilColorCommand(const QString& _name, KivioPage* _page, KivioStencil*  _stencil,
                                   const QColor& _oldColor,  const QColor& _newColor, ColorType _type,
                                   const QString& textBoxName = QString::null);
    ~KivioChangeStencilColorCommand();

    virtual void execute();
    virtual void unexecute();

  protected:
    KivioPage *m_page;
    KivioStencil *m_stencil;
    QColor oldColor;
    QColor newColor;
    ColorType type;
    QString m_textBoxName;
};

class KivioChangeRotationCommand : public KNamedCommand
{
public:
    KivioChangeRotationCommand( const QString &_name, KivioPage *_page, KivioStencil * _stencil, int _oldValue,  int _newValue);
    ~KivioChangeRotationCommand();

    virtual void execute();
    virtual void unexecute();

protected:
    KivioPage *m_page;
    KivioStencil *m_stencil;
    int oldValue;
    int newValue;
};

class KivioChangeLineStyleCommand : public KNamedCommand
{
public:
    KivioChangeLineStyleCommand( const QString &_name, KivioPage *_page, KivioStencil * _stencil, int _oldValue,  int _newValue);
    ~KivioChangeLineStyleCommand();

    virtual void execute();
    virtual void unexecute();

protected:
    KivioPage *m_page;
    KivioStencil *m_stencil;
    int oldValue;
    int newValue;
};


class KivioChangeLineWidthCommand : public KNamedCommand
{
public:
    KivioChangeLineWidthCommand( const QString &_name, KivioPage *_page, KivioStencil * _stencil, double _oldValue,  double _newValue);
    ~KivioChangeLineWidthCommand();

    virtual void execute();
    virtual void unexecute();

protected:
    KivioPage *m_page;
    KivioStencil *m_stencil;
    double oldValue;
    double newValue;
};

class KivioChangeBeginEndArrowCommand : public KNamedCommand
{
public:
    KivioChangeBeginEndArrowCommand( const QString &_name, KivioPage *_page, KivioStencil * _stencil, int _oldArrow,  int _newArrow, bool _beginArrow);

    ~KivioChangeBeginEndArrowCommand();

    virtual void execute();
    virtual void unexecute();

protected:
    KivioPage *m_page;
    KivioStencil *m_stencil;
    int oldArrow;
    int newArrow;
    bool beginArrow;
};

class KivioChangeBeginEndSizeArrowCommand : public KNamedCommand
{
public:
    KivioChangeBeginEndSizeArrowCommand( const QString &_name, KivioPage *_page, KivioStencil * _stencil, float _oldH,float _oldW, float _newH,float _newW, bool _beginArrow);

    ~KivioChangeBeginEndSizeArrowCommand();

    virtual void execute();
    virtual void unexecute();

protected:
    KivioPage *m_page;
    KivioStencil *m_stencil;
    float oldWidth;
    float oldLength;
    float newWidth;
    float newLength;

    bool beginArrow;
};

class KivioChangeStencilProtectCommand : public KNamedCommand
{
public:
    enum KVP_TYPE {KV_POSX, KV_POSY,KV_WIDTH, KV_HEIGHT, KV_ASPECT, KV_DELETE};
    KivioChangeStencilProtectCommand( const QString &_name, KivioPage *_page, KivioStencil * _stencil, bool _state, KVP_TYPE _type);

    ~KivioChangeStencilProtectCommand(){};

    virtual void execute();
    virtual void unexecute();

protected:
    void changeValue( bool b );
    KivioPage *m_page;
    KivioStencil *m_stencil;
    KVP_TYPE type;
    bool m_bValue;
};

class KivioAddConnectorTargetCommand : public KNamedCommand
{
  public:
    KivioAddConnectorTargetCommand(const QString& name, KivioPage* page,
                                   KivioStencil* stencil, const KoPoint& targetPoint);

    virtual void execute();
    virtual void unexecute();

  protected:
    KivioPage* m_page;
    KivioStencil* m_stencil;
    KoPoint m_targetPoint;
};

class KivioCustomDragCommand : public KNamedCommand
{
  public:
    KivioCustomDragCommand(const QString& name, KivioPage* page, KivioStencil* stencil,
                           int customID, const KoPoint& originalPoint, const KoPoint& newPoint);

    virtual void execute();
    virtual void unexecute();

  protected:
    KivioPage* m_page;
    KivioStencil* m_stencil;

    int m_customID;
    KoPoint m_originalPoint;
    KoPoint m_newPoint;
};

class KivioGroupCommand : public KNamedCommand
{
  public:
    KivioGroupCommand(const QString& name, KivioPage* page, KivioLayer* layer, KivioGroupStencil* group);

    virtual void execute();
    virtual void unexecute();

  protected:
    KivioPage* m_page;
    KivioLayer* m_layer;

    KivioGroupStencil* m_groupStencil;
};

class KivioUnGroupCommand : public KivioGroupCommand
{
  public:
    KivioUnGroupCommand(const QString& name, KivioPage* page, KivioLayer* layer, KivioGroupStencil* group)
      : KivioGroupCommand(name, page, layer, group)
    {}

    virtual void execute() { KivioGroupCommand::unexecute(); }
    virtual void unexecute() { KivioGroupCommand::execute(); }
};

#endif

