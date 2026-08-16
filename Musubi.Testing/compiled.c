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
typedef struct {Lambda *f;Lambda *n;Lambda *x;} eb2a2202cc9504dec9e31b178d9aebd94;Lambda *lb2a2202cc9504dec9e31b178d9aebd94(void *raw_env, Lambda *x) {eb2a2202cc9504dec9e31b178d9aebd94 *env = raw_env;env->x = x;return invoke(env->f, invoke(invoke(env->n, env->f), env->x));}typedef struct {Lambda *n;Lambda *f;} ed17e7cf3a01b4c6fbfbc1486d85eacf9;Lambda *ld17e7cf3a01b4c6fbfbc1486d85eacf9(void *raw_env, Lambda *f) {ed17e7cf3a01b4c6fbfbc1486d85eacf9 *env = raw_env;env->f = f;Lambda *_lb2a2202cc9504dec9e31b178d9aebd94 = malloc(sizeof(Lambda));_lb2a2202cc9504dec9e31b178d9aebd94->fn = &lb2a2202cc9504dec9e31b178d9aebd94;eb2a2202cc9504dec9e31b178d9aebd94 *_eb2a2202cc9504dec9e31b178d9aebd94 = malloc(sizeof(eb2a2202cc9504dec9e31b178d9aebd94));_eb2a2202cc9504dec9e31b178d9aebd94->f = env->f;_eb2a2202cc9504dec9e31b178d9aebd94->n = env->n;_lb2a2202cc9504dec9e31b178d9aebd94->env = _eb2a2202cc9504dec9e31b178d9aebd94;return _lb2a2202cc9504dec9e31b178d9aebd94;}typedef struct {Lambda *n;} e05ac3ea40a924e73bc94ad804dea6be6;Lambda *l05ac3ea40a924e73bc94ad804dea6be6(void *raw_env, Lambda *n) {e05ac3ea40a924e73bc94ad804dea6be6 *env = raw_env;env->n = n;Lambda *_ld17e7cf3a01b4c6fbfbc1486d85eacf9 = malloc(sizeof(Lambda));_ld17e7cf3a01b4c6fbfbc1486d85eacf9->fn = &ld17e7cf3a01b4c6fbfbc1486d85eacf9;ed17e7cf3a01b4c6fbfbc1486d85eacf9 *_ed17e7cf3a01b4c6fbfbc1486d85eacf9 = malloc(sizeof(ed17e7cf3a01b4c6fbfbc1486d85eacf9));_ed17e7cf3a01b4c6fbfbc1486d85eacf9->n = env->n;_ld17e7cf3a01b4c6fbfbc1486d85eacf9->env = _ed17e7cf3a01b4c6fbfbc1486d85eacf9;return _ld17e7cf3a01b4c6fbfbc1486d85eacf9;}Lambda *dd849b10434fc41a3b1626aa73846ce40() {Lambda *_l05ac3ea40a924e73bc94ad804dea6be6 = malloc(sizeof(Lambda));_l05ac3ea40a924e73bc94ad804dea6be6->fn = &l05ac3ea40a924e73bc94ad804dea6be6;e05ac3ea40a924e73bc94ad804dea6be6 *_e05ac3ea40a924e73bc94ad804dea6be6 = malloc(sizeof(e05ac3ea40a924e73bc94ad804dea6be6));_l05ac3ea40a924e73bc94ad804dea6be6->env = _e05ac3ea40a924e73bc94ad804dea6be6;return _l05ac3ea40a924e73bc94ad804dea6be6;}typedef struct {Lambda *n;Lambda *n;} e8ccdab721e904548bb0c54e73b7d1227;Lambda *l8ccdab721e904548bb0c54e73b7d1227(void *raw_env, Lambda *n) {e8ccdab721e904548bb0c54e73b7d1227 *env = raw_env;env->n = n;return invoke(invoke(env->n, dd849b10434fc41a3b1626aa73846ce40()), env->n);}typedef struct {Lambda *n;} e2911e237e5e245ae967b6b7b8fe4e420;Lambda *l2911e237e5e245ae967b6b7b8fe4e420(void *raw_env, Lambda *n) {e2911e237e5e245ae967b6b7b8fe4e420 *env = raw_env;env->n = n;Lambda *_l8ccdab721e904548bb0c54e73b7d1227 = malloc(sizeof(Lambda));_l8ccdab721e904548bb0c54e73b7d1227->fn = &l8ccdab721e904548bb0c54e73b7d1227;e8ccdab721e904548bb0c54e73b7d1227 *_e8ccdab721e904548bb0c54e73b7d1227 = malloc(sizeof(e8ccdab721e904548bb0c54e73b7d1227));_e8ccdab721e904548bb0c54e73b7d1227->n = env->n;_l8ccdab721e904548bb0c54e73b7d1227->env = _e8ccdab721e904548bb0c54e73b7d1227;return _l8ccdab721e904548bb0c54e73b7d1227;}Lambda *dd02a1ab2b8e24d43a5ee8c4caacb35b2() {Lambda *_l2911e237e5e245ae967b6b7b8fe4e420 = malloc(sizeof(Lambda));_l2911e237e5e245ae967b6b7b8fe4e420->fn = &l2911e237e5e245ae967b6b7b8fe4e420;e2911e237e5e245ae967b6b7b8fe4e420 *_e2911e237e5e245ae967b6b7b8fe4e420 = malloc(sizeof(e2911e237e5e245ae967b6b7b8fe4e420));_l2911e237e5e245ae967b6b7b8fe4e420->env = _e2911e237e5e245ae967b6b7b8fe4e420;return _l2911e237e5e245ae967b6b7b8fe4e420;}int main() {printf("%d\n", churchToInt(invoke(invoke(dd02a1ab2b8e24d43a5ee8c4caacb35b2(), churchNumeral(20)), invoke(invoke(dd02a1ab2b8e24d43a5ee8c4caacb35b2(), churchNumeral(4)), churchNumeral(5)))));}