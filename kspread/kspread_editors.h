#ifndef __kspread_editors_h__
#define __kspread_editors_h__

#include <klineedit.h>
#include <kcompletion.h>

class KSpreadCell;
class KSpreadCanvas;

class KFormulaEdit;

class QWidget;

class KSpreadCellEditor : public QWidget
{
    Q_OBJECT
public:
    KSpreadCellEditor( KSpreadCell*, KSpreadCanvas* _parent = 0, const char* _name = 0 );
    ~KSpreadCellEditor();

    KSpreadCell* cell() { return m_pCell; }

    virtual void handleKeyPressEvent( QKeyEvent* _ev ) = 0;
    virtual QString text() const = 0;
    virtual void setText(QString text) = 0;
    virtual int cursorPosition() const = 0;
    virtual void setCursorPosition(int pos) = 0;
    // virtual void setFocus() = 0;
    virtual void insertFormulaChar(int c) = 0;

    KSpreadCanvas* canvas() { return m_pCanvas; }

private:
    KSpreadCell* m_pCell;
    KSpreadCanvas* m_pCanvas;
};

class KSpreadTextEditor : public KSpreadCellEditor
{
    Q_OBJECT
public:
    KSpreadTextEditor( KSpreadCell*, KSpreadCanvas* _parent = 0, const char* _name = 0 );
    ~KSpreadTextEditor();

    virtual void handleKeyPressEvent( QKeyEvent* _ev );
    virtual QString text() const;
    virtual void setText(QString text);
    virtual int cursorPosition() const;
    virtual void setCursorPosition(int pos);
    // virtual void setFocus();
    virtual void insertFormulaChar(int c);

    void checkChoose();
    void blockCheckChoose( bool b ) { m_blockCheck = b; }

private slots:
    void slotTextChanged( const QString& text );
    void  slotCompletionModeChanged(KGlobalSettings::Completion _completion);
protected:
    void resizeEvent( QResizeEvent* );
    /**
     * Steals some key events from the QLineEdit and sends
     * it to the @ref KSpreadCancvas ( its parent ) instead.
     */
    bool eventFilter( QObject* o, QEvent* e );

private:
    //QLineEdit* m_pEdit;
    KLineEdit* m_pEdit;
    bool m_blockCheck;
};

class KSpreadFormulaEditor : public KSpreadCellEditor
{
    Q_OBJECT
public:
    KSpreadFormulaEditor( KSpreadCell*, KSpreadCanvas* _parent = 0, const char* _name = 0 );
    ~KSpreadFormulaEditor();

    virtual void handleKeyPressEvent( QKeyEvent* _ev );
    virtual QString text() const;
    virtual void setText(QString text);
    virtual int cursorPosition() const;
    virtual void setCursorPosition(int pos);
    // virtual void setFocus();
    virtual void insertFormulaChar(int c);

protected slots:
    void slotSizeHint( QSize );

protected:
    void resizeEvent( QResizeEvent* );
    /**
     * Steals some key events from the KFormulaEdit and sends
     * it to the @ref KSpreadCancvas ( its parent ) instead.
     */
    bool eventFilter( QObject* o, QEvent* e );

private:
    KFormulaEdit* m_pEdit;
};

#endif
