#ifndef FAKE_KNUMVALIDATOR_H
#define FAKE_KNUMVALIDATOR_H

#include <QString>
#include <QValidator>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QWidget>

class KIntValidator : public QIntValidator
{
public:
    KIntValidator ( QWidget * parent, int base = 10 ) : QIntValidator(parent) {}
    KIntValidator ( int bottom, int top, QWidget * parent, int base = 10 ) : QIntValidator(parent) {}
#if 0
    virtual State validate ( QString &, int & ) const;
    virtual void fixup ( QString & ) const;
    virtual void setRange ( int bottom, int top );
    virtual void setBase ( int base );
    virtual int bottom () const;
    virtual int top () const;
    virtual int base () const;
#endif
};

class KFloatValidator : public QDoubleValidator
{
public:
    KFloatValidator ( QWidget * parent ) : QDoubleValidator(parent) {}
    KFloatValidator ( double bottom, double top, QWidget * parent ) : QDoubleValidator(bottom, top, 1000, parent) {}
    KFloatValidator ( double bottom, double top, bool localeAware, QWidget * parent ) : QDoubleValidator(bottom, top, 1000, parent) {}
#if 0
    virtual State validate ( QString &, int & ) const;
    virtual void fixup ( QString & ) const;
    virtual void setRange ( double bottom, double top );
    virtual double bottom () const;
    virtual double top () const;
#endif
    void setAcceptLocalizedNumbers(bool b) {}
    bool acceptLocalizedNumbers() const { return false; }
};

class KDoubleValidator : public QDoubleValidator
{
public:
    KDoubleValidator( QObject * parent ) : QDoubleValidator(parent) {}
    KDoubleValidator( double bottom, double top, int decimals, QObject * parent ) : QDoubleValidator(bottom, top, decimals, parent) {}
#if 0
    virtual QValidator::State validate( QString & input, int & pos ) const;
#endif
    bool acceptLocalizedNumbers() const { return false; }
    void setAcceptLocalizedNumbers( bool accept ) {}
};

#endif
 
