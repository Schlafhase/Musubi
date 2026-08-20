#include <stdio.h>
#include <stdlib.h>
typedef struct Lambda Lambda;

struct Lambda {
  Lambda *(*fn)(void *raw_env, Lambda *arg);
  void *env;
};

Lambda *invoke(Lambda *f, Lambda *arg) { return f->fn(f->env, arg); };

// scott numeral

typedef struct {
  int n;
} esn1;
typedef struct {
  int n;
  Lambda *z;
} esn2;

Lambda *scottNumeral(int n);

Lambda *sn_s(void *raw_env, Lambda *s) {
  esn2 *env = raw_env;
  if (env->n == 0) {
    return env->z;
  } else {
    Lambda *pred = scottNumeral(env->n - 1);
    return invoke(s, pred);
  }
}

Lambda *sn_z(void *raw_env, Lambda *z) {
  esn1 *env = raw_env;
  Lambda *ret = malloc(sizeof(Lambda));
  ret->fn = &sn_s;
  esn2 *inner_env = malloc(sizeof(esn2));
  inner_env->n = env->n;
  inner_env->z = z;
  ret->env = inner_env;
  return ret;
}

Lambda *scottNumeral(int n) {
  Lambda *ret = malloc(sizeof(Lambda));
  ret->fn = &sn_z;
  esn1 *env = malloc(sizeof(esn1));
  env->n = n;
  ret->env = env;
  return ret;
}

// dummy zero value
Lambda *makeZeroMarker() {
  Lambda *m = malloc(sizeof(Lambda));
  m->fn = NULL;
  m->env = NULL;
  return m;
}

// lambda that acts as the successor function (takes m (predecessor) and does
// nothing with it)
typedef struct {
  Lambda *m;
} probeSuccessorEnv;
Lambda *probeSuccessorFn(void *raw_env, Lambda *unused) { return NULL; }

Lambda *probeSuccessor(void *raw_env, Lambda *m) {
  Lambda *ret = malloc(sizeof(Lambda));
  ret->fn = &probeSuccessorFn;
  probeSuccessorEnv *senv = malloc(sizeof(probeSuccessorEnv));
  senv->m = m;
  ret->env = senv;
  return ret;
}

Lambda *makeProbeSuccessor() {
  Lambda *ret = malloc(sizeof(Lambda));
  ret->fn = &probeSuccessor;
  ret->env = NULL;
  return ret;
}

int scottToInt(Lambda *n) {
  int count = 0;
  Lambda *current = n;
  while (1) {
    Lambda *zeroMarker = makeZeroMarker();
    Lambda *sProbe = makeProbeSuccessor();
    // zeroMarker if current is zero otherwise sProbe the return value of sProbe
    // (which is NULL)
    Lambda *result = invoke(invoke(current, zeroMarker), sProbe);

    if (result == zeroMarker)
      break;

    probeSuccessorEnv *senv = result->env;
    current = senv->m; // "steal" m, the predecessor
    count++;
  }
  return count;
}

// scott lists

// lambda that acts as the cons function (takes x and xs and does
// nothing with it)
typedef struct {
  Lambda *x;
  Lambda *xs;
} probeConsEnv;
Lambda *probeConsFn(void *raw_env, Lambda *unused) { return NULL; }

typedef struct {
  Lambda *x;
} probeConsInnerEnv;
Lambda *probeConsInner(void *raw_env, Lambda *xs) {
  probeConsInnerEnv *env = malloc(sizeof(probeConsInnerEnv));
  env = raw_env;
  Lambda *ret = malloc(sizeof(Lambda));
  ret->fn = &probeConsFn;
  probeConsEnv *cenv = malloc(sizeof(probeConsEnv));
  cenv->x = env->x;
  cenv->xs = xs;
  ret->env = cenv;
  return ret;
}

Lambda *probeConsOuter(void *raw_env, Lambda *x) {
  Lambda *ret = malloc(sizeof(Lambda));
  ret->fn = &probeConsInner;
  probeConsInnerEnv *cenv = malloc(sizeof(probeConsInnerEnv));
  cenv->x = x;
  ret->env = cenv;
  return ret;
}

Lambda *makeProbeCons() {
  Lambda *ret = malloc(sizeof(Lambda));
  ret->fn = &probeConsOuter;
  ret->env = NULL;
  return ret;
}

void printScottString(Lambda *n) {
  int count = 0;
  Lambda *current = n;
  Lambda *zeroMarker = makeZeroMarker();
  Lambda *cProbe = makeProbeCons();
  while (1) {
    // zeroMarker if current is zero otherwise sProbe the return value of sProbe
    // (which is NULL)
    Lambda *result = invoke(invoke(current, zeroMarker), cProbe);

    if (result == zeroMarker)
      break;

    probeConsEnv *cenv = result->env;
    char *string = malloc(2);
    string[0] = scottToInt(cenv->x);
    string[1] = '\0';
    printf("%s", string);
    current = cenv->xs;
    count++;
  }
}
// church numeral

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
