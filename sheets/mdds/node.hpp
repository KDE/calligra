/*************************************************************************
 *
 * Copyright (c) 2008-2010 Kohei Yoshida
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
#include <list>
#include <cassert>

#include <boost/intrusive_ptr.hpp>

namespace mdds {

#ifdef DEBUG_NODE_BASE
size_t node_instance_count = 0;
#endif

template<typename _NodePtr, typename _NodeType>
struct node_base
{
    static size_t get_instance_count()
    {
#ifdef DEBUG_NODE_BASE
        return node_instance_count;
#else
        return 0;
#endif
    }
    size_t          refcount;

    _NodePtr   parent; /// parent node
    _NodePtr   left;   /// left child node or previous sibling if it's a leaf node.
    _NodePtr   right;  /// right child node or next sibling if it's aleaf node.
    bool            is_leaf;

    node_base(bool _is_leaf) :
        refcount(0),
        is_leaf(_is_leaf)
    {
#ifdef DEBUG_NODE_BASE
        ++node_instance_count;
#endif
    }

    /** 
     * When copying node, only the stored values should be copied. 
     * Connections to the parent, left and right nodes must not be copied. 
     */
    node_base(const node_base& r) :
        refcount(0),
        is_leaf(r.is_leaf)
    {
#ifdef DEBUG_NODE_BASE
        ++node_instance_count;
#endif
    }

    /** 
     * Like the copy constructor, only the stored values should be copied. 
     */
    node_base& operator=(const node_base& r)
    {
        if (this == &r)
            // assignment to self.
            return *this;

        is_leaf = r.is_leaf;
        return *this;
    }

    ~node_base()
    {
#ifdef DEBUG_NODE_BASE
        --node_instance_count;
#endif
        static_cast<_NodeType*>(this)->dispose();
    }
};

template<typename _NodePtr>
inline void intrusive_ptr_add_ref(_NodePtr p)
{
    ++p->refcount;
}

template<typename _NodePtr>
inline void intrusive_ptr_release(_NodePtr p)
{
    --p->refcount;
    if (!p->refcount)
        delete p;
}

template<typename _NodePtr>
void disconnect_all_nodes(_NodePtr p)
{
    if (!p)
        return;

    p->left.reset();
    p->right.reset();
    p->parent.reset();
}

template<typename _NodePtr>
void disconnect_leaf_nodes(_NodePtr left_node, _NodePtr right_node)
{
    if (!left_node || !right_node)
        return;

    // Go through all leaf nodes, and disconnect their links.
    _NodePtr cur_node = left_node;
    do
    {
        _NodePtr next_node = cur_node->right.get();
        disconnect_all_nodes(cur_node);
        cur_node = next_node;
    }
    while (cur_node != right_node);

    disconnect_all_nodes(right_node);
}

template<typename _NodePtr>
void link_nodes(_NodePtr& left, _NodePtr& right)
{
    left->right = right;
    right->left = left;
}

/** 
 * Disconnect all non-leaf nodes so that their ref-counted instances will 
 * all get destroyed afterwards. 
 */
template<typename _NodePtr>
void clear_tree(_NodePtr node)
{
    if (!node)
        // Nothing to do.
        return;

    if (node->is_leaf)
    {
        node->parent.reset();    
        return;
    }

    clear_tree(node->left.get());
    clear_tree(node->right.get());
    disconnect_all_nodes(node);
}

template<typename _NodePtr, typename _NodeType>
_NodePtr make_parent_node(const _NodePtr& node1, const _NodePtr& node2)
{
    _NodePtr parent_node(new _NodeType(false));
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

template<typename _NodePtr, typename _NodeType>
_NodePtr build_tree_non_leaf(const ::std::list<_NodePtr>& node_list)
{
    size_t node_count = node_list.size();
    if (node_count == 1)
    {
        return node_list.front();
    }
    else if (node_count == 0)
        return _NodePtr();

    ::std::list<_NodePtr> new_node_list;
    _NodePtr node_pair[2];
    typename ::std::list<_NodePtr>::const_iterator itr    = node_list.begin();
    typename ::std::list<_NodePtr>::const_iterator itr_end = node_list.end();
    for (bool even_itr = false; itr != itr_end; ++itr, even_itr = !even_itr)
    {
        node_pair[even_itr] = *itr;
        if (even_itr)
        {
            _NodePtr parent_node = make_parent_node<_NodePtr, _NodeType>(node_pair[0], node_pair[1]);
            node_pair[0].reset();
            node_pair[1].reset();
            new_node_list.push_back(parent_node);
        }
    }

    if (node_pair[0])
    {
        // Un-paired node still needs a parent...
        _NodePtr parent_node = make_parent_node<_NodePtr, _NodeType>(node_pair[0], _NodePtr());
        node_pair[0].reset();
        node_pair[1].reset();
        new_node_list.push_back(parent_node);
    }

    // Move up one level, and do the same procedure until the root node is reached.
    return build_tree_non_leaf<_NodePtr, _NodeType>(new_node_list);
}

template<typename _NodePtr, typename _NodeType>
_NodePtr build_tree(const _NodePtr& left_leaf_node)
{
    if (!left_leaf_node)
        // The left leaf node is empty.  Nothing to build.
        return _NodePtr();

    _NodePtr node1, node2;
    node1 = left_leaf_node;

    ::std::list<_NodePtr> node_list;
    while (true)
    {
        node2 = node1->right;
        _NodePtr parent_node = make_parent_node<_NodePtr, _NodeType>(node1, node2);
        node_list.push_back(parent_node);
        
        if (!node2 || !node2->right)
            // no more nodes.  Break out of the loop.
            break;

        node1 = node2->right;
    }

    return build_tree_non_leaf<_NodePtr, _NodeType>(node_list);
}

#ifdef UNIT_TEST
template<typename _NodePtr>
size_t dump_tree_layer(const ::std::list<_NodePtr>& node_list, unsigned int level)
{
    using ::std::cout;
    using ::std::endl;

    if (node_list.empty())
        return 0;

    size_t node_count = node_list.size();

    bool isLeaf = node_list.front()->is_leaf;
    cout << "level " << level << " (" << (isLeaf?"leaf":"non-leaf") << ")" << endl;

    ::std::list<_NodePtr> newList;
    typename ::std::list<_NodePtr>::const_iterator itr = node_list.begin(), itrEnd = node_list.end();
    for (; itr != itrEnd; ++itr)
    {
        const _NodePtr& p = *itr;
        if (!p)
        {
            cout << "(x) ";
            continue;
        }

        p->dump_value();

        if (p->is_leaf)
            continue;

        if (p->left)
        {
            newList.push_back(p->left.get());
            if (p->right)
                newList.push_back(p->right.get());
        }
    }
    cout << endl;

    if (!newList.empty())
        node_count += dump_tree_layer(newList, level+1);

    return node_count;
}

template<typename _NodePtr>
size_t dump_tree(_NodePtr root_node)
{
    if (!root_node)
        return 0;

    ::std::list<_NodePtr> node_list;
    node_list.push_back(root_node);
    return dump_tree_layer(node_list, 0);
}
#endif

}

#endif
