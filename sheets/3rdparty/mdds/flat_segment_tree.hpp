/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * Copyright (c) 2008-2017 Kohei Yoshida
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

#ifndef INCLUDED_MDDS_FLAT_SEGMENT_TREE_HPP
#define INCLUDED_MDDS_FLAT_SEGMENT_TREE_HPP

#include <iostream>
#include <sstream>
#include <utility>
#include <cassert>

#include "node.hpp"
#include "flat_segment_tree_itr.hpp"
#include "global.hpp"

#ifdef MDDS_UNIT_TEST
#include <cstdio>
#include <vector>
#endif

namespace mdds {

template<typename _Key, typename _Value>
class flat_segment_tree
{
public:
    typedef _Key    key_type;
    typedef _Value  value_type;
    typedef size_t  size_type;

    struct nonleaf_value_type
    {
        key_type low;   /// low range value (inclusive)
        key_type high;  /// high range value (non-inclusive)

        bool operator== (const nonleaf_value_type& r) const
        {
            return low == r.low && high == r.high;
        }
    
        nonleaf_value_type()
            : low(0)
            , high(0)
        {
        }
    };

    struct leaf_value_type
    {
        key_type    key;
        value_type  value;
        
        bool operator== (const leaf_value_type& r) const
        {
            return key == r.key && value == r.value;
        }

        leaf_value_type()
            : key(0)
            , value()
        {
        }
    };

    // Handlers required by the node template class.
    struct fill_nonleaf_value_handler;
    struct init_handler;
    struct dispose_handler;
#ifdef MDDS_UNIT_TEST
    struct to_string_handler;
#endif

    typedef __st::node<flat_segment_tree> node;
    typedef typename node::node_ptr node_ptr;

    typedef __st::nonleaf_node<flat_segment_tree> nonleaf_node;

    struct fill_nonleaf_value_handler
    {
        void operator() (__st::nonleaf_node<flat_segment_tree>& _self, const __st::node_base* left_node, const __st::node_base* right_node)
        {
            // Parent node should carry the range of all of its child nodes.
            if (left_node)
            {
                _self.value_nonleaf.low =
                    left_node->is_leaf ?
                        static_cast<const node*>(left_node)->value_leaf.key :
                        static_cast<const nonleaf_node*>(left_node)->value_nonleaf.low;
            }
            else
            {
                // Having a left node is prerequisite.
                throw general_error("flat_segment_tree::fill_nonleaf_value_handler: Having a left node is prerequisite.");
            }

            if (right_node)
            {    
                if (right_node->is_leaf)
                {
                    // When the child nodes are leaf nodes, the upper bound
                    // must be the value of the node that comes after the
                    // right leaf node (if such node exists).
                    const node* p = static_cast<const node*>(right_node);
                    if (p->next)
                        _self.value_nonleaf.high = p->next->value_leaf.key;
                    else
                        _self.value_nonleaf.high = p->value_leaf.key;
                }
                else
                {
                    _self.value_nonleaf.high = static_cast<const nonleaf_node*>(right_node)->value_nonleaf.high;
                }
            }
            else
            {
                _self.value_nonleaf.high =
                    left_node->is_leaf ?
                    static_cast<const node*>(left_node)->value_leaf.key :
                    static_cast<const nonleaf_node*>(left_node)->value_nonleaf.high;
            }
        }
    };

#ifdef MDDS_UNIT_TEST
    struct to_string_handler
    {
        std::string operator() (const node& _self) const
        {
            std::ostringstream os;
            os << "(" << _self.value_leaf.key << ") ";
            return os.str();
        }

        std::string operator() (const mdds::__st::nonleaf_node<flat_segment_tree>& _self) const
        {
            std::ostringstream os;
            os << "(" << _self.value_nonleaf.low << "-" << _self.value_nonleaf.high << ") ";
            return os.str();
        }
    };
#endif

    struct init_handler
    {
        void operator() (node& /*_self*/) {}
        void operator() (__st::nonleaf_node<flat_segment_tree>& /*_self*/) {}
    };

    struct dispose_handler
    {
        void operator() (node& /*_self*/) {}
        void operator() (__st::nonleaf_node<flat_segment_tree>& /*_self*/) {}
    };

private:

    friend struct ::mdds::__fst::itr_forward_handler<flat_segment_tree>;
    friend struct ::mdds::__fst::itr_reverse_handler<flat_segment_tree>;

public:
    class const_iterator : public ::mdds::__fst::const_iterator_base<
        flat_segment_tree, ::mdds::__fst::itr_forward_handler<flat_segment_tree> > 
    {
        typedef ::mdds::__fst::const_iterator_base<
            flat_segment_tree, ::mdds::__fst::itr_forward_handler<flat_segment_tree> > 
                base_type;
        friend class flat_segment_tree;
    public:
        const_iterator() :
            base_type(nullptr, false) {}

    private:
        explicit const_iterator(const typename base_type::fst_type* _db, bool _end) :
            base_type(_db, _end) {}

        explicit const_iterator(const typename base_type::fst_type* _db, const node* p) :
            base_type(_db, p) {}
    };

    class const_reverse_iterator : public ::mdds::__fst::const_iterator_base<
        flat_segment_tree, ::mdds::__fst::itr_reverse_handler<flat_segment_tree> > 
    {
        typedef ::mdds::__fst::const_iterator_base<
            flat_segment_tree, ::mdds::__fst::itr_reverse_handler<flat_segment_tree> > 
                base_type;
        friend class flat_segment_tree;
    public:
        const_reverse_iterator() :
            base_type(nullptr, false) {}
    private:
        explicit const_reverse_iterator(const typename base_type::fst_type* _db, bool _end) : 
            base_type(_db, _end) {}
    };

    using const_segment_iterator = mdds::__fst::const_segment_iterator<flat_segment_tree>;

    /**
     * Return an iterator that points to the first leaf node that correspondes
     * with the start position of the first segment.
     *
     * @return immutable iterator that points to the first leaf node that
     *         corresponds with the start position of the first segment.
     */
    const_iterator begin() const
    {
        return const_iterator(this, false);
    }

    /**
     * Return an iterator that points to the position past the last leaf node
     * that corresponds with the end position of the last segment.
     *
     * @return immutable iterator that points to the position past last leaf
     *         node that corresponds with the end position of the last
     *         segment.
     */
    const_iterator end() const
    {
        return const_iterator(this, true);
    }

    /**
     * Return an iterator that points to the last leaf node that correspondes
     * with the end position of the last segment.  This iterator moves in the
     * reverse direction of a normal iterator.
     *
     * @return immutable reverse iterator that points to the last leaf node
     *         that corresponds with the end position of the last segment.
     */
    const_reverse_iterator rbegin() const
    {
        return const_reverse_iterator(this, false);
    }

    /**
     * Return an iterator that points to the position past the first leaf node
     * that corresponds with the start position of the first segment. This
     * iterator moves in the reverse direction of a normal iterator.
     *
     * @return immutable reverse iterator that points to the position past
     *         first leaf node that corresponds with the start position of the
     *         first segment.
     */
    const_reverse_iterator rend() const
    {
        return const_reverse_iterator(this, true);
    }

    /**
     * Return an immutable iterator that points to the first segment stored in
     * the tree.  It iterates through the segments one segment at a time.
     * Each iterator value consists of <code>start</code>, <code>end</code>,
     * and <code>value</code> members that correspond with the start and end
     * positions of a segment and the value of that segment, respectively.
     *
     * @return immutable iterator that points to the first segment stored in
     *         the tree.
     */
    const_segment_iterator begin_segment() const;

    /**
     * Return an immutable iterator that points to the position past the last
     * segment stored in the tree.  It iterates through the segments one
     * segment at a time.  Each iterator value consists of <code>start</code>,
     * <code>end</code>, and <code>value</code> members that correspond with
     * the start and end positions of a segment and the value of that segment,
     * respectively.
     *
     * @return immutable iterator that points to the position past the last
     *         segment stored in the tree.
     */
    const_segment_iterator end_segment() const;

    /**
     * Constructor that takes minimum and maximum keys and the value to be
     * used for the initial segment.
     *
     * @param min_val minimum allowed key value for the entire series of
     *                segments.
     * @param max_val maximum allowed key value for the entires series of
     *                segments.
     * @param init_val value to be used for the initial segment. This value
     *                 will also be used for empty segments.
     */
    flat_segment_tree(key_type min_val, key_type max_val, value_type init_val);

    /** 
     * Copy constructor only copies the leaf nodes.  
     */
    flat_segment_tree(const flat_segment_tree<key_type, value_type>& r);

    ~flat_segment_tree();

    /**
      * Assignment only copies the leaf nodes.
      */
    flat_segment_tree<key_type, value_type>&
    operator=(const flat_segment_tree<key_type, value_type>& other);

    /**
     * Swap the content of the tree with another instance.
     *
     * @param other instance of flat_segment_tree to swap content with.
     */
    void swap(flat_segment_tree<key_type, value_type>& other);

    /**
     * Remove all stored segments except for the initial segment. The minimum
     * and maximum keys and the default value will be retained after the call
     * returns.  This call will also remove the tree.
     */
    void clear();

    /** 
     * Insert a new segment into the tree.  It searches for the point of 
     * insertion from the first leaf node. 
     *
     * @param start_key start value of the segment being inserted.  The value 
     *              is inclusive.
     * @param end_key end value of the segment being inserted.  The value is 
     *            not inclusive.
     * @param val value associated with this segment. 
     *  
     * @return pair of const_iterator corresponding to the start position of 
     *         the inserted segment, and a boolean value indicating whether or
     *         not the insertion has modified the tree.
     */
    std::pair<const_iterator, bool>
    insert_front(key_type start_key, key_type end_key, value_type val)
    {
        return insert_segment_impl(start_key, end_key, val, true);
    }

    /** 
     * Insert a new segment into the tree.  Unlike the insert_front()
     * counterpart, this method searches for the point of insertion from the
     * last leaf node toward the first.
     *  
     * @param start_key start value of the segment being inserted.  The value 
     *              is inclusive.
     * @param end_key end value of the segment being inserted.  The value is 
     *            not inclusive.
     * @param val value associated with this segment. 
     *  
     * @return pair of const_iterator corresponding to the start position of 
     *         the inserted segment, and a boolean value indicating whether or
     *         not the insertion has modified the tree.
     */
    std::pair<const_iterator, bool>
    insert_back(key_type start_key, key_type end_key, value_type val)
    {
        return insert_segment_impl(start_key, end_key, val, false);
    }

    /**
     * Insert a new segment into the tree at or after specified point of
     * insertion.
     * 
     * @param pos specified insertion point
     * @param start_key start value of the segment being inserted.  The value 
     *              is inclusive.
     * @param end_key end value of the segment being inserted.  The value is 
     *            not inclusive.
     * @param val value associated with this segment. 
     *  
     * @return pair of const_iterator corresponding to the start position of 
     *         the inserted segment, and a boolean value indicating whether or
     *         not the insertion has modified the tree.
     */
    std::pair<const_iterator, bool>
    insert(const const_iterator& pos, key_type start_key, key_type end_key, value_type val);

    /** 
     * Remove a segment specified by the start and end key values, and shift 
     * the remaining segments (i.e. those segments that come after the removed
     * segment) to left.  Note that the start and end positions of the segment 
     * being removed <b>must</b> be within the base segment span.
     *
     * @param start_key start position of the segment being removed.
     * @param end_key end position of the segment being removed. 
     */
    void shift_left(key_type start_key, key_type end_key);

    /** 
     * Shift all segments that occur at or after the specified start position 
     * to right by the size specified.
     *
     * @param pos position where the right-shift occurs.
     * @param size amount of shift (must be greater than 0) 
     * @param skip_start_node if true, and the specified position is at an 
     *                        existing node position, that node will
     *                        <i>not</i> be shifted.  This argument has no
     *                        effect if the position specified does not
     *                        coincide with any of the existing nodes.
     */
    void shift_right(key_type pos, key_type size, bool skip_start_node);

    /**
     * Perform leaf-node search for a value associated with a key.
     * 
     * @param key key value
     * @param value value associated with key specified gets stored upon
     *              successful search.
     * @param start_key pointer to a variable where the start key value of the
     *                  segment that contains the key gets stored upon
     *                  successful search.
     * @param end_key pointer to a varaible where the end key value of the
     *                segment that contains the key gets stored upon
     *                successful search.
     * @return a pair of const_iterator corresponding to the start position of
     *         the segment containing the key, and a boolean value indicating
     *         whether or not the search has been successful.
     * 
     */
    std::pair<const_iterator, bool>
    search(key_type key, value_type& value, key_type* start_key = nullptr, key_type* end_key = nullptr) const;

    /**
     * Perform leaf-node search for a value associated with a key.
     *  
     * @param pos position from which the search should start.  When the 
     *            position is invalid, it falls back to the normal search.
     * @param key key value
     * @param value value associated with key specified gets stored upon
     *              successful search.
     * @param start_key pointer to a variable where the start key value of the
     *                  segment that contains the key gets stored upon
     *                  successful search.
     * @param end_key pointer to a varaible where the end key value of the
     *                segment that contains the key gets stored upon
     *                successful search.
     * @return a pair of const_iterator corresponding to the start position of
     *         the segment containing the key, and a boolean value indicating
     *         whether or not the search has been successful.
     */
    std::pair<const_iterator, bool>
    search(const const_iterator& pos, key_type key, value_type& value, key_type* start_key = nullptr, key_type* end_key = nullptr) const;

    /**
     * Perform tree search for a value associated with a key.  This method 
     * assumes that the tree is valid.  Call is_tree_valid() to find out
     * whether the tree is valid, and build_tree() to build a new tree in case
     * it's not.
     * 
     * @param key key value
     * @param value value associated with key specified gets stored upon
     *              successful search.
     * @param start_key pointer to a variable where the start key value of the
     *                  segment that contains the key gets stored upon
     *                  successful search.
     * @param end_key pointer to a varaible where the end key value of the
     *                segment that contains the key gets stored upon
     *                successful search.
     * @return a pair of const_iterator corresponding to the start position of
     *         the segment containing the key, and a boolean value indicating
     *         whether or not the search has been successful.
     */
    std::pair<const_iterator, bool>
    search_tree(key_type key, value_type& value, key_type* start_key = nullptr, key_type* end_key = nullptr) const;

    /**
     * Build a tree of non-leaf nodes based on the values stored in the leaf
     * nodes.  The tree must be valid before you can call the search_tree()
     * method.
     */
    void build_tree();

    /**
     * @return true if the tree is valid, otherwise false.  The tree must be
     *         valid before you can call the search_tree() method.
     */
    bool is_tree_valid() const
    {
        return m_valid_tree;
    }

    /** 
     * Equality between two flat_segment_tree instances is evaluated by 
     * comparing the keys and the values of the leaf nodes only.  Neither the 
     * non-leaf nodes nor the validity of the tree is evaluated. 
     */
    bool operator==(const flat_segment_tree<key_type, value_type>& r) const;

    bool operator !=(const flat_segment_tree<key_type, value_type>& r) const
    {
        return !operator==(r);
    }

    key_type min_key() const
    {
        return m_left_leaf->value_leaf.key;
    }

    key_type max_key() const
    {
        return m_right_leaf->value_leaf.key;
    }

    value_type default_value() const
    {
        return m_init_val;
    }

    /**
     * Return the number of leaf nodes.
     *
     * @return number of leaf nodes.
     */
    size_type leaf_size() const;

#ifdef MDDS_UNIT_TEST
    nonleaf_node* get_root_node() const
    {
        return m_root_node;
    }

    void dump_tree() const
    {
        using ::std::cout;
        using ::std::endl;

        if (!m_valid_tree)
            assert(!"attempted to dump an invalid tree!");

        size_t node_count = mdds::__st::tree_dumper<node, nonleaf_node>::dump(m_root_node);
        size_t node_instance_count = node::get_instance_count();
        size_t leaf_count = leaf_size();

        cout << "tree node count = " << node_count << "; node instance count = "
            << node_instance_count << "; leaf node count = " << leaf_count << endl;

        assert(leaf_count == node_instance_count);
    }

    void dump_leaf_nodes() const
    {
        using ::std::cout;
        using ::std::endl;

        cout << "------------------------------------------" << endl;

        node_ptr cur_node = m_left_leaf;
        long node_id = 0;
        while (cur_node)
        {
            cout << "  node " << node_id++ << ": key = " << cur_node->value_leaf.key
                << "; value = " << cur_node->value_leaf.value 
                << endl;
            cur_node = cur_node->next;
        }
        cout << endl << "  node instance count = " << node::get_instance_count() << endl;
    }

    /** 
     * Verify keys in the leaf nodes.
     *
     * @param key_values vector containing key values in the left-to-right 
     *                   order, including the key value of the rightmost leaf
     *                   node.
     */
    bool verify_keys(const ::std::vector<key_type>& key_values) const
    {
        {
            // Start from the left-most node, and traverse right.
            node* cur_node = m_left_leaf.get();
            typename ::std::vector<key_type>::const_iterator itr = key_values.begin(), itr_end = key_values.end();
            for (; itr != itr_end; ++itr)
            {
                if (!cur_node)
                    // Position past the right-mode node.  Invalid.
                    return false;
    
                if (cur_node->value_leaf.key != *itr)
                    // Key values differ.
                    return false;
    
                cur_node = cur_node->next.get();
            }

            if (cur_node)
                // At this point, we expect the current node to be at the position
                // past the right-most node, which is nullptr.
                return false;
        }

        {
            // Start from the right-most node, and traverse left.
            node* cur_node = m_right_leaf.get();
            typename ::std::vector<key_type>::const_reverse_iterator itr = key_values.rbegin(), itr_end = key_values.rend();
            for (; itr != itr_end; ++itr)
            {
                if (!cur_node)
                    // Position past the left-mode node.  Invalid.
                    return false;
    
                if (cur_node->value_leaf.key != *itr)
                    // Key values differ.
                    return false;
    
                cur_node = cur_node->prev.get();
            }

            if (cur_node)
                // Likewise, we expect the current position to be past the
                // left-most node, in which case the node value is nullptr.
                return false;
        }
        
        return true;
    }

    /** 
     * Verify values in the leaf nodes.
     *
     * @param values vector containing values to verify against, in the 
     *               left-to-right order, <i>not</i> including the value of
     *               the rightmost leaf node.
     */
    bool verify_values(const ::std::vector<value_type>& values) const
    {
        node* cur_node = m_left_leaf.get();
        node* end_node = m_right_leaf.get();
        typename ::std::vector<value_type>::const_iterator itr = values.begin(), itr_end = values.end();
        for (; itr != itr_end; ++itr)
        {
            if (cur_node == end_node || !cur_node)
                return false;

            if (cur_node->value_leaf.value != *itr)
                // Key values differ.
                return false;

            cur_node = cur_node->next.get();
        }

        if (cur_node != end_node)
            // At this point, we expect the current node to be at the end of 
            // range.
            return false;

        return true;
    }
#endif

private:
    flat_segment_tree() = delete; // default constructor is not allowed.

    void append_new_segment(key_type start_key)
    {
        if (m_right_leaf->prev->value_leaf.key == start_key)
        {
            m_right_leaf->prev->value_leaf.value = m_init_val;
            return;
        }

#ifdef MDDS_UNIT_TEST
        // The start position must come after the position of the last node 
        // before the right-most node.
        assert(m_right_leaf->prev->value_leaf.key < start_key);
#endif

        if (m_right_leaf->prev->value_leaf.value == m_init_val)
            // The existing segment has the same value.  No need to insert a 
            // new segment.
            return;

        node_ptr new_node(new node);
        new_node->value_leaf.key   = start_key;
        new_node->value_leaf.value = m_init_val;
        new_node->prev = m_right_leaf->prev;
        new_node->next = m_right_leaf;
        m_right_leaf->prev->next = new_node;
        m_right_leaf->prev = new_node;
        m_valid_tree = false;
    }

    ::std::pair<const_iterator, bool>
        insert_segment_impl(key_type start_key, key_type end_key, value_type val, bool forward);

    ::std::pair<const_iterator, bool>
        insert_to_pos(node_ptr& start_pos, key_type start_key, key_type end_key, value_type val);

    ::std::pair<const_iterator, bool>
        search_impl(const node* pos, key_type key, value_type& value, key_type* start_key, key_type* end_key) const;

    const node* get_insertion_pos_leaf_reverse(key_type key, const node* start_pos) const;

    const node* get_insertion_pos_leaf(key_type key, const node* start_pos) const;

    static void shift_leaf_key_left(node_ptr& begin_node, node_ptr& end_node, key_type shift_value)
    {
        node* cur_node_p = begin_node.get();
        node* end_node_p = end_node.get();
        while (cur_node_p != end_node_p)
        {
            cur_node_p->value_leaf.key -= shift_value;
            cur_node_p = cur_node_p->next.get();
        }
    }

    static void shift_leaf_key_right(node_ptr& cur_node, node_ptr& end_node, key_type shift_value)
    {
        key_type end_node_key = end_node->value_leaf.key;
        while (cur_node.get() != end_node.get())
        {
            cur_node->value_leaf.key += shift_value;
            if (cur_node->value_leaf.key < end_node_key)
            {
                // The node is still in-bound.  Keep shifting.
                cur_node = cur_node->next;
                continue;
            }

            // This node has been pushed outside the end node position.
            // Remove all nodes that follows, and connect the previous node
            // with the end node.

            node_ptr last_node = cur_node->prev;
            while (cur_node.get() != end_node.get())
            {
                node_ptr next_node = cur_node->next;
                disconnect_all_nodes(cur_node.get());
                cur_node = next_node;
            }
            last_node->next = end_node;
            end_node->prev = last_node;
            return;
        }
    }

    void destroy();

    /**
     * Check and optionally adjust the start and end key values if one of them
     * is out-of-bound.
     *
     * @return true if the start and end key values are valid, either with or
     *         without adjustments, otherwise false.
     */
    bool adjust_segment_range(key_type& start_key, key_type& end_key) const;

private:
    std::vector<nonleaf_node> m_nonleaf_node_pool;

    nonleaf_node* m_root_node;
    node_ptr   m_left_leaf;
    node_ptr   m_right_leaf;
    value_type m_init_val;
    bool       m_valid_tree;
};

template<typename _Key, typename _Value>
void
swap(flat_segment_tree<_Key, _Value>& left, flat_segment_tree<_Key, _Value>& right)
{
    left.swap(right);
}

} // namespace mdds

#include "flat_segment_tree_def.inl"

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
