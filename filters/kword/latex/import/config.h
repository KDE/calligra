
#ifndef __LATEXFILTER_CONFIG_H__
#define __LATEXFILTER_CONFIG_H__


enum _ELatexType
{
	TYPE_UNKNOWN,
	TYPE_DOC,
	TYPE_EMBEDED
};

enum _ELatexEncoding
{
	ENC_UNKNOWN,
	ENC_LATIN1,
	ENC_UNICODE
};

typedef enum _ELatexType ELatexType;
typedef enum _ELatexEncoding ELatexEncoding;

class Config
{
	public:
		Config();

		~Config();

		ELatexType getType() const { return _type; }

		ELatexEncoding getEncoding() const { return _encoding; }

		void setType(ELatexType type) { _type = type; }
		
		void setEncoding(ELatexEncoding encoding) { _encoding = encoding; }
		
	private:
		static ELatexType _type;
		static ELatexEncoding _encoding;

};

#endif /* __LATEXFILTER_CONFIG_H__ */
