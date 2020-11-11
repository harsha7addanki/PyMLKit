#ifndef NPY_CPU_DISPATCH_H_
#define NPY_CPU_DISPATCH_H_
/**
 * This file is part of the NumPy CPU dispatcher. Please have a look at doc/reference/simd-optimizations.html
 * To get a better understanding of the mechanism behind it.
 */
#include "npy_cpu_features.h" // NPY_CPU_HAVE
#include "numpy/utils.h" // NPY_EXPAND, NPY_CAT
/**
 * Bringing the main configration header '_cpu_dispatch.h'.
 *
 * This header is generated by the distutils module 'ccompiler_opt',
 * and contains all the #definitions and headers of instruction-sets,
 * that had been configured through command arguments '--cpu-baseline' and '--cpu-dispatch'.
 *
 * It also contains extra C #definitions and macros that are used for implementing
 * NumPy module's attributes `__cpu_baseline__` and `__cpu_dispaٍtch__`.
 */
/**
 * Note: Always guard the generated headers within 'NPY_DISABLE_OPTIMIZATION',
 * due the nature of command argument '--disable-optimization',
 * which is explicitly disabling the module ccompiler_opt.
 */
#ifndef NPY_DISABLE_OPTIMIZATION
    #if defined(__powerpc64__) && !defined(__cplusplus) && defined(bool)
        /**
         * "altivec.h" header contains the definitions(bool, vector, pixel),
         * usually in c++ we undefine them after including the header.
         * It's better anyway to take them off and use built-in types(__vector, __pixel, __bool) instead,
         * since c99 supports bool variables which may lead to ambiguous errors.
        */
        // backup 'bool' before including '_cpu_dispatch.h', since it may not defined as a compiler token.
        #define NPY__DISPATCH_DEFBOOL
        typedef bool npy__dispatch_bkbool;
    #endif
    #include "_cpu_dispatch.h"
    #ifdef NPY_HAVE_VSX
        #undef bool
        #undef vector
        #undef pixel
        #ifdef NPY__DISPATCH_DEFBOOL
            #define bool npy__dispatch_bkbool
        #endif
    #endif
#endif // !NPY_DISABLE_OPTIMIZATION
/**
 * Macro NPY_CPU_DISPATCH_CURFX(NAME)
 *
 * Returns @NAME suffixed with "_" + "the current target" during compiling
 * the wrapped sources that generated from the dispatch-able sources according
 * to the provided configuration statements.
 *
 * It also returns @NAME as-is without any suffix when it comes to the baseline or
 * in case if the optimization is disabled.
 *
 * The idea behind this Macro is to allow exporting certain symbols and to
 * avoid linking duplications due to the nature of the dispatch-able sources.
 *
 * Example:
 *    @targets baseline avx avx512_skx vsx3 asimdhp // configration statments
 *
 *    void NPY_CPU_DISPATCH_CURFX(dispatch_me)(const int *src, int *dst)
 *    {
 *       // the kernel
 *    }
 *
 * By assuming the required optimizations are enabled via '--cpu-dspatch' and
 * the compiler supported them too, then the generated symbols will be named as follows:
 *
 * - x86:
 *      dispatch_me(const int*, int*) // baseline
 *      dispatch_me_AVX(const int*, int*)
 *      dispatch_me_AVX512_SKX(const int*, int*)
 *
 * - ppc64:
 *      dispatch_me(const int*, int*)
 *      dispatch_me_VSX3(const int*, int*)
 *
 * - ARM:
 *      dispatch_me(const int*, int*)
 *      dispatch_me_ASIMHP(const int*, int*)
 *
 * - unsupported arch or when optimization is disabled:
 *      dispatch_me(const int*, int*)
 *
 * For forward declarations, see 'NPY_CPU_DISPATCH_DECLARE'.
 */
#ifdef NPY__CPU_TARGET_CURRENT
    // 'NPY__CPU_TARGET_CURRENT': only defined by the dispatch-able sources
    #define NPY_CPU_DISPATCH_CURFX(NAME) NPY_CAT(NPY_CAT(NAME, _), NPY__CPU_TARGET_CURRENT)
#else
    #define NPY_CPU_DISPATCH_CURFX(NAME) NPY_EXPAND(NAME)
#endif
/**
 * Defining the default behavior for the configurable macros of dispatch-able sources,
 * 'NPY__CPU_DISPATCH_CALL(...)' and 'NPY__CPU_DISPATCH_BASELINE_CALL(...)'
 *
 * These macros are defined inside the generated config files that been derived from
 * the configuration statements of the dispatch-able sources.
 *
 * The generated config file takes the same name of the dispatch-able source with replacing
 * the extension to '.h' instead of '.c', and it should be treated as a header template.
 *
 * For more clarification, please have a look at doc/reference/simd-optimizations.html.
 */
#ifndef NPY_DISABLE_OPTIMIZATION
    #define NPY__CPU_DISPATCH_BASELINE_CALL(CB, ...) \
        &&"Expected config header of the dispatch-able source";
    #define NPY__CPU_DISPATCH_CALL(CHK, CB, ...) \
        &&"Expected config header of the dispatch-able source";
#else
    /**
     * We assume by default that all configuration statements contains 'baseline' option however,
     * if the dispatch-able source doesn't require it, then the dispatch-able source and following macros
     * need to be guard it with '#ifndef NPY_DISABLE_OPTIMIZATION'
     */
    #define NPY__CPU_DISPATCH_BASELINE_CALL(CB, ...) \
        NPY_EXPAND(CB(__VA_ARGS__))
    #define NPY__CPU_DISPATCH_CALL(CHK, CB, ...)
#endif // !NPY_DISABLE_OPTIMIZATION
/**
 * Macro NPY_CPU_DISPATCH_DECLARE(LEFT, ...) is used to provide forward
 * declarations for the exported variables and functions that defined inside
 * the dispatch-able sources.
 *
 * The first argument should ends with the exported function or variable name,
 * while the Macro pasting the extra arguments.
 *
 * Examples:
 *    #ifndef NPY_DISABLE_OPTIMIZATION
 *       #include "dispatchable_source_name.dispatch.h"
 *    #endif
 *
 *    NPY_CPU_DISPATCH_DECLARE(void dispatch_me, (const int*, int*))
 *    NPY_CPU_DISPATCH_DECLARE(extern cb_type callback_tab, [TAB_SIZE])
 *
 * By assuming the provided config header derived from a dispatch-able source,
 * that configured with "@targets baseline sse41 vsx3 asimdhp",
 * they supported by the compiler and enabled via '--cpu-dspatch',
 * then the prototype declrations at the above example will equivalent to the follows:
 *
 * - x86:
 *      void dispatch_me(const int*, int*); // baseline
 *      void dispatch_me_SSE41(const int*, int*);
 *
 *      extern cb_type callback_tab[TAB_SIZE];
 *      extern cb_type callback_tab_SSE41[TAB_SIZE];
 *
 * - ppc64:
 *      void dispatch_me(const int*, int*);
 *      void dispatch_me_VSX3(const int*, int*);
 *
 *      extern cb_type callback_tab[TAB_SIZE];
 *      extern cb_type callback_tab_VSX3[TAB_SIZE];
 *
 * - ARM:
 *     void dispatch_me(const int*, int*);
 *     void dispatch_me_ASIMDHP(const int*, int*);
 *
 *     extern cb_type callback_tab[TAB_SIZE];
 *     extern cb_type callback_tab_ASIMDHP[TAB_SIZE];
 *
 * - unsupported arch or when optimization is disabled:
 *     void dispatch_me(const int*, int*);
 *     extern cb_type callback_tab[TAB_SIZE];
 *
 * For runtime dispatching, see 'NPY_CPU_DISPATCH_CALL'
 */
#define NPY_CPU_DISPATCH_DECLARE(...) \
    NPY__CPU_DISPATCH_CALL(NPY_CPU_DISPATCH_DECLARE_CHK_, NPY_CPU_DISPATCH_DECLARE_CB_, __VA_ARGS__) \
    NPY__CPU_DISPATCH_BASELINE_CALL(NPY_CPU_DISPATCH_DECLARE_BASE_CB_, __VA_ARGS__)
// Preprocessor callbacks
#define NPY_CPU_DISPATCH_DECLARE_CB_(DUMMY, TARGET_NAME, LEFT, ...) \
    NPY_CAT(NPY_CAT(LEFT, _), TARGET_NAME) __VA_ARGS__;
#define NPY_CPU_DISPATCH_DECLARE_BASE_CB_(LEFT, ...) \
    LEFT __VA_ARGS__;
// Dummy CPU runtime checking
#define NPY_CPU_DISPATCH_DECLARE_CHK_(FEATURE)
/**
 * Macro NPY_CPU_DISPATCH_DECLARE_XB(LEFT, ...)
 *
 * Same as `NPY_CPU_DISPATCH_DECLARE` but exclude the baseline declaration even
 * if it was provided within the configration statments.
 */
#define NPY_CPU_DISPATCH_DECLARE_XB(...) \
    NPY__CPU_DISPATCH_CALL(NPY_CPU_DISPATCH_DECLARE_CHK_, NPY_CPU_DISPATCH_DECLARE_CB_, __VA_ARGS__)
/**
 * Macro NPY_CPU_DISPATCH_CALL(LEFT, ...) is used for runtime dispatching
 * of the exported functions and variables within the dispatch-able sources
 * according to the highested interesed CPU features that supported by the
 * running machine depending on the required optimizations.
 *
 * The first argument should ends with the exported function or variable name,
 * while the Macro pasting the extra arguments.
 *
 * Example:
 *  Assume we have a dispatch-able source exporting the following function:
 *
 *    @targets baseline avx2 avx512_skx // configration statments
 *
 *    void NPY_CPU_DISPATCH_CURFX(dispatch_me)(const int *src, int *dst)
 *    {
 *       // the kernel
 *    }
 *
 *  In order to call or to assign the pointer of it from outside the dispatch-able source,
 *  you have to use this Macro as follows:
 *
 *    // bring the generated config header of the dispatch-able source
 *    #ifndef NPY_DISABLE_OPTIMIZATION
 *        #include "dispatchable_source_name.dispatch.h"
 *    #endif
 *    // forward declaration
 *    NPY_CPU_DISPATCH_DECLARE(dispatch_me, (const int *src, int *dst))
 *
 *    typedef void(*func_type)(const int*, int*);
 *    func_type the_callee(const int *src, int *dst, func_type *cb)
 *    {
 *        // direct call
 *        NPY_CPU_DISPATCH_CALL(dispatch_me, (src, dst));
 *        // assign the pointer
 *        *cb = NPY_CPU_DISPATCH_CALL(dispatch_me);
 *        // or
 *        NPY_CPU_DISPATCH_CALL(*cb = dispatch_me);
 *        // return the pointer
 *        return NPY_CPU_DISPATCH_CALL(dispatch_me);
 *    }
 */
#define NPY_CPU_DISPATCH_CALL(...) \
    NPY__CPU_DISPATCH_CALL(NPY_CPU_HAVE, NPY_CPU_DISPATCH_CALL_CB_, __VA_ARGS__) \
    NPY__CPU_DISPATCH_BASELINE_CALL(NPY_CPU_DISPATCH_CALL_BASE_CB_, __VA_ARGS__)
// Preprocessor callbacks
#define NPY_CPU_DISPATCH_CALL_CB_(TESTED_FEATURES, TARGET_NAME, LEFT, ...) \
    (TESTED_FEATURES) ? (NPY_CAT(NPY_CAT(LEFT, _), TARGET_NAME) __VA_ARGS__) :
#define NPY_CPU_DISPATCH_CALL_BASE_CB_(LEFT, ...) \
    (LEFT __VA_ARGS__)
/**
 * Macro NPY_CPU_DISPATCH_CALL_XB(LEFT, ...)
 *
 * Same as `NPY_CPU_DISPATCH_DECLARE` but exclude the baseline declaration even
 * if it was provided within the configration statements.
 * Returns void.
 */
#define NPY_CPU_DISPATCH_CALL_XB_CB_(TESTED_FEATURES, TARGET_NAME, LEFT, ...) \
    (TESTED_FEATURES) ? (void) (NPY_CAT(NPY_CAT(LEFT, _), TARGET_NAME) __VA_ARGS__) :
#define NPY_CPU_DISPATCH_CALL_XB(...) \
    NPY__CPU_DISPATCH_CALL(NPY_CPU_HAVE, NPY_CPU_DISPATCH_CALL_XB_CB_, __VA_ARGS__) \
    ((void) 0 /* discarded expression value */)
/**
 * Macro NPY_CPU_DISPATCH_CALL_ALL(LEFT, ...)
 *
 * Same as `NPY_CPU_DISPATCH_CALL` but dispatching all the required optimizations for
 * the exported functions and variables instead of highest interested one.
 * Returns void.
 */
#define NPY_CPU_DISPATCH_CALL_ALL(...) \
    (NPY__CPU_DISPATCH_CALL(NPY_CPU_HAVE, NPY_CPU_DISPATCH_CALL_ALL_CB_, __VA_ARGS__) \
    NPY__CPU_DISPATCH_BASELINE_CALL(NPY_CPU_DISPATCH_CALL_ALL_BASE_CB_, __VA_ARGS__))
// Preprocessor callbacks
#define NPY_CPU_DISPATCH_CALL_ALL_CB_(TESTED_FEATURES, TARGET_NAME, LEFT, ...) \
    ((TESTED_FEATURES) ? (NPY_CAT(NPY_CAT(LEFT, _), TARGET_NAME) __VA_ARGS__) : (void) 0),
#define NPY_CPU_DISPATCH_CALL_ALL_BASE_CB_(LEFT, ...) \
    ( LEFT __VA_ARGS__ )

#endif // NPY_CPU_DISPATCH_H_
