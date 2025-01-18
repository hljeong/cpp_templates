using uint = unsigned int;

// returns T^N(U)
template <template <typename> typename T, uint N, typename U>
struct ApplyNTimes_t;

template <template <typename> typename T, uint N, typename U>
using ApplyNTimes = typename ApplyNTimes_t<T, N, U>::value;

template <template <typename> typename T, uint N, typename U>
struct ApplyNTimes_t {
  using value = ApplyNTimes<T, N - 1, T<U>>;
};

template <template <typename> typename T, typename U>
struct ApplyNTimes_t<T, 0, U> {
  using value = U;
};

namespace templates {

using Unit = void;

struct True {
  static constexpr bool value = true;
};

struct False {
  static constexpr bool value = false;
};

template <bool B> struct MakeBool_t {};

template <bool B> using MakeBool = typename MakeBool_t<B>::value;

template <> struct MakeBool_t<true> {
  using value = True;
};

template <> struct MakeBool_t<false> {
  using value = False;
};

template <typename T, typename U> struct Equal_t {
  using value = False;
};

template <typename T> struct Equal_t<T, T> {
  using value = True;
};

template <typename T, typename U> using Equal = typename Equal_t<T, U>::value;

template <typename T, typename U, typename V> struct Ternary_t {};

template <typename T, typename U> struct Ternary_t<True, T, U> {
  using value = T;
};

template <typename T, typename U> struct Ternary_t<False, T, U> {
  using value = U;
};

template <typename T, typename U, typename V>
using Ternary = typename Ternary_t<T, U, V>::value;

template <typename T, typename U> struct In_t {};

template <typename T, typename U> using In = typename In_t<T, U>::value;

template <typename... Ts> struct List {};

template <typename T> struct In_t<T, List<>> {
  using value = False;
};

template <typename T, typename U, typename... Us>
struct In_t<T, List<U, Us...>> {
  using value = Ternary<Equal<T, U>, True, In<T, List<Us...>>>;
};

template <typename... Ts> struct Set : List<Ts...> {};

template <typename T, typename U> struct Union {};

template <typename... Ts, typename... Us>
struct Union<Set<Ts...>, Set<Us...>> : Set<Ts..., Us...> {};

using N0 = Set<>;

template <typename T> using S = Union<T, Set<T>>;

template <uint V> using N = ApplyNTimes<S, V, N0>;

template <typename T, typename U> struct Add_t {};

template <typename T, typename U> using Add = typename Add_t<T, U>::value;

template <typename T, typename U> struct Add_t<T, S<U>> {
  using value = Add<S<T>, U>;
};

template <typename T> struct Add_t<T, N0> {
  using value = T;
};

template <typename T, typename U> struct Multiply_t {};

template <typename T, typename U>
using Multiply = typename Multiply_t<T, U>::value;

template <typename T, typename U> struct Multiply_t<T, S<U>> {
  using value = Add<Multiply<T, U>, T>;
};

template <typename T> struct Multiply_t<T, N0> {
  using value = N0;
};

template <typename T, typename U> struct Integer {};

template <int Z>
using MakeInteger =
    Ternary<MakeBool<Z >= 0>, Integer<N<Z>, N0>, Integer<N0, N<-Z>>>;

template <typename T, typename U, typename V, typename W>
struct Equal_t<Integer<T, U>, Integer<V, W>> {
  using value = Equal<Add<T, W>, Add<U, V>>;
};

template <typename T, typename U, typename V, typename W>
struct Add_t<Integer<T, U>, Integer<V, W>> {
  using value = Integer<Add<T, V>, Add<U, W>>;
};

template <typename T> struct Negative_t {};

template <typename T> using Negative = typename Negative_t<T>::value;

template <typename T, typename U> struct Negative_t<Integer<T, U>> {
  using value = Integer<U, T>;
};

template <typename T, typename U> struct Subtract_t {};

template <typename T, typename U>
using Subtract = typename Subtract_t<T, U>::value;

template <typename T, typename U, typename V, typename W>
struct Subtract_t<Integer<T, U>, Integer<V, W>> {
  using value = Add<Integer<T, U>, Negative<Integer<V, W>>>;
};

template <typename T> struct Subtract_t<T, N0> {
  using value = T;
};

}; // namespace templates

int main() {
  using namespace templates;

  static_assert(Equal<List<>, List<>>::value);

  static_assert(!Equal<List<>, List<Unit>>::value);

  static_assert(Equal<List<Unit, Unit>, List<Unit, Unit>>::value);

  static_assert(!In<Unit, List<>>::value);

  static_assert(Ternary<True, True, False>::value);

  static_assert(Ternary<False, False, True>::value);

  static_assert(In<Unit, List<Unit>>::value);

  static_assert(Equal<S<N0>, S<N0>>::value);

  static_assert(Equal<N0, N0>::value);

  static_assert(Equal<ApplyNTimes<S, 0, N0>, N0>::value);

  static_assert(Equal<Set<>, Set<>>::value);

  static_assert(Equal<Set<Unit>, Set<Unit>>::value);

  // todo:
  // static_assert(Equal<Set<Unit>, Set<Unit, Unit>>::value);

  static_assert(Equal<ApplyNTimes<S, 1, N0>, S<N0>>::value);

  static_assert(Equal<N0, N<0>>::value);

  static_assert(Equal<Add<N<3>, N<5>>, N<8>>::value);

  static_assert(Equal<Multiply<N<3>, N<5>>, N<15>>::value);

  MakeInteger<0>();

  /*
   * todo: this doesnt work since the ternary evaluates the false branch
   * regardless of condition, which makes the compiler try to evaluate
   * N<static_cast<uint>(-1)>
   */
  // MakeInteger<1>();

  // static_assert(Equal < MakeInteger<-3>,
  //               Subtract<MakeInteger<7>, MakeInteger<10>>::value);

  return 0;
}
