#include <stdio.h>
#include <stdlib.h>
typedef struct Lambda Lambda;

struct Lambda {
  Lambda *(*fn)(void *raw_env, Lambda *arg);
  void *env;
};

Lambda *invoke(Lambda *f, Lambda *arg) { return f->fn(f->env, arg); };

// church numeral trick

// having int in the environment is not technically legal
// LC but it's necessary for this custom church numeral implementation
typedef struct {
  int n;
} ecn1;

typedef struct {
  int n;
  Lambda *f;
} ecn2;

Lambda *cn_x(void *raw_env, Lambda *x) {
  ecn2 *env = raw_env;
  Lambda *result = x;
  for (int i = 0; i < env->n; i++) {
    result = invoke(env->f, result);
  }
  return result;
}

Lambda *cn_f(void *raw_env, Lambda *f) {
  ecn1 *env = raw_env;
  Lambda *ret = malloc(sizeof(Lambda));
  ret->fn = &cn_x;
  ecn2 *inner_env = malloc(sizeof(ecn2));
  inner_env->n = env->n;
  inner_env->f = f;
  ret->env = inner_env;
  return ret;
}

Lambda *churchNumeral(int n) {
  Lambda *ret = malloc(sizeof(Lambda));
  ret->fn = &cn_f;
  ecn1 *env = malloc(sizeof(ecn1));
  env->n = n;
  ret->env = env;
  return ret;
}

typedef struct {
  int *counter;
} counter_env;

Lambda *increment(void *raw_env, Lambda *x) {
  counter_env *env = raw_env;
  (*env->counter)++;
  return x;
}

int churchToInt(Lambda *n) {
  int counter = 0;
  counter_env *env = malloc(sizeof(counter_env));
  env->counter = &counter;

  Lambda *inc = malloc(sizeof(Lambda));
  inc->fn = &increment;
  inc->env = env;

  Lambda *dummy_x = malloc(sizeof(Lambda));

  invoke(invoke(n, inc), dummy_x);
  return counter;
}

// booleans

typedef struct {
  Lambda *a;
} eb;

Lambda *ltrue(void *raw_env, Lambda *b) {
  eb *env = raw_env;
  return env->a;
}

Lambda *lfalse(void *raw_env, Lambda *b) { return b; }

Lambda *ltrueOuter(void *raw_env, Lambda *a) {
  Lambda *ret = malloc(sizeof(Lambda));
  ret->fn = &ltrue;
  eb *env = malloc(sizeof(eb));
  env->a = a;
  ret->env = env;
  return ret;
}

Lambda *lfalseOuter(void *raw_env, Lambda *a) {
  Lambda *ret = malloc(sizeof(Lambda));
  ret->fn = &lfalse;
  eb *env = malloc(sizeof(eb));
  env->a = a;
  ret->env = env;
  return ret;
}

Lambda *churchBoolean(int value) {
  Lambda *ret = malloc(sizeof(Lambda));
  if (value == 0) {
    ret->fn = &lfalseOuter;
  } else {
    ret->fn = &ltrueOuter;
  }
  ret->env = NULL;
  return ret;
}
typedef struct {Lambda *b;Lambda *a;Lambda *c;Lambda *f;Lambda *n;Lambda *x;} e6efac022b499450ca2afa14954c58ce7;Lambda *l6efac022b499450ca2afa14954c58ce7(void *raw_env, Lambda *x) {e6efac022b499450ca2afa14954c58ce7 *env = raw_env;env->x = x;return invoke(env->a, invoke(env->b, env->x));}typedef struct {Lambda *a;Lambda *c;Lambda *f;Lambda *n;Lambda *b;} e4cb42d13ce52421cbec0686f528c976a;Lambda *l4cb42d13ce52421cbec0686f528c976a(void *raw_env, Lambda *b) {e4cb42d13ce52421cbec0686f528c976a *env = raw_env;env->b = b;Lambda *_l6efac022b499450ca2afa14954c58ce7 = malloc(sizeof(Lambda));_l6efac022b499450ca2afa14954c58ce7->fn = &l6efac022b499450ca2afa14954c58ce7;e6efac022b499450ca2afa14954c58ce7 *_e6efac022b499450ca2afa14954c58ce7 = malloc(sizeof(e6efac022b499450ca2afa14954c58ce7));_e6efac022b499450ca2afa14954c58ce7->b = env->b;_e6efac022b499450ca2afa14954c58ce7->a = env->a;_e6efac022b499450ca2afa14954c58ce7->c = env->c;_e6efac022b499450ca2afa14954c58ce7->f = env->f;_e6efac022b499450ca2afa14954c58ce7->n = env->n;_l6efac022b499450ca2afa14954c58ce7->env = _e6efac022b499450ca2afa14954c58ce7;return invoke(invoke(env->c, env->b), _l6efac022b499450ca2afa14954c58ce7);}typedef struct {Lambda *c;Lambda *f;Lambda *n;Lambda *a;} e2655345c4f504bdd8601c769abb7757c;Lambda *l2655345c4f504bdd8601c769abb7757c(void *raw_env, Lambda *a) {e2655345c4f504bdd8601c769abb7757c *env = raw_env;env->a = a;Lambda *_l4cb42d13ce52421cbec0686f528c976a = malloc(sizeof(Lambda));_l4cb42d13ce52421cbec0686f528c976a->fn = &l4cb42d13ce52421cbec0686f528c976a;e4cb42d13ce52421cbec0686f528c976a *_e4cb42d13ce52421cbec0686f528c976a = malloc(sizeof(e4cb42d13ce52421cbec0686f528c976a));_e4cb42d13ce52421cbec0686f528c976a->a = env->a;_e4cb42d13ce52421cbec0686f528c976a->c = env->c;_e4cb42d13ce52421cbec0686f528c976a->f = env->f;_e4cb42d13ce52421cbec0686f528c976a->n = env->n;_l4cb42d13ce52421cbec0686f528c976a->env = _e4cb42d13ce52421cbec0686f528c976a;return _l4cb42d13ce52421cbec0686f528c976a;}typedef struct {Lambda *f;Lambda *n;Lambda *c;} e47c84656fa70462d98719fd08b27234f;Lambda *l47c84656fa70462d98719fd08b27234f(void *raw_env, Lambda *c) {e47c84656fa70462d98719fd08b27234f *env = raw_env;env->c = c;Lambda *_l2655345c4f504bdd8601c769abb7757c = malloc(sizeof(Lambda));_l2655345c4f504bdd8601c769abb7757c->fn = &l2655345c4f504bdd8601c769abb7757c;e2655345c4f504bdd8601c769abb7757c *_e2655345c4f504bdd8601c769abb7757c = malloc(sizeof(e2655345c4f504bdd8601c769abb7757c));_e2655345c4f504bdd8601c769abb7757c->c = env->c;_e2655345c4f504bdd8601c769abb7757c->f = env->f;_e2655345c4f504bdd8601c769abb7757c->n = env->n;_l2655345c4f504bdd8601c769abb7757c->env = _e2655345c4f504bdd8601c769abb7757c;return _l2655345c4f504bdd8601c769abb7757c;}typedef struct {Lambda *x;Lambda *f;Lambda *n;Lambda *y;} e3775b8467ec64ad78d0690388b3acc89;Lambda *l3775b8467ec64ad78d0690388b3acc89(void *raw_env, Lambda *y) {e3775b8467ec64ad78d0690388b3acc89 *env = raw_env;env->y = y;return env->x;}typedef struct {Lambda *f;Lambda *n;Lambda *x;} e4a613ca5ea7b496faf3712b128295773;Lambda *l4a613ca5ea7b496faf3712b128295773(void *raw_env, Lambda *x) {e4a613ca5ea7b496faf3712b128295773 *env = raw_env;env->x = x;Lambda *_l3775b8467ec64ad78d0690388b3acc89 = malloc(sizeof(Lambda));_l3775b8467ec64ad78d0690388b3acc89->fn = &l3775b8467ec64ad78d0690388b3acc89;e3775b8467ec64ad78d0690388b3acc89 *_e3775b8467ec64ad78d0690388b3acc89 = malloc(sizeof(e3775b8467ec64ad78d0690388b3acc89));_e3775b8467ec64ad78d0690388b3acc89->x = env->x;_e3775b8467ec64ad78d0690388b3acc89->f = env->f;_e3775b8467ec64ad78d0690388b3acc89->n = env->n;_l3775b8467ec64ad78d0690388b3acc89->env = _e3775b8467ec64ad78d0690388b3acc89;return _l3775b8467ec64ad78d0690388b3acc89;}typedef struct {Lambda *f;Lambda *n;Lambda *x;} efb65f3fa6f7e439ba85a1124c9dfedea;Lambda *lfb65f3fa6f7e439ba85a1124c9dfedea(void *raw_env, Lambda *x) {efb65f3fa6f7e439ba85a1124c9dfedea *env = raw_env;env->x = x;return env->x;}typedef struct {Lambda *n;Lambda *f;} e3622621f1a434e229f847c2aba68a724;Lambda *l3622621f1a434e229f847c2aba68a724(void *raw_env, Lambda *f) {e3622621f1a434e229f847c2aba68a724 *env = raw_env;env->f = f;Lambda *_l47c84656fa70462d98719fd08b27234f = malloc(sizeof(Lambda));_l47c84656fa70462d98719fd08b27234f->fn = &l47c84656fa70462d98719fd08b27234f;e47c84656fa70462d98719fd08b27234f *_e47c84656fa70462d98719fd08b27234f = malloc(sizeof(e47c84656fa70462d98719fd08b27234f));_e47c84656fa70462d98719fd08b27234f->f = env->f;_e47c84656fa70462d98719fd08b27234f->n = env->n;_l47c84656fa70462d98719fd08b27234f->env = _e47c84656fa70462d98719fd08b27234f;Lambda *_l4a613ca5ea7b496faf3712b128295773 = malloc(sizeof(Lambda));_l4a613ca5ea7b496faf3712b128295773->fn = &l4a613ca5ea7b496faf3712b128295773;e4a613ca5ea7b496faf3712b128295773 *_e4a613ca5ea7b496faf3712b128295773 = malloc(sizeof(e4a613ca5ea7b496faf3712b128295773));_e4a613ca5ea7b496faf3712b128295773->f = env->f;_e4a613ca5ea7b496faf3712b128295773->n = env->n;_l4a613ca5ea7b496faf3712b128295773->env = _e4a613ca5ea7b496faf3712b128295773;Lambda *_lfb65f3fa6f7e439ba85a1124c9dfedea = malloc(sizeof(Lambda));_lfb65f3fa6f7e439ba85a1124c9dfedea->fn = &lfb65f3fa6f7e439ba85a1124c9dfedea;efb65f3fa6f7e439ba85a1124c9dfedea *_efb65f3fa6f7e439ba85a1124c9dfedea = malloc(sizeof(efb65f3fa6f7e439ba85a1124c9dfedea));_efb65f3fa6f7e439ba85a1124c9dfedea->f = env->f;_efb65f3fa6f7e439ba85a1124c9dfedea->n = env->n;_lfb65f3fa6f7e439ba85a1124c9dfedea->env = _efb65f3fa6f7e439ba85a1124c9dfedea;return invoke(invoke(invoke(invoke(env->n, _l47c84656fa70462d98719fd08b27234f), _l4a613ca5ea7b496faf3712b128295773), _lfb65f3fa6f7e439ba85a1124c9dfedea), env->f);}typedef struct {Lambda *n;} e4a2cdc9f4aa84fe593ec32a4343f4e63;Lambda *l4a2cdc9f4aa84fe593ec32a4343f4e63(void *raw_env, Lambda *n) {e4a2cdc9f4aa84fe593ec32a4343f4e63 *env = raw_env;env->n = n;Lambda *_l3622621f1a434e229f847c2aba68a724 = malloc(sizeof(Lambda));_l3622621f1a434e229f847c2aba68a724->fn = &l3622621f1a434e229f847c2aba68a724;e3622621f1a434e229f847c2aba68a724 *_e3622621f1a434e229f847c2aba68a724 = malloc(sizeof(e3622621f1a434e229f847c2aba68a724));_e3622621f1a434e229f847c2aba68a724->n = env->n;_l3622621f1a434e229f847c2aba68a724->env = _e3622621f1a434e229f847c2aba68a724;return _l3622621f1a434e229f847c2aba68a724;}int main() {Lambda *_l4a2cdc9f4aa84fe593ec32a4343f4e63 = malloc(sizeof(Lambda));_l4a2cdc9f4aa84fe593ec32a4343f4e63->fn = &l4a2cdc9f4aa84fe593ec32a4343f4e63;e4a2cdc9f4aa84fe593ec32a4343f4e63 *_e4a2cdc9f4aa84fe593ec32a4343f4e63 = malloc(sizeof(e4a2cdc9f4aa84fe593ec32a4343f4e63));_l4a2cdc9f4aa84fe593ec32a4343f4e63->env = _e4a2cdc9f4aa84fe593ec32a4343f4e63;printf("%d\n", churchToInt(invoke(_l4a2cdc9f4aa84fe593ec32a4343f4e63, churchNumeral(5))));}