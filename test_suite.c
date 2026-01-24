#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <time.h>
#include <stdint.h>
#include <string.h>

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define CYN   "\x1B[36m"
#define RESET "\x1B[0m"

#define QUICK_TESTS        20000000
#define QUICK_TOL          1e-14
#define MAX_FAIL_PRINT     5

#define ROBUST_TRIALS      5
#define BUF_SIZE           65536
#define BUF_MASK           (BUF_SIZE - 1)
#define ROBUST_ITERS       20000000
#define MAX_ULP            500

double custom_sin(double x);
double custom_cos(double x);
double custom_tan(double x);
double custom_asin(double x);
double custom_acos(double x);
double custom_atan(double x);
double custom_atan2(double y, double x);
double custom_sinh(double x);
double custom_cosh(double x);
double custom_tanh(double x);
double custom_asinh(double x);
double custom_acosh(double x);
double custom_atanh(double x);
double custom_exp(double x);
double custom_log(double x);
double custom_log1p(double x);
double custom_pow(double base, double exp);
double custom_sqrt(double x);
double custom_fabs(double x);
double custom_ceil(double x);
double custom_floor(double x);
double custom_trunc(double x);
double custom_round(double x);
double custom_fmod(double x, double y);

double rand_range(double lo, double hi) {
    return lo + (rand() / (double)RAND_MAX) * (hi - lo);
}

int approx_eq(double a, double b) {
    if (isnan(a)) return isnan(b);
    if (isinf(a)) return isinf(b) && (signbit(a) == signbit(b));
    if (a == 0.0) return b == 0.0;
    double diff = fabs(a - b);
    if (diff < QUICK_TOL) return 1;
    double mx = fmax(fabs(a), fabs(b));
    return diff / mx < QUICK_TOL;
}

typedef union { double f; uint64_t u; } dbl_bits;

uint64_t to_bits(double d) {
    dbl_bits db = {.f = d};
    return db.u;
}

uint64_t ulp_distance(double a, double b) {
    if (isnan(a) && isnan(b)) return 0;
    if (isnan(a) || isnan(b)) return UINT64_MAX;
    if (isinf(a) && isinf(b)) return (signbit(a) == signbit(b)) ? 0 : UINT64_MAX;
    if (a == 0.0 && b == 0.0) return 0;
    uint64_t ua = to_bits(a), ub = to_bits(b);
    if ((ua >> 63) != (ub >> 63)) return UINT64_MAX;
    return (ua >= ub) ? (ua - ub) : (ub - ua);
}

double walltime() {
    return (double)clock() / CLOCKS_PER_SEC;
}

void fill_buf(double* buf, int n, double lo, double hi) {
    double special[] = {0.0, -0.0, 1.0, -1.0, INFINITY, -INFINITY, NAN,
                        DBL_MIN, -DBL_MIN, DBL_MAX, -DBL_MAX,
                        M_PI, M_PI_2, M_E, 1e-308, 1e300};
    int ns = sizeof(special) / sizeof(special[0]);
    for (int i = 0; i < n; i++) {
        if (i % 100 == 0) {
            buf[i] = special[(i / 100) % ns];
        } else if (i % 3 == 0) {
            double r = (double)rand() / RAND_MAX;
            buf[i] = ((rand() % 2) ? 1.0 : -1.0) * pow(10.0, r * 600.0 - 300.0);
        } else {
            buf[i] = rand_range(lo, hi);
        }
    }
}

void test_unary(const char* name, double (*ref)(double), double (*test)(double),
                double lo, double hi) {
    printf("%-6s ... ", name);
    fflush(stdout);
    
    int fails = 0;
    for (int i = 0; i < QUICK_TESTS; i++) {
        double x = rand_range(lo, hi);
        if (i == 0) x = 0.0;
        double r = ref(x), t = test(x);
        if (!approx_eq(r, t)) {
            fails++;
            if (fails <= MAX_FAIL_PRINT) {
                if (fails == 1) printf("\n");
                printf(RED "  x=%.10g ref=%.10g test=%.10g\n" RESET, x, r, t);
            }
        }
    }
    
    if (fails > 0) {
        printf(RED "FAIL (%d)\n" RESET, fails);
        return;
    }
    
    volatile double sink = 0;
    clock_t t0 = clock();
    for (int i = 0; i < QUICK_TESTS; i++) {
        double x = lo + (i % 1000) * (hi - lo) / 1000.0;
        sink += ref(x);
    }
    double t_ref = (double)(clock() - t0) / CLOCKS_PER_SEC;
    
    sink = 0;
    t0 = clock();
    for (int i = 0; i < QUICK_TESTS; i++) {
        double x = lo + (i % 1000) * (hi - lo) / 1000.0;
        sink += test(x);
    }
    double t_test = (double)(clock() - t0) / CLOCKS_PER_SEC;
    
    printf(GRN "PASS" RESET " | ref=" CYN "%.3fs" RESET " test=" CYN "%.3fs" RESET, t_ref, t_test);
    if (t_test < t_ref) printf(" speed=" GRN "%.2fx\n" RESET, t_ref / t_test);
    else printf(" speed=" YEL "%.2fx\n" RESET, t_test / t_ref);
}

void test_binary(const char* name, double (*ref)(double, double), double (*test)(double, double),
                 double lo, double hi) {
    printf("%-6s ... ", name);
    fflush(stdout);
    
    int fails = 0;
    for (int i = 0; i < 100000; i++) {
        double x = rand_range(lo, hi), y = rand_range(lo, hi);
        if (!approx_eq(ref(x, y), test(x, y))) {
            fails++;
            if (fails <= MAX_FAIL_PRINT)
                printf(RED "\n  x=%.5g y=%.5g\n" RESET, x, y);
        }
    }
    
    if (fails > 0) {
        printf(RED "FAIL\n" RESET);
        return;
    }
    
    volatile double sink = 0;
    clock_t t0 = clock();
    for (int i = 0; i < QUICK_TESTS; i++) {
        double x = lo + (i % 1000) * (hi - lo) / 1000.0;
        double y = lo + ((i + 1) % 1000) * (hi - lo) / 1000.0;
        sink += ref(x, y);
    }
    double t_ref = (double)(clock() - t0) / CLOCKS_PER_SEC;
    
    sink = 0;
    t0 = clock();
    for (int i = 0; i < QUICK_TESTS; i++) {
        double x = lo + (i % 1000) * (hi - lo) / 1000.0;
        double y = lo + ((i + 1) % 1000) * (hi - lo) / 1000.0;
        sink += test(x, y);
    }
    double t_test = (double)(clock() - t0) / CLOCKS_PER_SEC;
    
    printf(GRN "PASS" RESET " | ref=" CYN "%.3fs" RESET " test=" CYN "%.3fs" RESET, t_ref, t_test);
    if (t_test < t_ref) printf(" speed=" GRN "%.2fx\n" RESET, t_ref / t_test);
    else printf(" speed=" YEL "%.2fx\n" RESET, t_test / t_ref);
}

void robust_unary(const char* name, double (*ref)(double), double (*test)(double),
                  double lo, double hi) {
    double* buf = malloc(BUF_SIZE * sizeof(double));
    fill_buf(buf, BUF_SIZE, lo, hi);
    
    int fails = 0;
    uint64_t max_ulp = 0;
    
    for (int i = 0; i < BUF_SIZE; i++) {
        double x = buf[i], r = ref(x), t = test(x);
        uint64_t d = ulp_distance(r, t);
        if (d > max_ulp) max_ulp = d;
        
        if (d > MAX_ULP) {
            if (fails == 0) {
                printf("%-6s | " RED "FAIL" RESET "\n", name);
            }
            
            if (fails < 3) {
                printf("  x=%.5e ref=%.5e test=%.5e ulp=%llu\n", x, r, t, (unsigned long long)d);
            }
            fails++;
        }
    }
    
    if (fails > 0) {
        printf("  %d failures, maxulp=%llu\n", fails, (unsigned long long)max_ulp);
        free(buf);
        return;
    }
    
    double best_ref = 1e9, best_test = 1e9;
    volatile double sink = 0;
    
    for (int trial = 0; trial < ROBUST_TRIALS; trial++) {
        double t0 = walltime();
        for (int i = 0; i < ROBUST_ITERS; i++) sink += ref(buf[i & BUF_MASK]);
        double dt = walltime() - t0;
        if (dt < best_ref) best_ref = dt;
    }
    
    for (int trial = 0; trial < ROBUST_TRIALS; trial++) {
        double t0 = walltime();
        for (int i = 0; i < ROBUST_ITERS; i++) sink += test(buf[i & BUF_MASK]);
        double dt = walltime() - t0;
        if (dt < best_test) best_test = dt;
    }
    
    double ratio = best_test / best_ref;
    const char* col = (ratio <= 1.0) ? GRN : YEL;
    printf("%-6s | ulp=%4llu | ref=%.3fs | test=%s%.3fs" RESET " | ",
           name, (unsigned long long)max_ulp, best_ref, col, best_test);
    if (ratio < 1.0) printf("speed=" GRN "%.2fx\n" RESET, 1.0 / ratio);
    else printf("speed=" YEL "%.2fx\n" RESET, ratio);
    
    free(buf);
}

void robust_binary(const char* name, double (*ref)(double, double), double (*test)(double, double),
                   double lo, double hi) {
    double* buf1 = malloc(BUF_SIZE * sizeof(double));
    double* buf2 = malloc(BUF_SIZE * sizeof(double));
    fill_buf(buf1, BUF_SIZE, lo, hi);
    fill_buf(buf2, BUF_SIZE, lo, hi);
    
    uint64_t max_ulp = 0;
    int fails = 0;
    
    for (int i = 0; i < BUF_SIZE; i++) {
        double x = buf1[i], y = buf2[i];
        if (strcmp(name, "pow") == 0 && x < 0 && (int)y != y) continue;
        
        double r = ref(x, y), t = test(x, y);
        uint64_t d = ulp_distance(r, t);
        
        if (d > max_ulp) max_ulp = d;
        
        if (d > MAX_ULP) {
            if (fails == 0) {
                printf("%-6s | " RED "FAIL" RESET "\n", name);
            }
            
            if (fails < 3) {
                 printf("  x=%.5e y=%.5e ref=%.5e test=%.5e ulp=%llu\n", 
                       x, y, r, t, (unsigned long long)d);
            }
            fails++;
        }
    }
    
    if (fails > 0) {
        printf("  %d failures, maxulp=%llu\n", fails, (unsigned long long)max_ulp);
        free(buf1); free(buf2);
        return;
    }
    
    double best_ref = 1e9, best_test = 1e9;
    volatile double sink = 0;
    
    for (int trial = 0; trial < ROBUST_TRIALS; trial++) {
        double t0 = walltime();
        for (int i = 0; i < ROBUST_ITERS; i++)
            sink += ref(buf1[i & BUF_MASK], buf2[i & BUF_MASK]);
        double dt = walltime() - t0;
        if (dt < best_ref) best_ref = dt;
    }
    
    for (int trial = 0; trial < ROBUST_TRIALS; trial++) {
        double t0 = walltime();
        for (int i = 0; i < ROBUST_ITERS; i++)
            sink += test(buf1[i & BUF_MASK], buf2[i & BUF_MASK]);
        double dt = walltime() - t0;
        if (dt < best_test) best_test = dt;
    }
    
    double ratio = best_test / best_ref;
    const char* col = (ratio <= 1.0) ? GRN : YEL;
    printf("%-6s | ulp=%4llu | ref=%.3fs | test=%s%.3fs" RESET " | ",
           name, (unsigned long long)max_ulp, best_ref, col, best_test);
    if (ratio < 1.0) printf("speed=" GRN "%.2fx\n" RESET, 1.0 / ratio);
    else printf("speed=" YEL "%.2fx\n" RESET, ratio);
    
    free(buf1); free(buf2);
}

int main() {
    srand((unsigned int)time(NULL));
    
    printf("\n" CYN "Phase 1: Quick Validation\n\n" RESET);
    test_unary("sin", sin, custom_sin, -100.0, 100.0);
    test_unary("cos", cos, custom_cos, -100.0, 100.0);
    test_unary("tan", tan, custom_tan, -100.0, 100.0);
    test_unary("atan", atan, custom_atan, -100.0, 100.0);
    test_unary("asin", asin, custom_asin, -1.0, 1.0);
    test_unary("acos", acos, custom_acos, -1.0, 1.0);
    test_unary("sinh", sinh, custom_sinh, -10.0, 10.0);
    test_unary("cosh", cosh, custom_cosh, -10.0, 10.0);
    test_unary("tanh", tanh, custom_tanh, -10.0, 10.0);
    test_unary("exp", exp, custom_exp, -10.0, 10.0);
    test_unary("log", log, custom_log, 0.1, 1000.0);
    test_unary("sqrt", sqrt, custom_sqrt, 0.0, 10000.0);
    test_binary("pow", pow, custom_pow, 0.0, 10.0);
    test_binary("atan2", atan2, custom_atan2, -100.0, 100.0);
    test_binary("fmod", fmod, custom_fmod, -1000.0, 1000.0);
    test_unary("ceil", ceil, custom_ceil, -1000.0, 1000.0);
    test_unary("floor", floor, custom_floor, -1000.0, 1000.0);
    test_unary("trunc", trunc, custom_trunc, -1000.0, 1000.0);
    test_unary("round", round, custom_round, -1000.0, 1000.0);
    
    printf("\n" CYN "Phase 2: Robust Analysis\n\n" RESET);
    robust_unary("sin", sin, custom_sin, -100, 100);
    robust_unary("cos", cos, custom_cos, -100, 100);
    robust_unary("tan", tan, custom_tan, -100, 100);
    robust_unary("atan", atan, custom_atan, -100, 100);
    robust_unary("exp", exp, custom_exp, -100, 100);
    robust_unary("log", log, custom_log, 0, 1000);
    robust_unary("sinh", sinh, custom_sinh, -10, 10);
    robust_unary("cosh", cosh, custom_cosh, -10, 10);
    robust_unary("sqrt", sqrt, custom_sqrt, 0, 1e9);
    robust_binary("pow", pow, custom_pow, 0, 100);
    robust_binary("atan2", atan2, custom_atan2, -100, 100);
    robust_binary("fmod", fmod, custom_fmod, -1000, 1000);
    
    printf("\n" GRN "Complete\n" RESET);
    return 0;
}