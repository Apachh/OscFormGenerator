#ifndef __MY_ARR_H__
#define __MY_ARR_H__

namespace tpl {

    template<typename... Types>
    struct Tuple_t;

    template<typename Head, typename... Tail>
    struct Tuple_t<Head, Tail...> : Tuple_t<Tail...> {
        Tuple_t(Head h, Tail... tail) : Tuple_t<Tail...>(tail...), head(h) {}
        Tuple_t() : Tuple_t<Tail...>() { memset(this, 0, sizeof(*this)); };

        typedef Tuple_t<Tail...> base_type;
        typedef Head             value_type;

        // base_type& base = static_cast<base_type&>(*this);
        Head head;
        static constexpr unsigned COUNT = sizeof...(Tail) + 1;
    };

    template<>
    struct Tuple_t<> {};

    template<int I, typename Head, typename... Tail>
    struct Getter_t {
        typedef typename Getter_t<I-1, Tail...>::return_type return_type;

        static return_type& get(Tuple_t<Head, Tail...>& t) {
            return Getter_t<I-1, Tail...>::get(t);
        }
    };

    template<typename Head, typename... Tail>
    struct Getter_t<0, Head, Tail...> {
        typedef typename Tuple_t<Head, Tail...>::value_type return_type;
        static return_type& get(Tuple_t<Head, Tail...>& t) {
            return t.head;
        }
    };

    template<int I, typename Head, typename... Tail>
    typename Getter_t<I, Head, Tail...>::return_type& get(Tuple_t<Head, Tail...>& t) {
        return Getter_t<I, Head, Tail...>::get(t);
    }

    template<typename TUPLE>
    static constexpr unsigned getCount() {
        return TUPLE::COUNT;
    }

    template<typename TUPLE, bool ENOUGH, int TOTAL, int... N>
    struct Sizer_t {
        auto static getSizes(unsigned* arr, TUPLE& tuple) {
            arr[sizeof...(N)] = sizeof(get<sizeof...(N)>(tuple));
            return Sizer_t<TUPLE, TOTAL == 1 + sizeof...(N), TOTAL, N..., 
                            sizeof...(N)>::getSizes(arr, tuple);
        }
    };

    template<typename TUPLE, int TOTAL, int... N>
    struct Sizer_t<TUPLE, true, TOTAL, N...> {
        auto static getSizes(unsigned* arr, TUPLE& tuple) {
            return 0;
        }
    };

    template<typename TUPLE>
    void getSizes(unsigned* arr, TUPLE& tuple) {
        Sizer_t<TUPLE, 0 == getCount<TUPLE>(), getCount<TUPLE>()>::getSizes(arr, tuple);
    }
}

namespace arr {
    template<int SIZE, typename T0, typename... T>
    class Array_t {
        public:
        constexpr Array_t() { memset(data, 0, sizeof(data)); }

        int getCount() {
            return COUNT;
        }

        unsigned long getPiece() {
            return PIECE;
        }

        template<int I>
        auto get(int i) {
            return tpl::get<I>(data[i]);
        }

        private:
        
        union {
            tpl::Tuple_t<T0, T...> data[SIZE];
            void* ptr;
        };

        static constexpr int COUNT = sizeof...(T) + 1;
        const unsigned long PIECE = sizeof(data[0]);

        // template<typename Tr>
        // constexpr void 
    };
}






#endif // __MY_ARR_H__