

#ifndef __SOAPUTIL_H__
#define __SOAPUTIL_H__

#include <stdlib.h>

template<typename T>
inline T*
sp_alloc(size_t n)
{
	return (T*)malloc(n * sizeof(T));
}

template<typename T>
inline void
sp_free(T*& ptr)
{
	if (ptr)
		free((void *)ptr);
	ptr = 0;
}

inline char *
sp_strcpy(char *dest, const char *src)
{
	char *work = dest;
	if (src && dest)
	{
		while ((*work++ = *src++))
			;
	}
	return dest;
}

inline char *
sp_strncpy(char *dest, const char *src, size_t n)
{
	char *work = dest;
	if (src && dest)
	{
		while (n-- && (*work++ = *src++))
			;
	}
	return dest;
}

inline size_t
sp_strlen(const char *str)
{
	size_t ret = 0;
	if (str)
	{
		while (*str++)
			++ret;
	}
	return ret;
}

inline int
sp_toupper(int c)
{
	if (c >= 'a' && c <= 'z')
		c += 'A' - 'a';
	return c;
}

inline int
sp_strcmp(const char *a, const char *b)
{
	int ret = 0;
	if (a && b)
	{
		while (*a && *b && *a == *b)
			++a, ++b;
		ret = *a - *b;
	}
	else if (a)
		ret = 1;
	else if (b)
		ret = -1;
	return ret;
}

inline int
sp_strncmp(const char *a, const char *b, unsigned int n)
{
	int ret = 0;
	if (a && b)
	{
		while (n > 0 && *a && *b && *a == *b)
			++a, ++b, --n;
		if (n == 0)
			ret = 0;
		else
			ret = *a - *b;
	}
	else if (a)
		ret = 1;
	else if (b)
		ret = -1;
	return ret;
}

inline int
sp_strcasecmp(const char *a, const char *b)
{
	int ret = 0;
	if (a && b)
	{
		int ua = sp_toupper(*a++);
		int ub = sp_toupper(*b++);
		while (ua && ub && ua == ub)
		{
			ua = sp_toupper(*a++);
			ub = sp_toupper(*b++);
		}
		ret = ua - ub;
	}
	else if (a)
		ret = 1;
	else if (b)
		ret = -1;
	return ret;
}

inline char*
sp_strstr(const char *haystack, const char *needle)
{
	if (haystack && needle)
	{
		if (*needle == 0)
			return (char *)haystack;

		// step along the main string
		while (*haystack)
		{
			const char *nn = needle;
			const char *hh = haystack;
			// see if substring characters match
			while (*nn++ == *hh++)
			{
				if (*nn == 0)
					// we got all the way to the end of the substring
					// so we must've won
					return (char *)haystack;
			}
			++haystack;
		}
	}
	return 0;
}

inline bool
sp_isspace(char c)
{
	switch (c)
	{
	case '\r':
	case '\n':
	case '\t':
	case ' ':
		return true;
	}
	return false;
}

inline char*
sp_strchr(const char *s, char c)
{
	if (s)
	{
		do
		{
			if (*s == c)
				return (char*)s;
		}
		while (*s++);
	}
	return 0;
}

inline void *
sp_memset(void *s, int c, size_t n)
{
	unsigned char *ptr = (unsigned char *)s;
	if (ptr)
	{
		while (n--)
			*ptr++ = c;
	}
	return s;
}

inline unsigned int
sp_hashcode(const char *key)
{
	unsigned char* ptr = (unsigned char*)key;
	unsigned int h = 0;
	if (ptr)
	{
		unsigned int c;
		while ((c = *ptr++))
			h = 31 * h + c;
	}
	return h;
}

inline unsigned int
sp_hashcodecase(const char *key)
{
	unsigned char* ptr = (unsigned char*)key;
	unsigned int h = 0;
	if (ptr)
	{
		unsigned int c;
		while ((c = sp_toupper(*ptr++)))
			h = 31 * h + c;
	}
	return h;
}

template <class T>
inline T
sp_minimum(const T& a, const T& b)
{
	return (a < b) ? a : b;
}

template <class T>
inline T
sp_maximum(const T& a, const T& b)
{
	return (a < b) ? b : a;
}

template <typename T>
inline bool
ConvertUCStoUTF16(int c, T& utf16)
{
	if (c > 0xFFFF && c < 0x00110000)
	{ 
		*utf16++ = (0xD7C0 + (c >> 10));
		*utf16++ = (0xDC00 | c & 0x3FF);
	}
	else if (c >= 0)
	{
		*utf16++ = c;
	}
	else
	{
		return false;
	}
	return true;
}

template <typename T>
inline bool
ConvertUTF16toUCS(T& utf16, int& c)
{
	int x = utf16[0];

	if (x >= 0 && x <= 0xD7FF || x >= 0xE000 && x <= 0xFFFF)
	{
		c = x;
		++utf16;
	}
	else
	{
		int y = utf16[1];
		if (x >= 0xD800 && x <= 0xDBFF && y >= 0xDC00 && y <= 0xDFFF)
		{
			c = (x - 0xD800) * 0x400 + y - 0xDC00;
			++utf16; ++utf16;
		}
		else
		{
			return false;
		}
	}
	return true;
}

//
// Functions to convert between
// UTF8 and UCS character encoding.
//
// T is an iterator.  (or pointer)
template <typename T>
inline bool
ConvertUCStoUTF8(int c, T& utf8)
{
	if (c <= 0x7F)
	{
		/* Leave ASCII encoded */
		*utf8++ = (c);
	}
	else if (c <= 0x07FF)
	{
		/* 110xxxxx 10xxxxxx */
		*utf8++ = (0xC0 | (c >> 6));
		*utf8++ = (0x80 | (c & 0x3F));
	}
	else if (c <= 0xFFFF)
	{
		/* 1110xxxx + 2 */
		*utf8++ = (0xE0 | (c >> 12));
		*utf8++ = (0x80 | ((c >> 6) & 0x3F));
		*utf8++ = (0x80 | (c & 0x3F));
	}
	else if (c <= 0x1FFFFF)
	{
		/* 11110xxx + 3 */
		*utf8++ = (0xF0 | (c >> 18));
		*utf8++ = (0x80 | ((c >> 12) & 0x3F));
		*utf8++ = (0x80 | ((c >> 6) & 0x3F));
		*utf8++ = (0x80 | (c & 0x3F));
	}
	else if (c <= 0x3FFFFFF)
	{
		/* 111110xx + 4 */
		*utf8++ = (0xF8 | (c >> 24));
		*utf8++ = (0x80 | ((c >> 18) & 0x3F));
		*utf8++ = (0x80 | ((c >> 12) & 0x3F));
		*utf8++ = (0x80 | ((c >> 6) & 0x3F));
		*utf8++ = (0x80 | (c & 0x3F));
	}
	else if (c <= 0x7FFFFFFF)
	{
		/* 1111110x + 5 */
		*utf8++ = (0xFC | (c >> 30));
		*utf8++ = (0x80 | ((c >> 24) & 0x3F));
		*utf8++ = (0x80 | ((c >> 18) & 0x3F));
		*utf8++ = (0x80 | ((c >> 12) & 0x3F));
		*utf8++ = (0x80 | ((c >> 6) & 0x3F));
		*utf8++ = (0x80 | (c & 0x3F));
	}
	else
	{
		/* Not a valid character... */
		return false;
	}

	return true;
}

template <typename T>
inline bool
ConvertUTF8toUCS(T& utf8, int& c)
{
	int b = *utf8++;
	if (b <= 0x7F)
	{
		c = b;
	}
	else if ((b & 0xE0) == 0xC0)
	{	/* 110xxxxx 10xxxxxx */
		c = (b & 0x1F) << 6;
		b = *utf8++;
		c |= b & 0x3F;
	}
	else if ((b & 0xF0) == 0xE0)
	{	/* 1110xxxx + 2 */
		c = (b & 0x0F) << 12;
		b = *utf8++;
		c |= (b & 0x3F) << 6;
		b = *utf8++;
		c |= b & 0x3F;
	}
	else if ((b & 0xF1) == 0xF0)
	{	/* 11110xxx + 3 */
		c = (b & 0x0F) << 18;
		b = *utf8++;
		c |= (b & 0x3F) << 12;
		b = *utf8++;
		c |= (b & 0x3F) << 6;
		b = *utf8++;
		c |= b & 0x3F;
	}
	else if ((b & 0xFD) == 0xF8)
	{
		/* 111110xx + 4 */
		c = (b & 0x0F) << 24;
		b = *utf8++;
		c |= (b & 0x0F) << 18;
		b = *utf8++;
		c |= (b & 0x3F) << 12;
		b = *utf8++;
		c |= (b & 0x3F) << 6;
		b = *utf8++;
		c |= b & 0x3F;
	}
	else if ((b & 0xFE) == 0xFC)
	{	
		/* 1111110x + 5 */
		c = (b & 0x0F) << 30;
		b = *utf8++;
		c |= (b & 0x0F) << 24;
		b = *utf8++;
		c |= (b & 0x0F) << 18;
		b = *utf8++;
		c |= (b & 0x3F) << 12;
		b = *utf8++;
		c |= (b & 0x3F) << 6;
		b = *utf8++;
		c |= b & 0x3F;
	}
	else
	{
		/* Error */
		return false;
	}
  	return true;
}

#endif // __SOAPUTIL_H__

