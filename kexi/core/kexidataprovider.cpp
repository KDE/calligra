#include "kexidataprovider.h"

const char *KexiDataProvider::Parameter::typeNames[]={"UNKNOWN","TEXT","FLOAT","INTEGER","DATE",
			"TIME","DATETIME"};

const char *KexiDataProvider::Parameter::typeDescription[]={I18N_NOOP("unknown"),I18N_NOOP("Text"),
			I18N_NOOP("Float (eg 1.3)"),
                        I18N_NOOP("Integer (eg 10)"),I18N_NOOP("Date (eg 01-Apr-02)"),
                        I18N_NOOP("Time (eg 13:01)"),I18N_NOOP("DateTime (eg 01-Apr-02 13:01)")};

const int KexiDataProvider::Parameter::maxType=6;

KexiDataProvider::KexiDataProvider(){;}
KexiDataProvider::~KexiDataProvider(){;}

