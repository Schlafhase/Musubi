// trying to compile this by hand:
// basic:
// \x.x x
// successor of 3:
// (\n.\f.\s.f (n f s)) (\f.\s.f (f (f s)))
// addition:
// \a.\b.a (successor) b

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

// basic:

// lambda definition

Lambda *inner(void *_, Lambda *arg) { return arg->fn(_, arg->env); }

// successor of 3:

typedef struct {
  Lambda *n;
  Lambda *f;
} e1;

Lambda *l1(void *raw_env, Lambda *s) {
  e1 *env = raw_env;
  return invoke(env->f, invoke((invoke(env->n, env->f)), s));
}

typedef struct {
  Lambda *n;
} e2;

Lambda *l2(void *raw_env, Lambda *f) {
  e2 *env = raw_env;
  Lambda *ret = malloc(sizeof(Lambda));
  ret->fn = &l1;
  e1 *inner_env = malloc(sizeof(e1));
  inner_env->n = env->n;
  inner_env->f = f;
  ret->env = inner_env;
  return ret;
}

Lambda *l3(void *raw_env, Lambda *n) {
  Lambda *ret = malloc(sizeof(Lambda));
  ret->fn = &l2;
  e2 *inner_env = malloc(sizeof(e2));
  inner_env->n = n;
  ret->env = inner_env;
  return ret;
}

int main() {
  Lambda *toplevel = malloc(sizeof(Lambda));
  toplevel->fn = l3;
  toplevel->env = NULL;
  Lambda *result = invoke(toplevel, churchNumeral(3));
  printf("%d\n", churchToInt(result));
}
