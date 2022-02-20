/*************************************************************************
 *
 * Copyright (c) 2008-2014 Kohei Yoshida
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 ************************************************************************/

#ifndef __MDDS_NODE_HXX__
#define __MDDS_NODE_HXX__

#include <iostream>
#include <vector>
#include <cassert>

#include <boost/intrusive_ptr.hpp>

namespace mdds { namespace __st {

#ifdef MDDS_DEBUG_NODE_BASE
size_t node_instance_count = 0;
#endif

struct node_base
{
    node_base* parent; /// parent nonleaf_node
    bool is_leaf;

    node_base(bool _is_leaf) : parent(nullptr), is_leaf(_is_leaf) {}
    node_base(const node_base& r) : parent(nullptr), is_leaf(r.is_leaf) {}
};

template<typename T>
struct nonleaf_node : public node_base
{
    typedef typename T::nonleaf_value_type nonleaf_value_type;
    typedef typename T::fill_nonleaf_value_handler fill_nonleaf_value_handler;
    typedef typename T::init_handler init_handler;
    typedef typename T::dispose_handler dispose_handler;
#ifdef MDDS_UNIT_TEST
    typedef typename T::to_string_handler to_string_handler;
#endif
    nonleaf_value_type value_nonleaf;

    node_base* left;   /// left child nonleaf_node
    node_base* right;  /// right child nonleaf_node

private:
    fill_nonleaf_value_handler  _hdl_fill_nonleaf;
    init_handler                _hdl_init;
    dispose_handler             _hdl_dispose;
#ifdef MDDS_UNIT_TEST
    to_string_handler           _hdl_to_string;
#endif

public:
    nonleaf_node() :
        node_base(false),
        left(nullptr),
        right(nullptr)
    {
        _hdl_init(*this);
    }

    /**
     * When copying nonleaf_node, only the stored values should be copied.
     * Connections to the parent, left and right nodes must not be copied.
     */
    nonleaf_node(const nonleaf_node& r) :
        node_base(r),
        left(nullptr),
        right(nullptr)
    {
        value_nonleaf = r.value_nonleaf;
    }

    /**
     * Like the copy constructor, only the stored values should be copied.
     */
    nonleaf_node& operator=(const nonleaf_node& r)
    {
        if (this == &r)
            // assignment to self.
            return *this;

        value_nonleaf = r.value_nonleaf;
        return *this;
    }

    ~nonleaf_node()
    {
        dispose();
    }

    void dispose()
    {
        _hdl_dispose(*this);
    }

    bool equals(const nonleaf_node& r) const
    {
        return value_nonleaf == r.value_nonleaf;
    }

    void fill_nonleaf_value(const node_base* left_node, const node_base* right_node)
    {
        _hdl_fill_nonleaf(*this, left_node, right_node);
    }

#ifdef MDDS_UNIT_TEST
    void dump_value() const
    {
        ::std::cout << _hdl_to_string(*this);
    }

    ::std::string to_string() const
    {
        return _hdl_to_string(*this);
    }
#endif
};

template<typename T>
struct node : public node_base
{
    typedef ::boost::intrusive_ptr<node>  node_ptr;

    typedef typename T::leaf_value_type leaf_value_type;
    typedef typename T::init_handler init_handler;
    typedef typename T::dispose_handler dispose_handler;
#ifdef MDDS_UNIT_TEST
    typedef typename T::to_string_handler to_string_handler;
#endif

    static size_t get_instance_count()
    {
#ifdef MDDS_DEBUG_NODE_BASE
        return node_instance_count;
#else
        return 0;
#endif
    }

    leaf_value_type     value_leaf;

    node_ptr    prev;   /// previous sibling leaf node.
    node_ptr    next;  /// next sibling leaf node.

    size_t      refcount;
private:
    init_handler                _hdl_init;
    dispose_handler             _hdl_dispose;
#ifdef MDDS_UNIT_TEST
    to_string_handler           _hdl_to_string;
#endif

public:
    node() : node_base(true), refcount(0)
    {
#ifdef MDDS_DEBUG_NODE_BASE
        ++node_instance_count;
#endif
        _hdl_init(*this);
    }

    /**
     * When copying node, only the stored values should be copied.
     * Connections to the parent, left and right nodes must not be copied.
     */
    node(const node& r) : node_base(r), refcount(0)
    {
#ifdef MDDS_DEBUG_NODE_BASE
        ++node_instance_count;
#endif
        value_leaf = r.value_leaf;
    }

    /**
     * Like the copy constructor, only the stored values should be copied.
     */
    node& operator=(const node& r)
    {
        if (this == &r)
            // assignment to self.
            return *this;

        value_leaf = r.value_leaf;
        return *this;
    }

    ~node()
    {
#ifdef MDDS_DEBUG_NODE_BASE
        --node_instance_count;
#endif
        dispose();
    }

    void dispose()
    {
        _hdl_dispose(*this);
    }

    bool equals(const node& r) const
    {
        return value_leaf == r.value_leaf;
    }

#ifdef MDDS_UNIT_TEST
    void dump_value() const
    {
        ::std::cout << _hdl_to_string(*this);
    }

    ::std::string to_string() const
    {
        return _hdl_to_string(*this);
    }
#endif
};

template<typename T>
inline void intrusive_ptr_add_ref(node<T>* p)
{
    ++p->refcount;
}

template<typename T>
inline void intrusive_ptr_release(node<T>* p)
{
    --p->refcount;
    if (!p->refcount)
        delete p;
}

template<typename T>
void link_nodes(typename node<T>::node_ptr& left, typename node<T>::node_ptr& right)
{
    left->next = right;
    right->prev = left;
}

template<typename T>
class tree_builder
{
public:
    typedef mdds::__st::node<T> leaf_node;
    typedef typename mdds::__st::node<T>::node_ptr leaf_node_ptr;
    typedef mdds::__st::nonleaf_node<T> nonleaf_node;
    typedef std::vector<nonleaf_node> nonleaf_node_pool_type;

    tree_builder(nonleaf_node_pool_type& pool) :
        m_pool(pool), m_pool_pos(pool.begin()), m_pool_pos_end(pool.end()) {}

    nonleaf_node* build(const leaf_node_ptr& left_leaf_node)
    {
        if (!left_leaf_node)
            // The left leaf node is empty.  Nothing to build.
            return nullptr;

        leaf_node_ptr node1 = left_leaf_node;

        std::vector<nonleaf_node*> node_list;
        while (true)
        {
            leaf_node_ptr node2 = node1->next;
            nonleaf_node* parent_node = make_parent_node(node1.get(), node2.get());
            node_list.push_back(parent_node);

            if (!node2 || !node2->next)
                // no more nodes.  Break out of the loop.
                break;

            node1 = node2->next;
        }

        return build_tree_non_leaf(node_list);
    }

private:

    nonleaf_node* make_parent_node(node_base* node1, node_base* node2)
    {
        assert(m_pool_pos != m_pool_pos_end);

        nonleaf_node* parent_node = &(*m_pool_pos);
        ++m_pool_pos;
        node1->parent = parent_node;
        parent_node->left = node1;
        if (node2)
        {
            node2->parent = parent_node;
            parent_node->right = node2;
        }

        parent_node->fill_nonleaf_value(node1, node2);
        return parent_node;
    }

    nonleaf_node* build_tree_non_leaf(const std::vector<nonleaf_node*>& node_list)
    {
        size_t node_count = node_list.size();
        if (node_count == 1)
        {
            return node_list.front();
        }
        else if (node_count == 0)
            return nullptr;

        std::vector<nonleaf_node*> new_node_list;
        nonleaf_node* node1 = nullptr;
        typename std::vector<nonleaf_node*>::const_iterator it = node_list.begin();
        typename std::vector<nonleaf_node*>::const_iterator it_end = node_list.end();
        for (bool even_itr = false; it != it_end; ++it, even_itr = !even_itr)
        {
            if (even_itr)
            {
                nonleaf_node* node2 = *it;
                nonleaf_node* parent_node = make_parent_node(node1, node2);
                new_node_list.push_back(parent_node);
                node1 = nullptr;
                node2 = nullptr;
            }
            else
                node1 = *it;
        }

        if (node1)
        {
            // Un-paired node still needs a parent...
            nonleaf_node* parent_node = make_parent_node(node1, nullptr);
            new_node_list.push_back(parent_node);
        }

        // Move up one level, and do the same procedure until the root node is reached.
        return build_tree_non_leaf(new_node_list);
    }

    nonleaf_node_pool_type& m_pool;
    typename nonleaf_node_pool_type::iterator m_pool_pos;
    typename nonleaf_node_pool_type::iterator m_pool_pos_end;
};


template<typename T>
void disconnect_all_nodes(node<T>* p)
{
    if (!p)
        return;

    p->prev.reset();
    p->next.reset();
    p->parent = nullptr;
}

template<typename T>
void disconnect_leaf_nodes(node<T>* left_node, node<T>* right_node)
{
    if (!left_node || !right_node)
        return;

    // Go through all leaf nodes, and disconnect their links.
    node<T>* cur_node = left_node;
    do
    {
        node<T>* next_node = cur_node->next.get();
        disconnect_all_nodes(cur_node);
        cur_node = next_node;
    }
    while (cur_node != right_node);

    disconnect_all_nodes(right_node);
}

template<typename T>
size_t count_leaf_nodes(const node<T>* left_end, const node<T>* right_end)
{
    size_t leaf_count = 1;
    const node<T>* p = left_end;
    const node<T>* p_end = right_end;
    for (; p != p_end; p = p->next.get(), ++leaf_count)
        ;

    return leaf_count;
}

inline size_t count_needed_nonleaf_nodes(size_t leaf_count)
{
    size_t nonleaf_count = 0;
    while (true)
    {
        if (leaf_count == 1)
            break;

        if ((leaf_count % 2) == 1)
            // Add one to make it an even number.
            ++leaf_count;

        leaf_count /= 2;
        nonleaf_count += leaf_count;
    }

    return nonleaf_count;
}

#ifdef MDDS_UNIT_TEST

template<typename _Leaf, typename _NonLeaf>
class tree_dumper
{
    typedef std::vector<const node_base*> node_list_type;

public:
    static size_t dump(const node_base* root_node)
    {
        if (!root_node)
            return 0;

        node_list_type node_list;
        node_list.push_back(root_node);
        return dump_layer(node_list, 0);
    }

private:
    static size_t dump_layer(const node_list_type& node_list, unsigned int level)
    {
        using ::std::cout;
        using ::std::endl;

        if (node_list.empty())
            return 0;

        size_t node_count = node_list.size();

        bool is_leaf = node_list.front()->is_leaf;
        cout << "level " << level << " (" << (is_leaf?"leaf":"non-leaf") << ")" << endl;

        node_list_type new_list;
        typename node_list_type::const_iterator it = node_list.begin(), it_end = node_list.end();
        for (; it != it_end; ++it)
        {
            const node_base* p = *it;
            if (!p)
            {
                cout << "(x) ";
                continue;
            }

            if (p->is_leaf)
                static_cast<const _Leaf*>(p)->dump_value();
            else
                static_cast<const _NonLeaf*>(p)->dump_value();

            if (p->is_leaf)
                continue;

            if (static_cast<const _NonLeaf*>(p)->left)
            {
                new_list.push_back(static_cast<const _NonLeaf*>(p)->left);
                if (static_cast<const _NonLeaf*>(p)->right)
                    new_list.push_back(static_cast<const _NonLeaf*>(p)->right);
            }
        }
        cout << endl;

        if (!new_list.empty())
            node_count += dump_layer(new_list, level+1);

        return node_count;
    }
};

#endif

}}

#endif
