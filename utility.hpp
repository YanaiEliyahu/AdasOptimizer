/*
 * TODO (ENUMERATORS)
 * switch to && instead of const&. (can take by-value, and hide copies)
 * switch to captured_enumerator. (better handle by-value enumerators, can be moved around, passes lack of capture context in capture)
 * try to const-ize T* in places like vmany and etc. (correctness)
 - consider using another operator such as `*` `/` `+` `>` in instead of the long and weird `->*`.
 * think of a way to reduce copying of internal objects:
 * * vector<int> {1,2,3}->*expr(arg * 2). (it might copy vector<int> to the enumerator first, need to check)
 * * any copying occurring because an iterator is created. (shared_ptr on the internal enumerator?)
 * remove some enable_ifs that prevents compiler errors that are clearer than just subtitudes that could not be deduced.
 * * remove any trailing return types, as they are kind of decltypes.
 * TODO (OTHERS)
 * move weird stuff to details and expose API via `x_t<...>` and `x_v<...>`.
 * the constructor of `nanoseconds` is getting the current time, which is confusing.
 * sort the file by correct dependencies.
 * TOOD (MISC)
 * move all of those TODO's into the relevant functions with _fixme.
 */

//#stacktrace-block -- NOTE: DO NOT REMOVE, THIS IS TO HIDE IRRELEVANT STACKTRACES
#include <cstring>
#include <string>
#include <vector>
#include <deque>
#include <mutex>
#include <condition_variable>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <set>
#include <queue>
#include <sstream>
#include <cstddef>
#include <algorithm>
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <variant>
#include <unistd.h>
#include <math.h>
#include <cmath>
#include <csignal>
#include <iostream>
#include <ios>
#include <iomanip>
#include <thread>
#include <atomic>
#include <functional>
#include <limits>
#include <climits>
#include <cxxabi.h>
#include <immintrin.h>
#include <optional>
#include "span.hpp"

#define func(x) const auto &x = [&]
#define func_r(x,t,p) function<t p> x = [&] p -> t

// dynamic lambda expressions
#define expr_x_template(capture,args,body) ([capture] args {body})
#define expr0(...) expr_x_template(&,(),return (__VA_ARGS__);)
#define named_expr(x,...) expr_x_template(&,(auto &&x),return (__VA_ARGS__);)
#define named_const_expr(x,...) expr_x_template(=,(auto &&x),return (__VA_ARGS__);)
#define void_named_expr(x,...) named_expr(x,(__VA_ARGS__),void())
#define named_expr2(x,y,...) expr_x_template(&,(auto &&x, auto &&y),return (__VA_ARGS__);)
#define const_named_expr2(x,y,...) expr_x_template(=,(auto &&x, auto &&y),return (__VA_ARGS__);)
#define named_expr3(x,y,z,...) expr_x_template(&,(auto &&x, auto &&y, auto &&z),return (__VA_ARGS__);)
#define expr(...) named_expr(arg,__VA_ARGS__)
#define expra(...) named_expr(arga,__VA_ARGS__)
#define exprb(...) named_expr(argb,__VA_ARGS__)
#define exprc(...) named_expr(argc,__VA_ARGS__)
#define expr_unused(...) named_expr(,__VA_ARGS__)
#define const_expr_unused(...) named_const_expr(,__VA_ARGS__)
#define const_expr(...) named_const_expr(arg,__VA_ARGS__)
#define void_expr(...) expr((__VA_ARGS__),void())
#define void_expr2(...) expr2((__VA_ARGS__),void())
#define void_expra(...) expra((__VA_ARGS__),void())
#define void_expr2a(...) expr2a((__VA_ARGS__),void())
#define void_named_expr2(x,y,...) named_expr2(x,y,(__VA_ARGS__),void())
#define expr2(...) named_expr2(arg0,arg1,__VA_ARGS__)
#define expr2a(...) named_expr2(arg0a,arg1a,__VA_ARGS__)
#define const_expr2(...) const_named_expr2(arg0,arg1,__VA_ARGS__)
#define expr3(...) named_expr3(arg0,arg1,arg2,__VA_ARGS__)
#define expr3a(...) named_expr3(arg0a,arg1a,arg2a,__VA_ARGS__)

#define body [&](auto &&arg)
#define named_body(x) [&](auto &&x)
#define not_implemented {throw exception("not implemented");}
#define empty_implementation {}
#define true_impl {return true;}
#define false_impl {return false;}
#define STR string()+
#define STRINGIZE_DETAIL(...) #__VA_ARGS__
#define STRINGIZE(...) STRINGIZE_DETAIL(__VA_ARGS__)
#define META_TEXT(x) (normalize_path(__FILE__) + ":" + STRINGIZE(__LINE__) + ": " + (x))
#define scope(...) ([&]{__VA_ARGS__}())
#define CONCAT_(x,y) x##y
#define CONCAT(x,y) CONCAT_(x,y)
#define default_value_of_helper(name,...) ([&] { auto name = [&] {return (__VA_ARGS__);};return decltype(name())(); }())
#define default_value_of(...) default_value_of_helper(_uniquename,__VA_ARGS__)

namespace std {
#define imp_v(x) template <typename T> constexpr bool x##_v = x<T>::value
#define imp_v2(x) template <typename T, typename U> constexpr bool x##_v = x<T,U>::value
#define imp_t(x) template <typename T> constexpr bool x##_t = typename x<T>::type
#define imp_t2(x) template <typename T, typename U> constexpr bool x##_t = typename x<T,U>::type
    imp_v(is_reference);
    imp_v(is_const);
    imp_v(is_integral);
    imp_v(is_rvalue_reference);
    imp_v(is_unsigned);
    imp_v2(is_convertible);
    imp_v2(is_same);
#undef imp_v
#undef imp_v2
    template <typename T, typename U> constexpr bool decay_equiv_v = is_same_v<decay_t<T>,decay_t<U>>;
}

using std::string;
using std::shared_ptr;
using std::vector;
using std::lower_bound;
using nonstd::span;
using std::deque;
using std::stringstream;
using std::function;
using std::pair;
using std::tuple;
using std::decay;
using std::make_tuple;
using std::to_string;
using std::make_pair;
using std::sort;
using std::uncaught_exceptions;
using std::swap;
using std::get;
using std::random_access_iterator_tag;
using std::move;
using std::isfinite;
using std::begin;
using std::end;
using std::reverse;
using std::shuffle;
using std::random_shuffle;
using std::map;
using std::multimap;
using std::array;
using std::set;
using std::unordered_set;
using std::unordered_map;
using std::queue;
using std::enable_if;
using std::tuple_element;
using std::forward;
using std::mutex;
using std::condition_variable;
using std::cout;
using std::endl;
using std::index_sequence;
using std::tuple_size;
using std::make_index_sequence;
using std::unique_lock;
using std::try_to_lock;
using std::lock_guard;
using std::find;
using std::false_type;
using std::true_type;
using std::is_same;
using std::declval;
using std::remove_reference;
using std::is_const;
using std::forward_iterator_tag;
using std::remove_const;
using std::decay_t;
using std::add_const_t;
using std::remove_const_t;
using std::remove_reference_t;
using std::is_reference;
using std::raise;
using std::add_lvalue_reference;
using std::add_rvalue_reference;
using std::enable_if_t;
using std::next;
using std::is_reference_v;
using std::is_const_v;
using std::is_same_v;
using std::decay_equiv_v;
using std::is_rvalue_reference;
using std::is_convertible_v;
using std::is_integral_v;
using std::remove_pointer_t;
using std::set_terminate;
using std::is_rvalue_reference_v;
using std::add_lvalue_reference_t;
using std::add_rvalue_reference_t;
using std::void_t;
using std::ostream;
using std::boolalpha;
using std::setprecision;
using std::fixed;
using std::scientific;
using std::defaultfloat;
using std::greater;
using std::log;
using std::thread;
using std::atomic;
using std::tie;
using std::is_unsigned_v;
using std::numeric_limits;
using std::optional;
using std::nullopt;
using std::variant;
using std::holds_alternative;
using std::make_unique;
using std::is_array_v;
using std::is_base_of_v;
using std::hash;
using std::type_info;
using std::set_intersection;
using std::back_inserter;
using std::set_symmetric_difference;
using std::ifstream;
using std::ios;
constexpr size_t undefined_v = (size_t)-1;
constexpr size_t double_mantissa_precision = 53;
template<typename... Ts>
using v = vector<Ts...>;
typedef unsigned char byte;
#if _arch64
typedef __int128 int128_t;
typedef unsigned __int128 uint128_t;
#endif
typedef uint16_t ushort;
typedef uint8_t ubyte;
typedef uint8_t bool_t;
struct nothing_t {};

struct exception {
  string message;
  vector<string> stacktrace;

  exception();
  explicit exception(const string&);
};

struct scope_stacktrace {
  scope_stacktrace();
  ~scope_stacktrace();
  size_t previous_scope;

  scope_stacktrace(const scope_stacktrace&) = delete;
  scope_stacktrace &operator=(const scope_stacktrace&) = delete;
  scope_stacktrace(scope_stacktrace&&) = delete;
  scope_stacktrace &operator=(scope_stacktrace&&) = delete;
};

struct no_args_t {};

template<typename T>
struct u : std::unique_ptr<T,void(*)(void*)> {
  u(nullptr_t = nullptr) : std::unique_ptr<T,void(*)(void*)>(nullptr,nullptr) {}
  u(no_args_t) : std::unique_ptr<T,void(*)(void*)>(new T,[](void *y) {delete (T*)y;}) {}
  u(u<T>&) = delete;
  u(const u<T>&) = delete;
  u<T> &operator=(const u<T>&) = delete;
  u(u<T>&&) = default;
  u<T> &operator=(u<T>&&) = default;
  template<typename U>
  u(u<U> x, enable_if_t<is_base_of_v<T,U>>* = nullptr) : std::unique_ptr<T,void(*)(void*)>(x.release(),x.get_deleter()) {}
  template<typename ...Args>
  u(Args &&...args) : std::unique_ptr<T,void(*)(void*)>(new T(forward<remove_reference_t<Args>>(args)...),[](void *y) {delete (T*)y;}) {}
  T &operator[](size_t index) {return (*this)[index];}
};
template<typename T>
struct u<T[]> : std::unique_ptr<T[],void(*)(void*)> {
  u(nullptr_t = nullptr) : std::unique_ptr<T[],void(*)(void*)>(nullptr,nullptr) {}
  u(size_t item_count) : std::unique_ptr<T[],void(*)(void*)>(new T[item_count],[](void *y) {delete[] (T*)y;}) {}
};

// code attributes and contracts:
// _debug:              is a makefile-level macro that indicates whatever to compile the code for debugging.
// _debug_assert:       an assertion that is for hinting the assumpation that can be made on the code, and guards against their contradictions.
//                      this assertion is disabled in release.
//
// _assert:             similar to _debug_assert, just that in release mode it let's the compiler optimize by the assertion,
//                      and errors when the compiler cannot optimize out the expression.
//
// _assertr:            an assertion that is for checking whatever something failed to happen, not from a bug but failure of the system.
//                      this assertion gets through release.
//
// _check:              used for assertitions that cannot be done at compile-time, as it requires error message.
//                      this assertion is disabled in release.
//
// _fixme:              a print for indicating that some capability is missing.
// _breakpoint:         is used for putting a hardcoded breakpoint.
// _unreachable:        is used for disabling part of a code as it throws exception before that part of code executes,
//                      but in release mode it let the compiler optimize by that the code is not reachable.
//
// _unused:             to indicate that one function will never or maybe not used ever in that compilation unit.
// _speed:              hint the compiler to optimize a function more aggressively for performance.
// _size:               hint the compiler to optimize a function for size.
// _debug_unwind:       prints the expressions being printed and the return value of the expression, when exception is unwind in debug mode.
// _debug_expr:         similar to _debug_unwind, without the condition of when stack is unwinding.
// _debug_println:      a `println` that will be active only in debug mode.
// _debug_eval:         will evaluate the expression in debug mode, but leave default value for release mode.
// _undeclare:          undeclare a variable, useful when there are multiple similar names in the same scope.
// _nameof:             will return the arguments stringed with ` at the start and end of them, but checks that the expression can be compiled.
//                      useful when a string depends on variable's name that can be changed.
//
// _defined:            checks that a `size_t` is not `undefined_v`.
// _uniquename:         aliases to random name that is unique to that compilation unit.
// _exit:               traditional-like exit, but destruct stack's objects when exiting.
// _fallthrough         indicate fallthrough in switch-case, mainly to silent a worrying error.
// _likely:             tells the compiler to that expression is most likely be true.
// _unlikely:           tells the compiler to that expression is most likely be false.
#if _optimizations
#define no_side_effects(...) ([&]() __attribute__ ((noinline,error ("cannot be proven to have no side effects"))) {if (!(__VA_ARGS__)) {}}())
#else
#define no_side_effects(...) /* this doesn't work with optimization off */
#endif
extern map<pair<const char*,size_t>,bool> _fixme_prints;
#define _rfixme(x) scope(auto &var = _fixme_prints[pair(__FILE__,__LINE__)]; if (_unlikely(!var)) {var = true; fixme(META_TEXT(x).c_str());})
#if _debug
#define _debug_assert(...) scope(if (_unlikely(!(__VA_ARGS__))) throw exception("'" STRINGIZE(__VA_ARGS__) "', assertion failed");)
#define _assert(...) scope(_debug_assert(__VA_ARGS__);)
#define _check(x,y) _checkr(x,y);
#define _debug_unwind(expr) _unwind(expr)
#define _unreachable() throw exception("entered a code that is marked as unreachable")
#define _debug_expr(...) _expr(__VA_ARGS__)
#define _debug_println(...) println(__VA_ARGS__)
#define _debug_eval(...) (__VA_ARGS__)
#define _exit(...) (throw unwind(__VA_ARGS__))
#define _fixme _rfixme
#else
//#define _assert(...) scope(no_side_effects(__VA_ARGS__); if (!(__VA_ARGS__)) _unreachable();)
// USRF has bugs with the above `assert` need to convert the `if !() undefined_behavior();` to `if !() exception();`
#define _assert(...) scope(no_side_effects(__VA_ARGS__);)
#define _debug_assert(...) scope(default_value_of(__VA_ARGS__);)
#define _check(x,y) scope(_use(x,y);)
#define _fixme(x) scope(void_t<decltype(x)>();)
#define _debug_unwind(expr) do{[&] {(void)(expr);};}while(false)
#define _unreachable() __builtin_unreachable()
#define _debug_expr(...) _use(__VA_ARGS__)
#define _debug_println(...) _use(__VA_ARGS__)
#define _debug_eval(...) default_value_of(__VA_ARGS__)
#define _exit(...) exit(__VA_ARGS__)
#endif
#define _checkr(x,y) scope(if (_unlikely(!(x))) throw exception(y);)
#define _assertr(...) scope(if (_unlikely(!(__VA_ARGS__))) throw exception("'" STRINGIZE(__VA_ARGS__) "', assertion failed");)
#define _breakpoint() scope(raise(SIGINT);)
#define _unused [[gnu::unused]]
#define _speed __attribute__((hot))
#define _size __attribute__((cold))
#define _nameof(x) (_use(x),string("`" STRINGIZE(x) "`"))
#define _defined(x) _check((x) != undefined_v,STRINGIZE(x) " is undefined")
#define _undefined(x) _check((x) == undefined_v,STRINGIZE(x) " is defined")
#define _uniquename CONCAT(_name_generated_by_uniquename_, __COUNTER__)
#define _unwind(expr) destructor _uniquename([&] (const char *filename, size_t line) { \
    return [&,filename,line] {if (_unlikely(uncaught_exceptions())) println(normalize_path(filename),":",line,":",#expr," = \"",expr,"\"");}; \
}(__FILE__,__LINE__))
#define _expr(...) println(STRINGIZE(__VA_ARGS__) ": ",__VA_ARGS__)
#define _likely(...) __builtin_expect((__VA_ARGS__),1)
#define _unlikely(...) __builtin_expect((__VA_ARGS__),0)
#define anon_declare(...) auto _uniquename = __VA_ARGS__
#define _field(...) pair(STRINGIZE(__VA_ARGS__),stringify(__VA_ARGS__))
#define _this_to_string_formatting(stream,...) apply_to_string_formatting_2(typeid(*this),(void*)this,stream,v<pair<string,string>>{__VA_ARGS__})
#if __GNUC__ >= 7
#define _fallthrough __attribute__((fallthrough));
#else
#define _fallthrough
#endif
// rand srand
#pragma GCC poison exit valarray

inline void memory_copy(void *destination, const void *source, size_t byte_count) {
  if (!byte_count)
    return;
  memcpy(destination,source,byte_count);
#pragma GCC poison memcpy
}
inline void memory_move(void *destination, const void *source, size_t byte_count) {
  if (!byte_count)
    return;
  memmove(destination,source,byte_count);
#pragma GCC poison memmove
}
inline auto zero(void *buffer, size_t byte_count) {
  if (!byte_count)
    return;
  memset(buffer,0,byte_count);
#pragma GCC poison memset
}
struct destructor {
    template<typename T>
    destructor(T &&t) : f((decltype(f))forward<T>(t)) {}
    ~destructor() {f();}

    function<void()> f;
};

struct object_base {
  template<typename T>
  T *cast_or_null() {
    return dynamic_cast<T*>(this);
  }
  template<typename T>
  const T *cast_or_null() const {
    return dynamic_cast<const T*>(this);
  }
  template<typename T>
  T *cast() {
    auto result = cast_or_null<T>();
    _assert(result);
    return result;
  }
  template<typename T>
  const T *cast() const {
    auto result = cast_or_null<T>();
    _assert(result);
    return result;
  }
  template<typename T>
  bool is() const {
    return cast_or_null<T>();
  }

  string to_string() const;
  bool operator==(const object_base &other) const {
    return this == &other || equals(other) || other.equals(*this);
  }
  bool operator!=(const object_base &other) const {return !(*this == other);}
  virtual ostream &to_string_on_stream(ostream&) const;
  virtual string to_code() const;
  virtual ~object_base() = default;

protected:
  virtual bool equals(const object_base&) const {return false;}
};
ostream &operator<<(ostream &stream, const object_base &x);

string get_application_path();
string get_application_directory();
string get_working_directory();
size_t get_modifications_time(const string&);
string normalize_path(const string &path);
string path_concat(const string &left, const string &right);
string path_parent(const string &path);
string read_file(const string &filename);
void write_file(const string &filename, const string &content);
void remove_file(const string &filename);
bool file_exists(const string &filename);
string resolve_path(const string &path);
v<string> list_files(const string &directory_path);
v<string> list_directories(const string &directory_path);
string run_command(const string&);
string tput(const string&);
v<string> get_stacktrace();
bool is_tty();
string term_padding(const string &text, size_t pad_size);
string parse_precision(double d, size_t precision);
string parse_precision_scientific(double d, size_t precision);
void async(function<void()>&&);
void async_bulk(const v<function<void()>>&, bool recursive = false);
void set_process_memory_limit(size_t byte_count);
size_t random_number();
size_t random_number_range(size_t min, size_t max);
double random_real_range(double min, double max);
void explain_error(const string &what_happened, const struct exception &exception, bool is_critical);
bool random_bit();
bool random_bit(double);
void expected_fail(const function<void()>&);

template<typename V>
auto pop(V &x) {
  auto result = move(x.back());
  x.pop_back();
  return move(result);
}

struct random_number_object {
  typedef decltype(random_number()) result_type;
  auto operator()() const {return random_number();}
  auto min() const {return numeric_limits<result_type>::min();}
  auto max() const {return numeric_limits<result_type>::max();}
};

template<typename T, typename U>
auto min(T t, U u) {
  typedef decltype(t + u) result_t;
  return (result_t)t < (result_t)u ? (result_t)t : (result_t)u;
}
template<typename T, typename U>
auto max(T t, U u) {
  typedef decltype(t + u) result_t;
  return (result_t)t > (result_t)u ? (result_t)t : (result_t)u;
}
template<typename T, typename U, typename V>
auto safe_div(T left, U right, V def) {
  typedef decltype(left + right + def) result_t;
  return right != U(0) ? (result_t)(left / right) : (result_t)def;
}
template<typename T, typename U, typename V>
auto safe_mod(T left, U right, V def) {
  typedef decltype(left + right + def) result_t;
  return right != U(0) ? (result_t)(left % right) : (result_t)def;
}
template<typename T, typename U, typename V>
auto safe_minus(T left, U right, V def) {
  typedef decltype(left + right + def) result_t;
  return (result_t)left >= (result_t)right ? (result_t)(left - right) : (result_t)def;
}
template<typename T, typename U>
auto ceil_div(T t, U u) {
  return (t + u - 1) / u;
}
template<typename T, typename U, typename V>
auto median(T t, U u, V v) {
  return max(min(t,u),min(max(t,u),v));
}
auto sign(auto x) {
  return (decltype(x))(int(0 < x) - int(x < 0));
}
_unused static int get_exponent(double x) {
  int result;
  frexp(x,&result);
  return result;
}
template<typename T>
inline auto sqr(const T &t) {
  return t * t;
}

// prints type as error
template<typename>
struct print_type;

namespace details {
  struct can_call_test {
    template<typename F, typename... A>
    static decltype(declval<F>()(declval<A>()...), true_type())
    f(int);

    template<typename F, typename... A>
    static false_type
    f(...);
  };
  struct has_size_test {
    template<class T>
    static decltype(declval<T>().size(),true_type()) f(int);
    template<class>
    static false_type f(...);
  };
  template<typename T>
  struct has_iterator {
    typedef remove_reference_t<remove_const_t<T>> test_type;

    template<typename A>
    static constexpr bool test(
      A * pt,
      decltype(pt->begin()) * = nullptr,
      decltype(pt->end()) * = nullptr
    ) {return true;}

    template<typename A>
    static constexpr bool test(...) {return false;}

    static constexpr bool value = test<test_type>(nullptr);
  };
}

template<typename F, typename ...Args>
struct can_call : decltype(details::can_call_test::f<F,Args...>(0)) {};

template<typename T>
struct has_size : decltype(details::has_size_test::f<T>(0)) {};

template<typename F, typename ...Args>
static constexpr bool can_call_v = can_call<F,Args...>::value;

template<typename T>
static constexpr bool has_size_v = has_size<T>::value;

template<typename T>
static constexpr bool has_iterator_v = details::has_iterator<T>::value;

enum class enabler_t {};

template<bool b>
using EnableIf = enable_if_t<b, enabler_t>;

struct unwind {unwind(int return_code_) : return_code(return_code_) {} int return_code;};

template <typename T>
struct shared_queue {
  T pop() {
    unique_lock<mutex> mlock(mutex_);
    while (queue_.empty())
      cond_.wait(mlock);
    auto item = move(queue_.front());
    queue_.pop_front();
    return item;
  }
  template<typename U>
  void push(U &&item) {
    unique_lock<mutex> mlock(mutex_);
    queue_.push_back(item);
    cond_.notify_one();
  }
  void push(const v<T> &items) {
    unique_lock<mutex> mlock(mutex_);
    queue_.insert(queue_.end(),items.begin(),items.end());
    cond_.notify_all();
  }

  deque<T> queue_;
  mutex mutex_;
  condition_variable cond_;
};

void fixme(const string &message);

template<typename ...Args>
_unused static void _use(Args&&...) {}

struct pair_hash {
  template<typename T, typename T2>
  size_t operator()(const pair<T,T2> &x) const {
    return (hash<T>()(x.first) ^ 10433199877832032000ULL) - hash<T2>()(x.second);
  };
};

struct nanoseconds {
  nanoseconds();
  nanoseconds(uint64_t);
  nanoseconds &operator-=(const nanoseconds&);
  nanoseconds operator-(const nanoseconds&) const;
  nanoseconds operator-() const;
  nanoseconds &operator+=(const nanoseconds&);
  nanoseconds operator+(const nanoseconds&) const;
  nanoseconds operator+() const;
  bool operator<(const nanoseconds&) const;
  bool operator>(const nanoseconds&) const;
  bool operator<=(const nanoseconds&) const;
  bool operator>=(const nanoseconds&) const;
  operator bool() const;
  uint64_t get() const;

private:
  uint64_t value;
};

template<size_t I = 0, typename FuncT, typename... Tp>
inline typename enable_if<I == sizeof...(Tp), void>::type
foreach(const tuple<Tp...>&, FuncT) {}
template<size_t I = 0, typename FuncT, typename... Tp>
inline enable_if_t<I < sizeof...(Tp), void>
foreach(const tuple<Tp...>& t, FuncT f) {
  f(get<I>(t));
  foreach<I + 1, FuncT, Tp...>(t,f);
}
ostream& operator<<(ostream &stream, const exception&);
ostream& operator<<(ostream &stream, const nanoseconds&);
#if _arch64
ostream& operator<<(ostream &stream, uint128_t);
#endif
template<typename T, EnableIf<has_iterator_v<T> && !is_same_v<T,string>>...>
ostream& operator<<(ostream &stream, const T &t) {
  stream << "{";
  bool first = true;
  for (auto it = begin(t); it != end(t); ++it) {
    if (!first)
      stream << ",";
    else
      first = false;
    stream << *it;
  }
  stream << "}";
  return stream;
}
template<typename F, typename L>
ostream& operator<<(ostream &stream, const pair<F,L> &plr) {
  stream << "(" << plr.first << "," << plr.second << ")";
  return stream;
}
template<typename ...Ts>
ostream& operator<<(ostream &stream, const tuple<Ts...> &ts) {
  stream << "(";
  bool first = true;
  foreach(ts,[&](auto &p) {
    if (!first)
        stream << ",";
    else
        first = false;
    stream << p;
  });
  stream << ")";
  return stream;
}
string to_string(const type_info &x);

template<typename Shifted>
auto &apply_shifts(Shifted &shifted) {return shifted;}
template<typename Shifted, typename Arg, typename ...Args>
auto &apply_shifts(Shifted &shifted, const Arg &arg, const Args &...args) {
  return apply_shifts(shifted << arg,args...);
}
template<typename ...Args>
string stringify(const Args &...args) {
    stringstream x;
    apply_shifts(x,boolalpha,args...);
    return x.str();
}
string vector_to_string_formatting(const v<string>&);
string prefix_lines_with(const string&, const string &prefix);
ostream &apply_to_string_formatting_2(const type_info &type, void *self, ostream &x, const v<pair<string,string>> &fields);
extern string println_prefix;
extern __thread bool stdout_silence;
template<typename ...Args>
void print(const Args &...args) {
  if (stdout_silence)
    return;
  apply_shifts(cout,boolalpha,args...);
#pragma GCC poison cout
}
template<typename ...Args>
void println(const Args &...args) {
  print(println_prefix,args...,"\n");
}

template<typename T>
struct by_value_ptr {
  typedef T value_type;
  by_value_ptr() {}
  by_value_ptr(const by_value_ptr<T> &other) {
    if (other.initialized)
      reset(*other.get_pointer_impl());
  }
  template<typename U, typename = enable_if_t<decay_equiv_v<T,U>>>
  by_value_ptr(U &&u) {
    reset(forward<U>(u));
  }
  by_value_ptr &operator=(const by_value_ptr<T> &other) {
    this->~by_value_ptr<T>();
    new (this) by_value_ptr(other);
    return *this;
  }
  template<typename U, typename = enable_if_t<decay_equiv_v<T,U>>>
  by_value_ptr &operator=(U &&u) {
    reset(forward<U>(u));
  }
  template<typename U, typename = enable_if_t<decay_equiv_v<T,U>>>
  void reset(U &&u) {
    if (initialized)
      get_pointer_impl()->~T();
    new (get_pointer_impl()) T(forward<U>(u));
    initialized = true;
  }
  T *get_pointer() {
    _check(initialized,"pointer is null");
    return get_pointer_impl();
  }
  const T *get_pointer() const {
    _check(initialized,"pointer is null");
    return get_pointer_impl();
  }
  void reset() {
    if (!initialized)
      return;
    get_pointer()->~T();
    initialized = false;
  }
  bool is_initialized() const {
    return initialized;
  }
  T *operator->() {
    return get_pointer();
  }
  const T *operator->() const {
    return get_pointer();
  }
  operator T&() {
    return *get_pointer();
  }
  operator const T&() const {
    return *get_pointer();
  }
  T &operator*() {
    return *get_pointer();
  }
  const T &operator*() const {
    return *get_pointer();
  }
  ~by_value_ptr() {
    reset();
  }

private:
  T *get_pointer_impl() {
    return (T*)&data;
  }
  const T *get_pointer_impl() const {
    return (const T*)&data;
  }

  byte data[sizeof(T)];
  bool initialized = false;
};

template<typename T>
vector<T> vappend(const vector<T> &v1, const vector<T> &v2) {
  vector<T> result;
  result.reserve(v1.size() + v2.size());
  result.insert(result.end(),v1.begin(),v1.end());
  result.insert(result.end(),v2.begin(),v2.end());
  return result;
}
template<typename T, typename... Ts>
vector<T> vappend(const vector<T> &v1, const vector<T> &v2, const Ts &...ts) {
  return vappend(vappend(v1,v2),ts...);
}
template<typename V, typename V2, EnableIf<has_size_v<V2>>...>
void vappend_to(V &v1, const V2 &v2) {
  v1.reserve(v1.size() + v2.size());
  v1.insert(v1.end(),v2.begin(),v2.end());
}
template<typename V, typename V2, EnableIf<!has_size_v<V2>>...>
void vappend_to(V &v1, const V2 &v2) {
  for (auto &v : v2)
    v1.push_back(v);
}
template<typename V, typename I>
void vappend_to(V &v1, I begin, I end) {
  v1.reserve(v1.size() + (end - begin));
  v1.insert(v1.end(),begin,end);
}
template<typename T, typename V>
auto vfind(const T &t, const V &v) {
  return find(v.begin(),v.end(),t);
}
template<typename T, typename V>
size_t vindex(const T &t, const V &v) {
  auto it = vfind(t,v);
  if (it == v.end())
    return undefined_v;
  else
    return it - v.begin();
}

template<typename T, typename = void>
struct iterator_value_type {};
template<typename T>
struct iterator_value_type<T,enable_if_t<has_iterator_v<T>>> {
  typedef decltype(*declval<T>().begin()) type;
};

template<typename T>
using iterator_value_type_t = typename iterator_value_type<T>::type;

/*
 * ASSUMPTIONS:
 * any enumerator has begin and end.
 * any iterator has == != ++it *it it->. (cannot be moved backwards or it++)
 * any value previously produced value by iterator is disposed by incrementing the iterator.
 */

template<typename T,typename,bool = is_const_v<T>>
struct iterator_generic {};
template<typename T, typename F>
struct iterator_generic<T, F, false> {
  typedef T value_type;
  typedef forward_iterator_tag iterator_category;

  iterator_generic(const F &next_, bool is_end) : next(next_), value(is_end ? nullptr : next()) {}
  bool operator==(const iterator_generic &it) const {return value == it.value;}
  bool operator!=(const iterator_generic &it) const {return !(*this == it);}
  iterator_generic &operator++() {value = next(); return *this;}
  T &operator*() {return *value;}
  T &operator->() {return *value;}

  F next;
  T *value;
};
template<typename T, typename F>
struct iterator_generic<T, F, true> {
  typedef T value_type;
  typedef forward_iterator_tag iterator_category;

  iterator_generic(const F &next_, bool is_end) : next(next_) {
    if (is_end)
      return;
    auto ptr = next();
    if (ptr)
      value.reset(*ptr);
  }
  bool operator==(const iterator_generic &it) const {return value.is_initialized() == it.value.is_initialized();}
  bool operator!=(const iterator_generic &it) const {return !(*this == it);}
  iterator_generic &operator++() {
    auto ptr = next();
    if (ptr)
      value.reset(*ptr);
    else
      value.reset();
    return *this;
  }
  T &operator*() {return *value;}
  T &operator->() {return *value;}

  by_value_ptr<remove_const_t<T>> value;
  F next;
};

template<typename T, typename F = function<T*()>>
struct enumerator {
  typedef T value_type;
  typedef F function_type;
  static_assert(is_same_v<T*,decltype(declval<F>()())>,"the type of the return type of the function is not T");

  typedef iterator_generic<T,F> iterator;

  iterator begin() const {
    return iterator(next,false);
  }
  iterator end() const {
    return iterator(next,true);
  }
  operator vector<remove_reference_t<remove_const_t<T>>>() const {
    vector<remove_reference_t<remove_const_t<T>>> result;
    for (auto &v : *this)
      result.push_back(v);
    return result;
  }
  auto generic() {
    return enumerator<remove_reference_t<decltype(*next())>,function<decltype(next())()>>::make(forward<F>(next));
  }

  static auto make(F &&f) {
    return enumerator<T,F>(forward<F>(f));
  }

private:
  enumerator() {}
  enumerator(function_type &&next_) : next(move(next_)) {}
  function_type next;
};

template<typename F>
_unused static auto make_enumerator(F &&f) {
  return enumerator<remove_reference_t<decltype(*f())>,F>::make(forward<F>(f));
}
template<typename F>
_unused static auto make_enumerator_generic(F &&f) {
  return enumerator<remove_reference_t<decltype(*f())>,function<decltype(f())>>::make(forward<F>(f));
}

template<typename T, bool = is_reference_v<T>>
struct const_temporary {typedef T type;};
template<typename T>
struct const_temporary<T,false> {typedef const T type;};

template<typename T>
using const_temporary_t = typename const_temporary<T>::type;

template<typename T>
struct remove_rvalue_ref {
  typedef T type;
};
template<typename T>
struct remove_rvalue_ref<T&&> {
  typedef T type;
};
template<typename T>
using remove_rvalue_ref_t = typename remove_rvalue_ref<T>::type;

template<typename T>
struct captured_enumerator {
  template<typename U>
  captured_enumerator(U &&u, enable_if_t<(!is_reference_v<T> || is_rvalue_reference_v<T>) && is_same_v<T,decay_t<U>>>* = nullptr) : enumerator(move(u)), iterator(enumerator.begin()) {}
  template<typename U = T>
  captured_enumerator(T t, enable_if_t<is_reference_v<U>>* = 0) : enumerator(t), iterator(enumerator.begin()) {}
  captured_enumerator(const captured_enumerator<T> &t) : enumerator(t.enumerator), iterator(enumerator.begin()) {}
  captured_enumerator<T> &operator=(const captured_enumerator<T> &t) {
    this->~captured_enumerator();
    new (this) captured_enumerator(t);
    return *this;
  }

  typedef remove_rvalue_ref_t<const_temporary_t<T>> enumerator_t;
  enumerator_t enumerator;
  decltype(enumerator.begin()) iterator;
};

template<typename F>
auto vbox(F &&f) {
  return [=] {return f;};
}
template<typename F>
auto vunbox(F &&f) {
  return f();
}

template<typename E, typename F, EnableIf<can_call_v<F,iterator_value_type_t<E>>>..., EnableIf<!is_same_v<decltype(declval<F>()(declval<iterator_value_type_t<E>>())),void>>...>
auto operator->*(E &&e, F &&f) {
  typedef decltype(f(declval<iterator_value_type_t<E>>())) RT;
  return make_enumerator([first = true,f,c=move(captured_enumerator<E>(e)),result=by_value_ptr<remove_const_t<RT>>()]() mutable -> remove_reference_t<const_temporary_t<RT>>* {
    if (c.iterator != c.enumerator.end()) {
      if (first)
        first = false;
      else if (++c.iterator == c.enumerator.end())
        return nullptr;
      result.reset(f(*c.iterator));
      return result.get_pointer();
    }
    return nullptr;
  });
}
template<typename E, typename F, EnableIf<can_call_v<F,iterator_value_type_t<E>,size_t>>..., EnableIf<!is_same_v<decltype(declval<F>()(declval<iterator_value_type_t<E>>(),declval<size_t>())),void>>...>
auto operator->*(E &&e, F &&f) {
  typedef decltype(f(declval<iterator_value_type_t<E>>(),(size_t)0)) RT;
  return make_enumerator([first = true, f,c=move(captured_enumerator<E>(e)),result=by_value_ptr<remove_const_t<RT>>(),index=(size_t)0]() mutable -> remove_reference_t<const_temporary_t<RT>>* {
    if (c.iterator != c.enumerator.end()) {
      if (first)
        first = false;
      else if (++c.iterator == c.enumerator.end())
        return nullptr;
      result.reset(f(*c.iterator,index++));
      return result.get_pointer();
    }
    return nullptr;
  });
}
template<typename E, typename F, EnableIf<can_call_v<F,iterator_value_type_t<E>>>..., EnableIf<is_same_v<decltype(declval<F>()(declval<iterator_value_type_t<E>>())),void>>...>
void operator->*(E &&e, F &&f) {
  for (auto &p : e)
    f(p);
}
template<typename E, typename F, EnableIf<can_call_v<F,iterator_value_type_t<E>,size_t>>..., EnableIf<is_same_v<decltype(declval<F>()(declval<iterator_value_type_t<E>>(),declval<size_t>())),void>>...>
void operator->*(E &&e, F &&f) {
  size_t index = 0;
  for (auto &p : e)
    f(p,index++);
}
template<typename E, typename F, EnableIf<can_call_v<F>>...>
auto operator->*(E &&e, F &&f) {
  return f()(forward<E>(e));
}

template<typename T, typename U, bool is_const = is_const_v<remove_reference_t<T>> || is_const_v<remove_reference_t<U>>, bool is_same = decay_equiv_v<T,U>>
struct combine_const {};
template<typename T, typename U>
struct combine_const<T,U,false,true> {
  typedef remove_reference_t<T> type;
};
template<typename T, typename U>
struct combine_const<T,U,true,true> {
  typedef const remove_reference_t<remove_const_t<T>> type;
};

template<typename T, typename U>
using combine_const_t = typename combine_const<T,U>::type;

template<typename E1, typename E2, EnableIf<has_iterator_v<E1>>..., EnableIf<has_iterator_v<E2>>...>
auto operator->*(E1 &&e1, E2 &&e2) {
  typedef combine_const_t<
    iterator_value_type_t<typename captured_enumerator<E1>::enumerator_t>,
    iterator_value_type_t<typename captured_enumerator<E2>::enumerator_t>
  > RT;
  return make_enumerator([=,first1 = true, first2 = true,c1=captured_enumerator<E1>(e1),c2=captured_enumerator<E2>(e2)]() mutable -> RT* {
    if (c1.iterator == c1.enumerator.end())
      if (c2.iterator == c2.enumerator.end())
        return nullptr;
      else if (!first2)
        if (++c2.iterator == c2.enumerator.end())
          return nullptr;
        else
          return &*c2.iterator;
      else {
        first2 = false;
        return &*c2.iterator;
      }
    else if (!first1) {
      if (++c1.iterator == c1.enumerator.end()) {
        if (c2.iterator == c2.enumerator.end())
          return nullptr;
        first2 = false;
        return &*c2.iterator;
      }
      return &*c1.iterator;
    }
    else {
      first1 = false;
      return &*c1.iterator;
    }
  });
}
template<typename T, typename U>
_unused static auto vrange(T &&start, U &&end) {
  _assertr(start <= end);
  return make_enumerator([current = (decay_t<T>)(start - 1),end]() mutable -> add_const_t<decay_t<T>>* {
    ++current;
    if (current == end)
      return nullptr;
    return &current;
  });
}
template<typename T>
_unused static auto vrange(T &&end) {
  typedef decay_t<T> T_;
  return make_enumerator([current = T_() - 1,end]() mutable -> add_const_t<T_>* {
    ++current;
    if (current == end)
      return nullptr;
    return &current;
  });
}
template<typename F>
auto vreduce(F &&f) {
  return vbox([=](auto &&e)
    -> decltype(f(declval<iterator_value_type_t<decltype(e)>>(),declval<iterator_value_type_t<decltype(e)>>())) {
    static_assert(can_call_v<F,iterator_value_type_t<decltype(e)>,iterator_value_type_t<decltype(e)>>
      ,"cannot call the function with the enumerator's type");
    auto it = e.begin();
    _checkr(it != e.end(),"no elements");
    auto result = *it;
    while (++it != e.end())
      result = f(result,*it);
    return result;
  });
}
template<typename F, typename T>
auto vfold(F &&f, T &&inital) {
  return vbox([=](auto &&e)
    -> remove_const_t<remove_reference_t<T>> {
    auto it = e.begin();
    if (it == e.end())
      return inital;
    T result = f(inital,*it);
    while (++it != e.end())
      result = f(result,*it);
    return result;
  });
}
template<typename F>
auto vfold(F &&f) {
  return vbox([=](auto &&e) -> remove_reference_t<iterator_value_type_t<decltype(e)>> {
    typedef iterator_value_type_t<decltype(e)> e_type;
    typedef decay_t<decltype(f(declval<e_type>(),declval<e_type>()))> value_type;
    auto it = e.begin();
    if (it == e.end())
      return value_type();
    value_type result = f(value_type(),*it);
    while (++it != e.end())
      result = f(result,*it);
    return result;
  });
}
_unused static auto vsum() {
  return vunbox(vfold(expr2(arg0 + arg1)));
}
_unused static auto vavg() {
  return named_expr(x,x->*expr(make_pair(arg,1))->*vfold(expr2(make_pair(arg0.first + arg1.first,arg0.second + arg1.second)))->*vbox(expr(arg.second == 0 ? 0 : arg.first / arg.second)));
}
template<typename T, typename F>
auto vagg(F &&f, T &&initial) {
  return vbox([=](auto &&e) -> decay_t<T> {
    auto it = e.begin();
    if (it == e.end())
      return initial;
    T result = f(initial,*it);
    while (++it != e.end())
      result = f(result,*it);
    return result;
  });
}
template<typename F>
auto vagg(F &&f) {
  return vbox([=](auto &&e) -> decay_t<decltype(f(declval<iterator_value_type_t<decltype(e)>>(),declval<iterator_value_type_t<decltype(e)>>()))> {
    typedef decay_t<decltype(f(declval<iterator_value_type_t<decltype(e)>>(),declval<iterator_value_type_t<decltype(e)>>()))> T;
    return e->*vagg<T>(f,T());
  });
}
template<typename F, typename T>
auto vfirst(F &&f, T &&def) {
  return vbox([=](auto &&e) mutable -> const decay_t<iterator_value_type_t<decltype(e)>> {
    auto it = e.begin();
    if (it == e.end())
      return def;
    do {
      auto val = *it;
      if (f(val))
        return val;
    } while (++it != e.end());
    return def;
  });
}
template<typename F, typename T>
auto vlast(F &&f, T &&def) {
  return vbox([=](auto &&e) mutable -> const decay_t<iterator_value_type_t<decltype(e)>> {
    auto it = e.begin();
    if (it == e.end())
      return def;
    auto result = def;
    do {
      auto val = *it;
      if (f(val))
        result = val;
    } while (++it != e.end());
    return result;
  });
}
template<typename E>
struct levelled_captured_enumerator {
  levelled_captured_enumerator(E e) : enumerator(forward<E>(e)) {
    if (enumerator.iterator != enumerator.enumerator.end())
      inner_enumerator.reset(typename decltype(inner_enumerator)::value_type(*enumerator.iterator));
  }
  levelled_captured_enumerator(const levelled_captured_enumerator &o) : enumerator(o.enumerator) {
    if (enumerator.iterator != enumerator.enumerator.end())
      inner_enumerator.reset(typename decltype(inner_enumerator)::value_type(*enumerator.iterator));
  }

  captured_enumerator<E> enumerator;
  by_value_ptr<captured_enumerator<add_lvalue_reference_t<iterator_value_type_t<E>>>> inner_enumerator;
};
_unused static auto vmany() {
  return [](auto &&e) {
    typedef remove_reference_t<iterator_value_type_t<iterator_value_type_t<decltype(e)>>> value_type;
    return make_enumerator([first = true, v = levelled_captured_enumerator<remove_rvalue_ref_t<decltype(e)>>(e)]() mutable -> value_type* {
      if (v.inner_enumerator.is_initialized() == false)
        return nullptr;
      while (v.inner_enumerator->iterator == v.inner_enumerator->enumerator.end()) {
        start_searching:
        if (++v.enumerator.iterator == v.enumerator.enumerator.end())
          return nullptr;
        v.inner_enumerator.reset(typename decltype(v.inner_enumerator)::value_type(*v.enumerator.iterator));
        first = true;
      }
      if (first) {
        first = false;
        return &*v.inner_enumerator->iterator;
      }
      if (++v.inner_enumerator->iterator != v.inner_enumerator->enumerator.end())
        return &*v.inner_enumerator->iterator;
      goto start_searching;
    });
  };
}
_unused static auto vall() {
  return vunbox(vfirst(
    [](const auto &p) -> bool {
      return !(bool)p;
    },
    true
  ));
}
_unused static auto vany() {
  return vunbox(vfirst(
    [](const auto &p) -> bool {
      return (bool)p;
    },
    false
  ));
}
_unused static auto vone() {
  return [](const auto &e) -> decay_t<iterator_value_type_t<decltype(e)>> {
    auto it = e.begin();
    _checkr(it != e.end(),"no elements");
    auto result = *it;
    ++it;
    _checkr(it == e.end(),"more than one element");
    return result;
  };
}
_unused static auto to_vector() {
  _fixme("can make a case where the vectorized enumerator is already a const vector");
  return [](const auto &e) {
    vector<decay_t<iterator_value_type_t<decltype(e)>>> result;
    for (auto &p : e)
      result.push_back(p);
    return result;
  };
}
_unused static auto stringize() {
  _fixme("can make a case where the vectorized enumerator is already a const vector");
  return [](const auto &e) {
    string result;
    for (auto &p : e)
      result.push_back(p);
    return result;
  };
}
#define to_v to_vector
_unused static auto to_enumerator() {
  return [](auto &&e) {
  return make_enumerator([c=move(captured_enumerator<remove_rvalue_ref_t<decltype(e)>>(e)),first=true]() mutable -> decltype(&declval<iterator_value_type_t<remove_rvalue_ref_t<decltype(e)>>>()) {
    if (c.iterator == c.enumerator.end())
      return nullptr;
    if (first)
      first = false;
    else
      c.iterator++;
    if (c.iterator == c.enumerator.end())
      return nullptr;
    return &*c.iterator;
  });};
}
template<typename F>
_unused static auto vsortf(F &&f) {
  return vbox([f=move(f)](const auto &e) {
    auto result = e->*to_vector;
    stable_sort(result.begin(),result.end(),f);
    return result;
  });
}
_unused static auto vsort() {
  return [](const auto &e) {
    vector<decay_t<iterator_value_type_t<decltype(e)>>> result = e->*to_vector;
    stable_sort(result.begin(),result.end());
    return result;
  };
}
_unused static auto vshuffle() {
  return [](const auto &e) {
    vector<decay_t<iterator_value_type_t<decltype(e)>>> result = e->*to_vector;
    shuffle(result.begin(),result.end(),random_number_object());
    return result;
  };
}
_unused static auto vdistinct() {
  return [](const auto &e) {
    vector<decay_t<iterator_value_type_t<decltype(e)>>> result;
    for (auto &p : e) {
      bool found = false;
      for (auto &p2 : result)
        if (p == p2) {
          found = true;
          break;
        }
      if (!found)
        result.push_back(p);
    }
    return result;
  };
}
_unused static auto vtranspose() {
  return [](const auto &e) {
    return vrange(e[0].size())->*expr(vrange(e.size())->*expra(e[arga][arg])->*to_v)->*to_v;
  };
}
template<typename F>
_unused static auto vparallel(F &&f) {
  return vbox([f=move(f)](const auto &e) {
    _use("implement void as return type");
    auto input = e->*to_vector;
    vector<decltype(f(declval<decay_t<iterator_value_type_t<decltype(e)>>>()))> result(input.size());
    shared_queue<bool> done;
    v<function<void()>> fs(input.size());
    for (size_t i = 0; i < input.size(); i++)
      fs[i] = [&,i] {
        result[i] = f(input[i]);
        done.push(bool());
      };
    async_bulk(fs);
    for (size_t i = 0; i < input.size(); i++)
      done.pop();
    return result;
  });
}
template<typename F>
_unused static auto vparallel_ranges(size_t n, F &&f) {
  const size_t chunk_size = 256;
  return vrange(ceil_div(n,chunk_size))->*vparallel([=](size_t r) {
    return f(r * chunk_size,min(n,r * chunk_size + chunk_size));
  });
}
_unused static auto vcount() {
  return [](const auto &e) -> size_t {
    size_t result(0);
    for (auto &p : e)
      _use(p), result++;
    return result;
  };
}
_unused static auto vrun() {
  return [](const auto &e) {
    for (auto &p : e);
  };
}
template<typename F>
_unused static auto vfilter(F &&f) {
  return vbox([=](auto &&e) {
    typedef remove_reference_t<iterator_value_type_t<decltype(e)>> value_type;
    return make_enumerator([f, first = true, v = captured_enumerator<decltype(e)>(forward<decltype(e)>(e))]() mutable -> value_type* {
      if (!first)
        ++v.iterator;
      else
        first = false;
      while (v.iterator != v.enumerator.end())
        if (f(*v.iterator))
          return &*v.iterator;
        else
          ++v.iterator;
      return nullptr;
    });
  });
}
template<typename T>
_unused static auto vcontains(T &&t) {
  return vbox([=](auto &&e) mutable -> bool {
    auto it = e.begin();
    while (it != e.end())
      if (*it == t)
        return true;
      else
        ++it;
    return false;
  });
}
template<size_t N>
_unused static auto to_array() {
  return vbox([](auto &&e) -> array<decay_t<iterator_value_type_t<decltype(e)>>,N> {
    array<decay_t<iterator_value_type_t<decltype(e)>>,N> result;
    size_t i = 0;
    for (auto &p : e) {
      _assert(i < N);
      result[i++] = p;
    }
    _assert(i == N);
    return result;
  });
}
template<typename T>
_unused static auto repeat(T &&t, size_t n) {
  return make_enumerator([t,n]() mutable -> add_const_t<decay_t<T>>* {
    if (n--)
      return &t;
    return nullptr;
  });
}
template<typename F>
_unused static auto repeatf(F &&f, size_t n) {
  return vrange(n)->*expr_unused(f());
}
template<typename F>
_unused static auto repeatfv(F &&f, size_t n) {
  return vrange(n)->*expr_unused(f())->*to_v;
}
template<typename T>
_unused static auto vval(T &&t) {
  return repeat<T>(forward<T>(t),1);
}
template<typename F>
_unused static auto vbest(F &&f) {
  return vreduce([=](auto &&left, auto &&right) {return f(left,right) ? right : left;});
}
_unused static auto vempty() {
  return expr(arg.begin() == arg.end());
}
// arg1 is automatically generated as the index
static auto vpair = expr2(make_pair(arg0,arg1));
template<typename E>
_unused static auto vstarts_with(E &&e) {
  return vbox([=](auto &&e2) {
    auto it = e.begin();
    auto it2 = e2.begin();
    while (it != e.end() && it2 != e2.end() && *it == *it2) it++, it2++;
    return it == e.end();
  });
}
_unused static auto vdrop(size_t n) {
  return vbox([=](auto &&e) mutable {
    typedef remove_reference_t<iterator_value_type_t<typename captured_enumerator<remove_rvalue_ref_t<decltype(e)>>::enumerator_t>> value_type;
    return make_enumerator([n,ce=captured_enumerator<remove_rvalue_ref_t<decltype(e)>>(e),first=true]() mutable -> value_type* {
      if (first) {
        first = false;
        while (n-- && ce.iterator != ce.enumerator.end())
          ++ce.iterator;
        _check(n == undefined_v,"iterator reached `it.end()` too early");
      }
      else
        ++ce.iterator;
      if (ce.iterator == ce.enumerator.end())
        return first = true,nullptr;
      return &*ce.iterator;
    });
  });
}
_unused static auto vtake(size_t n) {
  return vbox([=](auto &&e) mutable {
    typedef remove_reference_t<iterator_value_type_t<typename captured_enumerator<remove_rvalue_ref_t<decltype(e)>>::enumerator_t>> value_type;
    return make_enumerator([n,ce=captured_enumerator<remove_rvalue_ref_t<decltype(e)>>(e),first=true]() mutable -> value_type* {
      if (ce.iterator == ce.enumerator.end() || !n--)
        return nullptr;
      if (first)
        first = false;
      else
        ++ce.iterator;
      if (ce.iterator == ce.enumerator.end())
        return nullptr;
      return &*ce.iterator;
    });
  });
}
_unused static auto vavgminmax() {
    return named_expr(
      x,
      x->*vcount == 0
      ?
        make_tuple(decay_t<iterator_value_type_t<decltype(x)>>(),decay_t<iterator_value_type_t<decltype(x)>>(),decay_t<iterator_value_type_t<decltype(x)>>())
      :
        x->*
        expr(make_tuple(arg,arg,arg,1))->*
        vreduce(expr2(make_tuple(
          get<0>(arg0) + get<0>(arg1),
          min(get<1>(arg0),get<1>(arg1)),
          max(get<2>(arg0),get<2>(arg1)),
          get<3>(arg0) + get<3>(arg1)
        )))->*
        vbox(expr(make_tuple(
          get<3>(arg) == 0 ? 0 : get<0>(arg) / get<3>(arg),
          get<1>(arg),
          get<2>(arg)
        )))
    );
}
_unused static auto vreverse() {
  return [](auto &&e) {
    auto result = e->*to_vector;
    reverse(result.begin(),result.end());
    return result;
  };
}
#define _(op) \
  template<typename T> \
  v<T> operator op (const v<T> &lhs, const v<T> &rhs) { \
    if (lhs.empty()) \
      return rhs; \
    if (rhs.empty()) \
      return lhs; \
    _unwind(lhs.size()); \
    _unwind(rhs.size()); \
    _assertr(lhs.size() == rhs.size()); \
    v<T> result(lhs.size()); \
    for (size_t i = 0; i < lhs.size(); i++) \
      result[i] = lhs[i] op rhs[i];\
    return result; \
  } \
  template<typename T> \
  v<T> operator op (const v<T> &lhs, const T &rhs) { \
    v<T> result(lhs.size()); \
    for (size_t i = 0; i < lhs.size(); i++) \
      result[i] = lhs[i] op rhs;\
    return result; \
  } \
  template<typename T> \
  v<T> operator op (const T &lhs, const v<T> &rhs) { \
    v<T> result(rhs.size()); \
    for (size_t i = 0; i < rhs.size(); i++) \
      result[i] = lhs op rhs[i];\
    return result; \
  }
_(+)
_(-)
_(*)
_(/)
#undef _
#define _(op) \
  template<typename T> \
  v<T> operator op (const v<T> &lhs) { \
    return lhs->*expr(op arg); \
  }
_(+)
_(-)
#undef _
template<typename T>
_unused vector<T> single(const T &t) {
  return vector<T> {t};
}
_unused static auto vprint() {
  return [](const auto &e) {print(e);};
}
_unused static auto vprintln() {
  return [](const auto &e) {println(e);};
}

template<typename ...Types, typename F, typename ...Args>
void call_by_types(const F&, Args&&...) {}
template<typename ...Args, typename F>
void call_by_types(const F &f, Args &&...args) {f(args...);}

struct prefix_cut {
  bool existed;
  string postfix;

  operator bool() {return existed;}
};

bool starts_with(const string &source, const string &prefix);
bool ends_with(const string &source, const string &suffix);
prefix_cut cut_prefix(const string &source, const string &prefix);

template<typename T>
T parse(const string &input) {
  T result;
  _assertr(stringstream(input) >> result);
  return result;
}
template<typename T>
string parse(const T &input) {
  stringstream ss;
  ss << input;
  return ss.str();
}

template<typename T>
static size_t get_type_id() {
  static bool local;
  return (size_t)&local;
}

template<typename Function, typename Tuple, size_t ... I>
auto call(Function f, Tuple t, index_sequence<I ...>) {
  return f(get<I>(t) ...);
}

template<typename Function, typename Tuple>
auto call(Function f, Tuple t) {
  static constexpr auto size = tuple_size<Tuple>::value;
  return call(f, t, make_index_sequence<size>{});
}

template<typename ...Ts>
_unused void do_nothing(Ts...) {}

#define lgamma(...) uncmath_lgamma(__VA_ARGS__)
#define tgamma(...) uncmath_tgamma(__VA_ARGS__)

// those std::s are needed.
_unused static double lgamma(double t) {
#if _debug
  _debug_unwind(t);
  errno = 0;
  auto res = (std::lgamma)(t);
  _check(errno == 0,strerror(errno));
  return res;
#else
  return (std::lgamma)(t);
#endif
}
_unused static double tgamma(double t) {
#if _debug
  _debug_unwind(t);
  errno = 0;
  auto res = (std::tgamma)(t);
  _check(errno == 0,strerror(errno));
  return res;
#else
  return (std::tgamma)(t);
#endif
}
void explain_error(const string &what_happened, const exception &exception, bool is_critical);
void setup_signals();
template<typename F>
static bool exception_handler(F &&f, bool rethrow) {
  try {
    f();
    return false;
  }
  catch (const exception &ex) {
    explain_error("unhandled exception",ex,true);
    if (rethrow)
      throw;
    return true;
  }
}

template<typename container_t, typename at_argument_t>
auto safe_at(container_t &&container, at_argument_t &&at_argument) -> decltype(&*container.find(at_argument)) {
  auto it = container.find(at_argument);
  if (it == container.end())
    return nullptr;
  return &*it;
}
template<typename F, EnableIf<can_call_v<F,size_t,size_t>>...>
void progress(F &&f, size_t count, size_t count_bulk = 1, size_t tick_rate = 1000 * 1000 * 10, size_t tick_rate_error_rate = 2, size_t tick_rate_acceptable_error_rate = 1) {
  auto timer = -nanoseconds();
  size_t ticks = 0,mini_ticks = 0;
  for (size_t i = 0; i < count; ) {
    size_t start = i, end = i + min(count_bulk,count - i);
    f(start,end);
    i += count_bulk;
    mini_ticks++;
    if ((timer + nanoseconds()).get() / tick_rate - ticks) {
      ticks = (timer + nanoseconds()).get() / tick_rate;
      if (mini_ticks <= tick_rate_error_rate - tick_rate_acceptable_error_rate && count_bulk != 1)
        count_bulk--;
      mini_ticks = 0;
      auto now = timer + nanoseconds();
      println("time spent estimation: ",now,"/",nanoseconds((double)now.get() / i * count)," (",i,"/",count,",",nanoseconds((double)now.get() / i),",",(size_t)((1000.0 * 1000.0 * 1000.0) / ((double)now.get() / i)),"/s)");
      print(tput("cuu 1"),tput("el"));
    }
    if (mini_ticks >= tick_rate_error_rate + tick_rate_acceptable_error_rate)
      count_bulk += mini_ticks - tick_rate_error_rate;
  }
  timer += nanoseconds();
  print(tput("el"));
  println("time spent: ",timer," (",count,",",nanoseconds((double)timer.get() / count),",",(size_t)((1000.0 * 1000.0 * 1000.0) / ((double)timer.get() / count)),"/s)");
}
template<typename F, EnableIf<can_call_v<F,size_t>>...>
void progress(F &&f, size_t count, size_t count_bulk = 1, size_t tick_rate = 1000 * 1000 * 10, size_t tick_rate_error_rate = 2, size_t tick_rate_acceptable_error_rate = 1) {
  return progress([&](size_t start, size_t end) {
    for (size_t j = start; j < end; j++)
      f(j);
  },count,count_bulk,tick_rate,tick_rate_error_rate,tick_rate_acceptable_error_rate);
}
template<typename T>
vector<T> create_array(size_t arg) {
  return vector<T>(arg);
}
template<typename T, typename ...SIZE_TS>
auto create_array(size_t arg, SIZE_TS ...args) {
  return vector<decltype(create_array<T>(args...))>(arg,create_array<T>(args...));
}
template<typename V>
auto pick_random(const V &v) -> decltype(v[random_number_range(0,v.size() - 1)]) {
  return v[random_number_range(0,v.size() - 1)];
}

struct region_impl {
  template<typename... Args>
  region_impl(Args &&...args) {
    println("#","region ",forward<Args>(args)...);
  }
  ~region_impl() {
    println("#","endregion (",nanoseconds() - timer,")");
  }

  nanoseconds timer;
};
#define region(args...) region_impl region_for_scope = region_impl(args);
struct test_suite_ {
  test_suite_(const string &suite, const v<function<void()>>&);
};
template<const char *suite_name_>
struct test_suite__ {
  test_suite__(const std::initializer_list<function<void()>> &x) {
    string suite_name = suite_name_;
    for (size_t i = suite_name.size(); i--; ) {
      if (suite_name[i] == '/') {
        suite_name = suite_name.substr(i + 1);
        break;
      }
    }
    for (size_t i = suite_name.size(); i--; ) {
      if (suite_name[i] == '.') {
        suite_name = suite_name.substr(0,i);
        break;
      }
    }
    test_suite_(suite_name,x);
  }
};
#define test_suite static char file_name_for_test_suite[] = __FILE__; static test_suite__<file_name_for_test_suite> file_test_suite = 
void verify_program_correctness(const string &test_case_name_filter);
void clear_verification_program_correctness();

namespace terminal {

string bold();
string red();
string green();
string yellow();
string blue();
string magenta();
string cyan();
string white();
string reset();
string clear();

}

struct program_option {
  optional<char> shortcut;
  optional<string> name;
  bool required, takes_argument, takes_single_argument;
  v<string> arguments;
  string description;
};

void parse_program_arguments(const v<string>&, v<program_option>&);

//#stacktrace-unblock -- NOTE: DO NOT REMOVE, THIS IS TO HIDE IRRELEVANT STACKTRACES
