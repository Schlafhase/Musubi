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


