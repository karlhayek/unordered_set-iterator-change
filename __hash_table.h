// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP__HASH_TABLE
#define _LIBCPP__HASH_TABLE

#include <__config>
#include <initializer_list>
#include <memory>
#include <iterator>
#include <algorithm>
#include <cmath>
#include <utility>

#include <__undef_min_max>

#include <__debug>
#include <__hash_table>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_BEGIN_NAMESPACE_STD


#ifndef _LIBCPP_CXX03_LANG
        template <class _Key, class _Tp>
        union __hash_value_type;
#else
        template <class _Key, class _Tp>
struct __hash_value_type;
#endif

#ifndef _LIBCPP_CXX03_LANG
        template <class _Tp>
        struct __is_hash_value_type_imp : false_type {};

        template <class _Key, class _Value>
        struct __is_hash_value_type_imp<__hash_value_type<_Key, _Value>> : true_type {};

        template <class ..._Args>
        struct __is_hash_value_type : false_type {};

        template <class _One>
        struct __is_hash_value_type<_One> : __is_hash_value_type_imp<typename __uncvref<_One>::type> {};
#endif

        _LIBCPP_FUNC_VIS
        size_t __next_prime(size_t __n);

        template <class _NodePtr>
        struct __hash_node_base
        {
            typedef typename pointer_traits<_NodePtr>::element_type __node_type;
            typedef __hash_node_base __first_node;
            typedef typename __rebind_pointer<_NodePtr, __first_node>::type __node_base_pointer;
            typedef _NodePtr __node_pointer;

#if defined(_LIBCPP_ABI_FIX_UNORDERED_NODE_POINTER_UB)
            typedef __node_base_pointer __next_pointer;
#else
            typedef typename conditional<
                    is_pointer<__node_pointer>::value,
                    __node_base_pointer,
                    __node_pointer>::type   __next_pointer;
#endif

            __next_pointer    __next_;

            _LIBCPP_INLINE_VISIBILITY
            __next_pointer __ptr() _NOEXCEPT {
                return static_cast<__next_pointer>(
                        pointer_traits<__node_base_pointer>::pointer_to(*this));
            }

            _LIBCPP_INLINE_VISIBILITY
            __node_pointer __upcast() _NOEXCEPT {
                return static_cast<__node_pointer>(
                        pointer_traits<__node_base_pointer>::pointer_to(*this));
            }

            _LIBCPP_INLINE_VISIBILITY
            size_t __hash() const _NOEXCEPT {
                return static_cast<__node_type const&>(*this).__hash_;
            }

            _LIBCPP_INLINE_VISIBILITY __hash_node_base() _NOEXCEPT : __next_(nullptr) {}
        };

        template <class _Tp, class _VoidPtr>
        struct __hash_node
                : public __hash_node_base
                        <
                                typename __rebind_pointer<_VoidPtr, __hash_node<_Tp, _VoidPtr> >::type
                        >
        {
            typedef _Tp __node_value_type;

            size_t            __hash_;
            __node_value_type __value_;
        };

        inline _LIBCPP_INLINE_VISIBILITY
        bool
        __is_hash_power2(size_t __bc)
        {
            return __bc > 2 && !(__bc & (__bc - 1));
        }

        inline _LIBCPP_INLINE_VISIBILITY
        size_t
        __constrain_hash(size_t __h, size_t __bc)
        {
            return !(__bc & (__bc - 1)) ? __h & (__bc - 1) :
                   (__h < __bc ? __h : __h % __bc);
        }

        inline _LIBCPP_INLINE_VISIBILITY
        size_t
        __next_hash_pow2(size_t __n)
        {
            return (__n > 1) ? (size_t(1) << (std::numeric_limits<size_t>::digits - __clz(__n-1))) : __n;
        }


        template <class _Tp, class _Hash, class _Equal, class _Alloc> class __hash_table;

        template <class _NodePtr>      class _LIBCPP_TEMPLATE_VIS __hash_iterator;
        template <class _ConstNodePtr> class _LIBCPP_TEMPLATE_VIS __hash_const_iterator;
        template <class _NodePtr>      class _LIBCPP_TEMPLATE_VIS __hash_local_iterator;
        template <class _ConstNodePtr> class _LIBCPP_TEMPLATE_VIS __hash_const_local_iterator;
        template <class _HashIterator> class _LIBCPP_TEMPLATE_VIS __hash_map_iterator;
        template <class _HashIterator> class _LIBCPP_TEMPLATE_VIS __hash_map_const_iterator;

        template <class _Tp>
        struct __hash_key_value_types {
            static_assert(!is_reference<_Tp>::value && !is_const<_Tp>::value, "");
            typedef _Tp key_type;
            typedef _Tp __node_value_type;
            typedef _Tp __container_value_type;
            static const bool __is_map = false;

            _LIBCPP_INLINE_VISIBILITY
            static key_type const& __get_key(_Tp const& __v) {
                return __v;
            }
            _LIBCPP_INLINE_VISIBILITY
            static __container_value_type const& __get_value(__node_value_type const& __v) {
                return __v;
            }
            _LIBCPP_INLINE_VISIBILITY
            static __container_value_type* __get_ptr(__node_value_type& __n) {
                return _VSTD::addressof(__n);
            }
#ifndef _LIBCPP_CXX03_LANG
            _LIBCPP_INLINE_VISIBILITY
            static  __container_value_type&& __move(__node_value_type& __v) {
                return _VSTD::move(__v);
            }
#endif
        };

        template <class _Key, class _Tp>
        struct __hash_key_value_types<__hash_value_type<_Key, _Tp> > {
            typedef _Key                                         key_type;
            typedef _Tp                                          mapped_type;
            typedef __hash_value_type<_Key, _Tp>                 __node_value_type;
            typedef pair<const _Key, _Tp>                        __container_value_type;
            typedef pair<_Key, _Tp>                              __nc_value_type;
            typedef __container_value_type                       __map_value_type;
            static const bool __is_map = true;

            _LIBCPP_INLINE_VISIBILITY
            static key_type const& __get_key(__container_value_type const& __v) {
                return __v.first;
            }

            template <class _Up>
            _LIBCPP_INLINE_VISIBILITY
            static typename enable_if<__is_same_uncvref<_Up, __node_value_type>::value,
                    __container_value_type const&>::type
            __get_value(_Up& __t) {
                return __t.__cc;
            }

            template <class _Up>
            _LIBCPP_INLINE_VISIBILITY
            static typename enable_if<__is_same_uncvref<_Up, __container_value_type>::value,
                    __container_value_type const&>::type
            __get_value(_Up& __t) {
                return __t;
            }

            _LIBCPP_INLINE_VISIBILITY
            static __container_value_type* __get_ptr(__node_value_type& __n) {
                return _VSTD::addressof(__n.__cc);
            }
#ifndef _LIBCPP_CXX03_LANG
            _LIBCPP_INLINE_VISIBILITY
            static __nc_value_type&& __move(__node_value_type& __v) {
                return _VSTD::move(__v.__nc);
            }
#endif

        };

        template <class _Tp, class _AllocPtr, class _KVTypes = __hash_key_value_types<_Tp>,
                bool = _KVTypes::__is_map>
        struct __hash_map_pointer_types {};

        template <class _Tp, class _AllocPtr, class _KVTypes>
        struct __hash_map_pointer_types<_Tp, _AllocPtr, _KVTypes, true> {
            typedef typename _KVTypes::__map_value_type   _Mv;
            typedef typename __rebind_pointer<_AllocPtr, _Mv>::type
                    __map_value_type_pointer;
            typedef typename __rebind_pointer<_AllocPtr, const _Mv>::type
                    __const_map_value_type_pointer;
        };

        template <class _NodePtr, class _NodeT = typename pointer_traits<_NodePtr>::element_type>
        struct __hash_node_types;

        template <class _NodePtr, class _Tp, class _VoidPtr>
        struct __hash_node_types<_NodePtr, __hash_node<_Tp, _VoidPtr> >
                : public __hash_key_value_types<_Tp>, __hash_map_pointer_types<_Tp, _VoidPtr>

        {
            typedef __hash_key_value_types<_Tp>           __base;

        public:
            typedef ptrdiff_t difference_type;
            typedef size_t size_type;

            typedef typename __rebind_pointer<_NodePtr, void>::type       __void_pointer;

            typedef typename pointer_traits<_NodePtr>::element_type       __node_type;
            typedef _NodePtr                                              __node_pointer;

            typedef __hash_node_base<__node_pointer>                      __node_base_type;
            typedef typename __rebind_pointer<_NodePtr, __node_base_type>::type
                    __node_base_pointer;

            typedef typename __node_base_type::__next_pointer          __next_pointer;

            typedef _Tp                                                 __node_value_type;
            typedef typename __rebind_pointer<_VoidPtr, __node_value_type>::type
                    __node_value_type_pointer;
            typedef typename __rebind_pointer<_VoidPtr, const __node_value_type>::type
                    __const_node_value_type_pointer;

        private:
            static_assert(!is_const<__node_type>::value,
                          "_NodePtr should never be a pointer to const");
            static_assert((is_same<typename pointer_traits<_VoidPtr>::element_type, void>::value),
                          "_VoidPtr does not point to unqualified void type");
            static_assert((is_same<typename __rebind_pointer<_VoidPtr, __node_type>::type,
                    _NodePtr>::value), "_VoidPtr does not rebind to _NodePtr.");
        };

        template <class _HashIterator>
        struct __hash_node_types_from_iterator;
        template <class _NodePtr>
        struct __hash_node_types_from_iterator<__hash_iterator<_NodePtr> > : __hash_node_types<_NodePtr> {};
        template <class _NodePtr>
        struct __hash_node_types_from_iterator<__hash_const_iterator<_NodePtr> > : __hash_node_types<_NodePtr> {};
        template <class _NodePtr>
        struct __hash_node_types_from_iterator<__hash_local_iterator<_NodePtr> > : __hash_node_types<_NodePtr> {};
        template <class _NodePtr>
        struct __hash_node_types_from_iterator<__hash_const_local_iterator<_NodePtr> > : __hash_node_types<_NodePtr> {};


        template <class _NodeValueTp, class _VoidPtr>
        struct __make_hash_node_types {
            typedef __hash_node<_NodeValueTp, _VoidPtr> _NodeTp;
            typedef typename __rebind_pointer<_VoidPtr, _NodeTp>::type _NodePtr;
            typedef __hash_node_types<_NodePtr> type;
        };

        template <class _NodePtr>
        class _LIBCPP_TEMPLATE_VIS __hash_iterator
        {
            typedef __hash_node_types<_NodePtr> _NodeTypes;
            typedef _NodePtr                            __node_pointer;
            typedef typename _NodeTypes::__next_pointer __next_pointer;

            __next_pointer            __node_;

        public:
            typedef forward_iterator_tag                           iterator_category;
            typedef typename _NodeTypes::__node_value_type         value_type;
            typedef typename _NodeTypes::difference_type           difference_type;
            typedef value_type&                                    reference;
            typedef typename _NodeTypes::__node_value_type_pointer pointer;

            _LIBCPP_INLINE_VISIBILITY __hash_iterator() _NOEXCEPT : __node_(nullptr) {
                _LIBCPP_DEBUG_MODE(__get_db()->__insert_i(this));
            }

#if _LIBCPP_DEBUG_LEVEL >= 2
            _LIBCPP_INLINE_VISIBILITY
    __hash_iterator(const __hash_iterator& __i)
        : __node_(__i.__node_)
    {
        __get_db()->__iterator_copy(this, &__i);
    }

    _LIBCPP_INLINE_VISIBILITY
    ~__hash_iterator()
    {
        __get_db()->__erase_i(this);
    }

    _LIBCPP_INLINE_VISIBILITY
    __hash_iterator& operator=(const __hash_iterator& __i)
    {
        if (this != &__i)
        {
            __get_db()->__iterator_copy(this, &__i);
            __node_ = __i.__node_;
        }
        return *this;
    }
#endif  // _LIBCPP_DEBUG_LEVEL >= 2

            _LIBCPP_INLINE_VISIBILITY
            reference operator*() const {
                _LIBCPP_DEBUG_ASSERT(__get_const_db()->__dereferenceable(this),
                                     "Attempted to dereference a non-dereferenceable unordered container iterator");
                return __node_->__upcast()->__value_;
            }

            _LIBCPP_INLINE_VISIBILITY
            pointer operator->() const {
                _LIBCPP_DEBUG_ASSERT(__get_const_db()->__dereferenceable(this),
                                     "Attempted to dereference a non-dereferenceable unordered container iterator");
                return pointer_traits<pointer>::pointer_to(__node_->__upcast()->__value_);
            }

            _LIBCPP_INLINE_VISIBILITY
            __hash_iterator& operator++() {
                _LIBCPP_DEBUG_ASSERT(__get_const_db()->__dereferenceable(this),
                                     "Attempted to increment non-incrementable unordered container iterator");
                __node_ = __node_->__next_;
                return *this;
            }

            _LIBCPP_INLINE_VISIBILITY
            __hash_iterator operator++(int)
            {
                __hash_iterator __t(*this);
                ++(*this);
                return __t;
            }

            friend _LIBCPP_INLINE_VISIBILITY
            bool operator==(const __hash_iterator& __x, const __hash_iterator& __y)
            {
                return __x.__node_ == __y.__node_;
            }
            friend _LIBCPP_INLINE_VISIBILITY
            bool operator!=(const __hash_iterator& __x, const __hash_iterator& __y)
            {return !(__x == __y);}


            // START ADDED CODE
            _LIBCPP_INLINE_VISIBILITY
            __next_pointer getNode() {
                return __node_;
            }
            _LIBCPP_INLINE_VISIBILITY
            void setNext(__hash_iterator next_it) {
                __node_->__next_ = next_it.getNode();
            }
            // END CODE


        private:
#if _LIBCPP_DEBUG_LEVEL >= 2
            _LIBCPP_INLINE_VISIBILITY
    __hash_iterator(__next_pointer __node, const void* __c) _NOEXCEPT
        : __node_(__node)
        {
            __get_db()->__insert_ic(this, __c);
        }
#else
            _LIBCPP_INLINE_VISIBILITY
            __hash_iterator(__next_pointer __node) _NOEXCEPT
                    : __node_(__node)
            {}
#endif
            template <class, class, class, class> friend class __hash_table;
            template <class> friend class _LIBCPP_TEMPLATE_VIS __hash_const_iterator;
            template <class> friend class _LIBCPP_TEMPLATE_VIS __hash_map_iterator;
            template <class, class, class, class, class> friend class _LIBCPP_TEMPLATE_VIS unordered_map;
            template <class, class, class, class, class> friend class _LIBCPP_TEMPLATE_VIS unordered_multimap;
        };

        template <class _NodePtr>
        class _LIBCPP_TEMPLATE_VIS __hash_const_iterator
        {
            static_assert(!is_const<typename pointer_traits<_NodePtr>::element_type>::value, "");
            typedef __hash_node_types<_NodePtr> _NodeTypes;
            typedef _NodePtr                            __node_pointer;
            typedef typename _NodeTypes::__next_pointer __next_pointer;

            __next_pointer __node_;

        public:
            typedef __hash_iterator<_NodePtr> __non_const_iterator;

            typedef forward_iterator_tag                                 iterator_category;
            typedef typename _NodeTypes::__node_value_type               value_type;
            typedef typename _NodeTypes::difference_type                 difference_type;
            typedef const value_type&                                    reference;
            typedef typename _NodeTypes::__const_node_value_type_pointer pointer;


            _LIBCPP_INLINE_VISIBILITY __hash_const_iterator() _NOEXCEPT : __node_(nullptr) {
                _LIBCPP_DEBUG_MODE(__get_db()->__insert_i(this));
            }

            _LIBCPP_INLINE_VISIBILITY
            __hash_const_iterator(const __non_const_iterator& __x) _NOEXCEPT
                    : __node_(__x.__node_)
            {
                _LIBCPP_DEBUG_MODE(__get_db()->__iterator_copy(this, &__x));
            }

#if _LIBCPP_DEBUG_LEVEL >= 2
            _LIBCPP_INLINE_VISIBILITY
    __hash_const_iterator(const __hash_const_iterator& __i)
        : __node_(__i.__node_)
    {
        __get_db()->__iterator_copy(this, &__i);
    }

    _LIBCPP_INLINE_VISIBILITY
    ~__hash_const_iterator()
    {
        __get_db()->__erase_i(this);
    }

    _LIBCPP_INLINE_VISIBILITY
    __hash_const_iterator& operator=(const __hash_const_iterator& __i)
    {
        if (this != &__i)
        {
            __get_db()->__iterator_copy(this, &__i);
            __node_ = __i.__node_;
        }
        return *this;
    }
#endif  // _LIBCPP_DEBUG_LEVEL >= 2

            _LIBCPP_INLINE_VISIBILITY
            reference operator*() const {
                _LIBCPP_DEBUG_ASSERT(__get_const_db()->__dereferenceable(this),
                                     "Attempted to dereference a non-dereferenceable unordered container const_iterator");
                return __node_->__upcast()->__value_;
            }
            _LIBCPP_INLINE_VISIBILITY
            pointer operator->() const {
                _LIBCPP_DEBUG_ASSERT(__get_const_db()->__dereferenceable(this),
                                     "Attempted to dereference a non-dereferenceable unordered container const_iterator");
                return pointer_traits<pointer>::pointer_to(__node_->__upcast()->__value_);
            }

            _LIBCPP_INLINE_VISIBILITY
            __hash_const_iterator& operator++() {
                _LIBCPP_DEBUG_ASSERT(__get_const_db()->__dereferenceable(this),
                                     "Attempted to increment non-incrementable unordered container const_iterator");
                __node_ = __node_->__next_;
                return *this;
            }

            _LIBCPP_INLINE_VISIBILITY
            __hash_const_iterator operator++(int)
            {
                __hash_const_iterator __t(*this);
                ++(*this);
                return __t;
            }

            friend _LIBCPP_INLINE_VISIBILITY
            bool operator==(const __hash_const_iterator& __x, const __hash_const_iterator& __y)
            {
                return __x.__node_ == __y.__node_;
            }
            friend _LIBCPP_INLINE_VISIBILITY
            bool operator!=(const __hash_const_iterator& __x, const __hash_const_iterator& __y)
            {return !(__x == __y);}

        private:
#if _LIBCPP_DEBUG_LEVEL >= 2
            _LIBCPP_INLINE_VISIBILITY
    __hash_const_iterator(__next_pointer __node, const void* __c) _NOEXCEPT
        : __node_(__node)
        {
            __get_db()->__insert_ic(this, __c);
        }
#else
            _LIBCPP_INLINE_VISIBILITY
            __hash_const_iterator(__next_pointer __node) _NOEXCEPT
                    : __node_(__node)
            {}
#endif
            template <class, class, class, class> friend class __hash_table;
            template <class> friend class _LIBCPP_TEMPLATE_VIS __hash_map_const_iterator;
            template <class, class, class, class, class> friend class _LIBCPP_TEMPLATE_VIS unordered_map;
            template <class, class, class, class, class> friend class _LIBCPP_TEMPLATE_VIS unordered_multimap;
        };

        template <class _NodePtr>
        class _LIBCPP_TEMPLATE_VIS __hash_local_iterator
        {
            typedef __hash_node_types<_NodePtr> _NodeTypes;
            typedef _NodePtr                            __node_pointer;
            typedef typename _NodeTypes::__next_pointer __next_pointer;

            __next_pointer         __node_;
            size_t                 __bucket_;
            size_t                 __bucket_count_;

        public:
            typedef forward_iterator_tag                                iterator_category;
            typedef typename _NodeTypes::__node_value_type              value_type;
            typedef typename _NodeTypes::difference_type                difference_type;
            typedef value_type&                                         reference;
            typedef typename _NodeTypes::__node_value_type_pointer      pointer;

            _LIBCPP_INLINE_VISIBILITY __hash_local_iterator() _NOEXCEPT : __node_(nullptr) {
                _LIBCPP_DEBUG_MODE(__get_db()->__insert_i(this));
            }

#if _LIBCPP_DEBUG_LEVEL >= 2
            _LIBCPP_INLINE_VISIBILITY
    __hash_local_iterator(const __hash_local_iterator& __i)
        : __node_(__i.__node_),
          __bucket_(__i.__bucket_),
          __bucket_count_(__i.__bucket_count_)
    {
        __get_db()->__iterator_copy(this, &__i);
    }

    _LIBCPP_INLINE_VISIBILITY
    ~__hash_local_iterator()
    {
        __get_db()->__erase_i(this);
    }

    _LIBCPP_INLINE_VISIBILITY
    __hash_local_iterator& operator=(const __hash_local_iterator& __i)
    {
        if (this != &__i)
        {
            __get_db()->__iterator_copy(this, &__i);
            __node_ = __i.__node_;
            __bucket_ = __i.__bucket_;
            __bucket_count_ = __i.__bucket_count_;
        }
        return *this;
    }
#endif  // _LIBCPP_DEBUG_LEVEL >= 2

            _LIBCPP_INLINE_VISIBILITY
            reference operator*() const {
                _LIBCPP_DEBUG_ASSERT(__get_const_db()->__dereferenceable(this),
                                     "Attempted to dereference a non-dereferenceable unordered container local_iterator");
                return __node_->__upcast()->__value_;
            }

            _LIBCPP_INLINE_VISIBILITY
            pointer operator->() const {
                _LIBCPP_DEBUG_ASSERT(__get_const_db()->__dereferenceable(this),
                                     "Attempted to dereference a non-dereferenceable unordered container local_iterator");
                return pointer_traits<pointer>::pointer_to(__node_->__upcast()->__value_);
            }

            _LIBCPP_INLINE_VISIBILITY
            __hash_local_iterator& operator++() {
                _LIBCPP_DEBUG_ASSERT(__get_const_db()->__dereferenceable(this),
                                     "Attempted to increment non-incrementable unordered container local_iterator");
                __node_ = __node_->__next_;
                if (__node_ != nullptr && __constrain_hash(__node_->__hash(), __bucket_count_) != __bucket_)
                    __node_ = nullptr;
                return *this;
            }

            _LIBCPP_INLINE_VISIBILITY
            __hash_local_iterator operator++(int)
            {
                __hash_local_iterator __t(*this);
                ++(*this);
                return __t;
            }

            friend _LIBCPP_INLINE_VISIBILITY
            bool operator==(const __hash_local_iterator& __x, const __hash_local_iterator& __y)
            {
                return __x.__node_ == __y.__node_;
            }
            friend _LIBCPP_INLINE_VISIBILITY
            bool operator!=(const __hash_local_iterator& __x, const __hash_local_iterator& __y)
            {return !(__x == __y);}

        private:
#if _LIBCPP_DEBUG_LEVEL >= 2
            _LIBCPP_INLINE_VISIBILITY
    __hash_local_iterator(__next_pointer __node, size_t __bucket,
                          size_t __bucket_count, const void* __c) _NOEXCEPT
        : __node_(__node),
          __bucket_(__bucket),
          __bucket_count_(__bucket_count)
        {
            __get_db()->__insert_ic(this, __c);
            if (__node_ != nullptr)
                __node_ = __node_->__next_;
        }
#else
            _LIBCPP_INLINE_VISIBILITY
            __hash_local_iterator(__next_pointer __node, size_t __bucket,
                                  size_t __bucket_count) _NOEXCEPT
                    : __node_(__node),
                      __bucket_(__bucket),
                      __bucket_count_(__bucket_count)
            {
                if (__node_ != nullptr)
                    __node_ = __node_->__next_;
            }
#endif
            template <class, class, class, class> friend class __hash_table;
            template <class> friend class _LIBCPP_TEMPLATE_VIS __hash_const_local_iterator;
            template <class> friend class _LIBCPP_TEMPLATE_VIS __hash_map_iterator;
        };

        template <class _ConstNodePtr>
        class _LIBCPP_TEMPLATE_VIS __hash_const_local_iterator
        {
            typedef __hash_node_types<_ConstNodePtr> _NodeTypes;
            typedef _ConstNodePtr                       __node_pointer;
            typedef typename _NodeTypes::__next_pointer __next_pointer;

            __next_pointer         __node_;
            size_t                 __bucket_;
            size_t                 __bucket_count_;

            typedef pointer_traits<__node_pointer>          __pointer_traits;
            typedef typename __pointer_traits::element_type __node;
            typedef typename remove_const<__node>::type     __non_const_node;
            typedef typename __rebind_pointer<__node_pointer, __non_const_node>::type
                    __non_const_node_pointer;
        public:
            typedef __hash_local_iterator<__non_const_node_pointer>
                    __non_const_iterator;

            typedef forward_iterator_tag                                 iterator_category;
            typedef typename _NodeTypes::__node_value_type               value_type;
            typedef typename _NodeTypes::difference_type                 difference_type;
            typedef const value_type&                                    reference;
            typedef typename _NodeTypes::__const_node_value_type_pointer pointer;


            _LIBCPP_INLINE_VISIBILITY __hash_const_local_iterator() _NOEXCEPT : __node_(nullptr) {
                _LIBCPP_DEBUG_MODE(__get_db()->__insert_i(this));
            }

            _LIBCPP_INLINE_VISIBILITY
            __hash_const_local_iterator(const __non_const_iterator& __x) _NOEXCEPT
                    : __node_(__x.__node_),
                      __bucket_(__x.__bucket_),
                      __bucket_count_(__x.__bucket_count_)
            {
                _LIBCPP_DEBUG_MODE(__get_db()->__iterator_copy(this, &__x));
            }

#if _LIBCPP_DEBUG_LEVEL >= 2
            _LIBCPP_INLINE_VISIBILITY
    __hash_const_local_iterator(const __hash_const_local_iterator& __i)
        : __node_(__i.__node_),
          __bucket_(__i.__bucket_),
          __bucket_count_(__i.__bucket_count_)
    {
        __get_db()->__iterator_copy(this, &__i);
    }

    _LIBCPP_INLINE_VISIBILITY
    ~__hash_const_local_iterator()
    {
        __get_db()->__erase_i(this);
    }

    _LIBCPP_INLINE_VISIBILITY
    __hash_const_local_iterator& operator=(const __hash_const_local_iterator& __i)
    {
        if (this != &__i)
        {
            __get_db()->__iterator_copy(this, &__i);
            __node_ = __i.__node_;
            __bucket_ = __i.__bucket_;
            __bucket_count_ = __i.__bucket_count_;
        }
        return *this;
    }
#endif  // _LIBCPP_DEBUG_LEVEL >= 2

            _LIBCPP_INLINE_VISIBILITY
            reference operator*() const {
                _LIBCPP_DEBUG_ASSERT(__get_const_db()->__dereferenceable(this),
                                     "Attempted to dereference a non-dereferenceable unordered container const_local_iterator");
                return __node_->__upcast()->__value_;
            }

            _LIBCPP_INLINE_VISIBILITY
            pointer operator->() const {
                _LIBCPP_DEBUG_ASSERT(__get_const_db()->__dereferenceable(this),
                                     "Attempted to dereference a non-dereferenceable unordered container const_local_iterator");
                return pointer_traits<pointer>::pointer_to(__node_->__upcast()->__value_);
            }

            _LIBCPP_INLINE_VISIBILITY
            __hash_const_local_iterator& operator++() {
                _LIBCPP_DEBUG_ASSERT(__get_const_db()->__dereferenceable(this),
                                     "Attempted to increment non-incrementable unordered container const_local_iterator");
                __node_ = __node_->__next_;
                if (__node_ != nullptr && __constrain_hash(__node_->__hash(), __bucket_count_) != __bucket_)
                    __node_ = nullptr;
                return *this;
            }

            _LIBCPP_INLINE_VISIBILITY
            __hash_const_local_iterator operator++(int)
            {
                __hash_const_local_iterator __t(*this);
                ++(*this);
                return __t;
            }

            friend _LIBCPP_INLINE_VISIBILITY
            bool operator==(const __hash_const_local_iterator& __x, const __hash_const_local_iterator& __y)
            {
                return __x.__node_ == __y.__node_;
            }
            friend _LIBCPP_INLINE_VISIBILITY
            bool operator!=(const __hash_const_local_iterator& __x, const __hash_const_local_iterator& __y)
            {return !(__x == __y);}

        private:
#if _LIBCPP_DEBUG_LEVEL >= 2
            _LIBCPP_INLINE_VISIBILITY
    __hash_const_local_iterator(__next_pointer __node, size_t __bucket,
                                size_t __bucket_count, const void* __c) _NOEXCEPT
        : __node_(__node),
          __bucket_(__bucket),
          __bucket_count_(__bucket_count)
        {
            __get_db()->__insert_ic(this, __c);
            if (__node_ != nullptr)
                __node_ = __node_->__next_;
        }
#else
            _LIBCPP_INLINE_VISIBILITY
            __hash_const_local_iterator(__next_pointer __node, size_t __bucket,
                                        size_t __bucket_count) _NOEXCEPT
                    : __node_(__node),
                      __bucket_(__bucket),
                      __bucket_count_(__bucket_count)
            {
                if (__node_ != nullptr)
                    __node_ = __node_->__next_;
            }
#endif
            template <class, class, class, class> friend class __hash_table;
            template <class> friend class _LIBCPP_TEMPLATE_VIS __hash_map_const_iterator;
        };

        template <class _Alloc>
        class __bucket_list_deallocator
        {
            typedef _Alloc                                          allocator_type;
            typedef allocator_traits<allocator_type>                __alloc_traits;
            typedef typename __alloc_traits::size_type              size_type;

            __compressed_pair<size_type, allocator_type> __data_;
        public:
            typedef typename __alloc_traits::pointer pointer;

            _LIBCPP_INLINE_VISIBILITY
            __bucket_list_deallocator()
            _NOEXCEPT_(is_nothrow_default_constructible<allocator_type>::value)
                    : __data_(0) {}

            _LIBCPP_INLINE_VISIBILITY
            __bucket_list_deallocator(const allocator_type& __a, size_type __size)
            _NOEXCEPT_(is_nothrow_copy_constructible<allocator_type>::value)
                    : __data_(__size, __a) {}

#ifndef _LIBCPP_HAS_NO_RVALUE_REFERENCES

            _LIBCPP_INLINE_VISIBILITY
            __bucket_list_deallocator(__bucket_list_deallocator&& __x)
            _NOEXCEPT_(is_nothrow_move_constructible<allocator_type>::value)
                    : __data_(_VSTD::move(__x.__data_))
            {
                __x.size() = 0;
            }

#endif  // _LIBCPP_HAS_NO_RVALUE_REFERENCES

            _LIBCPP_INLINE_VISIBILITY
            size_type& size() _NOEXCEPT {return __data_.first();}
            _LIBCPP_INLINE_VISIBILITY
            size_type  size() const _NOEXCEPT {return __data_.first();}

            _LIBCPP_INLINE_VISIBILITY
            allocator_type& __alloc() _NOEXCEPT {return __data_.second();}
            _LIBCPP_INLINE_VISIBILITY
            const allocator_type& __alloc() const _NOEXCEPT {return __data_.second();}

            _LIBCPP_INLINE_VISIBILITY
            void operator()(pointer __p) _NOEXCEPT
            {
                __alloc_traits::deallocate(__alloc(), __p, size());
            }
        };

        template <class _Alloc> class __hash_map_node_destructor;

        template <class _Alloc>
        class __hash_node_destructor
        {
            typedef _Alloc                                          allocator_type;
            typedef allocator_traits<allocator_type>                __alloc_traits;

        public:
            typedef typename __alloc_traits::pointer                pointer;
        private:
            typedef __hash_node_types<pointer> _NodeTypes;

            allocator_type& __na_;

            __hash_node_destructor& operator=(const __hash_node_destructor&);

        public:
            bool __value_constructed;

            _LIBCPP_INLINE_VISIBILITY
            explicit __hash_node_destructor(allocator_type& __na,
                                            bool __constructed = false) _NOEXCEPT
                    : __na_(__na),
                      __value_constructed(__constructed)
            {}

            _LIBCPP_INLINE_VISIBILITY
            void operator()(pointer __p) _NOEXCEPT
            {
                if (__value_constructed)
                    __alloc_traits::destroy(__na_, _NodeTypes::__get_ptr(__p->__value_));
                if (__p)
                    __alloc_traits::deallocate(__na_, __p, 1);
            }

            template <class> friend class __hash_map_node_destructor;
        };

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        class __hash_table
        {
        public:
            typedef _Tp    value_type;
            typedef _Hash  hasher;
            typedef _Equal key_equal;
            typedef _Alloc allocator_type;

        private:
            typedef allocator_traits<allocator_type> __alloc_traits;
            typedef typename
            __make_hash_node_types<value_type, typename __alloc_traits::void_pointer>::type
                    _NodeTypes;
        public:

            typedef typename _NodeTypes::__node_value_type           __node_value_type;
            typedef typename _NodeTypes::__container_value_type      __container_value_type;
            typedef typename _NodeTypes::key_type                    key_type;
            typedef value_type&                              reference;
            typedef const value_type&                        const_reference;
            typedef typename __alloc_traits::pointer         pointer;
            typedef typename __alloc_traits::const_pointer   const_pointer;
#ifndef _LIBCPP_ABI_FIX_UNORDERED_CONTAINER_SIZE_TYPE
            typedef typename __alloc_traits::size_type       size_type;
#else
            typedef typename _NodeTypes::size_type           size_type;
#endif
            typedef typename _NodeTypes::difference_type     difference_type;
        public:
            // Create __node

            typedef typename _NodeTypes::__node_type __node;
            typedef typename __rebind_alloc_helper<__alloc_traits, __node>::type __node_allocator;
            typedef allocator_traits<__node_allocator>       __node_traits;
            typedef typename _NodeTypes::__void_pointer      __void_pointer;
            typedef typename _NodeTypes::__node_pointer      __node_pointer;
            typedef typename _NodeTypes::__node_pointer      __node_const_pointer;
            typedef typename _NodeTypes::__node_base_type    __first_node;
            typedef typename _NodeTypes::__node_base_pointer __node_base_pointer;
            typedef typename _NodeTypes::__next_pointer      __next_pointer;

        private:
            // check for sane allocator pointer rebinding semantics. Rebinding the
            // allocator for a new pointer type should be exactly the same as rebinding
            // the pointer using 'pointer_traits'.
            static_assert((is_same<__node_pointer, typename __node_traits::pointer>::value),
                          "Allocator does not rebind pointers in a sane manner.");
            typedef typename __rebind_alloc_helper<__node_traits, __first_node>::type
                    __node_base_allocator;
            typedef allocator_traits<__node_base_allocator> __node_base_traits;
            static_assert((is_same<__node_base_pointer, typename __node_base_traits::pointer>::value),
                          "Allocator does not rebind pointers in a sane manner.");

        private:

            typedef typename __rebind_alloc_helper<__node_traits, __next_pointer>::type __pointer_allocator;
            typedef __bucket_list_deallocator<__pointer_allocator> __bucket_list_deleter;
            typedef unique_ptr<__next_pointer[], __bucket_list_deleter> __bucket_list;
            typedef allocator_traits<__pointer_allocator>          __pointer_alloc_traits;
            typedef typename __bucket_list_deleter::pointer       __node_pointer_pointer;

            // --- Member data begin ---
            __bucket_list                                         __bucket_list_;
            __compressed_pair<__first_node, __node_allocator>     __p1_;
            __compressed_pair<size_type, hasher>                  __p2_;
            __compressed_pair<float, key_equal>                   __p3_;
            // --- Member data end ---

            _LIBCPP_INLINE_VISIBILITY
            size_type& size() _NOEXCEPT {return __p2_.first();}
        public:
            _LIBCPP_INLINE_VISIBILITY
            size_type  size() const _NOEXCEPT {return __p2_.first();}

            _LIBCPP_INLINE_VISIBILITY
            hasher& hash_function() _NOEXCEPT {return __p2_.second();}
            _LIBCPP_INLINE_VISIBILITY
            const hasher& hash_function() const _NOEXCEPT {return __p2_.second();}

            _LIBCPP_INLINE_VISIBILITY
            float& max_load_factor() _NOEXCEPT {return __p3_.first();}
            _LIBCPP_INLINE_VISIBILITY
            float  max_load_factor() const _NOEXCEPT {return __p3_.first();}

            _LIBCPP_INLINE_VISIBILITY
            key_equal& key_eq() _NOEXCEPT {return __p3_.second();}
            _LIBCPP_INLINE_VISIBILITY
            const key_equal& key_eq() const _NOEXCEPT {return __p3_.second();}

            _LIBCPP_INLINE_VISIBILITY
            __node_allocator& __node_alloc() _NOEXCEPT {return __p1_.second();}
            _LIBCPP_INLINE_VISIBILITY
            const __node_allocator& __node_alloc() const _NOEXCEPT
            {return __p1_.second();}

        public:
            typedef __hash_iterator<__node_pointer>                   iterator;
            typedef __hash_const_iterator<__node_pointer>             const_iterator;
            typedef __hash_local_iterator<__node_pointer>             local_iterator;
            typedef __hash_const_local_iterator<__node_pointer>       const_local_iterator;

            _LIBCPP_INLINE_VISIBILITY
            __hash_table()
            _NOEXCEPT_(
                    is_nothrow_default_constructible<__bucket_list>::value &&
                    is_nothrow_default_constructible<__first_node>::value &&
                    is_nothrow_default_constructible<__node_allocator>::value &&
                    is_nothrow_default_constructible<hasher>::value &&
                    is_nothrow_default_constructible<key_equal>::value);
            _LIBCPP_INLINE_VISIBILITY
            __hash_table(const hasher& __hf, const key_equal& __eql);
            __hash_table(const hasher& __hf, const key_equal& __eql,
                         const allocator_type& __a);
            explicit __hash_table(const allocator_type& __a);
            __hash_table(const __hash_table& __u);
            __hash_table(const __hash_table& __u, const allocator_type& __a);
#ifndef _LIBCPP_CXX03_LANG
            __hash_table(__hash_table&& __u)
            _NOEXCEPT_(
                    is_nothrow_move_constructible<__bucket_list>::value &&
                    is_nothrow_move_constructible<__first_node>::value &&
                    is_nothrow_move_constructible<__node_allocator>::value &&
                    is_nothrow_move_constructible<hasher>::value &&
                    is_nothrow_move_constructible<key_equal>::value);
            __hash_table(__hash_table&& __u, const allocator_type& __a);
#endif  // _LIBCPP_CXX03_LANG
            ~__hash_table();

            __hash_table& operator=(const __hash_table& __u);
#ifndef _LIBCPP_CXX03_LANG
            _LIBCPP_INLINE_VISIBILITY
            __hash_table& operator=(__hash_table&& __u)
            _NOEXCEPT_(
                    __node_traits::propagate_on_container_move_assignment::value &&
                    is_nothrow_move_assignable<__node_allocator>::value &&
                    is_nothrow_move_assignable<hasher>::value &&
                    is_nothrow_move_assignable<key_equal>::value);
#endif
            template <class _InputIterator>
            void __assign_unique(_InputIterator __first, _InputIterator __last);
            template <class _InputIterator>
            void __assign_multi(_InputIterator __first, _InputIterator __last);

            _LIBCPP_INLINE_VISIBILITY
            size_type max_size() const _NOEXCEPT
            {
                return std::min<size_type>(
                        __node_traits::max_size(__node_alloc()),
                        numeric_limits<difference_type >::max()
                );
            }

            pair<iterator, bool> __node_insert_unique(__node_pointer __nd);
            iterator             __node_insert_multi(__node_pointer __nd);
            iterator             __node_insert_multi(const_iterator __p,
                                                     __node_pointer __nd);

#ifndef _LIBCPP_CXX03_LANG
            template <class _Key, class ..._Args>
            _LIBCPP_INLINE_VISIBILITY
            pair<iterator, bool> __emplace_unique_key_args(_Key const& __k, _Args&&... __args);

            template <class... _Args>
            _LIBCPP_INLINE_VISIBILITY
            pair<iterator, bool> __emplace_unique_impl(_Args&&... __args);

            template <class _Pp>
            _LIBCPP_INLINE_VISIBILITY
            pair<iterator, bool> __emplace_unique(_Pp&& __x) {
                return __emplace_unique_extract_key(_VSTD::forward<_Pp>(__x),
                                                    __can_extract_key<_Pp, key_type>());
            }

            template <class _First, class _Second>
            _LIBCPP_INLINE_VISIBILITY
            typename enable_if<
                    __can_extract_map_key<_First, key_type, __container_value_type>::value,
                    pair<iterator, bool>
            >::type __emplace_unique(_First&& __f, _Second&& __s) {
                return __emplace_unique_key_args(__f, _VSTD::forward<_First>(__f),
                                                 _VSTD::forward<_Second>(__s));
            }

            template <class... _Args>
            _LIBCPP_INLINE_VISIBILITY
            pair<iterator, bool> __emplace_unique(_Args&&... __args) {
                return __emplace_unique_impl(_VSTD::forward<_Args>(__args)...);
            }

            template <class _Pp>
            _LIBCPP_INLINE_VISIBILITY
            pair<iterator, bool>
            __emplace_unique_extract_key(_Pp&& __x, __extract_key_fail_tag) {
                return __emplace_unique_impl(_VSTD::forward<_Pp>(__x));
            }
            template <class _Pp>
            _LIBCPP_INLINE_VISIBILITY
            pair<iterator, bool>
            __emplace_unique_extract_key(_Pp&& __x, __extract_key_self_tag) {
                return __emplace_unique_key_args(__x, _VSTD::forward<_Pp>(__x));
            }
            template <class _Pp>
            _LIBCPP_INLINE_VISIBILITY
            pair<iterator, bool>
            __emplace_unique_extract_key(_Pp&& __x, __extract_key_first_tag) {
                return __emplace_unique_key_args(__x.first, _VSTD::forward<_Pp>(__x));
            }

            template <class... _Args>
            _LIBCPP_INLINE_VISIBILITY
            iterator __emplace_multi(_Args&&... __args);
            template <class... _Args>
            _LIBCPP_INLINE_VISIBILITY
            iterator __emplace_hint_multi(const_iterator __p, _Args&&... __args);


            _LIBCPP_INLINE_VISIBILITY
            pair<iterator, bool>
            __insert_unique(__container_value_type&& __x) {
                return __emplace_unique_key_args(_NodeTypes::__get_key(__x), _VSTD::move(__x));
            }

            template <class _Pp, class = typename enable_if<
                    !__is_same_uncvref<_Pp, __container_value_type>::value
            >::type>
            _LIBCPP_INLINE_VISIBILITY
            pair<iterator, bool> __insert_unique(_Pp&& __x) {
                return __emplace_unique(_VSTD::forward<_Pp>(__x));
            }

            template <class _Pp>
            _LIBCPP_INLINE_VISIBILITY
            iterator __insert_multi(_Pp&& __x) {
                return __emplace_multi(_VSTD::forward<_Pp>(__x));
            }

            template <class _Pp>
            _LIBCPP_INLINE_VISIBILITY
            iterator __insert_multi(const_iterator __p, _Pp&& __x) {
                return __emplace_hint_multi(__p, _VSTD::forward<_Pp>(__x));
            }

#else  // !defined(_LIBCPP_CXX03_LANG)
            template <class _Key, class _Args>
    _LIBCPP_INLINE_VISIBILITY
    pair<iterator, bool> __emplace_unique_key_args(_Key const&, _Args& __args);

    iterator __insert_multi(const __container_value_type& __x);
    iterator __insert_multi(const_iterator __p, const __container_value_type& __x);
#endif

            _LIBCPP_INLINE_VISIBILITY
            pair<iterator, bool> __insert_unique(const __container_value_type& __x) {
                return __emplace_unique_key_args(_NodeTypes::__get_key(__x), __x);
            }

            void clear() _NOEXCEPT;
            void rehash(size_type __n);
            _LIBCPP_INLINE_VISIBILITY void reserve(size_type __n)
            {rehash(static_cast<size_type>(ceil(__n / max_load_factor())));}

            _LIBCPP_INLINE_VISIBILITY
            size_type bucket_count() const _NOEXCEPT
            {
                return __bucket_list_.get_deleter().size();
            }

            _LIBCPP_INLINE_VISIBILITY
            iterator       begin() _NOEXCEPT;
            _LIBCPP_INLINE_VISIBILITY
            iterator       begin_random() _NOEXCEPT;
            _LIBCPP_INLINE_VISIBILITY
            iterator       end() _NOEXCEPT;
            _LIBCPP_INLINE_VISIBILITY
            const_iterator begin() const _NOEXCEPT;
            _LIBCPP_INLINE_VISIBILITY
            const_iterator end() const _NOEXCEPT;

            template <class _Key>
            _LIBCPP_INLINE_VISIBILITY
            size_type bucket(const _Key& __k) const
            {
                _LIBCPP_ASSERT(bucket_count() > 0,
                               "unordered container::bucket(key) called when bucket_count() == 0");
                return __constrain_hash(hash_function()(__k), bucket_count());
            }

            template <class _Key>
            iterator       find(const _Key& __x);
            template <class _Key>
            const_iterator find(const _Key& __x) const;

            typedef __hash_node_destructor<__node_allocator> _Dp;
            typedef unique_ptr<__node, _Dp> __node_holder;

            iterator erase(const_iterator __p);
            iterator erase(const_iterator __first, const_iterator __last);
            template <class _Key>
            size_type __erase_unique(const _Key& __k);
            template <class _Key>
            size_type __erase_multi(const _Key& __k);
            __node_holder remove(const_iterator __p) _NOEXCEPT;

            template <class _Key>
            _LIBCPP_INLINE_VISIBILITY
            size_type __count_unique(const _Key& __k) const;
            template <class _Key>
            size_type __count_multi(const _Key& __k) const;

            template <class _Key>
            pair<iterator, iterator>
            __equal_range_unique(const _Key& __k);
            template <class _Key>
            pair<const_iterator, const_iterator>
            __equal_range_unique(const _Key& __k) const;

            template <class _Key>
            pair<iterator, iterator>
            __equal_range_multi(const _Key& __k);
            template <class _Key>
            pair<const_iterator, const_iterator>
            __equal_range_multi(const _Key& __k) const;

            void swap(__hash_table& __u)
#if _LIBCPP_STD_VER <= 11
            _NOEXCEPT_DEBUG_(
                    __is_nothrow_swappable<hasher>::value && __is_nothrow_swappable<key_equal>::value
                    && (!allocator_traits<__pointer_allocator>::propagate_on_container_swap::value
                        || __is_nothrow_swappable<__pointer_allocator>::value)
                    && (!__node_traits::propagate_on_container_swap::value
                        || __is_nothrow_swappable<__node_allocator>::value)
            );
#else
            _NOEXCEPT_DEBUG_(__is_nothrow_swappable<hasher>::value && __is_nothrow_swappable<key_equal>::value);
#endif

            _LIBCPP_INLINE_VISIBILITY
            size_type max_bucket_count() const _NOEXCEPT
            {return max_size(); }
            size_type bucket_size(size_type __n) const;
            _LIBCPP_INLINE_VISIBILITY float load_factor() const _NOEXCEPT
            {
                size_type __bc = bucket_count();
                return __bc != 0 ? (float)size() / __bc : 0.f;
            }
            _LIBCPP_INLINE_VISIBILITY void max_load_factor(float __mlf) _NOEXCEPT
            {
                _LIBCPP_ASSERT(__mlf > 0,
                               "unordered container::max_load_factor(lf) called with lf <= 0");
                max_load_factor() = _VSTD::max(__mlf, load_factor());
            }

            _LIBCPP_INLINE_VISIBILITY
            local_iterator
            begin(size_type __n)
            {
                _LIBCPP_ASSERT(__n < bucket_count(),
                               "unordered container::begin(n) called with n >= bucket_count()");
#if _LIBCPP_DEBUG_LEVEL >= 2
                return local_iterator(__bucket_list_[__n], __n, bucket_count(), this);
#else
                return local_iterator(__bucket_list_[__n], __n, bucket_count());
#endif
            }

            _LIBCPP_INLINE_VISIBILITY
            local_iterator
            end(size_type __n)
            {
                _LIBCPP_ASSERT(__n < bucket_count(),
                               "unordered container::end(n) called with n >= bucket_count()");
#if _LIBCPP_DEBUG_LEVEL >= 2
                return local_iterator(nullptr, __n, bucket_count(), this);
#else
                return local_iterator(nullptr, __n, bucket_count());
#endif
            }

            _LIBCPP_INLINE_VISIBILITY
            const_local_iterator
            cbegin(size_type __n) const
            {
                _LIBCPP_ASSERT(__n < bucket_count(),
                               "unordered container::cbegin(n) called with n >= bucket_count()");
#if _LIBCPP_DEBUG_LEVEL >= 2
                return const_local_iterator(__bucket_list_[__n], __n, bucket_count(), this);
#else
                return const_local_iterator(__bucket_list_[__n], __n, bucket_count());
#endif
            }

            _LIBCPP_INLINE_VISIBILITY
            const_local_iterator
            cend(size_type __n) const
            {
                _LIBCPP_ASSERT(__n < bucket_count(),
                               "unordered container::cend(n) called with n >= bucket_count()");
#if _LIBCPP_DEBUG_LEVEL >= 2
                return const_local_iterator(nullptr, __n, bucket_count(), this);
#else
                return const_local_iterator(nullptr, __n, bucket_count());
#endif
            }

#if _LIBCPP_DEBUG_LEVEL >= 2

            bool __dereferenceable(const const_iterator* __i) const;
    bool __decrementable(const const_iterator* __i) const;
    bool __addable(const const_iterator* __i, ptrdiff_t __n) const;
    bool __subscriptable(const const_iterator* __i, ptrdiff_t __n) const;

#endif  // _LIBCPP_DEBUG_LEVEL >= 2

        private:
            void __rehash(size_type __n);

#ifndef _LIBCPP_CXX03_LANG
            template <class ..._Args>
            __node_holder __construct_node(_Args&& ...__args);

            template <class _First, class ..._Rest>
            __node_holder __construct_node_hash(size_t __hash, _First&& __f, _Rest&&... __rest);
#else // _LIBCPP_CXX03_LANG
            __node_holder __construct_node(const __container_value_type& __v);
    __node_holder __construct_node_hash(size_t __hash, const __container_value_type& __v);
#endif


            _LIBCPP_INLINE_VISIBILITY
            void __copy_assign_alloc(const __hash_table& __u)
            {__copy_assign_alloc(__u, integral_constant<bool,
                        __node_traits::propagate_on_container_copy_assignment::value>());}
            void __copy_assign_alloc(const __hash_table& __u, true_type);
            _LIBCPP_INLINE_VISIBILITY
            void __copy_assign_alloc(const __hash_table&, false_type) {}

#ifndef _LIBCPP_CXX03_LANG
            void __move_assign(__hash_table& __u, false_type);
            void __move_assign(__hash_table& __u, true_type)
            _NOEXCEPT_(
                    is_nothrow_move_assignable<__node_allocator>::value &&
                    is_nothrow_move_assignable<hasher>::value &&
                    is_nothrow_move_assignable<key_equal>::value);
            _LIBCPP_INLINE_VISIBILITY
            void __move_assign_alloc(__hash_table& __u)
            _NOEXCEPT_(
                    !__node_traits::propagate_on_container_move_assignment::value ||
                    (is_nothrow_move_assignable<__pointer_allocator>::value &&
                     is_nothrow_move_assignable<__node_allocator>::value))
            {__move_assign_alloc(__u, integral_constant<bool,
                        __node_traits::propagate_on_container_move_assignment::value>());}
            _LIBCPP_INLINE_VISIBILITY
            void __move_assign_alloc(__hash_table& __u, true_type)
            _NOEXCEPT_(
                    is_nothrow_move_assignable<__pointer_allocator>::value &&
                    is_nothrow_move_assignable<__node_allocator>::value)
            {
                __bucket_list_.get_deleter().__alloc() =
                        _VSTD::move(__u.__bucket_list_.get_deleter().__alloc());
                __node_alloc() = _VSTD::move(__u.__node_alloc());
            }
            _LIBCPP_INLINE_VISIBILITY
            void __move_assign_alloc(__hash_table&, false_type) _NOEXCEPT {}
#endif // _LIBCPP_CXX03_LANG

            void __deallocate_node(__next_pointer __np) _NOEXCEPT;
            __next_pointer __detach() _NOEXCEPT;

            template <class, class, class, class, class> friend class _LIBCPP_TEMPLATE_VIS unordered_map;
            template <class, class, class, class, class> friend class _LIBCPP_TEMPLATE_VIS unordered_multimap;
        };

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        inline
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::__hash_table()
        _NOEXCEPT_(
                is_nothrow_default_constructible<__bucket_list>::value &&
                is_nothrow_default_constructible<__first_node>::value &&
                is_nothrow_default_constructible<__node_allocator>::value &&
                is_nothrow_default_constructible<hasher>::value &&
                is_nothrow_default_constructible<key_equal>::value)
                : __p2_(0),
                  __p3_(1.0f)
        {
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        inline
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::__hash_table(const hasher& __hf,
                                                               const key_equal& __eql)
                : __bucket_list_(nullptr, __bucket_list_deleter()),
                  __p1_(),
                  __p2_(0, __hf),
                  __p3_(1.0f, __eql)
        {
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::__hash_table(const hasher& __hf,
                                                               const key_equal& __eql,
                                                               const allocator_type& __a)
                : __bucket_list_(nullptr, __bucket_list_deleter(__pointer_allocator(__a), 0)),
                  __p1_(__node_allocator(__a)),
                  __p2_(0, __hf),
                  __p3_(1.0f, __eql)
        {
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::__hash_table(const allocator_type& __a)
                : __bucket_list_(nullptr, __bucket_list_deleter(__pointer_allocator(__a), 0)),
                  __p1_(__node_allocator(__a)),
                  __p2_(0),
                  __p3_(1.0f)
        {
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::__hash_table(const __hash_table& __u)
                : __bucket_list_(nullptr,
                                 __bucket_list_deleter(allocator_traits<__pointer_allocator>::
                                                       select_on_container_copy_construction(
                                         __u.__bucket_list_.get_deleter().__alloc()), 0)),
                  __p1_(allocator_traits<__node_allocator>::
                        select_on_container_copy_construction(__u.__node_alloc())),
                  __p2_(0, __u.hash_function()),
                  __p3_(__u.__p3_)
        {
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::__hash_table(const __hash_table& __u,
                                                               const allocator_type& __a)
                : __bucket_list_(nullptr, __bucket_list_deleter(__pointer_allocator(__a), 0)),
                  __p1_(__node_allocator(__a)),
                  __p2_(0, __u.hash_function()),
                  __p3_(__u.__p3_)
        {
        }

#ifndef _LIBCPP_CXX03_LANG

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::__hash_table(__hash_table&& __u)
        _NOEXCEPT_(
                is_nothrow_move_constructible<__bucket_list>::value &&
                is_nothrow_move_constructible<__first_node>::value &&
                is_nothrow_move_constructible<__node_allocator>::value &&
                is_nothrow_move_constructible<hasher>::value &&
                is_nothrow_move_constructible<key_equal>::value)
                : __bucket_list_(_VSTD::move(__u.__bucket_list_)),
                  __p1_(_VSTD::move(__u.__p1_)),
                  __p2_(_VSTD::move(__u.__p2_)),
                  __p3_(_VSTD::move(__u.__p3_))
        {
            if (size() > 0)
            {
                __bucket_list_[__constrain_hash(__p1_.first().__next_->__hash(), bucket_count())] =
                        __p1_.first().__ptr();
                __u.__p1_.first().__next_ = nullptr;
                __u.size() = 0;
            }
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::__hash_table(__hash_table&& __u,
                                                               const allocator_type& __a)
                : __bucket_list_(nullptr, __bucket_list_deleter(__pointer_allocator(__a), 0)),
                  __p1_(__node_allocator(__a)),
                  __p2_(0, _VSTD::move(__u.hash_function())),
                  __p3_(_VSTD::move(__u.__p3_))
        {
            if (__a == allocator_type(__u.__node_alloc()))
            {
                __bucket_list_.reset(__u.__bucket_list_.release());
                __bucket_list_.get_deleter().size() = __u.__bucket_list_.get_deleter().size();
                __u.__bucket_list_.get_deleter().size() = 0;
                if (__u.size() > 0)
                {
                    __p1_.first().__next_ = __u.__p1_.first().__next_;
                    __u.__p1_.first().__next_ = nullptr;
                    __bucket_list_[__constrain_hash(__p1_.first().__next_->__hash(), bucket_count())] =
                            __p1_.first().__ptr();
                    size() = __u.size();
                    __u.size() = 0;
                }
            }
        }

#endif  // _LIBCPP_CXX03_LANG

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::~__hash_table()
        {
            static_assert((is_copy_constructible<key_equal>::value),
                          "Predicate must be copy-constructible.");
            static_assert((is_copy_constructible<hasher>::value),
                          "Hasher must be copy-constructible.");
            __deallocate_node(__p1_.first().__next_);
#if _LIBCPP_DEBUG_LEVEL >= 2
            __get_db()->__erase_c(this);
#endif
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        void
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::__copy_assign_alloc(
                const __hash_table& __u, true_type)
        {
            if (__node_alloc() != __u.__node_alloc())
            {
                clear();
                __bucket_list_.reset();
                __bucket_list_.get_deleter().size() = 0;
            }
            __bucket_list_.get_deleter().__alloc() = __u.__bucket_list_.get_deleter().__alloc();
            __node_alloc() = __u.__node_alloc();
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        __hash_table<_Tp, _Hash, _Equal, _Alloc>&
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::operator=(const __hash_table& __u)
        {
            if (this != &__u)
            {
                __copy_assign_alloc(__u);
                hash_function() = __u.hash_function();
                key_eq() = __u.key_eq();
                max_load_factor() = __u.max_load_factor();
                __assign_multi(__u.begin(), __u.end());
            }
            return *this;
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        void
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::__deallocate_node(__next_pointer __np)
        _NOEXCEPT
        {
            __node_allocator& __na = __node_alloc();
            while (__np != nullptr)
            {
                __next_pointer __next = __np->__next_;
#if _LIBCPP_DEBUG_LEVEL >= 2
                __c_node* __c = __get_db()->__find_c_and_lock(this);
        for (__i_node** __p = __c->end_; __p != __c->beg_; )
        {
            --__p;
            iterator* __i = static_cast<iterator*>((*__p)->__i_);
            if (__i->__node_ == __np)
            {
                (*__p)->__c_ = nullptr;
                if (--__c->end_ != __p)
                    memmove(__p, __p+1, (__c->end_ - __p)*sizeof(__i_node*));
            }
        }
        __get_db()->unlock();
#endif
                __node_pointer __real_np = __np->__upcast();
                __node_traits::destroy(__na, _NodeTypes::__get_ptr(__real_np->__value_));
                __node_traits::deallocate(__na, __real_np, 1);
                __np = __next;
            }
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::__next_pointer
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::__detach() _NOEXCEPT
        {
            size_type __bc = bucket_count();
            for (size_type __i = 0; __i < __bc; ++__i)
                __bucket_list_[__i] = nullptr;
            size() = 0;
            __next_pointer __cache = __p1_.first().__next_;
            __p1_.first().__next_ = nullptr;
            return __cache;
        }

#ifndef _LIBCPP_CXX03_LANG

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        void
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::__move_assign(
                __hash_table& __u, true_type)
        _NOEXCEPT_(
                is_nothrow_move_assignable<__node_allocator>::value &&
                is_nothrow_move_assignable<hasher>::value &&
                is_nothrow_move_assignable<key_equal>::value)
        {
            clear();
            __bucket_list_.reset(__u.__bucket_list_.release());
            __bucket_list_.get_deleter().size() = __u.__bucket_list_.get_deleter().size();
            __u.__bucket_list_.get_deleter().size() = 0;
            __move_assign_alloc(__u);
            size() = __u.size();
            hash_function() = _VSTD::move(__u.hash_function());
            max_load_factor() = __u.max_load_factor();
            key_eq() = _VSTD::move(__u.key_eq());
            __p1_.first().__next_ = __u.__p1_.first().__next_;
            if (size() > 0)
            {
                __bucket_list_[__constrain_hash(__p1_.first().__next_->__hash(), bucket_count())] =
                        __p1_.first().__ptr();
                __u.__p1_.first().__next_ = nullptr;
                __u.size() = 0;
            }
#if _LIBCPP_DEBUG_LEVEL >= 2
            __get_db()->swap(this, &__u);
#endif
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        void
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::__move_assign(
                __hash_table& __u, false_type)
        {
            if (__node_alloc() == __u.__node_alloc())
                __move_assign(__u, true_type());
            else
            {
                hash_function() = _VSTD::move(__u.hash_function());
                key_eq() = _VSTD::move(__u.key_eq());
                max_load_factor() = __u.max_load_factor();
                if (bucket_count() != 0)
                {
                    __next_pointer __cache = __detach();
#ifndef _LIBCPP_NO_EXCEPTIONS
                    try
                    {
#endif  // _LIBCPP_NO_EXCEPTIONS
                        const_iterator __i = __u.begin();
                        while (__cache != nullptr && __u.size() != 0)
                        {
                            __cache->__upcast()->__value_ =
                                    _VSTD::move(__u.remove(__i++)->__value_);
                            __next_pointer __next = __cache->__next_;
                            __node_insert_multi(__cache->__upcast());
                            __cache = __next;
                        }
#ifndef _LIBCPP_NO_EXCEPTIONS
                    }
                    catch (...)
                    {
                        __deallocate_node(__cache);
                        throw;
                    }
#endif  // _LIBCPP_NO_EXCEPTIONS
                    __deallocate_node(__cache);
                }
                const_iterator __i = __u.begin();
                while (__u.size() != 0)
                {
                    __node_holder __h = __construct_node(_NodeTypes::__move(__u.remove(__i++)->__value_));
                    __node_insert_multi(__h.get());
                    __h.release();
                }
            }
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        inline
        __hash_table<_Tp, _Hash, _Equal, _Alloc>&
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::operator=(__hash_table&& __u)
        _NOEXCEPT_(
                __node_traits::propagate_on_container_move_assignment::value &&
                is_nothrow_move_assignable<__node_allocator>::value &&
                is_nothrow_move_assignable<hasher>::value &&
                is_nothrow_move_assignable<key_equal>::value)
        {
            __move_assign(__u, integral_constant<bool,
                    __node_traits::propagate_on_container_move_assignment::value>());
            return *this;
        }

#endif  // _LIBCPP_CXX03_LANG

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        template <class _InputIterator>
        void
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::__assign_unique(_InputIterator __first,
                                                                  _InputIterator __last)
        {
            typedef iterator_traits<_InputIterator> _ITraits;
            typedef typename _ITraits::value_type _ItValueType;
            static_assert((is_same<_ItValueType, __container_value_type>::value),
                          "__assign_unique may only be called with the containers value type");

            if (bucket_count() != 0)
            {
                __next_pointer __cache = __detach();
#ifndef _LIBCPP_NO_EXCEPTIONS
                try
                {
#endif  // _LIBCPP_NO_EXCEPTIONS
                    for (; __cache != nullptr && __first != __last; ++__first)
                    {
                        __cache->__upcast()->__value_ = *__first;
                        __next_pointer __next = __cache->__next_;
                        __node_insert_unique(__cache->__upcast());
                        __cache = __next;
                    }
#ifndef _LIBCPP_NO_EXCEPTIONS
                }
                catch (...)
                {
                    __deallocate_node(__cache);
                    throw;
                }
#endif  // _LIBCPP_NO_EXCEPTIONS
                __deallocate_node(__cache);
            }
            for (; __first != __last; ++__first)
                __insert_unique(*__first);
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        template <class _InputIterator>
        void
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::__assign_multi(_InputIterator __first,
                                                                 _InputIterator __last)
        {
            typedef iterator_traits<_InputIterator> _ITraits;
            typedef typename _ITraits::value_type _ItValueType;
            static_assert((is_same<_ItValueType, __container_value_type>::value ||
                           is_same<_ItValueType, __node_value_type>::value),
                          "__assign_multi may only be called with the containers value type"
                                  " or the nodes value type");
            if (bucket_count() != 0)
            {
                __next_pointer __cache = __detach();
#ifndef _LIBCPP_NO_EXCEPTIONS
                try
                {
#endif  // _LIBCPP_NO_EXCEPTIONS
                    for (; __cache != nullptr && __first != __last; ++__first)
                    {
                        __cache->__upcast()->__value_ = *__first;
                        __next_pointer __next = __cache->__next_;
                        __node_insert_multi(__cache->__upcast());
                        __cache = __next;
                    }
#ifndef _LIBCPP_NO_EXCEPTIONS
                }
                catch (...)
                {
                    __deallocate_node(__cache);
                    throw;
                }
#endif  // _LIBCPP_NO_EXCEPTIONS
                __deallocate_node(__cache);
            }
            for (; __first != __last; ++__first)
                __insert_multi(_NodeTypes::__get_value(*__first));
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        inline
        typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::iterator
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::begin() _NOEXCEPT
        {
#if _LIBCPP_DEBUG_LEVEL >= 2
            return iterator(__p1_.first().__next_, this);
#else
            return iterator(__p1_.first().__next_);
#endif
        }

        // START ADDED CODE
        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        inline
        typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::iterator
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::begin_random() _NOEXCEPT
        {
#if _LIBCPP_DEBUG_LEVEL >= 2
            return iterator(__p1_.first().__next_, this);
#else
            vector <iterator> vect;
            iterator it_current, it_prev, it_init;
            int index;

            // insert elements from hash table into vector
            for (iterator it(__p1_.first().__next_); it != nullptr; it++){
                vect.push_back(static_cast<iterator&&>(it));
            }

            // shuffle elements and insert them into the new hash table
            index = rand() % vect.size();
            it_init = vect[index];
            it_prev = it_init;

            vect.erase(vect.begin()+index);
            while (!vect.empty()) {
                index = rand() % vect.size();

                it_current = vect[index];
                it_prev.setNext(it_current);
                it_prev = it_current;

                vect.erase(vect.begin() + index);
            }
            it_prev.setNext(iterator(nullptr));

            __p1_.first().__next_ = it_init.getNode();
            it_init = iterator(__p1_.first().__next_);
            return it_init;
#endif
        }
        // END CODE

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        inline
        typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::iterator
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::end() _NOEXCEPT
        {
#if _LIBCPP_DEBUG_LEVEL >= 2
            return iterator(nullptr, this);
#else
            return iterator(nullptr);
#endif
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        inline
        typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::const_iterator
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::begin() const _NOEXCEPT
        {
#if _LIBCPP_DEBUG_LEVEL >= 2
            return const_iterator(__p1_.first().__next_, this);
#else
            return const_iterator(__p1_.first().__next_);
#endif
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        inline
        typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::const_iterator
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::end() const _NOEXCEPT
        {
#if _LIBCPP_DEBUG_LEVEL >= 2
            return const_iterator(nullptr, this);
#else
            return const_iterator(nullptr);
#endif
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        void
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::clear() _NOEXCEPT
        {
            if (size() > 0)
            {
                __deallocate_node(__p1_.first().__next_);
                __p1_.first().__next_ = nullptr;
                size_type __bc = bucket_count();
                for (size_type __i = 0; __i < __bc; ++__i)
                    __bucket_list_[__i] = nullptr;
                size() = 0;
            }
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        pair<typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::iterator, bool>
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::__node_insert_unique(__node_pointer __nd)
        {
            __nd->__hash_ = hash_function()(__nd->__value_);
            size_type __bc = bucket_count();
            bool __inserted = false;
            __next_pointer __ndptr;
            size_t __chash;
            if (__bc != 0)
            {
                __chash = __constrain_hash(__nd->__hash_, __bc);
                __ndptr = __bucket_list_[__chash];
                if (__ndptr != nullptr)
                {
                    for (__ndptr = __ndptr->__next_; __ndptr != nullptr &&
                                                     __constrain_hash(__ndptr->__hash(), __bc) == __chash;
                         __ndptr = __ndptr->__next_)
                    {
                        if (key_eq()(__ndptr->__upcast()->__value_, __nd->__value_))
                            goto __done;
                    }
                }
            }
            {
                if (size()+1 > __bc * max_load_factor() || __bc == 0)
                {
                    rehash(_VSTD::max<size_type>(2 * __bc + !__is_hash_power2(__bc),
                                                 size_type(ceil(float(size() + 1) / max_load_factor()))));
                    __bc = bucket_count();
                    __chash = __constrain_hash(__nd->__hash_, __bc);
                }
                // insert_after __bucket_list_[__chash], or __first_node if bucket is null
                __next_pointer __pn = __bucket_list_[__chash];
                if (__pn == nullptr)
                {
                    __pn =__p1_.first().__ptr();
                    __nd->__next_ = __pn->__next_;
                    __pn->__next_ = __nd->__ptr();
                    // fix up __bucket_list_
                    __bucket_list_[__chash] = __pn;
                    if (__nd->__next_ != nullptr)
                        __bucket_list_[__constrain_hash(__nd->__next_->__hash(), __bc)] = __nd->__ptr();
                }
                else
                {
                    __nd->__next_ = __pn->__next_;
                    __pn->__next_ = __nd->__ptr();
                }
                __ndptr = __nd->__ptr();
                // increment size
                ++size();
                __inserted = true;
            }
            __done:
#if _LIBCPP_DEBUG_LEVEL >= 2
            return pair<iterator, bool>(iterator(__ndptr, this), __inserted);
#else
            return pair<iterator, bool>(iterator(__ndptr), __inserted);
#endif
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::iterator
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::__node_insert_multi(__node_pointer __cp)
        {
            __cp->__hash_ = hash_function()(__cp->__value_);
            size_type __bc = bucket_count();
            if (size()+1 > __bc * max_load_factor() || __bc == 0)
            {
                rehash(_VSTD::max<size_type>(2 * __bc + !__is_hash_power2(__bc),
                                             size_type(ceil(float(size() + 1) / max_load_factor()))));
                __bc = bucket_count();
            }
            size_t __chash = __constrain_hash(__cp->__hash_, __bc);
            __next_pointer __pn = __bucket_list_[__chash];
            if (__pn == nullptr)
            {
                __pn =__p1_.first().__ptr();
                __cp->__next_ = __pn->__next_;
                __pn->__next_ = __cp->__ptr();
                // fix up __bucket_list_
                __bucket_list_[__chash] = __pn;
                if (__cp->__next_ != nullptr)
                    __bucket_list_[__constrain_hash(__cp->__next_->__hash(), __bc)]
                            = __cp->__ptr();
            }
            else
            {
                for (bool __found = false; __pn->__next_ != nullptr &&
                                           __constrain_hash(__pn->__next_->__hash(), __bc) == __chash;
                     __pn = __pn->__next_)
                {
                    //      __found    key_eq()     action
                    //      false       false       loop
                    //      true        true        loop
                    //      false       true        set __found to true
                    //      true        false       break
                    if (__found != (__pn->__next_->__hash() == __cp->__hash_ &&
                                    key_eq()(__pn->__next_->__upcast()->__value_, __cp->__value_)))
                    {
                        if (!__found)
                            __found = true;
                        else
                            break;
                    }
                }
                __cp->__next_ = __pn->__next_;
                __pn->__next_ = __cp->__ptr();
                if (__cp->__next_ != nullptr)
                {
                    size_t __nhash = __constrain_hash(__cp->__next_->__hash(), __bc);
                    if (__nhash != __chash)
                        __bucket_list_[__nhash] = __cp->__ptr();
                }
            }
            ++size();
#if _LIBCPP_DEBUG_LEVEL >= 2
            return iterator(__cp->__ptr(), this);
#else
            return iterator(__cp->__ptr());
#endif
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::iterator
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::__node_insert_multi(
                const_iterator __p, __node_pointer __cp)
        {
#if _LIBCPP_DEBUG_LEVEL >= 2
            _LIBCPP_ASSERT(__get_const_db()->__find_c_from_i(&__p) == this,
        "unordered container::emplace_hint(const_iterator, args...) called with an iterator not"
        " referring to this unordered container");
#endif
            if (__p != end() && key_eq()(*__p, __cp->__value_))
            {
                __next_pointer __np = __p.__node_;
                __cp->__hash_ = __np->__hash();
                size_type __bc = bucket_count();
                if (size()+1 > __bc * max_load_factor() || __bc == 0)
                {
                    rehash(_VSTD::max<size_type>(2 * __bc + !__is_hash_power2(__bc),
                                                 size_type(ceil(float(size() + 1) / max_load_factor()))));
                    __bc = bucket_count();
                }
                size_t __chash = __constrain_hash(__cp->__hash_, __bc);
                __next_pointer __pp = __bucket_list_[__chash];
                while (__pp->__next_ != __np)
                    __pp = __pp->__next_;
                __cp->__next_ = __np;
                __pp->__next_ = static_cast<__next_pointer>(__cp);
                ++size();
#if _LIBCPP_DEBUG_LEVEL >= 2
                return iterator(static_cast<__next_pointer>(__cp), this);
#else
                return iterator(static_cast<__next_pointer>(__cp));
#endif
            }
            return __node_insert_multi(__cp);
        }



#ifndef _LIBCPP_CXX03_LANG
        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        template <class _Key, class ..._Args>
        pair<typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::iterator, bool>
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::__emplace_unique_key_args(_Key const& __k, _Args&&... __args)
#else
        template <class _Tp, class _Hash, class _Equal, class _Alloc>
template <class _Key, class _Args>
pair<typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::iterator, bool>
__hash_table<_Tp, _Hash, _Equal, _Alloc>::__emplace_unique_key_args(_Key const& __k, _Args& __args)
#endif
        {

            size_t __hash = hash_function()(__k);
            size_type __bc = bucket_count();
            bool __inserted = false;
            __next_pointer __nd;
            size_t __chash;
            if (__bc != 0)
            {
                __chash = __constrain_hash(__hash, __bc);
                __nd = __bucket_list_[__chash];
                if (__nd != nullptr)
                {
                    for (__nd = __nd->__next_; __nd != nullptr &&
                                               (__nd->__hash() == __hash || __constrain_hash(__nd->__hash(), __bc) == __chash);
                         __nd = __nd->__next_)
                    {
                        if (key_eq()(__nd->__upcast()->__value_, __k))
                            goto __done;
                    }
                }
            }
            {
#ifndef _LIBCPP_CXX03_LANG
                __node_holder __h = __construct_node_hash(__hash, _VSTD::forward<_Args>(__args)...);
#else
                __node_holder __h = __construct_node_hash(__hash, __args);
#endif
                if (size()+1 > __bc * max_load_factor() || __bc == 0)
                {
                    rehash(_VSTD::max<size_type>(2 * __bc + !__is_hash_power2(__bc),
                                                 size_type(ceil(float(size() + 1) / max_load_factor()))));
                    __bc = bucket_count();
                    __chash = __constrain_hash(__hash, __bc);
                }
                // insert_after __bucket_list_[__chash], or __first_node if bucket is null
                __next_pointer __pn = __bucket_list_[__chash];
                if (__pn == nullptr)
                {
                    __pn = __p1_.first().__ptr();
                    __h->__next_ = __pn->__next_;
                    __pn->__next_ = __h.get()->__ptr();
                    // fix up __bucket_list_
                    __bucket_list_[__chash] = __pn;
                    if (__h->__next_ != nullptr)
                        __bucket_list_[__constrain_hash(__h->__next_->__hash(), __bc)]
                                = __h.get()->__ptr();
                }
                else
                {
                    __h->__next_ = __pn->__next_;
                    __pn->__next_ = static_cast<__next_pointer>(__h.get());
                }
                __nd = static_cast<__next_pointer>(__h.release());
                // increment size
                ++size();
                __inserted = true;
            }
            __done:
#if _LIBCPP_DEBUG_LEVEL >= 2
            return pair<iterator, bool>(iterator(__nd, this), __inserted);
#else
            return pair<iterator, bool>(iterator(__nd), __inserted);
#endif
        }

#ifndef _LIBCPP_CXX03_LANG

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        template <class... _Args>
        pair<typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::iterator, bool>
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::__emplace_unique_impl(_Args&&... __args)
        {
            __node_holder __h = __construct_node(_VSTD::forward<_Args>(__args)...);
            pair<iterator, bool> __r = __node_insert_unique(__h.get());
            if (__r.second)
                __h.release();
            return __r;
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        template <class... _Args>
        typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::iterator
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::__emplace_multi(_Args&&... __args)
        {
            __node_holder __h = __construct_node(_VSTD::forward<_Args>(__args)...);
            iterator __r = __node_insert_multi(__h.get());
            __h.release();
            return __r;
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        template <class... _Args>
        typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::iterator
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::__emplace_hint_multi(
                const_iterator __p, _Args&&... __args)
        {
#if _LIBCPP_DEBUG_LEVEL >= 2
            _LIBCPP_ASSERT(__get_const_db()->__find_c_from_i(&__p) == this,
        "unordered container::emplace_hint(const_iterator, args...) called with an iterator not"
        " referring to this unordered container");
#endif
            __node_holder __h = __construct_node(_VSTD::forward<_Args>(__args)...);
            iterator __r = __node_insert_multi(__p, __h.get());
            __h.release();
            return __r;
        }

#else // _LIBCPP_CXX03_LANG

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::iterator
__hash_table<_Tp, _Hash, _Equal, _Alloc>::__insert_multi(const __container_value_type& __x)
{
    __node_holder __h = __construct_node(__x);
    iterator __r = __node_insert_multi(__h.get());
    __h.release();
    return __r;
}

template <class _Tp, class _Hash, class _Equal, class _Alloc>
typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::iterator
__hash_table<_Tp, _Hash, _Equal, _Alloc>::__insert_multi(const_iterator __p,
                                                         const __container_value_type& __x)
{
#if _LIBCPP_DEBUG_LEVEL >= 2
    _LIBCPP_ASSERT(__get_const_db()->__find_c_from_i(&__p) == this,
        "unordered container::insert(const_iterator, lvalue) called with an iterator not"
        " referring to this unordered container");
#endif
    __node_holder __h = __construct_node(__x);
    iterator __r = __node_insert_multi(__p, __h.get());
    __h.release();
    return __r;
}

#endif  // _LIBCPP_CXX03_LANG

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        void
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::rehash(size_type __n)
        {
            if (__n == 1)
                __n = 2;
            else if (__n & (__n - 1))
                __n = __next_prime(__n);
            size_type __bc = bucket_count();
            if (__n > __bc)
                __rehash(__n);
            else if (__n < __bc)
            {
                __n = _VSTD::max<size_type>
                        (
                                __n,
                                __is_hash_power2(__bc) ? __next_hash_pow2(size_t(ceil(float(size()) / max_load_factor()))) :
                                __next_prime(size_t(ceil(float(size()) / max_load_factor())))
                        );
                if (__n < __bc)
                    __rehash(__n);
            }
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        void
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::__rehash(size_type __nbc)
        {
#if _LIBCPP_DEBUG_LEVEL >= 2
            __get_db()->__invalidate_all(this);
#endif  // _LIBCPP_DEBUG_LEVEL >= 2
            __pointer_allocator& __npa = __bucket_list_.get_deleter().__alloc();
            __bucket_list_.reset(__nbc > 0 ?
                                 __pointer_alloc_traits::allocate(__npa, __nbc) : nullptr);
            __bucket_list_.get_deleter().size() = __nbc;
            if (__nbc > 0)
            {
                for (size_type __i = 0; __i < __nbc; ++__i)
                    __bucket_list_[__i] = nullptr;
                __next_pointer __pp = __p1_.first().__ptr();
                __next_pointer __cp = __pp->__next_;
                if (__cp != nullptr)
                {
                    size_type __chash = __constrain_hash(__cp->__hash(), __nbc);
                    __bucket_list_[__chash] = __pp;
                    size_type __phash = __chash;
                    for (__pp = __cp, __cp = __cp->__next_; __cp != nullptr;
                         __cp = __pp->__next_)
                    {
                        __chash = __constrain_hash(__cp->__hash(), __nbc);
                        if (__chash == __phash)
                            __pp = __cp;
                        else
                        {
                            if (__bucket_list_[__chash] == nullptr)
                            {
                                __bucket_list_[__chash] = __pp;
                                __pp = __cp;
                                __phash = __chash;
                            }
                            else
                            {
                                __next_pointer __np = __cp;
                                for (; __np->__next_ != nullptr &&
                                       key_eq()(__cp->__upcast()->__value_,
                                                __np->__next_->__upcast()->__value_);
                                       __np = __np->__next_)
                                    ;
                                __pp->__next_ = __np->__next_;
                                __np->__next_ = __bucket_list_[__chash]->__next_;
                                __bucket_list_[__chash]->__next_ = __cp;

                            }
                        }
                    }
                }
            }
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        template <class _Key>
        typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::iterator
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::find(const _Key& __k)
        {
            size_t __hash = hash_function()(__k);
            size_type __bc = bucket_count();
            if (__bc != 0)
            {
                size_t __chash = __constrain_hash(__hash, __bc);
                __next_pointer __nd = __bucket_list_[__chash];
                if (__nd != nullptr)
                {
                    for (__nd = __nd->__next_; __nd != nullptr &&
                                               (__nd->__hash() == __hash
                                                || __constrain_hash(__nd->__hash(), __bc) == __chash);
                         __nd = __nd->__next_)
                    {
                        if ((__nd->__hash() == __hash)
                            && key_eq()(__nd->__upcast()->__value_, __k))
#if _LIBCPP_DEBUG_LEVEL >= 2
                            return iterator(__nd, this);
#else
                            return iterator(__nd);
#endif
                    }
                }
            }
            return end();
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        template <class _Key>
        typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::const_iterator
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::find(const _Key& __k) const
        {
            size_t __hash = hash_function()(__k);
            size_type __bc = bucket_count();
            if (__bc != 0)
            {
                size_t __chash = __constrain_hash(__hash, __bc);
                __next_pointer __nd = __bucket_list_[__chash];
                if (__nd != nullptr)
                {
                    for (__nd = __nd->__next_; __nd != nullptr &&
                                               (__hash == __nd->__hash()
                                                || __constrain_hash(__nd->__hash(), __bc) == __chash);
                         __nd = __nd->__next_)
                    {
                        if ((__nd->__hash() == __hash)
                            && key_eq()(__nd->__upcast()->__value_, __k))
#if _LIBCPP_DEBUG_LEVEL >= 2
                            return const_iterator(__nd, this);
#else
                            return const_iterator(__nd);
#endif
                    }
                }

            }
            return end();
        }

#ifndef _LIBCPP_CXX03_LANG

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        template <class ..._Args>
        typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::__node_holder
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::__construct_node(_Args&& ...__args)
        {
            static_assert(!__is_hash_value_type<_Args...>::value,
                          "Construct cannot be called with a hash value type");
            __node_allocator& __na = __node_alloc();
            __node_holder __h(__node_traits::allocate(__na, 1), _Dp(__na));
            __node_traits::construct(__na, _NodeTypes::__get_ptr(__h->__value_), _VSTD::forward<_Args>(__args)...);
            __h.get_deleter().__value_constructed = true;
            __h->__hash_ = hash_function()(__h->__value_);
            __h->__next_ = nullptr;
            return __h;
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        template <class _First, class ..._Rest>
        typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::__node_holder
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::__construct_node_hash(
                size_t __hash, _First&& __f, _Rest&& ...__rest)
        {
            static_assert(!__is_hash_value_type<_First, _Rest...>::value,
                          "Construct cannot be called with a hash value type");
            __node_allocator& __na = __node_alloc();
            __node_holder __h(__node_traits::allocate(__na, 1), _Dp(__na));
            __node_traits::construct(__na, _NodeTypes::__get_ptr(__h->__value_),
                                     _VSTD::forward<_First>(__f),
                                     _VSTD::forward<_Rest>(__rest)...);
            __h.get_deleter().__value_constructed = true;
            __h->__hash_ = __hash;
            __h->__next_ = nullptr;
            return __h;
        }

#else  // _LIBCPP_CXX03_LANG

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::__node_holder
__hash_table<_Tp, _Hash, _Equal, _Alloc>::__construct_node(const __container_value_type& __v)
{
    __node_allocator& __na = __node_alloc();
    __node_holder __h(__node_traits::allocate(__na, 1), _Dp(__na));
    __node_traits::construct(__na, _NodeTypes::__get_ptr(__h->__value_), __v);
    __h.get_deleter().__value_constructed = true;
    __h->__hash_ = hash_function()(__h->__value_);
    __h->__next_ = nullptr;
    return _LIBCPP_EXPLICIT_MOVE(__h);  // explicitly moved for C++03
}

template <class _Tp, class _Hash, class _Equal, class _Alloc>
typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::__node_holder
__hash_table<_Tp, _Hash, _Equal, _Alloc>::__construct_node_hash(size_t __hash,
                                                                const __container_value_type& __v)
{
    __node_allocator& __na = __node_alloc();
    __node_holder __h(__node_traits::allocate(__na, 1), _Dp(__na));
    __node_traits::construct(__na, _NodeTypes::__get_ptr(__h->__value_), __v);
    __h.get_deleter().__value_constructed = true;
    __h->__hash_ = __hash;
    __h->__next_ = nullptr;
    return _LIBCPP_EXPLICIT_MOVE(__h);  // explicitly moved for C++03
}

#endif  // _LIBCPP_CXX03_LANG

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::iterator
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::erase(const_iterator __p)
        {
            __next_pointer __np = __p.__node_;
#if _LIBCPP_DEBUG_LEVEL >= 2
            _LIBCPP_ASSERT(__get_const_db()->__find_c_from_i(&__p) == this,
        "unordered container erase(iterator) called with an iterator not"
        " referring to this container");
    _LIBCPP_ASSERT(__p != end(),
        "unordered container erase(iterator) called with a non-dereferenceable iterator");
    iterator __r(__np, this);
#else
            iterator __r(__np);
#endif
            ++__r;
            remove(__p);
            return __r;
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::iterator
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::erase(const_iterator __first,
                                                        const_iterator __last)
        {
#if _LIBCPP_DEBUG_LEVEL >= 2
            _LIBCPP_ASSERT(__get_const_db()->__find_c_from_i(&__first) == this,
        "unodered container::erase(iterator, iterator) called with an iterator not"
        " referring to this unodered container");
    _LIBCPP_ASSERT(__get_const_db()->__find_c_from_i(&__last) == this,
        "unodered container::erase(iterator, iterator) called with an iterator not"
        " referring to this unodered container");
#endif
            for (const_iterator __p = __first; __first != __last; __p = __first)
            {
                ++__first;
                erase(__p);
            }
            __next_pointer __np = __last.__node_;
#if _LIBCPP_DEBUG_LEVEL >= 2
            return iterator (__np, this);
#else
            return iterator (__np);
#endif
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        template <class _Key>
        typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::size_type
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::__erase_unique(const _Key& __k)
        {
            iterator __i = find(__k);
            if (__i == end())
                return 0;
            erase(__i);
            return 1;
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        template <class _Key>
        typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::size_type
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::__erase_multi(const _Key& __k)
        {
            size_type __r = 0;
            iterator __i = find(__k);
            if (__i != end())
            {
                iterator __e = end();
                do
                {
                    erase(__i++);
                    ++__r;
                } while (__i != __e && key_eq()(*__i, __k));
            }
            return __r;
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::__node_holder
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::remove(const_iterator __p) _NOEXCEPT
        {
            // current node
            __next_pointer __cn = __p.__node_;
            size_type __bc = bucket_count();
            size_t __chash = __constrain_hash(__cn->__hash(), __bc);
            // find previous node
            __next_pointer __pn = __bucket_list_[__chash];
            for (; __pn->__next_ != __cn; __pn = __pn->__next_)
                ;
            // Fix up __bucket_list_
            // if __pn is not in same bucket (before begin is not in same bucket) &&
            //    if __cn->__next_ is not in same bucket (nullptr is not in same bucket)
            if (__pn == __p1_.first().__ptr()
                || __constrain_hash(__pn->__hash(), __bc) != __chash)
            {
                if (__cn->__next_ == nullptr
                    || __constrain_hash(__cn->__next_->__hash(), __bc) != __chash)
                    __bucket_list_[__chash] = nullptr;
            }
            // if __cn->__next_ is not in same bucket (nullptr is in same bucket)
            if (__cn->__next_ != nullptr)
            {
                size_t __nhash = __constrain_hash(__cn->__next_->__hash(), __bc);
                if (__nhash != __chash)
                    __bucket_list_[__nhash] = __pn;
            }
            // remove __cn
            __pn->__next_ = __cn->__next_;
            __cn->__next_ = nullptr;
            --size();
#if _LIBCPP_DEBUG_LEVEL >= 2
            __c_node* __c = __get_db()->__find_c_and_lock(this);
    for (__i_node** __dp = __c->end_; __dp != __c->beg_; )
    {
        --__dp;
        iterator* __i = static_cast<iterator*>((*__dp)->__i_);
        if (__i->__node_ == __cn)
        {
            (*__dp)->__c_ = nullptr;
            if (--__c->end_ != __dp)
                memmove(__dp, __dp+1, (__c->end_ - __dp)*sizeof(__i_node*));
        }
    }
    __get_db()->unlock();
#endif
            return __node_holder(__cn->__upcast(), _Dp(__node_alloc(), true));
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        template <class _Key>
        inline
        typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::size_type
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::__count_unique(const _Key& __k) const
        {
            return static_cast<size_type>(find(__k) != end());
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        template <class _Key>
        typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::size_type
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::__count_multi(const _Key& __k) const
        {
            size_type __r = 0;
            const_iterator __i = find(__k);
            if (__i != end())
            {
                const_iterator __e = end();
                do
                {
                    ++__i;
                    ++__r;
                } while (__i != __e && key_eq()(*__i, __k));
            }
            return __r;
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        template <class _Key>
        pair<typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::iterator,
                typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::iterator>
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::__equal_range_unique(
                const _Key& __k)
        {
            iterator __i = find(__k);
            iterator __j = __i;
            if (__i != end())
                ++__j;
            return pair<iterator, iterator>(__i, __j);
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        template <class _Key>
        pair<typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::const_iterator,
                typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::const_iterator>
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::__equal_range_unique(
                const _Key& __k) const
        {
            const_iterator __i = find(__k);
            const_iterator __j = __i;
            if (__i != end())
                ++__j;
            return pair<const_iterator, const_iterator>(__i, __j);
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        template <class _Key>
        pair<typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::iterator,
                typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::iterator>
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::__equal_range_multi(
                const _Key& __k)
        {
            iterator __i = find(__k);
            iterator __j = __i;
            if (__i != end())
            {
                iterator __e = end();
                do
                {
                    ++__j;
                } while (__j != __e && key_eq()(*__j, __k));
            }
            return pair<iterator, iterator>(__i, __j);
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        template <class _Key>
        pair<typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::const_iterator,
                typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::const_iterator>
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::__equal_range_multi(
                const _Key& __k) const
        {
            const_iterator __i = find(__k);
            const_iterator __j = __i;
            if (__i != end())
            {
                const_iterator __e = end();
                do
                {
                    ++__j;
                } while (__j != __e && key_eq()(*__j, __k));
            }
            return pair<const_iterator, const_iterator>(__i, __j);
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        void
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::swap(__hash_table& __u)
#if _LIBCPP_STD_VER <= 11
        _NOEXCEPT_DEBUG_(
                __is_nothrow_swappable<hasher>::value && __is_nothrow_swappable<key_equal>::value
                && (!allocator_traits<__pointer_allocator>::propagate_on_container_swap::value
                    || __is_nothrow_swappable<__pointer_allocator>::value)
                && (!__node_traits::propagate_on_container_swap::value
                    || __is_nothrow_swappable<__node_allocator>::value)
        )
#else
        _NOEXCEPT_DEBUG_(__is_nothrow_swappable<hasher>::value && __is_nothrow_swappable<key_equal>::value)
#endif
        {
            _LIBCPP_ASSERT(__node_traits::propagate_on_container_swap::value ||
                           this->__node_alloc() == __u.__node_alloc(),
                           "list::swap: Either propagate_on_container_swap must be true"
                                   " or the allocators must compare equal");
            {
                __node_pointer_pointer __npp = __bucket_list_.release();
                __bucket_list_.reset(__u.__bucket_list_.release());
                __u.__bucket_list_.reset(__npp);
            }
            _VSTD::swap(__bucket_list_.get_deleter().size(), __u.__bucket_list_.get_deleter().size());
            __swap_allocator(__bucket_list_.get_deleter().__alloc(),
                             __u.__bucket_list_.get_deleter().__alloc());
            __swap_allocator(__node_alloc(), __u.__node_alloc());
            _VSTD::swap(__p1_.first().__next_, __u.__p1_.first().__next_);
            __p2_.swap(__u.__p2_);
            __p3_.swap(__u.__p3_);
            if (size() > 0)
                __bucket_list_[__constrain_hash(__p1_.first().__next_->__hash(), bucket_count())] =
                        __p1_.first().__ptr();
            if (__u.size() > 0)
                __u.__bucket_list_[__constrain_hash(__u.__p1_.first().__next_->__hash(), __u.bucket_count())] =
                        __u.__p1_.first().__ptr();
#if _LIBCPP_DEBUG_LEVEL >= 2
            __get_db()->swap(this, &__u);
#endif
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        typename __hash_table<_Tp, _Hash, _Equal, _Alloc>::size_type
        __hash_table<_Tp, _Hash, _Equal, _Alloc>::bucket_size(size_type __n) const
        {
            _LIBCPP_ASSERT(__n < bucket_count(),
                           "unordered container::bucket_size(n) called with n >= bucket_count()");
            __next_pointer __np = __bucket_list_[__n];
            size_type __bc = bucket_count();
            size_type __r = 0;
            if (__np != nullptr)
            {
                for (__np = __np->__next_; __np != nullptr &&
                                           __constrain_hash(__np->__hash(), __bc) == __n;
                     __np = __np->__next_, ++__r)
                    ;
            }
            return __r;
        }

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
        inline _LIBCPP_INLINE_VISIBILITY
        void
        swap(__hash_table<_Tp, _Hash, _Equal, _Alloc>& __x,
             __hash_table<_Tp, _Hash, _Equal, _Alloc>& __y)
        _NOEXCEPT_(_NOEXCEPT_(__x.swap(__y)))
        {
            __x.swap(__y);
        }

#if _LIBCPP_DEBUG_LEVEL >= 2

        template <class _Tp, class _Hash, class _Equal, class _Alloc>
bool
__hash_table<_Tp, _Hash, _Equal, _Alloc>::__dereferenceable(const const_iterator* __i) const
{
    return __i->__node_ != nullptr;
}

template <class _Tp, class _Hash, class _Equal, class _Alloc>
bool
__hash_table<_Tp, _Hash, _Equal, _Alloc>::__decrementable(const const_iterator*) const
{
    return false;
}

template <class _Tp, class _Hash, class _Equal, class _Alloc>
bool
__hash_table<_Tp, _Hash, _Equal, _Alloc>::__addable(const const_iterator*, ptrdiff_t) const
{
    return false;
}

template <class _Tp, class _Hash, class _Equal, class _Alloc>
bool
__hash_table<_Tp, _Hash, _Equal, _Alloc>::__subscriptable(const const_iterator*, ptrdiff_t) const
{
    return false;
}

#endif  // _LIBCPP_DEBUG_LEVEL >= 2
_LIBCPP_END_NAMESPACE_STD

#endif  // _LIBCPP__HASH_TABLE
