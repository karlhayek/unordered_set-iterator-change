// -*- C++ -*-
//===-------------------------- unordered_set -----------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP_UNORDERED_SET
#define _LIBCPP_UNORDERED_SET

/*

    unordered_set synopsis

#include <initializer_list>

namespace std
{

template <class Value, class Hash = hash<Value>, class Pred = equal_to<Value>,
          class Alloc = allocator<Value>>
class unordered_set
{
public:
    // types
    typedef Value                                                      key_type;
    typedef key_type                                                   value_type;
    typedef Hash                                                       hasher;
    typedef Pred                                                       key_equal;
    typedef Alloc                                                      allocator_type;
    typedef value_type&                                                reference;
    typedef const value_type&                                          const_reference;
    typedef typename allocator_traits<allocator_type>::pointer         pointer;
    typedef typename allocator_traits<allocator_type>::const_pointer   const_pointer;
    typedef typename allocator_traits<allocator_type>::size_type       size_type;
    typedef typename allocator_traits<allocator_type>::difference_type difference_type;

    typedef /unspecified/ iterator;
    typedef /unspecified/ const_iterator;
    typedef /unspecified/ local_iterator;
    typedef /unspecified/ const_local_iterator;

    unordered_set()
        noexcept(
            is_nothrow_default_constructible<hasher>::value &&
            is_nothrow_default_constructible<key_equal>::value &&
            is_nothrow_default_constructible<allocator_type>::value);
    explicit unordered_set(size_type n, const hasher& hf = hasher(),
                           const key_equal& eql = key_equal(),
                           const allocator_type& a = allocator_type());
    template <class InputIterator>
        unordered_set(InputIterator f, InputIterator l,
                      size_type n = 0, const hasher& hf = hasher(),
                      const key_equal& eql = key_equal(),
                      const allocator_type& a = allocator_type());
    explicit unordered_set(const allocator_type&);
    unordered_set(const unordered_set&);
    unordered_set(const unordered_set&, const Allocator&);
    unordered_set(unordered_set&&)
        noexcept(
            is_nothrow_move_constructible<hasher>::value &&
            is_nothrow_move_constructible<key_equal>::value &&
            is_nothrow_move_constructible<allocator_type>::value);
    unordered_set(unordered_set&&, const Allocator&);
    unordered_set(initializer_list<value_type>, size_type n = 0,
                  const hasher& hf = hasher(), const key_equal& eql = key_equal(),
                  const allocator_type& a = allocator_type());
    unordered_set(size_type n, const allocator_type& a); // C++14
    unordered_set(size_type n, const hasher& hf, const allocator_type& a); // C++14
    template <class InputIterator>
      unordered_set(InputIterator f, InputIterator l, size_type n, const allocator_type& a); // C++14
    template <class InputIterator>
      unordered_set(InputIterator f, InputIterator l, size_type n, 
                    const hasher& hf,  const allocator_type& a); // C++14
    unordered_set(initializer_list<value_type> il, size_type n, const allocator_type& a); // C++14
    unordered_set(initializer_list<value_type> il, size_type n,
                  const hasher& hf,  const allocator_type& a); // C++14
    ~unordered_set();
    unordered_set& operator=(const unordered_set&);
    unordered_set& operator=(unordered_set&&)
        noexcept(
            allocator_type::propagate_on_container_move_assignment::value &&
            is_nothrow_move_assignable<allocator_type>::value &&
            is_nothrow_move_assignable<hasher>::value &&
            is_nothrow_move_assignable<key_equal>::value);
    unordered_set& operator=(initializer_list<value_type>);

    allocator_type get_allocator() const noexcept;

    bool      empty() const noexcept;
    size_type size() const noexcept;
    size_type max_size() const noexcept;

    iterator       begin() noexcept;
    iterator       end() noexcept;
    const_iterator begin()  const noexcept;
    const_iterator end()    const noexcept;
    const_iterator cbegin() const noexcept;
    const_iterator cend()   const noexcept;

    template <class... Args>
        pair<iterator, bool> emplace(Args&&... args);
    template <class... Args>
        iterator emplace_hint(const_iterator position, Args&&... args);
    pair<iterator, bool> insert(const value_type& obj);
    pair<iterator, bool> insert(value_type&& obj);
    iterator insert(const_iterator hint, const value_type& obj);
    iterator insert(const_iterator hint, value_type&& obj);
    template <class InputIterator>
        void insert(InputIterator first, InputIterator last);
    void insert(initializer_list<value_type>);

    iterator erase(const_iterator position);
    iterator erase(iterator position);  // C++14
    size_type erase(const key_type& k);
    iterator erase(const_iterator first, const_iterator last);
    void clear() noexcept;

    void swap(unordered_set&)
       noexcept(allocator_traits<Allocator>::is_always_equal::value &&
                 noexcept(swap(declval<hasher&>(), declval<hasher&>())) &&
                 noexcept(swap(declval<key_equal&>(), declval<key_equal&>()))); // C++17

    hasher hash_function() const;
    key_equal key_eq() const;

    iterator       find(const key_type& k);
    const_iterator find(const key_type& k) const;
    size_type count(const key_type& k) const;
    pair<iterator, iterator>             equal_range(const key_type& k);
    pair<const_iterator, const_iterator> equal_range(const key_type& k) const;

    size_type bucket_count() const noexcept;
    size_type max_bucket_count() const noexcept;

    size_type bucket_size(size_type n) const;
    size_type bucket(const key_type& k) const;

    local_iterator       begin(size_type n);
    local_iterator       end(size_type n);
    const_local_iterator begin(size_type n) const;
    const_local_iterator end(size_type n) const;
    const_local_iterator cbegin(size_type n) const;
    const_local_iterator cend(size_type n) const;

    float load_factor() const noexcept;
    float max_load_factor() const noexcept;
    void max_load_factor(float z);
    void rehash(size_type n);
    void reserve(size_type n);
};

template <class Value, class Hash, class Pred, class Alloc>
    void swap(unordered_set<Value, Hash, Pred, Alloc>& x,
              unordered_set<Value, Hash, Pred, Alloc>& y)
              noexcept(noexcept(x.swap(y)));

template <class Value, class Hash, class Pred, class Alloc>
    bool
    operator==(const unordered_set<Value, Hash, Pred, Alloc>& x,
               const unordered_set<Value, Hash, Pred, Alloc>& y);

template <class Value, class Hash, class Pred, class Alloc>
    bool
    operator!=(const unordered_set<Value, Hash, Pred, Alloc>& x,
               const unordered_set<Value, Hash, Pred, Alloc>& y);

template <class Value, class Hash = hash<Value>, class Pred = equal_to<Value>,
          class Alloc = allocator<Value>>
class unordered_multiset
{
public:
    // types
    typedef Value                                                      key_type;
    typedef key_type                                                   value_type;
    typedef Hash                                                       hasher;
    typedef Pred                                                       key_equal;
    typedef Alloc                                                      allocator_type;
    typedef value_type&                                                reference;
    typedef const value_type&                                          const_reference;
    typedef typename allocator_traits<allocator_type>::pointer         pointer;
    typedef typename allocator_traits<allocator_type>::const_pointer   const_pointer;
    typedef typename allocator_traits<allocator_type>::size_type       size_type;
    typedef typename allocator_traits<allocator_type>::difference_type difference_type;

    typedef /unspecified/ iterator;
    typedef /unspecified/ const_iterator;
    typedef /unspecified/ local_iterator;
    typedef /unspecified/ const_local_iterator;

    unordered_multiset()
        noexcept(
            is_nothrow_default_constructible<hasher>::value &&
            is_nothrow_default_constructible<key_equal>::value &&
            is_nothrow_default_constructible<allocator_type>::value);
    explicit unordered_multiset(size_type n, const hasher& hf = hasher(),
                           const key_equal& eql = key_equal(),
                           const allocator_type& a = allocator_type());
    template <class InputIterator>
        unordered_multiset(InputIterator f, InputIterator l,
                      size_type n = 0, const hasher& hf = hasher(),
                      const key_equal& eql = key_equal(),
                      const allocator_type& a = allocator_type());
    explicit unordered_multiset(const allocator_type&);
    unordered_multiset(const unordered_multiset&);
    unordered_multiset(const unordered_multiset&, const Allocator&);
    unordered_multiset(unordered_multiset&&)
        noexcept(
            is_nothrow_move_constructible<hasher>::value &&
            is_nothrow_move_constructible<key_equal>::value &&
            is_nothrow_move_constructible<allocator_type>::value);
    unordered_multiset(unordered_multiset&&, const Allocator&);
    unordered_multiset(initializer_list<value_type>, size_type n = /see below/,
                  const hasher& hf = hasher(), const key_equal& eql = key_equal(),
                  const allocator_type& a = allocator_type());
    unordered_multiset(size_type n, const allocator_type& a); // C++14
    unordered_multiset(size_type n, const hasher& hf, const allocator_type& a); // C++14
    template <class InputIterator>
      unordered_multiset(InputIterator f, InputIterator l, size_type n, const allocator_type& a); // C++14
    template <class InputIterator>
      unordered_multiset(InputIterator f, InputIterator l, size_type n,
                         const hasher& hf, const allocator_type& a); // C++14
    unordered_multiset(initializer_list<value_type> il, size_type n, const allocator_type& a); // C++14
    unordered_multiset(initializer_list<value_type> il, size_type n, 
                       const hasher& hf,  const allocator_type& a); // C++14
    ~unordered_multiset();
    unordered_multiset& operator=(const unordered_multiset&);
    unordered_multiset& operator=(unordered_multiset&&)
        noexcept(
            allocator_type::propagate_on_container_move_assignment::value &&
            is_nothrow_move_assignable<allocator_type>::value &&
            is_nothrow_move_assignable<hasher>::value &&
            is_nothrow_move_assignable<key_equal>::value);
    unordered_multiset& operator=(initializer_list<value_type>);

    allocator_type get_allocator() const noexcept;

    bool      empty() const noexcept;
    size_type size() const noexcept;
    size_type max_size() const noexcept;

    iterator       begin() noexcept;
    iterator       end() noexcept;
    const_iterator begin()  const noexcept;
    const_iterator end()    const noexcept;
    const_iterator cbegin() const noexcept;
    const_iterator cend()   const noexcept;

    template <class... Args>
        iterator emplace(Args&&... args);
    template <class... Args>
        iterator emplace_hint(const_iterator position, Args&&... args);
    iterator insert(const value_type& obj);
    iterator insert(value_type&& obj);
    iterator insert(const_iterator hint, const value_type& obj);
    iterator insert(const_iterator hint, value_type&& obj);
    template <class InputIterator>
        void insert(InputIterator first, InputIterator last);
    void insert(initializer_list<value_type>);

    iterator erase(const_iterator position);
    iterator erase(iterator position);  // C++14
    size_type erase(const key_type& k);
    iterator erase(const_iterator first, const_iterator last);
    void clear() noexcept;

    void swap(unordered_multiset&)
       noexcept(allocator_traits<Allocator>::is_always_equal::value &&
                 noexcept(swap(declval<hasher&>(), declval<hasher&>())) &&
                 noexcept(swap(declval<key_equal&>(), declval<key_equal&>()))); // C++17

    hasher hash_function() const;
    key_equal key_eq() const;

    iterator       find(const key_type& k);
    const_iterator find(const key_type& k) const;
    size_type count(const key_type& k) const;
    pair<iterator, iterator>             equal_range(const key_type& k);
    pair<const_iterator, const_iterator> equal_range(const key_type& k) const;

    size_type bucket_count() const noexcept;
    size_type max_bucket_count() const noexcept;

    size_type bucket_size(size_type n) const;
    size_type bucket(const key_type& k) const;

    local_iterator       begin(size_type n);
    local_iterator       end(size_type n);
    const_local_iterator begin(size_type n) const;
    const_local_iterator end(size_type n) const;
    const_local_iterator cbegin(size_type n) const;
    const_local_iterator cend(size_type n) const;

    float load_factor() const noexcept;
    float max_load_factor() const noexcept;
    void max_load_factor(float z);
    void rehash(size_type n);
    void reserve(size_type n);
};

template <class Value, class Hash, class Pred, class Alloc>
    void swap(unordered_multiset<Value, Hash, Pred, Alloc>& x,
              unordered_multiset<Value, Hash, Pred, Alloc>& y)
              noexcept(noexcept(x.swap(y)));

template <class Value, class Hash, class Pred, class Alloc>
    bool
    operator==(const unordered_multiset<Value, Hash, Pred, Alloc>& x,
               const unordered_multiset<Value, Hash, Pred, Alloc>& y);

template <class Value, class Hash, class Pred, class Alloc>
    bool
    operator!=(const unordered_multiset<Value, Hash, Pred, Alloc>& x,
               const unordered_multiset<Value, Hash, Pred, Alloc>& y);
}  // std

*/

#include <__config>
#include <__hash_table>
#include <functional>

#include <__debug>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

template <class _Value, class _Hash = hash<_Value>, class _Pred = equal_to<_Value>,
          class _Alloc = allocator<_Value> >
class _LIBCPP_TEMPLATE_VIS unordered_set
{
public:
    // types
    typedef _Value                                                     key_type;
    typedef key_type                                                   value_type;
    typedef _Hash                                                      hasher;
    typedef _Pred                                                      key_equal;
    typedef _Alloc                                                     allocator_type;
    typedef value_type&                                                reference;
    typedef const value_type&                                          const_reference;
    static_assert((is_same<value_type, typename allocator_type::value_type>::value),
                  "Invalid allocator::value_type");

private:
    typedef __hash_table<value_type, hasher, key_equal, allocator_type> __table;

    __table __table_;

public:
    typedef typename __table::pointer         pointer;
    typedef typename __table::const_pointer   const_pointer;
    typedef typename __table::size_type       size_type;
    typedef typename __table::difference_type difference_type;

    typedef typename __table::const_iterator       iterator;
    typedef typename __table::const_iterator       const_iterator;
    typedef typename __table::const_local_iterator local_iterator;
    typedef typename __table::const_local_iterator const_local_iterator;

    _LIBCPP_INLINE_VISIBILITY
    unordered_set()
        _NOEXCEPT_(is_nothrow_default_constructible<__table>::value)
        {
#if _LIBCPP_DEBUG_LEVEL >= 2
            __get_db()->__insert_c(this);
#endif
        }
    explicit unordered_set(size_type __n, const hasher& __hf = hasher(),
                           const key_equal& __eql = key_equal());
#if _LIBCPP_STD_VER > 11
    inline _LIBCPP_INLINE_VISIBILITY
    unordered_set(size_type __n, const allocator_type& __a)
        : unordered_set(__n, hasher(), key_equal(), __a) {}
    inline _LIBCPP_INLINE_VISIBILITY
    unordered_set(size_type __n, const hasher& __hf, const allocator_type& __a)
        : unordered_set(__n, __hf, key_equal(), __a) {}
#endif
    unordered_set(size_type __n, const hasher& __hf, const key_equal& __eql,
                  const allocator_type& __a);
    template <class _InputIterator>
        unordered_set(_InputIterator __first, _InputIterator __last);
    template <class _InputIterator>
        unordered_set(_InputIterator __first, _InputIterator __last,
                      size_type __n, const hasher& __hf = hasher(),
                      const key_equal& __eql = key_equal());
    template <class _InputIterator>
        unordered_set(_InputIterator __first, _InputIterator __last,
                      size_type __n, const hasher& __hf, const key_equal& __eql,
                      const allocator_type& __a);
#if _LIBCPP_STD_VER > 11
    template <class _InputIterator>
    inline _LIBCPP_INLINE_VISIBILITY
        unordered_set(_InputIterator __first, _InputIterator __last, 
                    size_type __n, const allocator_type& __a)
            : unordered_set(__first, __last, __n, hasher(), key_equal(), __a) {}
    template <class _InputIterator>
        unordered_set(_InputIterator __first, _InputIterator __last, 
                      size_type __n, const hasher& __hf, const allocator_type& __a)
            : unordered_set(__first, __last, __n, __hf, key_equal(), __a) {}
#endif
    _LIBCPP_INLINE_VISIBILITY
    explicit unordered_set(const allocator_type& __a);
    unordered_set(const unordered_set& __u);
    unordered_set(const unordered_set& __u, const allocator_type& __a);
#ifndef _LIBCPP_HAS_NO_RVALUE_REFERENCES
    _LIBCPP_INLINE_VISIBILITY
    unordered_set(unordered_set&& __u)
        _NOEXCEPT_(is_nothrow_move_constructible<__table>::value);
    unordered_set(unordered_set&& __u, const allocator_type& __a);
#endif  // _LIBCPP_HAS_NO_RVALUE_REFERENCES
#ifndef _LIBCPP_HAS_NO_GENERALIZED_INITIALIZERS
    unordered_set(initializer_list<value_type> __il);
    unordered_set(initializer_list<value_type> __il, size_type __n,
                  const hasher& __hf = hasher(),
                  const key_equal& __eql = key_equal());
    unordered_set(initializer_list<value_type> __il, size_type __n,
                  const hasher& __hf, const key_equal& __eql,
                  const allocator_type& __a);
#if _LIBCPP_STD_VER > 11
    inline _LIBCPP_INLINE_VISIBILITY
    unordered_set(initializer_list<value_type> __il, size_type __n,
                                                      const allocator_type& __a)
        : unordered_set(__il, __n, hasher(), key_equal(), __a) {}
    inline _LIBCPP_INLINE_VISIBILITY
    unordered_set(initializer_list<value_type> __il, size_type __n, 
                                  const hasher& __hf, const allocator_type& __a)
        : unordered_set(__il, __n, __hf, key_equal(), __a) {}
#endif
#endif  // _LIBCPP_HAS_NO_GENERALIZED_INITIALIZERS
    // ~unordered_set() = default;
    _LIBCPP_INLINE_VISIBILITY
    unordered_set& operator=(const unordered_set& __u)
    {
        __table_ = __u.__table_;
        return *this;
    }
#ifndef _LIBCPP_HAS_NO_RVALUE_REFERENCES
    _LIBCPP_INLINE_VISIBILITY
    unordered_set& operator=(unordered_set&& __u)
        _NOEXCEPT_(is_nothrow_move_assignable<__table>::value);
#endif
#ifndef _LIBCPP_HAS_NO_GENERALIZED_INITIALIZERS
    _LIBCPP_INLINE_VISIBILITY
    unordered_set& operator=(initializer_list<value_type> __il);
#endif  // _LIBCPP_HAS_NO_GENERALIZED_INITIALIZERS

    _LIBCPP_INLINE_VISIBILITY
    allocator_type get_allocator() const _NOEXCEPT
        {return allocator_type(__table_.__node_alloc());}

    _LIBCPP_INLINE_VISIBILITY
    bool      empty() const _NOEXCEPT {return __table_.size() == 0;}
    _LIBCPP_INLINE_VISIBILITY
    size_type size() const _NOEXCEPT  {return __table_.size();}
    _LIBCPP_INLINE_VISIBILITY
    size_type max_size() const _NOEXCEPT {return __table_.max_size();}

    _LIBCPP_INLINE_VISIBILITY
    iterator       begin() _NOEXCEPT        {return __table_.begin();}
    _LIBCPP_INLINE_VISIBILITY
    iterator       end() _NOEXCEPT          {return __table_.end();}
    _LIBCPP_INLINE_VISIBILITY
    const_iterator begin()  const _NOEXCEPT {return __table_.begin();}
    _LIBCPP_INLINE_VISIBILITY
    const_iterator end()    const _NOEXCEPT {return __table_.end();}
    _LIBCPP_INLINE_VISIBILITY
    const_iterator cbegin() const _NOEXCEPT {return __table_.begin();}
    _LIBCPP_INLINE_VISIBILITY
    const_iterator cend()   const _NOEXCEPT {return __table_.end();}

#if !defined(_LIBCPP_HAS_NO_RVALUE_REFERENCES) && !defined(_LIBCPP_HAS_NO_VARIADICS)
    template <class... _Args>
        _LIBCPP_INLINE_VISIBILITY
        pair<iterator, bool> emplace(_Args&&... __args)
            {return __table_.__emplace_unique(_VSTD::forward<_Args>(__args)...);}
    template <class... _Args>
        _LIBCPP_INLINE_VISIBILITY
#if _LIBCPP_DEBUG_LEVEL >= 2
        iterator emplace_hint(const_iterator __p, _Args&&... __args)
        {
            _LIBCPP_ASSERT(__get_const_db()->__find_c_from_i(&__p) == this,
                "unordered_set::emplace_hint(const_iterator, args...) called with an iterator not"
                " referring to this unordered_set");
            return __table_.__emplace_unique(_VSTD::forward<_Args>(__args)...).first;
        }
#else
        iterator emplace_hint(const_iterator, _Args&&... __args)
            {return __table_.__emplace_unique(_VSTD::forward<_Args>(__args)...).first;}
#endif
#endif  // !defined(_LIBCPP_HAS_NO_RVALUE_REFERENCES) && !defined(_LIBCPP_HAS_NO_VARIADICS)
    _LIBCPP_INLINE_VISIBILITY
    pair<iterator, bool> insert(const value_type& __x)
        {return __table_.__insert_unique(__x);}
#ifndef _LIBCPP_HAS_NO_RVALUE_REFERENCES
    _LIBCPP_INLINE_VISIBILITY
    pair<iterator, bool> insert(value_type&& __x)
        {return __table_.__insert_unique(_VSTD::move(__x));}
#endif  // _LIBCPP_HAS_NO_RVALUE_REFERENCES
    _LIBCPP_INLINE_VISIBILITY
#if _LIBCPP_DEBUG_LEVEL >= 2
    iterator insert(const_iterator __p, const value_type& __x)
        {
            _LIBCPP_ASSERT(__get_const_db()->__find_c_from_i(&__p) == this,
                "unordered_set::insert(const_iterator, const value_type&) called with an iterator not"
                " referring to this unordered_set");
            return insert(__x).first;
        }
#else
    iterator insert(const_iterator, const value_type& __x)
        {return insert(__x).first;}
#endif
#ifndef _LIBCPP_HAS_NO_RVALUE_REFERENCES
    _LIBCPP_INLINE_VISIBILITY
#if _LIBCPP_DEBUG_LEVEL >= 2
    iterator insert(const_iterator __p, value_type&& __x)
        {
            _LIBCPP_ASSERT(__get_const_db()->__find_c_from_i(&__p) == this,
                "unordered_set::insert(const_iterator, value_type&&) called with an iterator not"
                " referring to this unordered_set");
            return insert(_VSTD::move(__x)).first;
        }
#else
    iterator insert(const_iterator, value_type&& __x)
        {return insert(_VSTD::move(__x)).first;}
#endif
#endif  // _LIBCPP_HAS_NO_RVALUE_REFERENCES
    template <class _InputIterator>
        _LIBCPP_INLINE_VISIBILITY
        void insert(_InputIterator __first, _InputIterator __last);
#ifndef _LIBCPP_HAS_NO_GENERALIZED_INITIALIZERS
    _LIBCPP_INLINE_VISIBILITY
    void insert(initializer_list<value_type> __il)
        {insert(__il.begin(), __il.end());}
#endif  // _LIBCPP_HAS_NO_GENERALIZED_INITIALIZERS

    _LIBCPP_INLINE_VISIBILITY
    iterator erase(const_iterator __p) {return __table_.erase(__p);}
    _LIBCPP_INLINE_VISIBILITY
    size_type erase(const key_type& __k) {return __table_.__erase_unique(__k);}
    _LIBCPP_INLINE_VISIBILITY
    iterator erase(const_iterator __first, const_iterator __last)
        {return __table_.erase(__first, __last);}
    _LIBCPP_INLINE_VISIBILITY
    void clear() _NOEXCEPT {__table_.clear();}

    _LIBCPP_INLINE_VISIBILITY
    void swap(unordered_set& __u)
        _NOEXCEPT_(__is_nothrow_swappable<__table>::value)
        {__table_.swap(__u.__table_);}

    _LIBCPP_INLINE_VISIBILITY
    hasher hash_function() const {return __table_.hash_function();}
    _LIBCPP_INLINE_VISIBILITY
    key_equal key_eq() const {return __table_.key_eq();}

    _LIBCPP_INLINE_VISIBILITY
    iterator       find(const key_type& __k)       {return __table_.find(__k);}
    _LIBCPP_INLINE_VISIBILITY
    const_iterator find(const key_type& __k) const {return __table_.find(__k);}
    _LIBCPP_INLINE_VISIBILITY
    size_type count(const key_type& __k) const {return __table_.__count_unique(__k);}
    _LIBCPP_INLINE_VISIBILITY
    pair<iterator, iterator>             equal_range(const key_type& __k)
        {return __table_.__equal_range_unique(__k);}
    _LIBCPP_INLINE_VISIBILITY
    pair<const_iterator, const_iterator> equal_range(const key_type& __k) const
        {return __table_.__equal_range_unique(__k);}

    _LIBCPP_INLINE_VISIBILITY
    size_type bucket_count() const _NOEXCEPT {return __table_.bucket_count();}
    _LIBCPP_INLINE_VISIBILITY
    size_type max_bucket_count() const _NOEXCEPT {return __table_.max_bucket_count();}

    _LIBCPP_INLINE_VISIBILITY
    size_type bucket_size(size_type __n) const {return __table_.bucket_size(__n);}
    _LIBCPP_INLINE_VISIBILITY
    size_type bucket(const key_type& __k) const {return __table_.bucket(__k);}

    _LIBCPP_INLINE_VISIBILITY
    local_iterator       begin(size_type __n)        {return __table_.begin(__n);}
    _LIBCPP_INLINE_VISIBILITY
    local_iterator       end(size_type __n)          {return __table_.end(__n);}
    _LIBCPP_INLINE_VISIBILITY
    const_local_iterator begin(size_type __n) const  {return __table_.cbegin(__n);}
    _LIBCPP_INLINE_VISIBILITY
    const_local_iterator end(size_type __n) const    {return __table_.cend(__n);}
    _LIBCPP_INLINE_VISIBILITY
    const_local_iterator cbegin(size_type __n) const {return __table_.cbegin(__n);}
    _LIBCPP_INLINE_VISIBILITY
    const_local_iterator cend(size_type __n) const   {return __table_.cend(__n);}

    _LIBCPP_INLINE_VISIBILITY
    float load_factor() const _NOEXCEPT {return __table_.load_factor();}
    _LIBCPP_INLINE_VISIBILITY
    float max_load_factor() const _NOEXCEPT {return __table_.max_load_factor();}
    _LIBCPP_INLINE_VISIBILITY
    void max_load_factor(float __mlf) {__table_.max_load_factor(__mlf);}
    _LIBCPP_INLINE_VISIBILITY
    void rehash(size_type __n) {__table_.rehash(__n);}
    _LIBCPP_INLINE_VISIBILITY
    void reserve(size_type __n) {__table_.reserve(__n);}

#if _LIBCPP_DEBUG_LEVEL >= 2

    bool __dereferenceable(const const_iterator* __i) const
        {return __table_.__dereferenceable(__i);}
    bool __decrementable(const const_iterator* __i) const
        {return __table_.__decrementable(__i);}
    bool __addable(const const_iterator* __i, ptrdiff_t __n) const
        {return __table_.__addable(__i, __n);}
    bool __subscriptable(const const_iterator* __i, ptrdiff_t __n) const
        {return __table_.__addable(__i, __n);}

#endif  // _LIBCPP_DEBUG_LEVEL >= 2

};

template <class _Value, class _Hash, class _Pred, class _Alloc>
unordered_set<_Value, _Hash, _Pred, _Alloc>::unordered_set(size_type __n,
        const hasher& __hf, const key_equal& __eql)
    : __table_(__hf, __eql)
{
#if _LIBCPP_DEBUG_LEVEL >= 2
    __get_db()->__insert_c(this);
#endif
    __table_.rehash(__n);
}

template <class _Value, class _Hash, class _Pred, class _Alloc>
unordered_set<_Value, _Hash, _Pred, _Alloc>::unordered_set(size_type __n,
        const hasher& __hf, const key_equal& __eql, const allocator_type& __a)
    : __table_(__hf, __eql, __a)
{
#if _LIBCPP_DEBUG_LEVEL >= 2
    __get_db()->__insert_c(this);
#endif
    __table_.rehash(__n);
}

template <class _Value, class _Hash, class _Pred, class _Alloc>
template <class _InputIterator>
unordered_set<_Value, _Hash, _Pred, _Alloc>::unordered_set(
        _InputIterator __first, _InputIterator __last)
{
#if _LIBCPP_DEBUG_LEVEL >= 2
    __get_db()->__insert_c(this);
#endif
    insert(__first, __last);
}

template <class _Value, class _Hash, class _Pred, class _Alloc>
template <class _InputIterator>
unordered_set<_Value, _Hash, _Pred, _Alloc>::unordered_set(
        _InputIterator __first, _InputIterator __last, size_type __n,
        const hasher& __hf, const key_equal& __eql)
    : __table_(__hf, __eql)
{
#if _LIBCPP_DEBUG_LEVEL >= 2
    __get_db()->__insert_c(this);
#endif
    __table_.rehash(__n);
    insert(__first, __last);
}

template <class _Value, class _Hash, class _Pred, class _Alloc>
template <class _InputIterator>
unordered_set<_Value, _Hash, _Pred, _Alloc>::unordered_set(
        _InputIterator __first, _InputIterator __last, size_type __n,
        const hasher& __hf, const key_equal& __eql, const allocator_type& __a)
    : __table_(__hf, __eql, __a)
{
#if _LIBCPP_DEBUG_LEVEL >= 2
    __get_db()->__insert_c(this);
#endif
    __table_.rehash(__n);
    insert(__first, __last);
}

template <class _Value, class _Hash, class _Pred, class _Alloc>
inline
unordered_set<_Value, _Hash, _Pred, _Alloc>::unordered_set(
        const allocator_type& __a)
    : __table_(__a)
{
#if _LIBCPP_DEBUG_LEVEL >= 2
    __get_db()->__insert_c(this);
#endif
}

template <class _Value, class _Hash, class _Pred, class _Alloc>
unordered_set<_Value, _Hash, _Pred, _Alloc>::unordered_set(
        const unordered_set& __u)
    : __table_(__u.__table_)
{
#if _LIBCPP_DEBUG_LEVEL >= 2
    __get_db()->__insert_c(this);
#endif
    __table_.rehash(__u.bucket_count());
    insert(__u.begin(), __u.end());
}

template <class _Value, class _Hash, class _Pred, class _Alloc>
unordered_set<_Value, _Hash, _Pred, _Alloc>::unordered_set(
        const unordered_set& __u, const allocator_type& __a)
    : __table_(__u.__table_, __a)
{
#if _LIBCPP_DEBUG_LEVEL >= 2
    __get_db()->__insert_c(this);
#endif
    __table_.rehash(__u.bucket_count());
    insert(__u.begin(), __u.end());
}

#ifndef _LIBCPP_HAS_NO_RVALUE_REFERENCES

template <class _Value, class _Hash, class _Pred, class _Alloc>
inline
unordered_set<_Value, _Hash, _Pred, _Alloc>::unordered_set(
        unordered_set&& __u)
    _NOEXCEPT_(is_nothrow_move_constructible<__table>::value)
    : __table_(_VSTD::move(__u.__table_))
{
#if _LIBCPP_DEBUG_LEVEL >= 2
    __get_db()->__insert_c(this);
    __get_db()->swap(this, &__u);
#endif
}

template <class _Value, class _Hash, class _Pred, class _Alloc>
unordered_set<_Value, _Hash, _Pred, _Alloc>::unordered_set(
        unordered_set&& __u, const allocator_type& __a)
    : __table_(_VSTD::move(__u.__table_), __a)
{
#if _LIBCPP_DEBUG_LEVEL >= 2
    __get_db()->__insert_c(this);
#endif
    if (__a != __u.get_allocator())
    {
        iterator __i = __u.begin();
        while (__u.size() != 0)
            __table_.__insert_unique(_VSTD::move(__u.__table_.remove(__i++)->__value_));
    }
#if _LIBCPP_DEBUG_LEVEL >= 2
    else
        __get_db()->swap(this, &__u);
#endif
}

#endif  // _LIBCPP_HAS_NO_RVALUE_REFERENCES

#ifndef _LIBCPP_HAS_NO_GENERALIZED_INITIALIZERS

template <class _Value, class _Hash, class _Pred, class _Alloc>
unordered_set<_Value, _Hash, _Pred, _Alloc>::unordered_set(
        initializer_list<value_type> __il)
{
#if _LIBCPP_DEBUG_LEVEL >= 2
    __get_db()->__insert_c(this);
#endif
    insert(__il.begin(), __il.end());
}

template <class _Value, class _Hash, class _Pred, class _Alloc>
unordered_set<_Value, _Hash, _Pred, _Alloc>::unordered_set(
        initializer_list<value_type> __il, size_type __n, const hasher& __hf,
        const key_equal& __eql)
    : __table_(__hf, __eql)
{
#if _LIBCPP_DEBUG_LEVEL >= 2
    __get_db()->__insert_c(this);
#endif
    __table_.rehash(__n);
    insert(__il.begin(), __il.end());
}

template <class _Value, class _Hash, class _Pred, class _Alloc>
unordered_set<_Value, _Hash, _Pred, _Alloc>::unordered_set(
        initializer_list<value_type> __il, size_type __n, const hasher& __hf,
        const key_equal& __eql, const allocator_type& __a)
    : __table_(__hf, __eql, __a)
{
#if _LIBCPP_DEBUG_LEVEL >= 2
    __get_db()->__insert_c(this);
#endif
    __table_.rehash(__n);
    insert(__il.begin(), __il.end());
}

#endif  // _LIBCPP_HAS_NO_GENERALIZED_INITIALIZERS

#ifndef _LIBCPP_HAS_NO_RVALUE_REFERENCES

template <class _Value, class _Hash, class _Pred, class _Alloc>
inline
unordered_set<_Value, _Hash, _Pred, _Alloc>&
unordered_set<_Value, _Hash, _Pred, _Alloc>::operator=(unordered_set&& __u)
    _NOEXCEPT_(is_nothrow_move_assignable<__table>::value)
{
    __table_ = _VSTD::move(__u.__table_);
    return *this;
}

#endif  // _LIBCPP_HAS_NO_RVALUE_REFERENCES

#ifndef _LIBCPP_HAS_NO_GENERALIZED_INITIALIZERS

template <class _Value, class _Hash, class _Pred, class _Alloc>
inline
unordered_set<_Value, _Hash, _Pred, _Alloc>&
unordered_set<_Value, _Hash, _Pred, _Alloc>::operator=(
        initializer_list<value_type> __il)
{
    __table_.__assign_unique(__il.begin(), __il.end());
    return *this;
}

#endif  // _LIBCPP_HAS_NO_GENERALIZED_INITIALIZERS

template <class _Value, class _Hash, class _Pred, class _Alloc>
template <class _InputIterator>
inline
void
unordered_set<_Value, _Hash, _Pred, _Alloc>::insert(_InputIterator __first,
                                                    _InputIterator __last)
{
    for (; __first != __last; ++__first)
        __table_.__insert_unique(*__first);
}

template <class _Value, class _Hash, class _Pred, class _Alloc>
inline _LIBCPP_INLINE_VISIBILITY
void
swap(unordered_set<_Value, _Hash, _Pred, _Alloc>& __x,
     unordered_set<_Value, _Hash, _Pred, _Alloc>& __y)
    _NOEXCEPT_(_NOEXCEPT_(__x.swap(__y)))
{
    __x.swap(__y);
}

template <class _Value, class _Hash, class _Pred, class _Alloc>
bool
operator==(const unordered_set<_Value, _Hash, _Pred, _Alloc>& __x,
           const unordered_set<_Value, _Hash, _Pred, _Alloc>& __y)
{
    if (__x.size() != __y.size())
        return false;
    typedef typename unordered_set<_Value, _Hash, _Pred, _Alloc>::const_iterator
                                                                 const_iterator;
    for (const_iterator __i = __x.begin(), __ex = __x.end(), __ey = __y.end();
            __i != __ex; ++__i)
    {
        const_iterator __j = __y.find(*__i);
        if (__j == __ey || !(*__i == *__j))
            return false;
    }
    return true;
}

template <class _Value, class _Hash, class _Pred, class _Alloc>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator!=(const unordered_set<_Value, _Hash, _Pred, _Alloc>& __x,
           const unordered_set<_Value, _Hash, _Pred, _Alloc>& __y)
{
    return !(__x == __y);
}

template <class _Value, class _Hash = hash<_Value>, class _Pred = equal_to<_Value>,
          class _Alloc = allocator<_Value> >
class _LIBCPP_TEMPLATE_VIS unordered_multiset
{
public:
    // types
    typedef _Value                                                     key_type;
    typedef key_type                                                   value_type;
    typedef _Hash                                                      hasher;
    typedef _Pred                                                      key_equal;
    typedef _Alloc                                                     allocator_type;
    typedef value_type&                                                reference;
    typedef const value_type&                                          const_reference;
    static_assert((is_same<value_type, typename allocator_type::value_type>::value),
                  "Invalid allocator::value_type");

private:
    typedef __hash_table<value_type, hasher, key_equal, allocator_type> __table;

    __table __table_;

public:
    typedef typename __table::pointer         pointer;
    typedef typename __table::const_pointer   const_pointer;
    typedef typename __table::size_type       size_type;
    typedef typename __table::difference_type difference_type;

    typedef typename __table::const_iterator       iterator;
    typedef typename __table::const_iterator       const_iterator;
    typedef typename __table::const_local_iterator local_iterator;
    typedef typename __table::const_local_iterator const_local_iterator;

    _LIBCPP_INLINE_VISIBILITY
    unordered_multiset()
        _NOEXCEPT_(is_nothrow_default_constructible<__table>::value)
        {
#if _LIBCPP_DEBUG_LEVEL >= 2
            __get_db()->__insert_c(this);
#endif
        }
    explicit unordered_multiset(size_type __n, const hasher& __hf = hasher(),
                                const key_equal& __eql = key_equal());
    unordered_multiset(size_type __n, const hasher& __hf,
                       const key_equal& __eql, const allocator_type& __a);
#if _LIBCPP_STD_VER > 11
    inline _LIBCPP_INLINE_VISIBILITY
    unordered_multiset(size_type __n, const allocator_type& __a)
        : unordered_multiset(__n, hasher(), key_equal(), __a) {}
    inline _LIBCPP_INLINE_VISIBILITY
    unordered_multiset(size_type __n, const hasher& __hf, const allocator_type& __a)
        : unordered_multiset(__n, __hf, key_equal(), __a) {}
#endif
    template <class _InputIterator>
        unordered_multiset(_InputIterator __first, _InputIterator __last);
    template <class _InputIterator>
        unordered_multiset(_InputIterator __first, _InputIterator __last,
                      size_type __n, const hasher& __hf = hasher(),
                      const key_equal& __eql = key_equal());
    template <class _InputIterator>
        unordered_multiset(_InputIterator __first, _InputIterator __last,
                      size_type __n , const hasher& __hf,
                      const key_equal& __eql, const allocator_type& __a);
#if _LIBCPP_STD_VER > 11
    template <class _InputIterator>
    inline _LIBCPP_INLINE_VISIBILITY
    unordered_multiset(_InputIterator __first, _InputIterator __last, 
                       size_type __n, const allocator_type& __a)
        : unordered_multiset(__first, __last, __n, hasher(), key_equal(), __a) {}
    template <class _InputIterator>
    inline _LIBCPP_INLINE_VISIBILITY
    unordered_multiset(_InputIterator __first, _InputIterator __last,
                       size_type __n, const hasher& __hf, const allocator_type& __a)
        : unordered_multiset(__first, __last, __n, __hf, key_equal(), __a) {}
#endif
    _LIBCPP_INLINE_VISIBILITY
    explicit unordered_multiset(const allocator_type& __a);
    unordered_multiset(const unordered_multiset& __u);
    unordered_multiset(const unordered_multiset& __u, const allocator_type& __a);
#ifndef _LIBCPP_HAS_NO_RVALUE_REFERENCES
    _LIBCPP_INLINE_VISIBILITY
    unordered_multiset(unordered_multiset&& __u)
        _NOEXCEPT_(is_nothrow_move_constructible<__table>::value);
    unordered_multiset(unordered_multiset&& __u, const allocator_type& __a);
#endif  // _LIBCPP_HAS_NO_RVALUE_REFERENCES
#ifndef _LIBCPP_HAS_NO_GENERALIZED_INITIALIZERS
    unordered_multiset(initializer_list<value_type> __il);
    unordered_multiset(initializer_list<value_type> __il, size_type __n,
                       const hasher& __hf = hasher(),
                       const key_equal& __eql = key_equal());
    unordered_multiset(initializer_list<value_type> __il, size_type __n,
                       const hasher& __hf, const key_equal& __eql,
                       const allocator_type& __a);
#if _LIBCPP_STD_VER > 11
    inline _LIBCPP_INLINE_VISIBILITY
    unordered_multiset(initializer_list<value_type> __il, size_type __n, const allocator_type& __a)
      : unordered_multiset(__il, __n, hasher(), key_equal(), __a) {}
    inline _LIBCPP_INLINE_VISIBILITY
    unordered_multiset(initializer_list<value_type> __il, size_type __n, const hasher& __hf, const allocator_type& __a)
      : unordered_multiset(__il, __n, __hf, key_equal(), __a) {}
#endif
#endif  // _LIBCPP_HAS_NO_GENERALIZED_INITIALIZERS
    // ~unordered_multiset() = default;
    _LIBCPP_INLINE_VISIBILITY
    unordered_multiset& operator=(const unordered_multiset& __u)
    {
        __table_ = __u.__table_;
        return *this;
    }
#ifndef _LIBCPP_HAS_NO_RVALUE_REFERENCES
    _LIBCPP_INLINE_VISIBILITY
    unordered_multiset& operator=(unordered_multiset&& __u)
        _NOEXCEPT_(is_nothrow_move_assignable<__table>::value);
#endif
#ifndef _LIBCPP_HAS_NO_GENERALIZED_INITIALIZERS
    unordered_multiset& operator=(initializer_list<value_type> __il);
#endif  // _LIBCPP_HAS_NO_GENERALIZED_INITIALIZERS

    _LIBCPP_INLINE_VISIBILITY
    allocator_type get_allocator() const _NOEXCEPT
        {return allocator_type(__table_.__node_alloc());}

    _LIBCPP_INLINE_VISIBILITY
    bool      empty() const _NOEXCEPT {return __table_.size() == 0;}
    _LIBCPP_INLINE_VISIBILITY
    size_type size() const _NOEXCEPT  {return __table_.size();}
    _LIBCPP_INLINE_VISIBILITY
    size_type max_size() const _NOEXCEPT {return __table_.max_size();}

    _LIBCPP_INLINE_VISIBILITY
    iterator       begin() _NOEXCEPT        {return __table_.begin();}
    _LIBCPP_INLINE_VISIBILITY
    iterator       end() _NOEXCEPT          {return __table_.end();}
    _LIBCPP_INLINE_VISIBILITY
    const_iterator begin()  const _NOEXCEPT {return __table_.begin();}
    _LIBCPP_INLINE_VISIBILITY
    const_iterator end()    const _NOEXCEPT {return __table_.end();}
    _LIBCPP_INLINE_VISIBILITY
    const_iterator cbegin() const _NOEXCEPT {return __table_.begin();}
    _LIBCPP_INLINE_VISIBILITY
    const_iterator cend()   const _NOEXCEPT {return __table_.end();}

#if !defined(_LIBCPP_HAS_NO_RVALUE_REFERENCES) && !defined(_LIBCPP_HAS_NO_VARIADICS)
    template <class... _Args>
        _LIBCPP_INLINE_VISIBILITY
        iterator emplace(_Args&&... __args)
            {return __table_.__emplace_multi(_VSTD::forward<_Args>(__args)...);}
    template <class... _Args>
        _LIBCPP_INLINE_VISIBILITY
        iterator emplace_hint(const_iterator __p, _Args&&... __args)
            {return __table_.__emplace_hint_multi(__p, _VSTD::forward<_Args>(__args)...);}
#endif  // !defined(_LIBCPP_HAS_NO_RVALUE_REFERENCES) && !defined(_LIBCPP_HAS_NO_VARIADICS)
    _LIBCPP_INLINE_VISIBILITY
    iterator insert(const value_type& __x) {return __table_.__insert_multi(__x);}
#ifndef _LIBCPP_HAS_NO_RVALUE_REFERENCES
    _LIBCPP_INLINE_VISIBILITY
    iterator insert(value_type&& __x) {return __table_.__insert_multi(_VSTD::move(__x));}
#endif
    _LIBCPP_INLINE_VISIBILITY
    iterator insert(const_iterator __p, const value_type& __x)
        {return __table_.__insert_multi(__p, __x);}
#ifndef _LIBCPP_HAS_NO_RVALUE_REFERENCES
    _LIBCPP_INLINE_VISIBILITY
    iterator insert(const_iterator __p, value_type&& __x)
        {return __table_.__insert_multi(__p, _VSTD::move(__x));}
#endif  // _LIBCPP_HAS_NO_RVALUE_REFERENCES
    template <class _InputIterator>
        _LIBCPP_INLINE_VISIBILITY
        void insert(_InputIterator __first, _InputIterator __last);
#ifndef _LIBCPP_HAS_NO_GENERALIZED_INITIALIZERS
    _LIBCPP_INLINE_VISIBILITY
    void insert(initializer_list<value_type> __il)
        {insert(__il.begin(), __il.end());}
#endif  // _LIBCPP_HAS_NO_GENERALIZED_INITIALIZERS

    _LIBCPP_INLINE_VISIBILITY
    iterator erase(const_iterator __p) {return __table_.erase(__p);}
    _LIBCPP_INLINE_VISIBILITY
    size_type erase(const key_type& __k) {return __table_.__erase_multi(__k);}
    _LIBCPP_INLINE_VISIBILITY
    iterator erase(const_iterator __first, const_iterator __last)
        {return __table_.erase(__first, __last);}
    _LIBCPP_INLINE_VISIBILITY
    void clear() _NOEXCEPT {__table_.clear();}

    _LIBCPP_INLINE_VISIBILITY
    void swap(unordered_multiset& __u)
        _NOEXCEPT_(__is_nothrow_swappable<__table>::value)
        {__table_.swap(__u.__table_);}

    _LIBCPP_INLINE_VISIBILITY
    hasher hash_function() const {return __table_.hash_function();}
    _LIBCPP_INLINE_VISIBILITY
    key_equal key_eq() const {return __table_.key_eq();}

    _LIBCPP_INLINE_VISIBILITY
    iterator       find(const key_type& __k)       {return __table_.find(__k);}
    _LIBCPP_INLINE_VISIBILITY
    const_iterator find(const key_type& __k) const {return __table_.find(__k);}
    _LIBCPP_INLINE_VISIBILITY
    size_type count(const key_type& __k) const {return __table_.__count_multi(__k);}
    _LIBCPP_INLINE_VISIBILITY
    pair<iterator, iterator>             equal_range(const key_type& __k)
        {return __table_.__equal_range_multi(__k);}
    _LIBCPP_INLINE_VISIBILITY
    pair<const_iterator, const_iterator> equal_range(const key_type& __k) const
        {return __table_.__equal_range_multi(__k);}

    _LIBCPP_INLINE_VISIBILITY
    size_type bucket_count() const _NOEXCEPT {return __table_.bucket_count();}
    _LIBCPP_INLINE_VISIBILITY
    size_type max_bucket_count() const _NOEXCEPT {return __table_.max_bucket_count();}

    _LIBCPP_INLINE_VISIBILITY
    size_type bucket_size(size_type __n) const {return __table_.bucket_size(__n);}
    _LIBCPP_INLINE_VISIBILITY
    size_type bucket(const key_type& __k) const {return __table_.bucket(__k);}

    _LIBCPP_INLINE_VISIBILITY
    local_iterator       begin(size_type __n)        {return __table_.begin(__n);}
    _LIBCPP_INLINE_VISIBILITY
    local_iterator       end(size_type __n)          {return __table_.end(__n);}
    _LIBCPP_INLINE_VISIBILITY
    const_local_iterator begin(size_type __n) const  {return __table_.cbegin(__n);}
    _LIBCPP_INLINE_VISIBILITY
    const_local_iterator end(size_type __n) const    {return __table_.cend(__n);}
    _LIBCPP_INLINE_VISIBILITY
    const_local_iterator cbegin(size_type __n) const {return __table_.cbegin(__n);}
    _LIBCPP_INLINE_VISIBILITY
    const_local_iterator cend(size_type __n) const   {return __table_.cend(__n);}

    _LIBCPP_INLINE_VISIBILITY
    float load_factor() const _NOEXCEPT {return __table_.load_factor();}
    _LIBCPP_INLINE_VISIBILITY
    float max_load_factor() const _NOEXCEPT {return __table_.max_load_factor();}
    _LIBCPP_INLINE_VISIBILITY
    void max_load_factor(float __mlf) {__table_.max_load_factor(__mlf);}
    _LIBCPP_INLINE_VISIBILITY
    void rehash(size_type __n) {__table_.rehash(__n);}
    _LIBCPP_INLINE_VISIBILITY
    void reserve(size_type __n) {__table_.reserve(__n);}

#if _LIBCPP_DEBUG_LEVEL >= 2

    bool __dereferenceable(const const_iterator* __i) const
        {return __table_.__dereferenceable(__i);}
    bool __decrementable(const const_iterator* __i) const
        {return __table_.__decrementable(__i);}
    bool __addable(const const_iterator* __i, ptrdiff_t __n) const
        {return __table_.__addable(__i, __n);}
    bool __subscriptable(const const_iterator* __i, ptrdiff_t __n) const
        {return __table_.__addable(__i, __n);}

#endif  // _LIBCPP_DEBUG_LEVEL >= 2

};

template <class _Value, class _Hash, class _Pred, class _Alloc>
unordered_multiset<_Value, _Hash, _Pred, _Alloc>::unordered_multiset(
        size_type __n, const hasher& __hf, const key_equal& __eql)
    : __table_(__hf, __eql)
{
#if _LIBCPP_DEBUG_LEVEL >= 2
    __get_db()->__insert_c(this);
#endif
    __table_.rehash(__n);
}

template <class _Value, class _Hash, class _Pred, class _Alloc>
unordered_multiset<_Value, _Hash, _Pred, _Alloc>::unordered_multiset(
        size_type __n, const hasher& __hf, const key_equal& __eql,
        const allocator_type& __a)
    : __table_(__hf, __eql, __a)
{
#if _LIBCPP_DEBUG_LEVEL >= 2
    __get_db()->__insert_c(this);
#endif
    __table_.rehash(__n);
}

template <class _Value, class _Hash, class _Pred, class _Alloc>
template <class _InputIterator>
unordered_multiset<_Value, _Hash, _Pred, _Alloc>::unordered_multiset(
        _InputIterator __first, _InputIterator __last)
{
#if _LIBCPP_DEBUG_LEVEL >= 2
    __get_db()->__insert_c(this);
#endif
    insert(__first, __last);
}

template <class _Value, class _Hash, class _Pred, class _Alloc>
template <class _InputIterator>
unordered_multiset<_Value, _Hash, _Pred, _Alloc>::unordered_multiset(
        _InputIterator __first, _InputIterator __last, size_type __n,
        const hasher& __hf, const key_equal& __eql)
    : __table_(__hf, __eql)
{
#if _LIBCPP_DEBUG_LEVEL >= 2
    __get_db()->__insert_c(this);
#endif
    __table_.rehash(__n);
    insert(__first, __last);
}

template <class _Value, class _Hash, class _Pred, class _Alloc>
template <class _InputIterator>
unordered_multiset<_Value, _Hash, _Pred, _Alloc>::unordered_multiset(
        _InputIterator __first, _InputIterator __last, size_type __n,
        const hasher& __hf, const key_equal& __eql, const allocator_type& __a)
    : __table_(__hf, __eql, __a)
{
#if _LIBCPP_DEBUG_LEVEL >= 2
    __get_db()->__insert_c(this);
#endif
    __table_.rehash(__n);
    insert(__first, __last);
}

template <class _Value, class _Hash, class _Pred, class _Alloc>
inline
unordered_multiset<_Value, _Hash, _Pred, _Alloc>::unordered_multiset(
        const allocator_type& __a)
    : __table_(__a)
{
#if _LIBCPP_DEBUG_LEVEL >= 2
    __get_db()->__insert_c(this);
#endif
}

template <class _Value, class _Hash, class _Pred, class _Alloc>
unordered_multiset<_Value, _Hash, _Pred, _Alloc>::unordered_multiset(
        const unordered_multiset& __u)
    : __table_(__u.__table_)
{
#if _LIBCPP_DEBUG_LEVEL >= 2
    __get_db()->__insert_c(this);
#endif
    __table_.rehash(__u.bucket_count());
    insert(__u.begin(), __u.end());
}

template <class _Value, class _Hash, class _Pred, class _Alloc>
unordered_multiset<_Value, _Hash, _Pred, _Alloc>::unordered_multiset(
        const unordered_multiset& __u, const allocator_type& __a)
    : __table_(__u.__table_, __a)
{
#if _LIBCPP_DEBUG_LEVEL >= 2
    __get_db()->__insert_c(this);
#endif
    __table_.rehash(__u.bucket_count());
    insert(__u.begin(), __u.end());
}

#ifndef _LIBCPP_HAS_NO_RVALUE_REFERENCES

template <class _Value, class _Hash, class _Pred, class _Alloc>
inline
unordered_multiset<_Value, _Hash, _Pred, _Alloc>::unordered_multiset(
        unordered_multiset&& __u)
    _NOEXCEPT_(is_nothrow_move_constructible<__table>::value)
    : __table_(_VSTD::move(__u.__table_))
{
#if _LIBCPP_DEBUG_LEVEL >= 2
    __get_db()->__insert_c(this);
    __get_db()->swap(this, &__u);
#endif
}

template <class _Value, class _Hash, class _Pred, class _Alloc>
unordered_multiset<_Value, _Hash, _Pred, _Alloc>::unordered_multiset(
        unordered_multiset&& __u, const allocator_type& __a)
    : __table_(_VSTD::move(__u.__table_), __a)
{
#if _LIBCPP_DEBUG_LEVEL >= 2
    __get_db()->__insert_c(this);
#endif
    if (__a != __u.get_allocator())
    {
        iterator __i = __u.begin();
        while (__u.size() != 0)
            __table_.__insert_multi(_VSTD::move(__u.__table_.remove(__i++)->__value_));
    }
#if _LIBCPP_DEBUG_LEVEL >= 2
    else
        __get_db()->swap(this, &__u);
#endif
}

#endif  // _LIBCPP_HAS_NO_RVALUE_REFERENCES

#ifndef _LIBCPP_HAS_NO_GENERALIZED_INITIALIZERS

template <class _Value, class _Hash, class _Pred, class _Alloc>
unordered_multiset<_Value, _Hash, _Pred, _Alloc>::unordered_multiset(
        initializer_list<value_type> __il)
{
#if _LIBCPP_DEBUG_LEVEL >= 2
    __get_db()->__insert_c(this);
#endif
    insert(__il.begin(), __il.end());
}

template <class _Value, class _Hash, class _Pred, class _Alloc>
unordered_multiset<_Value, _Hash, _Pred, _Alloc>::unordered_multiset(
        initializer_list<value_type> __il, size_type __n, const hasher& __hf,
        const key_equal& __eql)
    : __table_(__hf, __eql)
{
#if _LIBCPP_DEBUG_LEVEL >= 2
    __get_db()->__insert_c(this);
#endif
    __table_.rehash(__n);
    insert(__il.begin(), __il.end());
}

template <class _Value, class _Hash, class _Pred, class _Alloc>
unordered_multiset<_Value, _Hash, _Pred, _Alloc>::unordered_multiset(
        initializer_list<value_type> __il, size_type __n, const hasher& __hf,
        const key_equal& __eql, const allocator_type& __a)
    : __table_(__hf, __eql, __a)
{
#if _LIBCPP_DEBUG_LEVEL >= 2
    __get_db()->__insert_c(this);
#endif
    __table_.rehash(__n);
    insert(__il.begin(), __il.end());
}

#endif  // _LIBCPP_HAS_NO_GENERALIZED_INITIALIZERS

#ifndef _LIBCPP_HAS_NO_RVALUE_REFERENCES

template <class _Value, class _Hash, class _Pred, class _Alloc>
inline
unordered_multiset<_Value, _Hash, _Pred, _Alloc>&
unordered_multiset<_Value, _Hash, _Pred, _Alloc>::operator=(
        unordered_multiset&& __u)
    _NOEXCEPT_(is_nothrow_move_assignable<__table>::value)
{
    __table_ = _VSTD::move(__u.__table_);
    return *this;
}

#endif  // _LIBCPP_HAS_NO_RVALUE_REFERENCES

#ifndef _LIBCPP_HAS_NO_GENERALIZED_INITIALIZERS

template <class _Value, class _Hash, class _Pred, class _Alloc>
inline
unordered_multiset<_Value, _Hash, _Pred, _Alloc>&
unordered_multiset<_Value, _Hash, _Pred, _Alloc>::operator=(
        initializer_list<value_type> __il)
{
    __table_.__assign_multi(__il.begin(), __il.end());
    return *this;
}

#endif  // _LIBCPP_HAS_NO_GENERALIZED_INITIALIZERS

template <class _Value, class _Hash, class _Pred, class _Alloc>
template <class _InputIterator>
inline
void
unordered_multiset<_Value, _Hash, _Pred, _Alloc>::insert(_InputIterator __first,
                                                         _InputIterator __last)
{
    for (; __first != __last; ++__first)
        __table_.__insert_multi(*__first);
}

template <class _Value, class _Hash, class _Pred, class _Alloc>
inline _LIBCPP_INLINE_VISIBILITY
void
swap(unordered_multiset<_Value, _Hash, _Pred, _Alloc>& __x,
     unordered_multiset<_Value, _Hash, _Pred, _Alloc>& __y)
    _NOEXCEPT_(_NOEXCEPT_(__x.swap(__y)))
{
    __x.swap(__y);
}

template <class _Value, class _Hash, class _Pred, class _Alloc>
bool
operator==(const unordered_multiset<_Value, _Hash, _Pred, _Alloc>& __x,
           const unordered_multiset<_Value, _Hash, _Pred, _Alloc>& __y)
{
    if (__x.size() != __y.size())
        return false;
    typedef typename unordered_multiset<_Value, _Hash, _Pred, _Alloc>::const_iterator
                                                                 const_iterator;
    typedef pair<const_iterator, const_iterator> _EqRng;
    for (const_iterator __i = __x.begin(), __ex = __x.end(); __i != __ex;)
    {
        _EqRng __xeq = __x.equal_range(*__i);
        _EqRng __yeq = __y.equal_range(*__i);
        if (_VSTD::distance(__xeq.first, __xeq.second) !=
            _VSTD::distance(__yeq.first, __yeq.second) ||
                  !_VSTD::is_permutation(__xeq.first, __xeq.second, __yeq.first))
            return false;
        __i = __xeq.second;
    }
    return true;
}

template <class _Value, class _Hash, class _Pred, class _Alloc>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator!=(const unordered_multiset<_Value, _Hash, _Pred, _Alloc>& __x,
           const unordered_multiset<_Value, _Hash, _Pred, _Alloc>& __y)
{
    return !(__x == __y);
}

_LIBCPP_END_NAMESPACE_STD

#endif  // _LIBCPP_UNORDERED_SET
