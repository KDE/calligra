/* 
 * EasySoap++ - A C++ library for SOAP (Simple Object Access Protocol)
 * Copyright (C) 2001 David Crowley; SciTegic, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#ifndef __SOAPHASHMAP_H__
#define __SOAPHASHMAP_H__

#include "SOAPUtil.h"

#ifdef _MSC_VER
#pragma warning(disable: 4284)
#endif // _MSC_VER

// pre-decalure some functors
template<typename T>
struct SOAPHashCodeFunctor;

template<typename T>
struct SOAPEqualsFunctor
{
	bool operator()(const T& a, const T& b) const
	{
		return a == b;
	}
};

template <typename K, typename I,
	typename H = SOAPHashCodeFunctor<K>,
	typename E = SOAPEqualsFunctor<K> >
class EASYSOAP_EXPORT SOAPHashMap
{
private:
	H hashcode;
	E equals;

	// structure for keeping a linked-list of elements
	struct HashElement {
		HashElement(size_t hash, HashElement* next, const K& key)
			: m_hash(hash), m_next(next), m_key(key) {}

		HashElement(size_t hash, HashElement* next, const K& key, const I& item)
			: m_hash(hash), m_next(next), m_key(key), m_item(item) {}

		size_t	m_hash;
		HashElement *m_next;
		K		m_key;
		I		m_item;
	};

	// structure for keeping memory pools
	struct HashElementPool {
		HashElement *m_elements;
		size_t		m_size;
	};

	// our Iterator class
	class ForwardHashMapIterator
	{
	private:
		HashElement			*m_he;
		const SOAPHashMap	*m_map;
		size_t				m_index;
		
		friend class SOAPHashMap<K,I,H,E>;

		// private constuctor that can only be called by SOAPHashMap
		ForwardHashMapIterator(const SOAPHashMap *map, HashElement *he, size_t index)
			: m_he(he), m_map(map), m_index(index)
		{
			if (m_map)
			{
				// Find first bucket with an element
				while (!m_he && m_index != m_map->m_buckets)
						m_he = m_map->m_table[m_index++];
			}
		}

	public:
		// default constructor
		ForwardHashMapIterator()
			: m_he(0), m_map(0), m_index(0)
		{
		}

		//copy constructor
		ForwardHashMapIterator(const ForwardHashMapIterator& r)
			: m_he(r.m_he), m_map(r.m_map), m_index(r.m_index)
		{
		}

		// assignment operator
		ForwardHashMapIterator& operator=(const ForwardHashMapIterator& r)
		{
			m_he = r.m_he;
			m_map = r.m_map;
			m_index = r.m_index;
			return *this;
		}

		// equals operator
		bool operator==(const ForwardHashMapIterator& r) const
		{
			// make sure we're pointing to the exact same element
			return m_he == r.m_he;
		}

		// not equals operator
		bool operator!=(const ForwardHashMapIterator& r) const
		{
			// can't be pointing to the same element...
			return m_he != r.m_he;
		}

		// Move to next element
		ForwardHashMapIterator& Next()
		{
			if (m_he)
			{
				m_he = m_he->m_next;
				while (!m_he && m_index != m_map->m_buckets)
						m_he = m_map->m_table[m_index++];

			}
			return *this;
		}

		// prefix-increment
		ForwardHashMapIterator& operator++()
		{
			return Next();
		}

		// postfix-increment
		ForwardHashMapIterator operator++(int)
		{
			// copy our current position
			ForwardHashMapIterator ret(*this);
			// move to the next item
			Next();
			// return the old position
			return ret;
		}

		// some boolean operators
		operator bool()
		{
			return m_he != 0;
		}

		bool operator!()
		{
			return m_he == 0;
		}

		// access the hash key.  can't modify it!
		const K& Key() const
		{
			return m_he->m_key;
		}

		// access the data item.
		I& operator*()
		{
			return m_he->m_item;
		}

		const I& operator*() const
		{
			return m_he->m_item;
		}

		I* operator->()
		{
			return &m_he->m_item;
		}

		const I* operator->() const
		{
			return &m_he->m_item;
		}

		// maybe get rid of these since we can use *
		const I& Item() const
		{
			return m_he->m_item;
		}

		I& Item()
		{
			return m_he->m_item;
		}
	};

public:
	typedef ForwardHashMapIterator Iterator;

	~SOAPHashMap()
	{
		Clear();
		sp_free(m_table);
	}
	
	SOAPHashMap(size_t size = 31, float fillfactor = 0.75) :
		m_table(0), m_buckets(0), m_numItems(0),
		m_fillfactor(fillfactor), m_resizeThreshold(0)
	{
		Resize(size); // this sets m_resizeThreshold
	}

	template<typename A, typename B, typename C, typename D>
	SOAPHashMap(const SOAPHashMap<A,B,C,D>& r) :
		m_table(0), m_buckets(0), m_numItems(0),
		m_fillfactor(r.GetFillFactor()), m_resizeThreshold(0)
	{
		Resize(r.GetNumBuckets()); // this sets m_resizeThreshold
		*this = r;
	}

	SOAPHashMap(const SOAPHashMap& r) :
		m_table(0), m_buckets(0), m_numItems(0),
		m_fillfactor(r.GetFillFactor()), m_resizeThreshold(0)
	{
		Resize(r.GetNumBuckets()); // this sets m_resizeThreshold
		*this = r;
	}

	template<typename A, typename B, typename C, typename D>
	SOAPHashMap& operator=(const SOAPHashMap<A,B,C,D>& r)
	{
		if ((void *)this != (void *)&r)
		{
			Clear();
			Resize(r.GetNumBuckets());
			SOAPHashMap<A,B,C,D>::Iterator end = r.End();
			for (SOAPHashMap<A,B,C,D>::Iterator it = r.Begin(); it != end; ++it)
				Add(it.Key(), it.Item());
		}
		return *this;
	}

	SOAPHashMap& operator=(const SOAPHashMap& r)
	{
		if (this != &r)
		{
			Clear();
			Resize(r.GetNumBuckets());
			Iterator end = r.End();
			for (Iterator it = r.Begin(); it != end; ++it)
				Add(it.Key(), it.Item());
		}
		return *this;
	}

	Iterator Begin() const
	{
		return Iterator(this, 0, 0);
	}

	Iterator End() const
	{
		return Iterator(0, 0, 0);
	}


	// find & set or add
	I& Add(const K& key, const I& item)
	{
		// see if we can find it
		size_t hash = hashcode(key);
		Iterator found = Find(key, hash);
		if (found)
			return *found = item;
		return Put(hash, key, item);
	}

	// find or add
	I& operator[](const K& key)
	{
		// see if we can find it
		size_t hash = hashcode(key);
		Iterator found = Find(key, hash);
		if (found)
			return *found;
		return Put(hash, key);
	}


	// returns true if we found the key and removed it.
	bool Remove(const K& key)
	{
		if (m_buckets > 0)
		{
			size_t hash = hashcode(key);
			// we use ** here so we can treat the first element the
			// same was as the other elements in the linked list
			HashElement **he = &m_table[hash % m_buckets];
			while (*he)
			{
				if ((*he)->m_hash == hash && equals((*he)->m_key, key))
				{
					HashElement *temp = (*he)->m_next;
					PutBackHashElement(*he);
					*he = temp;

					--m_numItems;
					return true;
				}
				he = &((*he)->m_next);
			}
		}
		return false;
	}

	// clear all elements.
	void Clear()
	{
		if (m_table)
		{
			for (size_t i = 0; i < m_buckets; ++i)
			{
				HashElement *he = m_table[i];
				while (he)
				{
					HashElement *next = he->m_next;
					PutBackHashElement(he);
					he = next;
				}
				m_table[i] = 0;
			}
			m_numItems = 0;
		}
	}

	size_t Size() const
	{
		return m_numItems;
	}

	// find the item associated with the given key.
	// returns null if we can't find the key.
	template<typename X>
	Iterator Find(const X& key) const
	{
		size_t hash = hashcode(key);
		return Find(key, hash);
	}

	size_t GetNumBuckets() const {return m_buckets;}
	float GetFillFactor() const {return m_fillfactor;}

private:
	// Does the actual find.  We pass in the hashcode so we don't have
	// to recompute it since it is used in other places.
	template<typename X>
	Iterator Find(const X& key, size_t hash) const
	{
		if (m_buckets > 0)
		{
			size_t index = hash % m_buckets;
			HashElement *he = m_table[index];
			while (he)
			{
				if (he->m_hash == hash && equals(he->m_key, key))
					return Iterator(this, he, ++index);
				he = he->m_next;
			}
		}
		return End();
	}

	// resize only if it makes the table bigger
	void Resize(size_t newsize)
	{
		if (newsize <= m_buckets)
			return;

		// get a new array to copy the elements into
		HashElement **newtable =
			sp_alloc<HashElement*>(newsize);
		size_t i = 0;

		// clear out the new array;
		for (i = 0; i < newsize; ++i)
			newtable[i] = 0;

		// move elements into the new array.
		for (i = 0; i < m_buckets; ++i)
		{
			HashElement *he = m_table[i];
			while (he)
			{
				HashElement *next = he->m_next;
				size_t newindex = he->m_hash % newsize;
				he->m_next = newtable[newindex];
				newtable[newindex] = he;
				he = next;
			}
		}

		// blow away the old array and set the pointer
		// to the new array
		sp_free(m_table);
		m_table = newtable;
		m_buckets = newsize;
		// precompute the threshold so we don't have to
		// recompute for every Put()
		m_resizeThreshold = (size_t) (m_buckets * m_fillfactor);
	}


	void PutBackHashElement(HashElement *he)
	{
		delete he;
	}

	HashElement *GetNextHashElement(
			size_t hash,
			HashElement* next,
			const K& key,
			const I& item)
	{
		return new HashElement(hash, next, key, item);
	}

	HashElement *GetNextHashElement(
			size_t hash,
			HashElement* next,
			const K& key)
	{
		return new HashElement(hash, next, key);
	}

	// This method actually puts an object into the hashtable.
	I& Put(size_t hash, const K& key, const I& item)
	{
		// check for resize
		if (m_numItems >= m_resizeThreshold)
			Resize(m_buckets * 2 + 1);

		size_t index = hash % m_buckets;
		HashElement *he = GetNextHashElement(hash, m_table[index], key, item);
		m_table[index] = he;
		++m_numItems;

		return he->m_item;
	}

	// This method actually puts an object into the hashtable.
	I& Put(size_t hash, const K& key)
	{
		// check for resize
		if (m_numItems >= m_resizeThreshold)
			Resize(m_buckets * 2 + 1);

		size_t index = hash % m_buckets;
		HashElement *he = GetNextHashElement(hash, m_table[index], key);
		m_table[index] = he;
		++m_numItems;

		return he->m_item;
	}

	friend class ForwardHashMapIterator;

	HashElement **m_table;
	size_t	m_buckets;
	size_t	m_numItems;
	float	m_fillfactor;
	size_t	m_resizeThreshold;
};


////////////////////////////////
///  Make it easier to use no case
////////////////////////////////
template<typename T>
struct SOAPHashCodeFunctorNoCase;

template<typename T>
struct SOAPEqualsFunctorNoCase;


template<typename K, typename I>
class SOAPHashMapNoCase : public SOAPHashMap<K, I,
				SOAPHashCodeFunctorNoCase<K>,
				SOAPEqualsFunctorNoCase<K> >
{
private:
	typedef SOAPHashMap<K, I,
				SOAPHashCodeFunctorNoCase<K>,
				SOAPEqualsFunctorNoCase<K> > super;

public:
	SOAPHashMapNoCase(size_t size = 31, float fillfactor = 0.75)
		: super(size, fillfactor)
	{
	}
};


#endif // __SOAPHASHMAP_H__

