#ifndef GLOBAL_ATTRIBUTES_HPP
#define GLOBAL_ATTRIBUTES_HPP

/* Functions declared with this attribute tend to return the same values for the same arguments, such as math functions.
   That said, this attribute makes it so subsequent calls to a function with the same exact arguments are subsituted with
   the return value from the first call, basically having the function called just once per unique return value. This can only
   be used on functions that do NOT directly modify the state of a program. Functions of this sort can accept pointers or
   references to objects that are non-local, but modifying them must not affect their own return values nor affect the general
   state of the program. */
#ifdef __GNUG__
#  define PURE __attribute__ ((pure))
#endif

/* This attribute is meant to be placed on functions that yield the same exact value when given the same exact
   arguments (mostly math functions). If such a function is called with the argument twice or more, the value of
   a previous call is returned without the function actually being executed. Functions with this attribute cannot
   read objects that affect their return value between invocations. So in general, non-local variables cannot be read.
   Further, it is not generally allowed or recommended for any arguments to be pointers to references. Pointers and references
   to non-volatile constants are allowed, however. */
#ifdef __GNUG__
#  define ATTR_CONST __attribute__ ((const))
#endif

// compiler airs a warning if a function's return value goes unused when this is applied
#ifdef __GNUG__
  #ifndef UNUSED
    #define UNUSED __attribute__ ((warn_unused_result))
  #endif
#endif

// forces the compiler to inline a function, except for when the function is called indirectly
#if defined(__GNUG__) || defined(__clang__)
  #define INLINE __attribute__ ((always_inline)) static __inline__
#elif defined(_MSC_VER)
  #define INLINE __forceinline
#endif

// tells the compiler this function will not see much usage in the program and thus can be passively ignored
#ifdef __GNUG__
#  define COLD __attribute__ ((cold))
#endif

// tells the compiler this function will be used a lot, perhaps every frame, and so it must be optimized like crazy
#ifdef __GNUG__
#  define HOT __attribute__ ((hot))
#endif

/* Functions declared with this attribute are not meant to return. Even void functions have their own "return" method.
   So a function declared with this attribute MUST call exit() or some similar command to basically force the program
   to close down in a non-conventional way. */
#ifdef __GNUG__
#  define NORETURN __attribute__ ((noreturn))
#endif

/* Functions declared with this attribute will automatically be called after main() returns, pretty much like
   a program-wide destructor */
#ifdef __GNUG__
  #define DTOR __attribute__ ((destructor))
#else
  #warning "Attribute destructor not supported by this compiler."
#endif

/* Functions */
#ifdef __GNUG__
  #define PRINTF_FORMAT(stridx, argsidx) __attribute__ ((format (printf, stridx, argsidx)))
#else
  #define PRINTF_FORMAT(stridx, argsidx)
#endif

/**/
#ifdef __GNUG__
# define PRINTF(str, args) __attribute__ ((format (printf, str, args)))
#endif

/* Functions that have this attribute do not expect to return a NULL pointer. */
#ifdef __GNUG__
#  define NONNULL __attribute__ ((nonnull))
#endif

/* A combination of NONNULL and UNUSED; made specifically for memory allocation functions. */
#ifdef __GNUG__
#  define NONNULL_UNUSED __attribute__ ((nonnull,unused))
#endif

/* Defines a function as a "weak" alias of another */
#ifdef __GNUG__
#  define WEAKALIAS(f) __attribute__ ((weak, alias (#f)))
#endif

#endif /* GLOBAL_ATTRIBUTES_HPP */
