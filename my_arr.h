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

    template<unsigned I, typename T>
    struct TupleElement_t;

    template<unsigned I, typename Head, typename... Tail>
    struct TupleElement_t<I, Tuple_t<Head, Tail...>> : TupleElement_t<I - 1, Tuple_t<Tail...>> {};

    template<typename Head, typename... Tail>
    struct TupleElement_t<0, Tuple_t<Head, Tail...>> {
        using Type = Head;
    };

    // template<typename Type>
    // union Universal_t {
    //     Type _type;
        
    // };


}

namespace arr {
    template<int SIZE, typename T0, typename... T>
    class Array_t {
        public:
        Array_t() { 
            unsigned sizes[COUNT];
            memset(this, 0, sizeof(*this));
            memset(sizes, 0, sizeof(sizes));
            tpl::getSizes(sizes, data[0]);

            // unsigned i = 0;
            // while (++i < COUNT)
            //     SHIFTS[i] = sizes[i - 1] + SHIFTS[i - 1];
        }

        int getCount() {
            return COUNT;
        }

        template<int I>
        auto& get(int i) {
            return tpl::get<I>(data[i]);
        }

        // unsigned getMemberShifts(unsigned i) {
        //     return SHIFTS[i];
        // }

        // template<typename Tr>
        // Tr& get(int i0, int i1) {
        //     void* nnptr = static_cast<void*>(&data[i0]);
        //     return *static_cast<Tr*>((nnptr + SHIFTS[i1]));
        // }

        private:
        
        tpl::Tuple_t<T0, T...> data[SIZE];

        static constexpr int COUNT = sizeof...(T) + 1;

        // unsigned SHIFTS[COUNT];
    };
}






#endif // __MY_ARR_H__