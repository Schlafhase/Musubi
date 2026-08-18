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
  while (1) {
    Lambda *zeroMarker = makeZeroMarker();
    Lambda *cProbe = makeProbeCons();
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
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
} e5d05703582884ab3a897257e79d64211;
Lambda *l5d05703582884ab3a897257e79d64211(void *raw_env, Lambda *arg) {
  e5d05703582884ab3a897257e79d64211 *env = raw_env;
  env->v0 = arg;
  return invoke(invoke(env->v1, env->v1), env->v0);
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} efd14c5431f1b4b74bc323897c037cb41;
Lambda *lfd14c5431f1b4b74bc323897c037cb41(void *raw_env, Lambda *arg) {
  efd14c5431f1b4b74bc323897c037cb41 *env = raw_env;
  env->v0 = arg;
  Lambda *_l5d05703582884ab3a897257e79d64211 = malloc(sizeof(Lambda));
  _l5d05703582884ab3a897257e79d64211->fn = &l5d05703582884ab3a897257e79d64211;
  e5d05703582884ab3a897257e79d64211 *_e5d05703582884ab3a897257e79d64211 =
      malloc(sizeof(e5d05703582884ab3a897257e79d64211));
  _e5d05703582884ab3a897257e79d64211->v1 = env->v0;
  _e5d05703582884ab3a897257e79d64211->v2 = env->v1;
  _l5d05703582884ab3a897257e79d64211->env = _e5d05703582884ab3a897257e79d64211;
  return invoke(env->v1, _l5d05703582884ab3a897257e79d64211);
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
} e9a65e37d6836482a8bd22f8139c594c1;
Lambda *l9a65e37d6836482a8bd22f8139c594c1(void *raw_env, Lambda *arg) {
  e9a65e37d6836482a8bd22f8139c594c1 *env = raw_env;
  env->v0 = arg;
  return invoke(invoke(env->v1, env->v1), env->v0);
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} e9d6eb24e78c145e1ab4b678a04780692;
Lambda *l9d6eb24e78c145e1ab4b678a04780692(void *raw_env, Lambda *arg) {
  e9d6eb24e78c145e1ab4b678a04780692 *env = raw_env;
  env->v0 = arg;
  Lambda *_l9a65e37d6836482a8bd22f8139c594c1 = malloc(sizeof(Lambda));
  _l9a65e37d6836482a8bd22f8139c594c1->fn = &l9a65e37d6836482a8bd22f8139c594c1;
  e9a65e37d6836482a8bd22f8139c594c1 *_e9a65e37d6836482a8bd22f8139c594c1 =
      malloc(sizeof(e9a65e37d6836482a8bd22f8139c594c1));
  _e9a65e37d6836482a8bd22f8139c594c1->v1 = env->v0;
  _e9a65e37d6836482a8bd22f8139c594c1->v2 = env->v1;
  _l9a65e37d6836482a8bd22f8139c594c1->env = _e9a65e37d6836482a8bd22f8139c594c1;
  return invoke(env->v1, _l9a65e37d6836482a8bd22f8139c594c1);
}
typedef struct {
  Lambda *v0;
} eedf51c2ba497401ebf8b2ca30554c986;
Lambda *ledf51c2ba497401ebf8b2ca30554c986(void *raw_env, Lambda *arg) {
  eedf51c2ba497401ebf8b2ca30554c986 *env = raw_env;
  env->v0 = arg;
  Lambda *_lfd14c5431f1b4b74bc323897c037cb41 = malloc(sizeof(Lambda));
  _lfd14c5431f1b4b74bc323897c037cb41->fn = &lfd14c5431f1b4b74bc323897c037cb41;
  efd14c5431f1b4b74bc323897c037cb41 *_efd14c5431f1b4b74bc323897c037cb41 =
      malloc(sizeof(efd14c5431f1b4b74bc323897c037cb41));
  _efd14c5431f1b4b74bc323897c037cb41->v1 = env->v0;
  _lfd14c5431f1b4b74bc323897c037cb41->env = _efd14c5431f1b4b74bc323897c037cb41;
  Lambda *_l9d6eb24e78c145e1ab4b678a04780692 = malloc(sizeof(Lambda));
  _l9d6eb24e78c145e1ab4b678a04780692->fn = &l9d6eb24e78c145e1ab4b678a04780692;
  e9d6eb24e78c145e1ab4b678a04780692 *_e9d6eb24e78c145e1ab4b678a04780692 =
      malloc(sizeof(e9d6eb24e78c145e1ab4b678a04780692));
  _e9d6eb24e78c145e1ab4b678a04780692->v1 = env->v0;
  _l9d6eb24e78c145e1ab4b678a04780692->env = _e9d6eb24e78c145e1ab4b678a04780692;
  return invoke(_lfd14c5431f1b4b74bc323897c037cb41,
                _l9d6eb24e78c145e1ab4b678a04780692);
}
// Z
Lambda *d5bf2caf388c2400a98f2757ad1b7daa3cached = NULL;
Lambda *d5bf2caf388c2400a98f2757ad1b7daa3() {
  Lambda *_ledf51c2ba497401ebf8b2ca30554c986 = malloc(sizeof(Lambda));
  _ledf51c2ba497401ebf8b2ca30554c986->fn = &ledf51c2ba497401ebf8b2ca30554c986;
  eedf51c2ba497401ebf8b2ca30554c986 *_eedf51c2ba497401ebf8b2ca30554c986 =
      malloc(sizeof(eedf51c2ba497401ebf8b2ca30554c986));
  _ledf51c2ba497401ebf8b2ca30554c986->env = _eedf51c2ba497401ebf8b2ca30554c986;
  if (d5bf2caf388c2400a98f2757ad1b7daa3cached == NULL)
    d5bf2caf388c2400a98f2757ad1b7daa3cached =
        _ledf51c2ba497401ebf8b2ca30554c986;
  return d5bf2caf388c2400a98f2757ad1b7daa3cached;
}
// rec
Lambda *d5e7283a86e4845b6aff9a4990c4bd70bcached = NULL;
Lambda *d5e7283a86e4845b6aff9a4990c4bd70b() {
  if (d5e7283a86e4845b6aff9a4990c4bd70bcached == NULL)
    d5e7283a86e4845b6aff9a4990c4bd70bcached =
        d5bf2caf388c2400a98f2757ad1b7daa3();
  return d5e7283a86e4845b6aff9a4990c4bd70bcached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} e6c122e50792a4aadb00d07fd57ecd143;
Lambda *l6c122e50792a4aadb00d07fd57ecd143(void *raw_env, Lambda *arg) {
  e6c122e50792a4aadb00d07fd57ecd143 *env = raw_env;
  env->v0 = arg;
  return invoke(env->v1, env->v0);
}
typedef struct {
  Lambda *v0;
} ea3669e35702d4307a126a82980dec12e;
Lambda *la3669e35702d4307a126a82980dec12e(void *raw_env, Lambda *arg) {
  ea3669e35702d4307a126a82980dec12e *env = raw_env;
  env->v0 = arg;
  Lambda *_l6c122e50792a4aadb00d07fd57ecd143 = malloc(sizeof(Lambda));
  _l6c122e50792a4aadb00d07fd57ecd143->fn = &l6c122e50792a4aadb00d07fd57ecd143;
  e6c122e50792a4aadb00d07fd57ecd143 *_e6c122e50792a4aadb00d07fd57ecd143 =
      malloc(sizeof(e6c122e50792a4aadb00d07fd57ecd143));
  _e6c122e50792a4aadb00d07fd57ecd143->v1 = env->v0;
  _l6c122e50792a4aadb00d07fd57ecd143->env = _e6c122e50792a4aadb00d07fd57ecd143;
  return _l6c122e50792a4aadb00d07fd57ecd143;
}
// lazy
Lambda *dc1bddb81d3014253a02fad3696d3e24bcached = NULL;
Lambda *dc1bddb81d3014253a02fad3696d3e24b() {
  Lambda *_la3669e35702d4307a126a82980dec12e = malloc(sizeof(Lambda));
  _la3669e35702d4307a126a82980dec12e->fn = &la3669e35702d4307a126a82980dec12e;
  ea3669e35702d4307a126a82980dec12e *_ea3669e35702d4307a126a82980dec12e =
      malloc(sizeof(ea3669e35702d4307a126a82980dec12e));
  _la3669e35702d4307a126a82980dec12e->env = _ea3669e35702d4307a126a82980dec12e;
  if (dc1bddb81d3014253a02fad3696d3e24bcached == NULL)
    dc1bddb81d3014253a02fad3696d3e24bcached =
        _la3669e35702d4307a126a82980dec12e;
  return dc1bddb81d3014253a02fad3696d3e24bcached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} eca644aec9a6c4e90b90e72114076cdd6;
Lambda *lca644aec9a6c4e90b90e72114076cdd6(void *raw_env, Lambda *arg) {
  eca644aec9a6c4e90b90e72114076cdd6 *env = raw_env;
  env->v0 = arg;
  return env->v1;
}
typedef struct {
  Lambda *v0;
} eb1fe559b23f1425ca2f5a14bddc945d2;
Lambda *lb1fe559b23f1425ca2f5a14bddc945d2(void *raw_env, Lambda *arg) {
  eb1fe559b23f1425ca2f5a14bddc945d2 *env = raw_env;
  env->v0 = arg;
  Lambda *_lca644aec9a6c4e90b90e72114076cdd6 = malloc(sizeof(Lambda));
  _lca644aec9a6c4e90b90e72114076cdd6->fn = &lca644aec9a6c4e90b90e72114076cdd6;
  eca644aec9a6c4e90b90e72114076cdd6 *_eca644aec9a6c4e90b90e72114076cdd6 =
      malloc(sizeof(eca644aec9a6c4e90b90e72114076cdd6));
  _eca644aec9a6c4e90b90e72114076cdd6->v1 = env->v0;
  _lca644aec9a6c4e90b90e72114076cdd6->env = _eca644aec9a6c4e90b90e72114076cdd6;
  return _lca644aec9a6c4e90b90e72114076cdd6;
}
// lazyNoArg
Lambda *da70e72ec54c54ceba2cb151c918008b6cached = NULL;
Lambda *da70e72ec54c54ceba2cb151c918008b6() {
  Lambda *_lb1fe559b23f1425ca2f5a14bddc945d2 = malloc(sizeof(Lambda));
  _lb1fe559b23f1425ca2f5a14bddc945d2->fn = &lb1fe559b23f1425ca2f5a14bddc945d2;
  eb1fe559b23f1425ca2f5a14bddc945d2 *_eb1fe559b23f1425ca2f5a14bddc945d2 =
      malloc(sizeof(eb1fe559b23f1425ca2f5a14bddc945d2));
  _lb1fe559b23f1425ca2f5a14bddc945d2->env = _eb1fe559b23f1425ca2f5a14bddc945d2;
  if (da70e72ec54c54ceba2cb151c918008b6cached == NULL)
    da70e72ec54c54ceba2cb151c918008b6cached =
        _lb1fe559b23f1425ca2f5a14bddc945d2;
  return da70e72ec54c54ceba2cb151c918008b6cached;
}
typedef struct {
  Lambda *v0;
} e042283e052d44667a12e68f5ef0c3bc9;
Lambda *l042283e052d44667a12e68f5ef0c3bc9(void *raw_env, Lambda *arg) {
  e042283e052d44667a12e68f5ef0c3bc9 *env = raw_env;
  env->v0 = arg;
  return invoke(env->v0, scottNumeral(0));
}
// force
Lambda *d60cacdaaf5b64e8fa2df19b923e06a29cached = NULL;
Lambda *d60cacdaaf5b64e8fa2df19b923e06a29() {
  Lambda *_l042283e052d44667a12e68f5ef0c3bc9 = malloc(sizeof(Lambda));
  _l042283e052d44667a12e68f5ef0c3bc9->fn = &l042283e052d44667a12e68f5ef0c3bc9;
  e042283e052d44667a12e68f5ef0c3bc9 *_e042283e052d44667a12e68f5ef0c3bc9 =
      malloc(sizeof(e042283e052d44667a12e68f5ef0c3bc9));
  _l042283e052d44667a12e68f5ef0c3bc9->env = _e042283e052d44667a12e68f5ef0c3bc9;
  if (d60cacdaaf5b64e8fa2df19b923e06a29cached == NULL)
    d60cacdaaf5b64e8fa2df19b923e06a29cached =
        _l042283e052d44667a12e68f5ef0c3bc9;
  return d60cacdaaf5b64e8fa2df19b923e06a29cached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} e5b1c682b92c74ef28743eed4ce01ba9f;
Lambda *l5b1c682b92c74ef28743eed4ce01ba9f(void *raw_env, Lambda *arg) {
  e5b1c682b92c74ef28743eed4ce01ba9f *env = raw_env;
  env->v0 = arg;
  return env->v1;
}
typedef struct {
  Lambda *v0;
} e257cc15512d943218670814e89f3587f;
Lambda *l257cc15512d943218670814e89f3587f(void *raw_env, Lambda *arg) {
  e257cc15512d943218670814e89f3587f *env = raw_env;
  env->v0 = arg;
  Lambda *_l5b1c682b92c74ef28743eed4ce01ba9f = malloc(sizeof(Lambda));
  _l5b1c682b92c74ef28743eed4ce01ba9f->fn = &l5b1c682b92c74ef28743eed4ce01ba9f;
  e5b1c682b92c74ef28743eed4ce01ba9f *_e5b1c682b92c74ef28743eed4ce01ba9f =
      malloc(sizeof(e5b1c682b92c74ef28743eed4ce01ba9f));
  _e5b1c682b92c74ef28743eed4ce01ba9f->v1 = env->v0;
  _l5b1c682b92c74ef28743eed4ce01ba9f->env = _e5b1c682b92c74ef28743eed4ce01ba9f;
  return _l5b1c682b92c74ef28743eed4ce01ba9f;
}
// true
Lambda *d0765517682aa40a7a0f40802211c37fbcached = NULL;
Lambda *d0765517682aa40a7a0f40802211c37fb() {
  Lambda *_l257cc15512d943218670814e89f3587f = malloc(sizeof(Lambda));
  _l257cc15512d943218670814e89f3587f->fn = &l257cc15512d943218670814e89f3587f;
  e257cc15512d943218670814e89f3587f *_e257cc15512d943218670814e89f3587f =
      malloc(sizeof(e257cc15512d943218670814e89f3587f));
  _l257cc15512d943218670814e89f3587f->env = _e257cc15512d943218670814e89f3587f;
  if (d0765517682aa40a7a0f40802211c37fbcached == NULL)
    d0765517682aa40a7a0f40802211c37fbcached =
        _l257cc15512d943218670814e89f3587f;
  return d0765517682aa40a7a0f40802211c37fbcached;
}
// false
Lambda *d000b66270a2f4a6d82641ef12fdc1adacached = NULL;
Lambda *d000b66270a2f4a6d82641ef12fdc1ada() {
  if (d000b66270a2f4a6d82641ef12fdc1adacached == NULL)
    d000b66270a2f4a6d82641ef12fdc1adacached = churchNumeral(0);
  return d000b66270a2f4a6d82641ef12fdc1adacached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} e93041790681543c9a520764ead70f340;
Lambda *l93041790681543c9a520764ead70f340(void *raw_env, Lambda *arg) {
  e93041790681543c9a520764ead70f340 *env = raw_env;
  env->v0 = arg;
  return invoke(invoke(env->v1, env->v0), d000b66270a2f4a6d82641ef12fdc1ada());
}
typedef struct {
  Lambda *v0;
} e951885e622dd49a0b5a35abc305c2ab7;
Lambda *l951885e622dd49a0b5a35abc305c2ab7(void *raw_env, Lambda *arg) {
  e951885e622dd49a0b5a35abc305c2ab7 *env = raw_env;
  env->v0 = arg;
  Lambda *_l93041790681543c9a520764ead70f340 = malloc(sizeof(Lambda));
  _l93041790681543c9a520764ead70f340->fn = &l93041790681543c9a520764ead70f340;
  e93041790681543c9a520764ead70f340 *_e93041790681543c9a520764ead70f340 =
      malloc(sizeof(e93041790681543c9a520764ead70f340));
  _e93041790681543c9a520764ead70f340->v1 = env->v0;
  _l93041790681543c9a520764ead70f340->env = _e93041790681543c9a520764ead70f340;
  return _l93041790681543c9a520764ead70f340;
}
// and
Lambda *d842d9baced154df0bd09e4286f51e40ecached = NULL;
Lambda *d842d9baced154df0bd09e4286f51e40e() {
  Lambda *_l951885e622dd49a0b5a35abc305c2ab7 = malloc(sizeof(Lambda));
  _l951885e622dd49a0b5a35abc305c2ab7->fn = &l951885e622dd49a0b5a35abc305c2ab7;
  e951885e622dd49a0b5a35abc305c2ab7 *_e951885e622dd49a0b5a35abc305c2ab7 =
      malloc(sizeof(e951885e622dd49a0b5a35abc305c2ab7));
  _l951885e622dd49a0b5a35abc305c2ab7->env = _e951885e622dd49a0b5a35abc305c2ab7;
  if (d842d9baced154df0bd09e4286f51e40ecached == NULL)
    d842d9baced154df0bd09e4286f51e40ecached =
        _l951885e622dd49a0b5a35abc305c2ab7;
  return d842d9baced154df0bd09e4286f51e40ecached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} ec74e3c32b846497a81b515352cd15e03;
Lambda *lc74e3c32b846497a81b515352cd15e03(void *raw_env, Lambda *arg) {
  ec74e3c32b846497a81b515352cd15e03 *env = raw_env;
  env->v0 = arg;
  return invoke(invoke(env->v1, d0765517682aa40a7a0f40802211c37fb()), env->v0);
}
typedef struct {
  Lambda *v0;
} ea4eced59093e45ceaabf4e722c8e891a;
Lambda *la4eced59093e45ceaabf4e722c8e891a(void *raw_env, Lambda *arg) {
  ea4eced59093e45ceaabf4e722c8e891a *env = raw_env;
  env->v0 = arg;
  Lambda *_lc74e3c32b846497a81b515352cd15e03 = malloc(sizeof(Lambda));
  _lc74e3c32b846497a81b515352cd15e03->fn = &lc74e3c32b846497a81b515352cd15e03;
  ec74e3c32b846497a81b515352cd15e03 *_ec74e3c32b846497a81b515352cd15e03 =
      malloc(sizeof(ec74e3c32b846497a81b515352cd15e03));
  _ec74e3c32b846497a81b515352cd15e03->v1 = env->v0;
  _lc74e3c32b846497a81b515352cd15e03->env = _ec74e3c32b846497a81b515352cd15e03;
  return _lc74e3c32b846497a81b515352cd15e03;
}
// or
Lambda *dea818acabf044d0cab634e8513c2bd59cached = NULL;
Lambda *dea818acabf044d0cab634e8513c2bd59() {
  Lambda *_la4eced59093e45ceaabf4e722c8e891a = malloc(sizeof(Lambda));
  _la4eced59093e45ceaabf4e722c8e891a->fn = &la4eced59093e45ceaabf4e722c8e891a;
  ea4eced59093e45ceaabf4e722c8e891a *_ea4eced59093e45ceaabf4e722c8e891a =
      malloc(sizeof(ea4eced59093e45ceaabf4e722c8e891a));
  _la4eced59093e45ceaabf4e722c8e891a->env = _ea4eced59093e45ceaabf4e722c8e891a;
  if (dea818acabf044d0cab634e8513c2bd59cached == NULL)
    dea818acabf044d0cab634e8513c2bd59cached =
        _la4eced59093e45ceaabf4e722c8e891a;
  return dea818acabf044d0cab634e8513c2bd59cached;
}
typedef struct {
  Lambda *v0;
} ebdbb653f66c5431595bf9d12708acbdf;
Lambda *lbdbb653f66c5431595bf9d12708acbdf(void *raw_env, Lambda *arg) {
  ebdbb653f66c5431595bf9d12708acbdf *env = raw_env;
  env->v0 = arg;
  return invoke(invoke(env->v0, d000b66270a2f4a6d82641ef12fdc1ada()),
                d0765517682aa40a7a0f40802211c37fb());
}
// not
Lambda *d0ce8331506aa43f8accacea259f4ac98cached = NULL;
Lambda *d0ce8331506aa43f8accacea259f4ac98() {
  Lambda *_lbdbb653f66c5431595bf9d12708acbdf = malloc(sizeof(Lambda));
  _lbdbb653f66c5431595bf9d12708acbdf->fn = &lbdbb653f66c5431595bf9d12708acbdf;
  ebdbb653f66c5431595bf9d12708acbdf *_ebdbb653f66c5431595bf9d12708acbdf =
      malloc(sizeof(ebdbb653f66c5431595bf9d12708acbdf));
  _lbdbb653f66c5431595bf9d12708acbdf->env = _ebdbb653f66c5431595bf9d12708acbdf;
  if (d0ce8331506aa43f8accacea259f4ac98cached == NULL)
    d0ce8331506aa43f8accacea259f4ac98cached =
        _lbdbb653f66c5431595bf9d12708acbdf;
  return d0ce8331506aa43f8accacea259f4ac98cached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
} e7b382cc645fe47acbd97740b57ad10cb;
Lambda *l7b382cc645fe47acbd97740b57ad10cb(void *raw_env, Lambda *arg) {
  e7b382cc645fe47acbd97740b57ad10cb *env = raw_env;
  env->v0 = arg;
  return invoke(env->v0, env->v2);
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} e0cdaefaf7cfb43c9865655d2ff271a53;
Lambda *l0cdaefaf7cfb43c9865655d2ff271a53(void *raw_env, Lambda *arg) {
  e0cdaefaf7cfb43c9865655d2ff271a53 *env = raw_env;
  env->v0 = arg;
  Lambda *_l7b382cc645fe47acbd97740b57ad10cb = malloc(sizeof(Lambda));
  _l7b382cc645fe47acbd97740b57ad10cb->fn = &l7b382cc645fe47acbd97740b57ad10cb;
  e7b382cc645fe47acbd97740b57ad10cb *_e7b382cc645fe47acbd97740b57ad10cb =
      malloc(sizeof(e7b382cc645fe47acbd97740b57ad10cb));
  _e7b382cc645fe47acbd97740b57ad10cb->v1 = env->v0;
  _e7b382cc645fe47acbd97740b57ad10cb->v2 = env->v1;
  _l7b382cc645fe47acbd97740b57ad10cb->env = _e7b382cc645fe47acbd97740b57ad10cb;
  return _l7b382cc645fe47acbd97740b57ad10cb;
}
typedef struct {
  Lambda *v0;
} e1ede3ceb0b494274988ac0350c488797;
Lambda *l1ede3ceb0b494274988ac0350c488797(void *raw_env, Lambda *arg) {
  e1ede3ceb0b494274988ac0350c488797 *env = raw_env;
  env->v0 = arg;
  Lambda *_l0cdaefaf7cfb43c9865655d2ff271a53 = malloc(sizeof(Lambda));
  _l0cdaefaf7cfb43c9865655d2ff271a53->fn = &l0cdaefaf7cfb43c9865655d2ff271a53;
  e0cdaefaf7cfb43c9865655d2ff271a53 *_e0cdaefaf7cfb43c9865655d2ff271a53 =
      malloc(sizeof(e0cdaefaf7cfb43c9865655d2ff271a53));
  _e0cdaefaf7cfb43c9865655d2ff271a53->v1 = env->v0;
  _l0cdaefaf7cfb43c9865655d2ff271a53->env = _e0cdaefaf7cfb43c9865655d2ff271a53;
  return _l0cdaefaf7cfb43c9865655d2ff271a53;
}
// Ss
Lambda *dbf9642fd339242ea87f46fefe2ea20afcached = NULL;
Lambda *dbf9642fd339242ea87f46fefe2ea20af() {
  Lambda *_l1ede3ceb0b494274988ac0350c488797 = malloc(sizeof(Lambda));
  _l1ede3ceb0b494274988ac0350c488797->fn = &l1ede3ceb0b494274988ac0350c488797;
  e1ede3ceb0b494274988ac0350c488797 *_e1ede3ceb0b494274988ac0350c488797 =
      malloc(sizeof(e1ede3ceb0b494274988ac0350c488797));
  _l1ede3ceb0b494274988ac0350c488797->env = _e1ede3ceb0b494274988ac0350c488797;
  if (dbf9642fd339242ea87f46fefe2ea20afcached == NULL)
    dbf9642fd339242ea87f46fefe2ea20afcached =
        _l1ede3ceb0b494274988ac0350c488797;
  return dbf9642fd339242ea87f46fefe2ea20afcached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} e228ecd1e771a4645be5ca7f72cd11d56;
Lambda *l228ecd1e771a4645be5ca7f72cd11d56(void *raw_env, Lambda *arg) {
  e228ecd1e771a4645be5ca7f72cd11d56 *env = raw_env;
  env->v0 = arg;
  return env->v0;
}
typedef struct {
  Lambda *v0;
} e841870c8aaf54423a45fcf460d6060b2;
Lambda *l841870c8aaf54423a45fcf460d6060b2(void *raw_env, Lambda *arg) {
  e841870c8aaf54423a45fcf460d6060b2 *env = raw_env;
  env->v0 = arg;
  Lambda *_l228ecd1e771a4645be5ca7f72cd11d56 = malloc(sizeof(Lambda));
  _l228ecd1e771a4645be5ca7f72cd11d56->fn = &l228ecd1e771a4645be5ca7f72cd11d56;
  e228ecd1e771a4645be5ca7f72cd11d56 *_e228ecd1e771a4645be5ca7f72cd11d56 =
      malloc(sizeof(e228ecd1e771a4645be5ca7f72cd11d56));
  _e228ecd1e771a4645be5ca7f72cd11d56->v1 = env->v0;
  _l228ecd1e771a4645be5ca7f72cd11d56->env = _e228ecd1e771a4645be5ca7f72cd11d56;
  return invoke(invoke(env->v0, scottNumeral(0)),
                _l228ecd1e771a4645be5ca7f72cd11d56);
}
// preds
Lambda *d666ad9b3406c471da29bfad2a05f90e7cached = NULL;
Lambda *d666ad9b3406c471da29bfad2a05f90e7() {
  Lambda *_l841870c8aaf54423a45fcf460d6060b2 = malloc(sizeof(Lambda));
  _l841870c8aaf54423a45fcf460d6060b2->fn = &l841870c8aaf54423a45fcf460d6060b2;
  e841870c8aaf54423a45fcf460d6060b2 *_e841870c8aaf54423a45fcf460d6060b2 =
      malloc(sizeof(e841870c8aaf54423a45fcf460d6060b2));
  _l841870c8aaf54423a45fcf460d6060b2->env = _e841870c8aaf54423a45fcf460d6060b2;
  if (d666ad9b3406c471da29bfad2a05f90e7cached == NULL)
    d666ad9b3406c471da29bfad2a05f90e7cached =
        _l841870c8aaf54423a45fcf460d6060b2;
  return d666ad9b3406c471da29bfad2a05f90e7cached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} e31fa4c0752df492bac28796a2afea08e;
Lambda *l31fa4c0752df492bac28796a2afea08e(void *raw_env, Lambda *arg) {
  e31fa4c0752df492bac28796a2afea08e *env = raw_env;
  env->v0 = arg;
  return d000b66270a2f4a6d82641ef12fdc1ada();
}
typedef struct {
  Lambda *v0;
} e6dc414bf62cf4d8f9d7b3d8a0cd7bdec;
Lambda *l6dc414bf62cf4d8f9d7b3d8a0cd7bdec(void *raw_env, Lambda *arg) {
  e6dc414bf62cf4d8f9d7b3d8a0cd7bdec *env = raw_env;
  env->v0 = arg;
  Lambda *_l31fa4c0752df492bac28796a2afea08e = malloc(sizeof(Lambda));
  _l31fa4c0752df492bac28796a2afea08e->fn = &l31fa4c0752df492bac28796a2afea08e;
  e31fa4c0752df492bac28796a2afea08e *_e31fa4c0752df492bac28796a2afea08e =
      malloc(sizeof(e31fa4c0752df492bac28796a2afea08e));
  _e31fa4c0752df492bac28796a2afea08e->v1 = env->v0;
  _l31fa4c0752df492bac28796a2afea08e->env = _e31fa4c0752df492bac28796a2afea08e;
  return invoke(invoke(env->v0, d0765517682aa40a7a0f40802211c37fb()),
                _l31fa4c0752df492bac28796a2afea08e);
}
// isZeros
Lambda *d2932b30f62e74d9eafca1518d25b86c2cached = NULL;
Lambda *d2932b30f62e74d9eafca1518d25b86c2() {
  Lambda *_l6dc414bf62cf4d8f9d7b3d8a0cd7bdec = malloc(sizeof(Lambda));
  _l6dc414bf62cf4d8f9d7b3d8a0cd7bdec->fn = &l6dc414bf62cf4d8f9d7b3d8a0cd7bdec;
  e6dc414bf62cf4d8f9d7b3d8a0cd7bdec *_e6dc414bf62cf4d8f9d7b3d8a0cd7bdec =
      malloc(sizeof(e6dc414bf62cf4d8f9d7b3d8a0cd7bdec));
  _l6dc414bf62cf4d8f9d7b3d8a0cd7bdec->env = _e6dc414bf62cf4d8f9d7b3d8a0cd7bdec;
  if (d2932b30f62e74d9eafca1518d25b86c2cached == NULL)
    d2932b30f62e74d9eafca1518d25b86c2cached =
        _l6dc414bf62cf4d8f9d7b3d8a0cd7bdec;
  return d2932b30f62e74d9eafca1518d25b86c2cached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
  Lambda *v3;
} e23efc236820044c0a60844add85fa1f3;
Lambda *l23efc236820044c0a60844add85fa1f3(void *raw_env, Lambda *arg) {
  e23efc236820044c0a60844add85fa1f3 *env = raw_env;
  env->v0 = arg;
  return invoke(dbf9642fd339242ea87f46fefe2ea20af(),
                invoke(invoke(env->v3, env->v0), env->v1));
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
} e1f4a50cae9f94d40b3ee9b6660ecb6d2;
Lambda *l1f4a50cae9f94d40b3ee9b6660ecb6d2(void *raw_env, Lambda *arg) {
  e1f4a50cae9f94d40b3ee9b6660ecb6d2 *env = raw_env;
  env->v0 = arg;
  Lambda *_l23efc236820044c0a60844add85fa1f3 = malloc(sizeof(Lambda));
  _l23efc236820044c0a60844add85fa1f3->fn = &l23efc236820044c0a60844add85fa1f3;
  e23efc236820044c0a60844add85fa1f3 *_e23efc236820044c0a60844add85fa1f3 =
      malloc(sizeof(e23efc236820044c0a60844add85fa1f3));
  _e23efc236820044c0a60844add85fa1f3->v1 = env->v0;
  _e23efc236820044c0a60844add85fa1f3->v2 = env->v1;
  _e23efc236820044c0a60844add85fa1f3->v3 = env->v2;
  _l23efc236820044c0a60844add85fa1f3->env = _e23efc236820044c0a60844add85fa1f3;
  return invoke(invoke(env->v1, env->v0), _l23efc236820044c0a60844add85fa1f3);
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} ee77c20c3201b48c0a149ee8d6a3d63df;
Lambda *le77c20c3201b48c0a149ee8d6a3d63df(void *raw_env, Lambda *arg) {
  ee77c20c3201b48c0a149ee8d6a3d63df *env = raw_env;
  env->v0 = arg;
  Lambda *_l1f4a50cae9f94d40b3ee9b6660ecb6d2 = malloc(sizeof(Lambda));
  _l1f4a50cae9f94d40b3ee9b6660ecb6d2->fn = &l1f4a50cae9f94d40b3ee9b6660ecb6d2;
  e1f4a50cae9f94d40b3ee9b6660ecb6d2 *_e1f4a50cae9f94d40b3ee9b6660ecb6d2 =
      malloc(sizeof(e1f4a50cae9f94d40b3ee9b6660ecb6d2));
  _e1f4a50cae9f94d40b3ee9b6660ecb6d2->v1 = env->v0;
  _e1f4a50cae9f94d40b3ee9b6660ecb6d2->v2 = env->v1;
  _l1f4a50cae9f94d40b3ee9b6660ecb6d2->env = _e1f4a50cae9f94d40b3ee9b6660ecb6d2;
  return _l1f4a50cae9f94d40b3ee9b6660ecb6d2;
}
typedef struct {
  Lambda *v0;
} e41c934e1a5294407b23bb2738c8aff0e;
Lambda *l41c934e1a5294407b23bb2738c8aff0e(void *raw_env, Lambda *arg) {
  e41c934e1a5294407b23bb2738c8aff0e *env = raw_env;
  env->v0 = arg;
  Lambda *_le77c20c3201b48c0a149ee8d6a3d63df = malloc(sizeof(Lambda));
  _le77c20c3201b48c0a149ee8d6a3d63df->fn = &le77c20c3201b48c0a149ee8d6a3d63df;
  ee77c20c3201b48c0a149ee8d6a3d63df *_ee77c20c3201b48c0a149ee8d6a3d63df =
      malloc(sizeof(ee77c20c3201b48c0a149ee8d6a3d63df));
  _ee77c20c3201b48c0a149ee8d6a3d63df->v1 = env->v0;
  _le77c20c3201b48c0a149ee8d6a3d63df->env = _ee77c20c3201b48c0a149ee8d6a3d63df;
  return _le77c20c3201b48c0a149ee8d6a3d63df;
}
// adds
Lambda *d4f6b7e65d84c44ddbbad75049f3cea15cached = NULL;
Lambda *d4f6b7e65d84c44ddbbad75049f3cea15() {
  Lambda *_l41c934e1a5294407b23bb2738c8aff0e = malloc(sizeof(Lambda));
  _l41c934e1a5294407b23bb2738c8aff0e->fn = &l41c934e1a5294407b23bb2738c8aff0e;
  e41c934e1a5294407b23bb2738c8aff0e *_e41c934e1a5294407b23bb2738c8aff0e =
      malloc(sizeof(e41c934e1a5294407b23bb2738c8aff0e));
  _l41c934e1a5294407b23bb2738c8aff0e->env = _e41c934e1a5294407b23bb2738c8aff0e;
  if (d4f6b7e65d84c44ddbbad75049f3cea15cached == NULL)
    d4f6b7e65d84c44ddbbad75049f3cea15cached =
        invoke(d5e7283a86e4845b6aff9a4990c4bd70b(),
               _l41c934e1a5294407b23bb2738c8aff0e);
  return d4f6b7e65d84c44ddbbad75049f3cea15cached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
  Lambda *v3;
} e7a04f0020bbe41d58ef0072987af1588;
Lambda *l7a04f0020bbe41d58ef0072987af1588(void *raw_env, Lambda *arg) {
  e7a04f0020bbe41d58ef0072987af1588 *env = raw_env;
  env->v0 = arg;
  return invoke(d666ad9b3406c471da29bfad2a05f90e7(),
                invoke(invoke(env->v3, env->v2), env->v0));
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
} e7b8f0b42b6aa47bc803af7d403e4a25b;
Lambda *l7b8f0b42b6aa47bc803af7d403e4a25b(void *raw_env, Lambda *arg) {
  e7b8f0b42b6aa47bc803af7d403e4a25b *env = raw_env;
  env->v0 = arg;
  Lambda *_l7a04f0020bbe41d58ef0072987af1588 = malloc(sizeof(Lambda));
  _l7a04f0020bbe41d58ef0072987af1588->fn = &l7a04f0020bbe41d58ef0072987af1588;
  e7a04f0020bbe41d58ef0072987af1588 *_e7a04f0020bbe41d58ef0072987af1588 =
      malloc(sizeof(e7a04f0020bbe41d58ef0072987af1588));
  _e7a04f0020bbe41d58ef0072987af1588->v1 = env->v0;
  _e7a04f0020bbe41d58ef0072987af1588->v2 = env->v1;
  _e7a04f0020bbe41d58ef0072987af1588->v3 = env->v2;
  _l7a04f0020bbe41d58ef0072987af1588->env = _e7a04f0020bbe41d58ef0072987af1588;
  return invoke(invoke(env->v0, env->v1), _l7a04f0020bbe41d58ef0072987af1588);
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} e811c678aa12d4b30929a100ad4312238;
Lambda *l811c678aa12d4b30929a100ad4312238(void *raw_env, Lambda *arg) {
  e811c678aa12d4b30929a100ad4312238 *env = raw_env;
  env->v0 = arg;
  Lambda *_l7b8f0b42b6aa47bc803af7d403e4a25b = malloc(sizeof(Lambda));
  _l7b8f0b42b6aa47bc803af7d403e4a25b->fn = &l7b8f0b42b6aa47bc803af7d403e4a25b;
  e7b8f0b42b6aa47bc803af7d403e4a25b *_e7b8f0b42b6aa47bc803af7d403e4a25b =
      malloc(sizeof(e7b8f0b42b6aa47bc803af7d403e4a25b));
  _e7b8f0b42b6aa47bc803af7d403e4a25b->v1 = env->v0;
  _e7b8f0b42b6aa47bc803af7d403e4a25b->v2 = env->v1;
  _l7b8f0b42b6aa47bc803af7d403e4a25b->env = _e7b8f0b42b6aa47bc803af7d403e4a25b;
  return _l7b8f0b42b6aa47bc803af7d403e4a25b;
}
typedef struct {
  Lambda *v0;
} ef8d26873ee564974b8cb4a9b1640207a;
Lambda *lf8d26873ee564974b8cb4a9b1640207a(void *raw_env, Lambda *arg) {
  ef8d26873ee564974b8cb4a9b1640207a *env = raw_env;
  env->v0 = arg;
  Lambda *_l811c678aa12d4b30929a100ad4312238 = malloc(sizeof(Lambda));
  _l811c678aa12d4b30929a100ad4312238->fn = &l811c678aa12d4b30929a100ad4312238;
  e811c678aa12d4b30929a100ad4312238 *_e811c678aa12d4b30929a100ad4312238 =
      malloc(sizeof(e811c678aa12d4b30929a100ad4312238));
  _e811c678aa12d4b30929a100ad4312238->v1 = env->v0;
  _l811c678aa12d4b30929a100ad4312238->env = _e811c678aa12d4b30929a100ad4312238;
  return _l811c678aa12d4b30929a100ad4312238;
}
// subs
Lambda *dbfda69ba516d4fca9334c637af8cafebcached = NULL;
Lambda *dbfda69ba516d4fca9334c637af8cafeb() {
  Lambda *_lf8d26873ee564974b8cb4a9b1640207a = malloc(sizeof(Lambda));
  _lf8d26873ee564974b8cb4a9b1640207a->fn = &lf8d26873ee564974b8cb4a9b1640207a;
  ef8d26873ee564974b8cb4a9b1640207a *_ef8d26873ee564974b8cb4a9b1640207a =
      malloc(sizeof(ef8d26873ee564974b8cb4a9b1640207a));
  _lf8d26873ee564974b8cb4a9b1640207a->env = _ef8d26873ee564974b8cb4a9b1640207a;
  if (dbfda69ba516d4fca9334c637af8cafebcached == NULL)
    dbfda69ba516d4fca9334c637af8cafebcached =
        invoke(d5e7283a86e4845b6aff9a4990c4bd70b(),
               _lf8d26873ee564974b8cb4a9b1640207a);
  return dbfda69ba516d4fca9334c637af8cafebcached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
  Lambda *v3;
} ebcb29f5f7b8e46658f86e0da4ecaa30c;
Lambda *lbcb29f5f7b8e46658f86e0da4ecaa30c(void *raw_env, Lambda *arg) {
  ebcb29f5f7b8e46658f86e0da4ecaa30c *env = raw_env;
  env->v0 = arg;
  return invoke(invoke(d4f6b7e65d84c44ddbbad75049f3cea15(), env->v1),
                invoke(invoke(env->v3, env->v0), env->v1));
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
} e7d1980f269f3447aa91ef5dd8ec040bd;
Lambda *l7d1980f269f3447aa91ef5dd8ec040bd(void *raw_env, Lambda *arg) {
  e7d1980f269f3447aa91ef5dd8ec040bd *env = raw_env;
  env->v0 = arg;
  Lambda *_lbcb29f5f7b8e46658f86e0da4ecaa30c = malloc(sizeof(Lambda));
  _lbcb29f5f7b8e46658f86e0da4ecaa30c->fn = &lbcb29f5f7b8e46658f86e0da4ecaa30c;
  ebcb29f5f7b8e46658f86e0da4ecaa30c *_ebcb29f5f7b8e46658f86e0da4ecaa30c =
      malloc(sizeof(ebcb29f5f7b8e46658f86e0da4ecaa30c));
  _ebcb29f5f7b8e46658f86e0da4ecaa30c->v1 = env->v0;
  _ebcb29f5f7b8e46658f86e0da4ecaa30c->v2 = env->v1;
  _ebcb29f5f7b8e46658f86e0da4ecaa30c->v3 = env->v2;
  _lbcb29f5f7b8e46658f86e0da4ecaa30c->env = _ebcb29f5f7b8e46658f86e0da4ecaa30c;
  return invoke(invoke(env->v1, scottNumeral(0)),
                _lbcb29f5f7b8e46658f86e0da4ecaa30c);
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} e0b7659e3c3ff4d44a83ab5ba3fccc250;
Lambda *l0b7659e3c3ff4d44a83ab5ba3fccc250(void *raw_env, Lambda *arg) {
  e0b7659e3c3ff4d44a83ab5ba3fccc250 *env = raw_env;
  env->v0 = arg;
  Lambda *_l7d1980f269f3447aa91ef5dd8ec040bd = malloc(sizeof(Lambda));
  _l7d1980f269f3447aa91ef5dd8ec040bd->fn = &l7d1980f269f3447aa91ef5dd8ec040bd;
  e7d1980f269f3447aa91ef5dd8ec040bd *_e7d1980f269f3447aa91ef5dd8ec040bd =
      malloc(sizeof(e7d1980f269f3447aa91ef5dd8ec040bd));
  _e7d1980f269f3447aa91ef5dd8ec040bd->v1 = env->v0;
  _e7d1980f269f3447aa91ef5dd8ec040bd->v2 = env->v1;
  _l7d1980f269f3447aa91ef5dd8ec040bd->env = _e7d1980f269f3447aa91ef5dd8ec040bd;
  return _l7d1980f269f3447aa91ef5dd8ec040bd;
}
typedef struct {
  Lambda *v0;
} ea7f58e68567d4bcbb1275aa12bf3454e;
Lambda *la7f58e68567d4bcbb1275aa12bf3454e(void *raw_env, Lambda *arg) {
  ea7f58e68567d4bcbb1275aa12bf3454e *env = raw_env;
  env->v0 = arg;
  Lambda *_l0b7659e3c3ff4d44a83ab5ba3fccc250 = malloc(sizeof(Lambda));
  _l0b7659e3c3ff4d44a83ab5ba3fccc250->fn = &l0b7659e3c3ff4d44a83ab5ba3fccc250;
  e0b7659e3c3ff4d44a83ab5ba3fccc250 *_e0b7659e3c3ff4d44a83ab5ba3fccc250 =
      malloc(sizeof(e0b7659e3c3ff4d44a83ab5ba3fccc250));
  _e0b7659e3c3ff4d44a83ab5ba3fccc250->v1 = env->v0;
  _l0b7659e3c3ff4d44a83ab5ba3fccc250->env = _e0b7659e3c3ff4d44a83ab5ba3fccc250;
  return _l0b7659e3c3ff4d44a83ab5ba3fccc250;
}
// mults
Lambda *d562036dc193242d4ad7ade6792c3fc73cached = NULL;
Lambda *d562036dc193242d4ad7ade6792c3fc73() {
  Lambda *_la7f58e68567d4bcbb1275aa12bf3454e = malloc(sizeof(Lambda));
  _la7f58e68567d4bcbb1275aa12bf3454e->fn = &la7f58e68567d4bcbb1275aa12bf3454e;
  ea7f58e68567d4bcbb1275aa12bf3454e *_ea7f58e68567d4bcbb1275aa12bf3454e =
      malloc(sizeof(ea7f58e68567d4bcbb1275aa12bf3454e));
  _la7f58e68567d4bcbb1275aa12bf3454e->env = _ea7f58e68567d4bcbb1275aa12bf3454e;
  if (d562036dc193242d4ad7ade6792c3fc73cached == NULL)
    d562036dc193242d4ad7ade6792c3fc73cached =
        invoke(d5e7283a86e4845b6aff9a4990c4bd70b(),
               _la7f58e68567d4bcbb1275aa12bf3454e);
  return d562036dc193242d4ad7ade6792c3fc73cached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} ee6b9d8fd3c994a3a98c704ef4b83f30e;
Lambda *le6b9d8fd3c994a3a98c704ef4b83f30e(void *raw_env, Lambda *arg) {
  ee6b9d8fd3c994a3a98c704ef4b83f30e *env = raw_env;
  env->v0 = arg;
  return invoke(
      d2932b30f62e74d9eafca1518d25b86c2(),
      invoke(invoke(dbfda69ba516d4fca9334c637af8cafeb(), env->v1), env->v0));
}
typedef struct {
  Lambda *v0;
} eddc7341c40d24f789e1c2ed823952d6e;
Lambda *lddc7341c40d24f789e1c2ed823952d6e(void *raw_env, Lambda *arg) {
  eddc7341c40d24f789e1c2ed823952d6e *env = raw_env;
  env->v0 = arg;
  Lambda *_le6b9d8fd3c994a3a98c704ef4b83f30e = malloc(sizeof(Lambda));
  _le6b9d8fd3c994a3a98c704ef4b83f30e->fn = &le6b9d8fd3c994a3a98c704ef4b83f30e;
  ee6b9d8fd3c994a3a98c704ef4b83f30e *_ee6b9d8fd3c994a3a98c704ef4b83f30e =
      malloc(sizeof(ee6b9d8fd3c994a3a98c704ef4b83f30e));
  _ee6b9d8fd3c994a3a98c704ef4b83f30e->v1 = env->v0;
  _le6b9d8fd3c994a3a98c704ef4b83f30e->env = _ee6b9d8fd3c994a3a98c704ef4b83f30e;
  return _le6b9d8fd3c994a3a98c704ef4b83f30e;
}
// leqs
Lambda *df8c2628dcba14ab78413e848b72cb4decached = NULL;
Lambda *df8c2628dcba14ab78413e848b72cb4de() {
  Lambda *_lddc7341c40d24f789e1c2ed823952d6e = malloc(sizeof(Lambda));
  _lddc7341c40d24f789e1c2ed823952d6e->fn = &lddc7341c40d24f789e1c2ed823952d6e;
  eddc7341c40d24f789e1c2ed823952d6e *_eddc7341c40d24f789e1c2ed823952d6e =
      malloc(sizeof(eddc7341c40d24f789e1c2ed823952d6e));
  _lddc7341c40d24f789e1c2ed823952d6e->env = _eddc7341c40d24f789e1c2ed823952d6e;
  if (df8c2628dcba14ab78413e848b72cb4decached == NULL)
    df8c2628dcba14ab78413e848b72cb4decached =
        _lddc7341c40d24f789e1c2ed823952d6e;
  return df8c2628dcba14ab78413e848b72cb4decached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} e30e04f16ae464d70a252e97b02a844c7;
Lambda *l30e04f16ae464d70a252e97b02a844c7(void *raw_env, Lambda *arg) {
  e30e04f16ae464d70a252e97b02a844c7 *env = raw_env;
  env->v0 = arg;
  return invoke(
      invoke(d842d9baced154df0bd09e4286f51e40e(),
             invoke(invoke(df8c2628dcba14ab78413e848b72cb4de(), env->v1),
                    env->v0)),
      invoke(invoke(df8c2628dcba14ab78413e848b72cb4de(), env->v0), env->v1));
}
typedef struct {
  Lambda *v0;
} ece8b6e32dbd3473dab03b782102e9611;
Lambda *lce8b6e32dbd3473dab03b782102e9611(void *raw_env, Lambda *arg) {
  ece8b6e32dbd3473dab03b782102e9611 *env = raw_env;
  env->v0 = arg;
  Lambda *_l30e04f16ae464d70a252e97b02a844c7 = malloc(sizeof(Lambda));
  _l30e04f16ae464d70a252e97b02a844c7->fn = &l30e04f16ae464d70a252e97b02a844c7;
  e30e04f16ae464d70a252e97b02a844c7 *_e30e04f16ae464d70a252e97b02a844c7 =
      malloc(sizeof(e30e04f16ae464d70a252e97b02a844c7));
  _e30e04f16ae464d70a252e97b02a844c7->v1 = env->v0;
  _l30e04f16ae464d70a252e97b02a844c7->env = _e30e04f16ae464d70a252e97b02a844c7;
  return _l30e04f16ae464d70a252e97b02a844c7;
}
// eqs
Lambda *d46a40c128a4b47939a4719890e5ea09dcached = NULL;
Lambda *d46a40c128a4b47939a4719890e5ea09d() {
  Lambda *_lce8b6e32dbd3473dab03b782102e9611 = malloc(sizeof(Lambda));
  _lce8b6e32dbd3473dab03b782102e9611->fn = &lce8b6e32dbd3473dab03b782102e9611;
  ece8b6e32dbd3473dab03b782102e9611 *_ece8b6e32dbd3473dab03b782102e9611 =
      malloc(sizeof(ece8b6e32dbd3473dab03b782102e9611));
  _lce8b6e32dbd3473dab03b782102e9611->env = _ece8b6e32dbd3473dab03b782102e9611;
  if (d46a40c128a4b47939a4719890e5ea09dcached == NULL)
    d46a40c128a4b47939a4719890e5ea09dcached =
        _lce8b6e32dbd3473dab03b782102e9611;
  return d46a40c128a4b47939a4719890e5ea09dcached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} e37732bbfe1f6411e8f469cf4ede20a06;
Lambda *l37732bbfe1f6411e8f469cf4ede20a06(void *raw_env, Lambda *arg) {
  e37732bbfe1f6411e8f469cf4ede20a06 *env = raw_env;
  env->v0 = arg;
  return invoke(
      d0ce8331506aa43f8accacea259f4ac98(),
      invoke(invoke(df8c2628dcba14ab78413e848b72cb4de(), env->v1), env->v0));
}
typedef struct {
  Lambda *v0;
} e4b773e946101434993b5d7a988ae6f0a;
Lambda *l4b773e946101434993b5d7a988ae6f0a(void *raw_env, Lambda *arg) {
  e4b773e946101434993b5d7a988ae6f0a *env = raw_env;
  env->v0 = arg;
  Lambda *_l37732bbfe1f6411e8f469cf4ede20a06 = malloc(sizeof(Lambda));
  _l37732bbfe1f6411e8f469cf4ede20a06->fn = &l37732bbfe1f6411e8f469cf4ede20a06;
  e37732bbfe1f6411e8f469cf4ede20a06 *_e37732bbfe1f6411e8f469cf4ede20a06 =
      malloc(sizeof(e37732bbfe1f6411e8f469cf4ede20a06));
  _e37732bbfe1f6411e8f469cf4ede20a06->v1 = env->v0;
  _l37732bbfe1f6411e8f469cf4ede20a06->env = _e37732bbfe1f6411e8f469cf4ede20a06;
  return _l37732bbfe1f6411e8f469cf4ede20a06;
}
// gts
Lambda *d03d2c36ffecb4a55869f1a74bca2ca42cached = NULL;
Lambda *d03d2c36ffecb4a55869f1a74bca2ca42() {
  Lambda *_l4b773e946101434993b5d7a988ae6f0a = malloc(sizeof(Lambda));
  _l4b773e946101434993b5d7a988ae6f0a->fn = &l4b773e946101434993b5d7a988ae6f0a;
  e4b773e946101434993b5d7a988ae6f0a *_e4b773e946101434993b5d7a988ae6f0a =
      malloc(sizeof(e4b773e946101434993b5d7a988ae6f0a));
  _l4b773e946101434993b5d7a988ae6f0a->env = _e4b773e946101434993b5d7a988ae6f0a;
  if (d03d2c36ffecb4a55869f1a74bca2ca42cached == NULL)
    d03d2c36ffecb4a55869f1a74bca2ca42cached =
        _l4b773e946101434993b5d7a988ae6f0a;
  return d03d2c36ffecb4a55869f1a74bca2ca42cached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} e02da1fc26c104deeb98013aac8caf9d7;
Lambda *l02da1fc26c104deeb98013aac8caf9d7(void *raw_env, Lambda *arg) {
  e02da1fc26c104deeb98013aac8caf9d7 *env = raw_env;
  env->v0 = arg;
  return invoke(
      invoke(dea818acabf044d0cab634e8513c2bd59(),
             invoke(invoke(d03d2c36ffecb4a55869f1a74bca2ca42(), env->v1),
                    env->v0)),
      invoke(dc1bddb81d3014253a02fad3696d3e24b(),
             invoke(invoke(d46a40c128a4b47939a4719890e5ea09d(), env->v1),
                    env->v0)));
}
typedef struct {
  Lambda *v0;
} ec1eda636d9124fd59520236e8e329f16;
Lambda *lc1eda636d9124fd59520236e8e329f16(void *raw_env, Lambda *arg) {
  ec1eda636d9124fd59520236e8e329f16 *env = raw_env;
  env->v0 = arg;
  Lambda *_l02da1fc26c104deeb98013aac8caf9d7 = malloc(sizeof(Lambda));
  _l02da1fc26c104deeb98013aac8caf9d7->fn = &l02da1fc26c104deeb98013aac8caf9d7;
  e02da1fc26c104deeb98013aac8caf9d7 *_e02da1fc26c104deeb98013aac8caf9d7 =
      malloc(sizeof(e02da1fc26c104deeb98013aac8caf9d7));
  _e02da1fc26c104deeb98013aac8caf9d7->v1 = env->v0;
  _l02da1fc26c104deeb98013aac8caf9d7->env = _e02da1fc26c104deeb98013aac8caf9d7;
  return _l02da1fc26c104deeb98013aac8caf9d7;
}
// geqs
Lambda *d2961a2dd59024fb2977eec0d8285f0e7cached = NULL;
Lambda *d2961a2dd59024fb2977eec0d8285f0e7() {
  Lambda *_lc1eda636d9124fd59520236e8e329f16 = malloc(sizeof(Lambda));
  _lc1eda636d9124fd59520236e8e329f16->fn = &lc1eda636d9124fd59520236e8e329f16;
  ec1eda636d9124fd59520236e8e329f16 *_ec1eda636d9124fd59520236e8e329f16 =
      malloc(sizeof(ec1eda636d9124fd59520236e8e329f16));
  _lc1eda636d9124fd59520236e8e329f16->env = _ec1eda636d9124fd59520236e8e329f16;
  if (d2961a2dd59024fb2977eec0d8285f0e7cached == NULL)
    d2961a2dd59024fb2977eec0d8285f0e7cached =
        _lc1eda636d9124fd59520236e8e329f16;
  return d2961a2dd59024fb2977eec0d8285f0e7cached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} ee48ed78e33104d208eed2ddae6cf8559;
Lambda *le48ed78e33104d208eed2ddae6cf8559(void *raw_env, Lambda *arg) {
  ee48ed78e33104d208eed2ddae6cf8559 *env = raw_env;
  env->v0 = arg;
  return invoke(
      invoke(d842d9baced154df0bd09e4286f51e40e(),
             invoke(invoke(df8c2628dcba14ab78413e848b72cb4de(), env->v1),
                    env->v0)),
      invoke(dc1bddb81d3014253a02fad3696d3e24b(),
             invoke(d0ce8331506aa43f8accacea259f4ac98(),
                    invoke(invoke(d46a40c128a4b47939a4719890e5ea09d(), env->v1),
                           env->v0))));
}
typedef struct {
  Lambda *v0;
} e1e06f85271ca43ce859616b7af6cce23;
Lambda *l1e06f85271ca43ce859616b7af6cce23(void *raw_env, Lambda *arg) {
  e1e06f85271ca43ce859616b7af6cce23 *env = raw_env;
  env->v0 = arg;
  Lambda *_le48ed78e33104d208eed2ddae6cf8559 = malloc(sizeof(Lambda));
  _le48ed78e33104d208eed2ddae6cf8559->fn = &le48ed78e33104d208eed2ddae6cf8559;
  ee48ed78e33104d208eed2ddae6cf8559 *_ee48ed78e33104d208eed2ddae6cf8559 =
      malloc(sizeof(ee48ed78e33104d208eed2ddae6cf8559));
  _ee48ed78e33104d208eed2ddae6cf8559->v1 = env->v0;
  _le48ed78e33104d208eed2ddae6cf8559->env = _ee48ed78e33104d208eed2ddae6cf8559;
  return _le48ed78e33104d208eed2ddae6cf8559;
}
// lts
Lambda *d382fc460bdd74552880142064e598408cached = NULL;
Lambda *d382fc460bdd74552880142064e598408() {
  Lambda *_l1e06f85271ca43ce859616b7af6cce23 = malloc(sizeof(Lambda));
  _l1e06f85271ca43ce859616b7af6cce23->fn = &l1e06f85271ca43ce859616b7af6cce23;
  e1e06f85271ca43ce859616b7af6cce23 *_e1e06f85271ca43ce859616b7af6cce23 =
      malloc(sizeof(e1e06f85271ca43ce859616b7af6cce23));
  _l1e06f85271ca43ce859616b7af6cce23->env = _e1e06f85271ca43ce859616b7af6cce23;
  if (d382fc460bdd74552880142064e598408cached == NULL)
    d382fc460bdd74552880142064e598408cached =
        _l1e06f85271ca43ce859616b7af6cce23;
  return d382fc460bdd74552880142064e598408cached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
  Lambda *v3;
} ea539449063294f2bb6dee478f16e6f4c;
Lambda *la539449063294f2bb6dee478f16e6f4c(void *raw_env, Lambda *arg) {
  ea539449063294f2bb6dee478f16e6f4c *env = raw_env;
  env->v0 = arg;
  return env->v2;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
  Lambda *v3;
} e69da263093d64db3b10cc411abba97d8;
Lambda *l69da263093d64db3b10cc411abba97d8(void *raw_env, Lambda *arg) {
  e69da263093d64db3b10cc411abba97d8 *env = raw_env;
  env->v0 = arg;
  return invoke(
      invoke(env->v3,
             invoke(invoke(dbfda69ba516d4fca9334c637af8cafeb(), env->v2),
                    env->v1)),
      env->v1);
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
} e057997c8dff94c5b824623a78253b668;
Lambda *l057997c8dff94c5b824623a78253b668(void *raw_env, Lambda *arg) {
  e057997c8dff94c5b824623a78253b668 *env = raw_env;
  env->v0 = arg;
  Lambda *_la539449063294f2bb6dee478f16e6f4c = malloc(sizeof(Lambda));
  _la539449063294f2bb6dee478f16e6f4c->fn = &la539449063294f2bb6dee478f16e6f4c;
  ea539449063294f2bb6dee478f16e6f4c *_ea539449063294f2bb6dee478f16e6f4c =
      malloc(sizeof(ea539449063294f2bb6dee478f16e6f4c));
  _ea539449063294f2bb6dee478f16e6f4c->v1 = env->v0;
  _ea539449063294f2bb6dee478f16e6f4c->v2 = env->v1;
  _ea539449063294f2bb6dee478f16e6f4c->v3 = env->v2;
  _la539449063294f2bb6dee478f16e6f4c->env = _ea539449063294f2bb6dee478f16e6f4c;
  Lambda *_l69da263093d64db3b10cc411abba97d8 = malloc(sizeof(Lambda));
  _l69da263093d64db3b10cc411abba97d8->fn = &l69da263093d64db3b10cc411abba97d8;
  e69da263093d64db3b10cc411abba97d8 *_e69da263093d64db3b10cc411abba97d8 =
      malloc(sizeof(e69da263093d64db3b10cc411abba97d8));
  _e69da263093d64db3b10cc411abba97d8->v1 = env->v0;
  _e69da263093d64db3b10cc411abba97d8->v2 = env->v1;
  _e69da263093d64db3b10cc411abba97d8->v3 = env->v2;
  _l69da263093d64db3b10cc411abba97d8->env = _e69da263093d64db3b10cc411abba97d8;
  return invoke(
      invoke(invoke(invoke(invoke(d382fc460bdd74552880142064e598408(), env->v1),
                           env->v0),
                    _la539449063294f2bb6dee478f16e6f4c),
             _l69da263093d64db3b10cc411abba97d8),
      scottNumeral(0));
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} eeca47e6b5fde42f2903af4e573cb93df;
Lambda *leca47e6b5fde42f2903af4e573cb93df(void *raw_env, Lambda *arg) {
  eeca47e6b5fde42f2903af4e573cb93df *env = raw_env;
  env->v0 = arg;
  Lambda *_l057997c8dff94c5b824623a78253b668 = malloc(sizeof(Lambda));
  _l057997c8dff94c5b824623a78253b668->fn = &l057997c8dff94c5b824623a78253b668;
  e057997c8dff94c5b824623a78253b668 *_e057997c8dff94c5b824623a78253b668 =
      malloc(sizeof(e057997c8dff94c5b824623a78253b668));
  _e057997c8dff94c5b824623a78253b668->v1 = env->v0;
  _e057997c8dff94c5b824623a78253b668->v2 = env->v1;
  _l057997c8dff94c5b824623a78253b668->env = _e057997c8dff94c5b824623a78253b668;
  return _l057997c8dff94c5b824623a78253b668;
}
typedef struct {
  Lambda *v0;
} eb56c1ad8648b4837ab3b3e4f81481384;
Lambda *lb56c1ad8648b4837ab3b3e4f81481384(void *raw_env, Lambda *arg) {
  eb56c1ad8648b4837ab3b3e4f81481384 *env = raw_env;
  env->v0 = arg;
  Lambda *_leca47e6b5fde42f2903af4e573cb93df = malloc(sizeof(Lambda));
  _leca47e6b5fde42f2903af4e573cb93df->fn = &leca47e6b5fde42f2903af4e573cb93df;
  eeca47e6b5fde42f2903af4e573cb93df *_eeca47e6b5fde42f2903af4e573cb93df =
      malloc(sizeof(eeca47e6b5fde42f2903af4e573cb93df));
  _eeca47e6b5fde42f2903af4e573cb93df->v1 = env->v0;
  _leca47e6b5fde42f2903af4e573cb93df->env = _eeca47e6b5fde42f2903af4e573cb93df;
  return _leca47e6b5fde42f2903af4e573cb93df;
}
// mods
Lambda *dc592046349064f87b6e8aa282ad44d89cached = NULL;
Lambda *dc592046349064f87b6e8aa282ad44d89() {
  Lambda *_lb56c1ad8648b4837ab3b3e4f81481384 = malloc(sizeof(Lambda));
  _lb56c1ad8648b4837ab3b3e4f81481384->fn = &lb56c1ad8648b4837ab3b3e4f81481384;
  eb56c1ad8648b4837ab3b3e4f81481384 *_eb56c1ad8648b4837ab3b3e4f81481384 =
      malloc(sizeof(eb56c1ad8648b4837ab3b3e4f81481384));
  _lb56c1ad8648b4837ab3b3e4f81481384->env = _eb56c1ad8648b4837ab3b3e4f81481384;
  if (dc592046349064f87b6e8aa282ad44d89cached == NULL)
    dc592046349064f87b6e8aa282ad44d89cached =
        invoke(d5e7283a86e4845b6aff9a4990c4bd70b(),
               _lb56c1ad8648b4837ab3b3e4f81481384);
  return dc592046349064f87b6e8aa282ad44d89cached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
  Lambda *v3;
} e648830b9e81c477f9dd1f7d310421df8;
Lambda *l648830b9e81c477f9dd1f7d310421df8(void *raw_env, Lambda *arg) {
  e648830b9e81c477f9dd1f7d310421df8 *env = raw_env;
  env->v0 = arg;
  return scottNumeral(0);
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
  Lambda *v3;
} ea7360aadff5342a893ed1e268d7218bc;
Lambda *la7360aadff5342a893ed1e268d7218bc(void *raw_env, Lambda *arg) {
  ea7360aadff5342a893ed1e268d7218bc *env = raw_env;
  env->v0 = arg;
  return invoke(
      dbf9642fd339242ea87f46fefe2ea20af(),
      invoke(invoke(env->v3,
                    invoke(invoke(dbfda69ba516d4fca9334c637af8cafeb(), env->v2),
                           env->v1)),
             env->v1));
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
} ec78b05670d1f4cd48be066678495f2ef;
Lambda *lc78b05670d1f4cd48be066678495f2ef(void *raw_env, Lambda *arg) {
  ec78b05670d1f4cd48be066678495f2ef *env = raw_env;
  env->v0 = arg;
  Lambda *_l648830b9e81c477f9dd1f7d310421df8 = malloc(sizeof(Lambda));
  _l648830b9e81c477f9dd1f7d310421df8->fn = &l648830b9e81c477f9dd1f7d310421df8;
  e648830b9e81c477f9dd1f7d310421df8 *_e648830b9e81c477f9dd1f7d310421df8 =
      malloc(sizeof(e648830b9e81c477f9dd1f7d310421df8));
  _e648830b9e81c477f9dd1f7d310421df8->v1 = env->v0;
  _e648830b9e81c477f9dd1f7d310421df8->v2 = env->v1;
  _e648830b9e81c477f9dd1f7d310421df8->v3 = env->v2;
  _l648830b9e81c477f9dd1f7d310421df8->env = _e648830b9e81c477f9dd1f7d310421df8;
  Lambda *_la7360aadff5342a893ed1e268d7218bc = malloc(sizeof(Lambda));
  _la7360aadff5342a893ed1e268d7218bc->fn = &la7360aadff5342a893ed1e268d7218bc;
  ea7360aadff5342a893ed1e268d7218bc *_ea7360aadff5342a893ed1e268d7218bc =
      malloc(sizeof(ea7360aadff5342a893ed1e268d7218bc));
  _ea7360aadff5342a893ed1e268d7218bc->v1 = env->v0;
  _ea7360aadff5342a893ed1e268d7218bc->v2 = env->v1;
  _ea7360aadff5342a893ed1e268d7218bc->v3 = env->v2;
  _la7360aadff5342a893ed1e268d7218bc->env = _ea7360aadff5342a893ed1e268d7218bc;
  return invoke(
      invoke(invoke(invoke(invoke(d382fc460bdd74552880142064e598408(), env->v1),
                           env->v0),
                    _l648830b9e81c477f9dd1f7d310421df8),
             _la7360aadff5342a893ed1e268d7218bc),
      scottNumeral(0));
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} ec412e6ae46264479bf875909e22f4fb3;
Lambda *lc412e6ae46264479bf875909e22f4fb3(void *raw_env, Lambda *arg) {
  ec412e6ae46264479bf875909e22f4fb3 *env = raw_env;
  env->v0 = arg;
  Lambda *_lc78b05670d1f4cd48be066678495f2ef = malloc(sizeof(Lambda));
  _lc78b05670d1f4cd48be066678495f2ef->fn = &lc78b05670d1f4cd48be066678495f2ef;
  ec78b05670d1f4cd48be066678495f2ef *_ec78b05670d1f4cd48be066678495f2ef =
      malloc(sizeof(ec78b05670d1f4cd48be066678495f2ef));
  _ec78b05670d1f4cd48be066678495f2ef->v1 = env->v0;
  _ec78b05670d1f4cd48be066678495f2ef->v2 = env->v1;
  _lc78b05670d1f4cd48be066678495f2ef->env = _ec78b05670d1f4cd48be066678495f2ef;
  return _lc78b05670d1f4cd48be066678495f2ef;
}
typedef struct {
  Lambda *v0;
} e507809ed1bd246abac0f5f8b4fcd0a11;
Lambda *l507809ed1bd246abac0f5f8b4fcd0a11(void *raw_env, Lambda *arg) {
  e507809ed1bd246abac0f5f8b4fcd0a11 *env = raw_env;
  env->v0 = arg;
  Lambda *_lc412e6ae46264479bf875909e22f4fb3 = malloc(sizeof(Lambda));
  _lc412e6ae46264479bf875909e22f4fb3->fn = &lc412e6ae46264479bf875909e22f4fb3;
  ec412e6ae46264479bf875909e22f4fb3 *_ec412e6ae46264479bf875909e22f4fb3 =
      malloc(sizeof(ec412e6ae46264479bf875909e22f4fb3));
  _ec412e6ae46264479bf875909e22f4fb3->v1 = env->v0;
  _lc412e6ae46264479bf875909e22f4fb3->env = _ec412e6ae46264479bf875909e22f4fb3;
  return _lc412e6ae46264479bf875909e22f4fb3;
}
// divs
Lambda *dbb0d090f95a640339f32ddf768fb7fd1cached = NULL;
Lambda *dbb0d090f95a640339f32ddf768fb7fd1() {
  Lambda *_l507809ed1bd246abac0f5f8b4fcd0a11 = malloc(sizeof(Lambda));
  _l507809ed1bd246abac0f5f8b4fcd0a11->fn = &l507809ed1bd246abac0f5f8b4fcd0a11;
  e507809ed1bd246abac0f5f8b4fcd0a11 *_e507809ed1bd246abac0f5f8b4fcd0a11 =
      malloc(sizeof(e507809ed1bd246abac0f5f8b4fcd0a11));
  _l507809ed1bd246abac0f5f8b4fcd0a11->env = _e507809ed1bd246abac0f5f8b4fcd0a11;
  if (dbb0d090f95a640339f32ddf768fb7fd1cached == NULL)
    dbb0d090f95a640339f32ddf768fb7fd1cached =
        invoke(d5e7283a86e4845b6aff9a4990c4bd70b(),
               _l507809ed1bd246abac0f5f8b4fcd0a11);
  return dbb0d090f95a640339f32ddf768fb7fd1cached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
} edda1ee90717840eaa899c63be90ee4f6;
Lambda *ldda1ee90717840eaa899c63be90ee4f6(void *raw_env, Lambda *arg) {
  edda1ee90717840eaa899c63be90ee4f6 *env = raw_env;
  env->v0 = arg;
  return invoke(env->v1, invoke(invoke(env->v2, env->v1), env->v0));
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} e953b0943ce52434cbaad0a5f78296262;
Lambda *l953b0943ce52434cbaad0a5f78296262(void *raw_env, Lambda *arg) {
  e953b0943ce52434cbaad0a5f78296262 *env = raw_env;
  env->v0 = arg;
  Lambda *_ldda1ee90717840eaa899c63be90ee4f6 = malloc(sizeof(Lambda));
  _ldda1ee90717840eaa899c63be90ee4f6->fn = &ldda1ee90717840eaa899c63be90ee4f6;
  edda1ee90717840eaa899c63be90ee4f6 *_edda1ee90717840eaa899c63be90ee4f6 =
      malloc(sizeof(edda1ee90717840eaa899c63be90ee4f6));
  _edda1ee90717840eaa899c63be90ee4f6->v1 = env->v0;
  _edda1ee90717840eaa899c63be90ee4f6->v2 = env->v1;
  _ldda1ee90717840eaa899c63be90ee4f6->env = _edda1ee90717840eaa899c63be90ee4f6;
  return _ldda1ee90717840eaa899c63be90ee4f6;
}
typedef struct {
  Lambda *v0;
} ecfb091b4ff9d4100ae9895b79cf6e3b8;
Lambda *lcfb091b4ff9d4100ae9895b79cf6e3b8(void *raw_env, Lambda *arg) {
  ecfb091b4ff9d4100ae9895b79cf6e3b8 *env = raw_env;
  env->v0 = arg;
  Lambda *_l953b0943ce52434cbaad0a5f78296262 = malloc(sizeof(Lambda));
  _l953b0943ce52434cbaad0a5f78296262->fn = &l953b0943ce52434cbaad0a5f78296262;
  e953b0943ce52434cbaad0a5f78296262 *_e953b0943ce52434cbaad0a5f78296262 =
      malloc(sizeof(e953b0943ce52434cbaad0a5f78296262));
  _e953b0943ce52434cbaad0a5f78296262->v1 = env->v0;
  _l953b0943ce52434cbaad0a5f78296262->env = _e953b0943ce52434cbaad0a5f78296262;
  return _l953b0943ce52434cbaad0a5f78296262;
}
// Sc
Lambda *df0d6fc945f0c427ca9eb96e00fdfe585cached = NULL;
Lambda *df0d6fc945f0c427ca9eb96e00fdfe585() {
  Lambda *_lcfb091b4ff9d4100ae9895b79cf6e3b8 = malloc(sizeof(Lambda));
  _lcfb091b4ff9d4100ae9895b79cf6e3b8->fn = &lcfb091b4ff9d4100ae9895b79cf6e3b8;
  ecfb091b4ff9d4100ae9895b79cf6e3b8 *_ecfb091b4ff9d4100ae9895b79cf6e3b8 =
      malloc(sizeof(ecfb091b4ff9d4100ae9895b79cf6e3b8));
  _lcfb091b4ff9d4100ae9895b79cf6e3b8->env = _ecfb091b4ff9d4100ae9895b79cf6e3b8;
  if (df0d6fc945f0c427ca9eb96e00fdfe585cached == NULL)
    df0d6fc945f0c427ca9eb96e00fdfe585cached =
        _lcfb091b4ff9d4100ae9895b79cf6e3b8;
  return df0d6fc945f0c427ca9eb96e00fdfe585cached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
  Lambda *v3;
  Lambda *v4;
} e1cad232abbb949bab3e9b9b6c012b67c;
Lambda *l1cad232abbb949bab3e9b9b6c012b67c(void *raw_env, Lambda *arg) {
  e1cad232abbb949bab3e9b9b6c012b67c *env = raw_env;
  env->v0 = arg;
  return invoke(env->v0, invoke(env->v1, env->v3));
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
  Lambda *v3;
} e73ccbb6eddd24a828160dbe10ea65e87;
Lambda *l73ccbb6eddd24a828160dbe10ea65e87(void *raw_env, Lambda *arg) {
  e73ccbb6eddd24a828160dbe10ea65e87 *env = raw_env;
  env->v0 = arg;
  Lambda *_l1cad232abbb949bab3e9b9b6c012b67c = malloc(sizeof(Lambda));
  _l1cad232abbb949bab3e9b9b6c012b67c->fn = &l1cad232abbb949bab3e9b9b6c012b67c;
  e1cad232abbb949bab3e9b9b6c012b67c *_e1cad232abbb949bab3e9b9b6c012b67c =
      malloc(sizeof(e1cad232abbb949bab3e9b9b6c012b67c));
  _e1cad232abbb949bab3e9b9b6c012b67c->v1 = env->v0;
  _e1cad232abbb949bab3e9b9b6c012b67c->v2 = env->v1;
  _e1cad232abbb949bab3e9b9b6c012b67c->v3 = env->v2;
  _e1cad232abbb949bab3e9b9b6c012b67c->v4 = env->v3;
  _l1cad232abbb949bab3e9b9b6c012b67c->env = _e1cad232abbb949bab3e9b9b6c012b67c;
  return _l1cad232abbb949bab3e9b9b6c012b67c;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
  Lambda *v3;
} e72f4061a33dc4f5a820f79df8aac049a;
Lambda *l72f4061a33dc4f5a820f79df8aac049a(void *raw_env, Lambda *arg) {
  e72f4061a33dc4f5a820f79df8aac049a *env = raw_env;
  env->v0 = arg;
  return env->v1;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
  Lambda *v3;
} e5329f592fdfc47fe9744c8eca7b9f47d;
Lambda *l5329f592fdfc47fe9744c8eca7b9f47d(void *raw_env, Lambda *arg) {
  e5329f592fdfc47fe9744c8eca7b9f47d *env = raw_env;
  env->v0 = arg;
  return env->v0;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
} e2ee57324854047fc86fad717ae75307e;
Lambda *l2ee57324854047fc86fad717ae75307e(void *raw_env, Lambda *arg) {
  e2ee57324854047fc86fad717ae75307e *env = raw_env;
  env->v0 = arg;
  Lambda *_l73ccbb6eddd24a828160dbe10ea65e87 = malloc(sizeof(Lambda));
  _l73ccbb6eddd24a828160dbe10ea65e87->fn = &l73ccbb6eddd24a828160dbe10ea65e87;
  e73ccbb6eddd24a828160dbe10ea65e87 *_e73ccbb6eddd24a828160dbe10ea65e87 =
      malloc(sizeof(e73ccbb6eddd24a828160dbe10ea65e87));
  _e73ccbb6eddd24a828160dbe10ea65e87->v1 = env->v0;
  _e73ccbb6eddd24a828160dbe10ea65e87->v2 = env->v1;
  _e73ccbb6eddd24a828160dbe10ea65e87->v3 = env->v2;
  _l73ccbb6eddd24a828160dbe10ea65e87->env = _e73ccbb6eddd24a828160dbe10ea65e87;
  Lambda *_l72f4061a33dc4f5a820f79df8aac049a = malloc(sizeof(Lambda));
  _l72f4061a33dc4f5a820f79df8aac049a->fn = &l72f4061a33dc4f5a820f79df8aac049a;
  e72f4061a33dc4f5a820f79df8aac049a *_e72f4061a33dc4f5a820f79df8aac049a =
      malloc(sizeof(e72f4061a33dc4f5a820f79df8aac049a));
  _e72f4061a33dc4f5a820f79df8aac049a->v1 = env->v0;
  _e72f4061a33dc4f5a820f79df8aac049a->v2 = env->v1;
  _e72f4061a33dc4f5a820f79df8aac049a->v3 = env->v2;
  _l72f4061a33dc4f5a820f79df8aac049a->env = _e72f4061a33dc4f5a820f79df8aac049a;
  Lambda *_l5329f592fdfc47fe9744c8eca7b9f47d = malloc(sizeof(Lambda));
  _l5329f592fdfc47fe9744c8eca7b9f47d->fn = &l5329f592fdfc47fe9744c8eca7b9f47d;
  e5329f592fdfc47fe9744c8eca7b9f47d *_e5329f592fdfc47fe9744c8eca7b9f47d =
      malloc(sizeof(e5329f592fdfc47fe9744c8eca7b9f47d));
  _e5329f592fdfc47fe9744c8eca7b9f47d->v1 = env->v0;
  _e5329f592fdfc47fe9744c8eca7b9f47d->v2 = env->v1;
  _e5329f592fdfc47fe9744c8eca7b9f47d->v3 = env->v2;
  _l5329f592fdfc47fe9744c8eca7b9f47d->env = _e5329f592fdfc47fe9744c8eca7b9f47d;
  return invoke(invoke(invoke(env->v2, _l73ccbb6eddd24a828160dbe10ea65e87),
                       _l72f4061a33dc4f5a820f79df8aac049a),
                _l5329f592fdfc47fe9744c8eca7b9f47d);
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} eb2b1a1dbeef94eb9a3f097e41572072f;
Lambda *lb2b1a1dbeef94eb9a3f097e41572072f(void *raw_env, Lambda *arg) {
  eb2b1a1dbeef94eb9a3f097e41572072f *env = raw_env;
  env->v0 = arg;
  Lambda *_l2ee57324854047fc86fad717ae75307e = malloc(sizeof(Lambda));
  _l2ee57324854047fc86fad717ae75307e->fn = &l2ee57324854047fc86fad717ae75307e;
  e2ee57324854047fc86fad717ae75307e *_e2ee57324854047fc86fad717ae75307e =
      malloc(sizeof(e2ee57324854047fc86fad717ae75307e));
  _e2ee57324854047fc86fad717ae75307e->v1 = env->v0;
  _e2ee57324854047fc86fad717ae75307e->v2 = env->v1;
  _l2ee57324854047fc86fad717ae75307e->env = _e2ee57324854047fc86fad717ae75307e;
  return _l2ee57324854047fc86fad717ae75307e;
}
typedef struct {
  Lambda *v0;
} e5b8073f32a8e4cec948180db41e1ae98;
Lambda *l5b8073f32a8e4cec948180db41e1ae98(void *raw_env, Lambda *arg) {
  e5b8073f32a8e4cec948180db41e1ae98 *env = raw_env;
  env->v0 = arg;
  Lambda *_lb2b1a1dbeef94eb9a3f097e41572072f = malloc(sizeof(Lambda));
  _lb2b1a1dbeef94eb9a3f097e41572072f->fn = &lb2b1a1dbeef94eb9a3f097e41572072f;
  eb2b1a1dbeef94eb9a3f097e41572072f *_eb2b1a1dbeef94eb9a3f097e41572072f =
      malloc(sizeof(eb2b1a1dbeef94eb9a3f097e41572072f));
  _eb2b1a1dbeef94eb9a3f097e41572072f->v1 = env->v0;
  _lb2b1a1dbeef94eb9a3f097e41572072f->env = _eb2b1a1dbeef94eb9a3f097e41572072f;
  return _lb2b1a1dbeef94eb9a3f097e41572072f;
}
// predc
Lambda *d86ec9267933f447d8316738bdc96e8a1cached = NULL;
Lambda *d86ec9267933f447d8316738bdc96e8a1() {
  Lambda *_l5b8073f32a8e4cec948180db41e1ae98 = malloc(sizeof(Lambda));
  _l5b8073f32a8e4cec948180db41e1ae98->fn = &l5b8073f32a8e4cec948180db41e1ae98;
  e5b8073f32a8e4cec948180db41e1ae98 *_e5b8073f32a8e4cec948180db41e1ae98 =
      malloc(sizeof(e5b8073f32a8e4cec948180db41e1ae98));
  _l5b8073f32a8e4cec948180db41e1ae98->env = _e5b8073f32a8e4cec948180db41e1ae98;
  if (d86ec9267933f447d8316738bdc96e8a1cached == NULL)
    d86ec9267933f447d8316738bdc96e8a1cached =
        _l5b8073f32a8e4cec948180db41e1ae98;
  return d86ec9267933f447d8316738bdc96e8a1cached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} ea0bae8df51b74cf5be6bb353a310cf7e;
Lambda *la0bae8df51b74cf5be6bb353a310cf7e(void *raw_env, Lambda *arg) {
  ea0bae8df51b74cf5be6bb353a310cf7e *env = raw_env;
  env->v0 = arg;
  return invoke(invoke(env->v1, df0d6fc945f0c427ca9eb96e00fdfe585()), env->v0);
}
typedef struct {
  Lambda *v0;
} ea081386716b34ac899e8aa97c5637619;
Lambda *la081386716b34ac899e8aa97c5637619(void *raw_env, Lambda *arg) {
  ea081386716b34ac899e8aa97c5637619 *env = raw_env;
  env->v0 = arg;
  Lambda *_la0bae8df51b74cf5be6bb353a310cf7e = malloc(sizeof(Lambda));
  _la0bae8df51b74cf5be6bb353a310cf7e->fn = &la0bae8df51b74cf5be6bb353a310cf7e;
  ea0bae8df51b74cf5be6bb353a310cf7e *_ea0bae8df51b74cf5be6bb353a310cf7e =
      malloc(sizeof(ea0bae8df51b74cf5be6bb353a310cf7e));
  _ea0bae8df51b74cf5be6bb353a310cf7e->v1 = env->v0;
  _la0bae8df51b74cf5be6bb353a310cf7e->env = _ea0bae8df51b74cf5be6bb353a310cf7e;
  return _la0bae8df51b74cf5be6bb353a310cf7e;
}
// addc
Lambda *d0bf2fa3671294a33ae468a227bc91cafcached = NULL;
Lambda *d0bf2fa3671294a33ae468a227bc91caf() {
  Lambda *_la081386716b34ac899e8aa97c5637619 = malloc(sizeof(Lambda));
  _la081386716b34ac899e8aa97c5637619->fn = &la081386716b34ac899e8aa97c5637619;
  ea081386716b34ac899e8aa97c5637619 *_ea081386716b34ac899e8aa97c5637619 =
      malloc(sizeof(ea081386716b34ac899e8aa97c5637619));
  _la081386716b34ac899e8aa97c5637619->env = _ea081386716b34ac899e8aa97c5637619;
  if (d0bf2fa3671294a33ae468a227bc91cafcached == NULL)
    d0bf2fa3671294a33ae468a227bc91cafcached =
        _la081386716b34ac899e8aa97c5637619;
  return d0bf2fa3671294a33ae468a227bc91cafcached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} eb09425d06bea4ff180f03f1d4d1a5ccb;
Lambda *lb09425d06bea4ff180f03f1d4d1a5ccb(void *raw_env, Lambda *arg) {
  eb09425d06bea4ff180f03f1d4d1a5ccb *env = raw_env;
  env->v0 = arg;
  return invoke(invoke(env->v1, d0bf2fa3671294a33ae468a227bc91caf()), env->v0);
}
typedef struct {
  Lambda *v0;
} eb6b7dc28563940bca748a86a316b7a2f;
Lambda *lb6b7dc28563940bca748a86a316b7a2f(void *raw_env, Lambda *arg) {
  eb6b7dc28563940bca748a86a316b7a2f *env = raw_env;
  env->v0 = arg;
  Lambda *_lb09425d06bea4ff180f03f1d4d1a5ccb = malloc(sizeof(Lambda));
  _lb09425d06bea4ff180f03f1d4d1a5ccb->fn = &lb09425d06bea4ff180f03f1d4d1a5ccb;
  eb09425d06bea4ff180f03f1d4d1a5ccb *_eb09425d06bea4ff180f03f1d4d1a5ccb =
      malloc(sizeof(eb09425d06bea4ff180f03f1d4d1a5ccb));
  _eb09425d06bea4ff180f03f1d4d1a5ccb->v1 = env->v0;
  _lb09425d06bea4ff180f03f1d4d1a5ccb->env = _eb09425d06bea4ff180f03f1d4d1a5ccb;
  return _lb09425d06bea4ff180f03f1d4d1a5ccb;
}
// multc
Lambda *d0424502dd43842628bba18e57588cd74cached = NULL;
Lambda *d0424502dd43842628bba18e57588cd74() {
  Lambda *_lb6b7dc28563940bca748a86a316b7a2f = malloc(sizeof(Lambda));
  _lb6b7dc28563940bca748a86a316b7a2f->fn = &lb6b7dc28563940bca748a86a316b7a2f;
  eb6b7dc28563940bca748a86a316b7a2f *_eb6b7dc28563940bca748a86a316b7a2f =
      malloc(sizeof(eb6b7dc28563940bca748a86a316b7a2f));
  _lb6b7dc28563940bca748a86a316b7a2f->env = _eb6b7dc28563940bca748a86a316b7a2f;
  if (d0424502dd43842628bba18e57588cd74cached == NULL)
    d0424502dd43842628bba18e57588cd74cached =
        _lb6b7dc28563940bca748a86a316b7a2f;
  return d0424502dd43842628bba18e57588cd74cached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} ef45779600ef2417bb655d25612d6e26c;
Lambda *lf45779600ef2417bb655d25612d6e26c(void *raw_env, Lambda *arg) {
  ef45779600ef2417bb655d25612d6e26c *env = raw_env;
  env->v0 = arg;
  return invoke(invoke(env->v0, d86ec9267933f447d8316738bdc96e8a1()), env->v1);
}
typedef struct {
  Lambda *v0;
} ecec2ee30959a4779878fbad227babf5b;
Lambda *lcec2ee30959a4779878fbad227babf5b(void *raw_env, Lambda *arg) {
  ecec2ee30959a4779878fbad227babf5b *env = raw_env;
  env->v0 = arg;
  Lambda *_lf45779600ef2417bb655d25612d6e26c = malloc(sizeof(Lambda));
  _lf45779600ef2417bb655d25612d6e26c->fn = &lf45779600ef2417bb655d25612d6e26c;
  ef45779600ef2417bb655d25612d6e26c *_ef45779600ef2417bb655d25612d6e26c =
      malloc(sizeof(ef45779600ef2417bb655d25612d6e26c));
  _ef45779600ef2417bb655d25612d6e26c->v1 = env->v0;
  _lf45779600ef2417bb655d25612d6e26c->env = _ef45779600ef2417bb655d25612d6e26c;
  return _lf45779600ef2417bb655d25612d6e26c;
}
// subc
Lambda *d34c071e8847c4b55a3cc92eb322ad0cccached = NULL;
Lambda *d34c071e8847c4b55a3cc92eb322ad0cc() {
  Lambda *_lcec2ee30959a4779878fbad227babf5b = malloc(sizeof(Lambda));
  _lcec2ee30959a4779878fbad227babf5b->fn = &lcec2ee30959a4779878fbad227babf5b;
  ecec2ee30959a4779878fbad227babf5b *_ecec2ee30959a4779878fbad227babf5b =
      malloc(sizeof(ecec2ee30959a4779878fbad227babf5b));
  _lcec2ee30959a4779878fbad227babf5b->env = _ecec2ee30959a4779878fbad227babf5b;
  if (d34c071e8847c4b55a3cc92eb322ad0cccached == NULL)
    d34c071e8847c4b55a3cc92eb322ad0cccached =
        _lcec2ee30959a4779878fbad227babf5b;
  return d34c071e8847c4b55a3cc92eb322ad0cccached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} ea4f64ba70c3644c282440f0182f9f905;
Lambda *la4f64ba70c3644c282440f0182f9f905(void *raw_env, Lambda *arg) {
  ea4f64ba70c3644c282440f0182f9f905 *env = raw_env;
  env->v0 = arg;
  return d000b66270a2f4a6d82641ef12fdc1ada();
}
typedef struct {
  Lambda *v0;
} e69903a8e11314104b80110f7815d2f3f;
Lambda *l69903a8e11314104b80110f7815d2f3f(void *raw_env, Lambda *arg) {
  e69903a8e11314104b80110f7815d2f3f *env = raw_env;
  env->v0 = arg;
  Lambda *_la4f64ba70c3644c282440f0182f9f905 = malloc(sizeof(Lambda));
  _la4f64ba70c3644c282440f0182f9f905->fn = &la4f64ba70c3644c282440f0182f9f905;
  ea4f64ba70c3644c282440f0182f9f905 *_ea4f64ba70c3644c282440f0182f9f905 =
      malloc(sizeof(ea4f64ba70c3644c282440f0182f9f905));
  _ea4f64ba70c3644c282440f0182f9f905->v1 = env->v0;
  _la4f64ba70c3644c282440f0182f9f905->env = _ea4f64ba70c3644c282440f0182f9f905;
  return invoke(invoke(env->v0, _la4f64ba70c3644c282440f0182f9f905),
                d0765517682aa40a7a0f40802211c37fb());
}
// isZeroc
Lambda *d8284316dea7449a19cb5eefe7a10a3f8cached = NULL;
Lambda *d8284316dea7449a19cb5eefe7a10a3f8() {
  Lambda *_l69903a8e11314104b80110f7815d2f3f = malloc(sizeof(Lambda));
  _l69903a8e11314104b80110f7815d2f3f->fn = &l69903a8e11314104b80110f7815d2f3f;
  e69903a8e11314104b80110f7815d2f3f *_e69903a8e11314104b80110f7815d2f3f =
      malloc(sizeof(e69903a8e11314104b80110f7815d2f3f));
  _l69903a8e11314104b80110f7815d2f3f->env = _e69903a8e11314104b80110f7815d2f3f;
  if (d8284316dea7449a19cb5eefe7a10a3f8cached == NULL)
    d8284316dea7449a19cb5eefe7a10a3f8cached =
        _l69903a8e11314104b80110f7815d2f3f;
  return d8284316dea7449a19cb5eefe7a10a3f8cached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} e7455a4f3cc4a4dff85e7d15c1db4bb5b;
Lambda *l7455a4f3cc4a4dff85e7d15c1db4bb5b(void *raw_env, Lambda *arg) {
  e7455a4f3cc4a4dff85e7d15c1db4bb5b *env = raw_env;
  env->v0 = arg;
  return invoke(
      d8284316dea7449a19cb5eefe7a10a3f8(),
      invoke(invoke(d34c071e8847c4b55a3cc92eb322ad0cc(), env->v1), env->v0));
}
typedef struct {
  Lambda *v0;
} e15ced0e2af1f46ca9aeda9d4acb2b9ba;
Lambda *l15ced0e2af1f46ca9aeda9d4acb2b9ba(void *raw_env, Lambda *arg) {
  e15ced0e2af1f46ca9aeda9d4acb2b9ba *env = raw_env;
  env->v0 = arg;
  Lambda *_l7455a4f3cc4a4dff85e7d15c1db4bb5b = malloc(sizeof(Lambda));
  _l7455a4f3cc4a4dff85e7d15c1db4bb5b->fn = &l7455a4f3cc4a4dff85e7d15c1db4bb5b;
  e7455a4f3cc4a4dff85e7d15c1db4bb5b *_e7455a4f3cc4a4dff85e7d15c1db4bb5b =
      malloc(sizeof(e7455a4f3cc4a4dff85e7d15c1db4bb5b));
  _e7455a4f3cc4a4dff85e7d15c1db4bb5b->v1 = env->v0;
  _l7455a4f3cc4a4dff85e7d15c1db4bb5b->env = _e7455a4f3cc4a4dff85e7d15c1db4bb5b;
  return _l7455a4f3cc4a4dff85e7d15c1db4bb5b;
}
// leqc
Lambda *d63663e3eb7374533a6a1335bbc2025c9cached = NULL;
Lambda *d63663e3eb7374533a6a1335bbc2025c9() {
  Lambda *_l15ced0e2af1f46ca9aeda9d4acb2b9ba = malloc(sizeof(Lambda));
  _l15ced0e2af1f46ca9aeda9d4acb2b9ba->fn = &l15ced0e2af1f46ca9aeda9d4acb2b9ba;
  e15ced0e2af1f46ca9aeda9d4acb2b9ba *_e15ced0e2af1f46ca9aeda9d4acb2b9ba =
      malloc(sizeof(e15ced0e2af1f46ca9aeda9d4acb2b9ba));
  _l15ced0e2af1f46ca9aeda9d4acb2b9ba->env = _e15ced0e2af1f46ca9aeda9d4acb2b9ba;
  if (d63663e3eb7374533a6a1335bbc2025c9cached == NULL)
    d63663e3eb7374533a6a1335bbc2025c9cached =
        _l15ced0e2af1f46ca9aeda9d4acb2b9ba;
  return d63663e3eb7374533a6a1335bbc2025c9cached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} e0e681c83013340289927bb57c842b0b0;
Lambda *l0e681c83013340289927bb57c842b0b0(void *raw_env, Lambda *arg) {
  e0e681c83013340289927bb57c842b0b0 *env = raw_env;
  env->v0 = arg;
  return invoke(
      invoke(d842d9baced154df0bd09e4286f51e40e(),
             invoke(invoke(d63663e3eb7374533a6a1335bbc2025c9(), env->v1),
                    env->v0)),
      invoke(invoke(d63663e3eb7374533a6a1335bbc2025c9(), env->v0), env->v1));
}
typedef struct {
  Lambda *v0;
} efcf31bdc5a554851b951abcb16c3d2fe;
Lambda *lfcf31bdc5a554851b951abcb16c3d2fe(void *raw_env, Lambda *arg) {
  efcf31bdc5a554851b951abcb16c3d2fe *env = raw_env;
  env->v0 = arg;
  Lambda *_l0e681c83013340289927bb57c842b0b0 = malloc(sizeof(Lambda));
  _l0e681c83013340289927bb57c842b0b0->fn = &l0e681c83013340289927bb57c842b0b0;
  e0e681c83013340289927bb57c842b0b0 *_e0e681c83013340289927bb57c842b0b0 =
      malloc(sizeof(e0e681c83013340289927bb57c842b0b0));
  _e0e681c83013340289927bb57c842b0b0->v1 = env->v0;
  _l0e681c83013340289927bb57c842b0b0->env = _e0e681c83013340289927bb57c842b0b0;
  return _l0e681c83013340289927bb57c842b0b0;
}
// eqc
Lambda *dc6c6534274fe4dd099ff2eba6b94544ccached = NULL;
Lambda *dc6c6534274fe4dd099ff2eba6b94544c() {
  Lambda *_lfcf31bdc5a554851b951abcb16c3d2fe = malloc(sizeof(Lambda));
  _lfcf31bdc5a554851b951abcb16c3d2fe->fn = &lfcf31bdc5a554851b951abcb16c3d2fe;
  efcf31bdc5a554851b951abcb16c3d2fe *_efcf31bdc5a554851b951abcb16c3d2fe =
      malloc(sizeof(efcf31bdc5a554851b951abcb16c3d2fe));
  _lfcf31bdc5a554851b951abcb16c3d2fe->env = _efcf31bdc5a554851b951abcb16c3d2fe;
  if (dc6c6534274fe4dd099ff2eba6b94544ccached == NULL)
    dc6c6534274fe4dd099ff2eba6b94544ccached =
        _lfcf31bdc5a554851b951abcb16c3d2fe;
  return dc6c6534274fe4dd099ff2eba6b94544ccached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} e98ad3b5ae60d442b847dfeb76bb87d83;
Lambda *l98ad3b5ae60d442b847dfeb76bb87d83(void *raw_env, Lambda *arg) {
  e98ad3b5ae60d442b847dfeb76bb87d83 *env = raw_env;
  env->v0 = arg;
  return invoke(
      d0ce8331506aa43f8accacea259f4ac98(),
      invoke(invoke(d63663e3eb7374533a6a1335bbc2025c9(), env->v1), env->v0));
}
typedef struct {
  Lambda *v0;
} e2b380f3931e24900beac6a549c8bdf64;
Lambda *l2b380f3931e24900beac6a549c8bdf64(void *raw_env, Lambda *arg) {
  e2b380f3931e24900beac6a549c8bdf64 *env = raw_env;
  env->v0 = arg;
  Lambda *_l98ad3b5ae60d442b847dfeb76bb87d83 = malloc(sizeof(Lambda));
  _l98ad3b5ae60d442b847dfeb76bb87d83->fn = &l98ad3b5ae60d442b847dfeb76bb87d83;
  e98ad3b5ae60d442b847dfeb76bb87d83 *_e98ad3b5ae60d442b847dfeb76bb87d83 =
      malloc(sizeof(e98ad3b5ae60d442b847dfeb76bb87d83));
  _e98ad3b5ae60d442b847dfeb76bb87d83->v1 = env->v0;
  _l98ad3b5ae60d442b847dfeb76bb87d83->env = _e98ad3b5ae60d442b847dfeb76bb87d83;
  return _l98ad3b5ae60d442b847dfeb76bb87d83;
}
// gtc
Lambda *d2b2b52edba544ea3ba7c43863cef1947cached = NULL;
Lambda *d2b2b52edba544ea3ba7c43863cef1947() {
  Lambda *_l2b380f3931e24900beac6a549c8bdf64 = malloc(sizeof(Lambda));
  _l2b380f3931e24900beac6a549c8bdf64->fn = &l2b380f3931e24900beac6a549c8bdf64;
  e2b380f3931e24900beac6a549c8bdf64 *_e2b380f3931e24900beac6a549c8bdf64 =
      malloc(sizeof(e2b380f3931e24900beac6a549c8bdf64));
  _l2b380f3931e24900beac6a549c8bdf64->env = _e2b380f3931e24900beac6a549c8bdf64;
  if (d2b2b52edba544ea3ba7c43863cef1947cached == NULL)
    d2b2b52edba544ea3ba7c43863cef1947cached =
        _l2b380f3931e24900beac6a549c8bdf64;
  return d2b2b52edba544ea3ba7c43863cef1947cached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} eadd45624d2834fb1b9645fc85a8482a7;
Lambda *ladd45624d2834fb1b9645fc85a8482a7(void *raw_env, Lambda *arg) {
  eadd45624d2834fb1b9645fc85a8482a7 *env = raw_env;
  env->v0 = arg;
  return invoke(
      invoke(dea818acabf044d0cab634e8513c2bd59(),
             invoke(invoke(d2b2b52edba544ea3ba7c43863cef1947(), env->v1),
                    env->v0)),
      invoke(invoke(dc6c6534274fe4dd099ff2eba6b94544c(), env->v1), env->v0));
}
typedef struct {
  Lambda *v0;
} ecf36c237c86946f19a0dd7d71b03ed2b;
Lambda *lcf36c237c86946f19a0dd7d71b03ed2b(void *raw_env, Lambda *arg) {
  ecf36c237c86946f19a0dd7d71b03ed2b *env = raw_env;
  env->v0 = arg;
  Lambda *_ladd45624d2834fb1b9645fc85a8482a7 = malloc(sizeof(Lambda));
  _ladd45624d2834fb1b9645fc85a8482a7->fn = &ladd45624d2834fb1b9645fc85a8482a7;
  eadd45624d2834fb1b9645fc85a8482a7 *_eadd45624d2834fb1b9645fc85a8482a7 =
      malloc(sizeof(eadd45624d2834fb1b9645fc85a8482a7));
  _eadd45624d2834fb1b9645fc85a8482a7->v1 = env->v0;
  _ladd45624d2834fb1b9645fc85a8482a7->env = _eadd45624d2834fb1b9645fc85a8482a7;
  return _ladd45624d2834fb1b9645fc85a8482a7;
}
// geqc
Lambda *da613d34528c242ca9ad659abae3aa9f4cached = NULL;
Lambda *da613d34528c242ca9ad659abae3aa9f4() {
  Lambda *_lcf36c237c86946f19a0dd7d71b03ed2b = malloc(sizeof(Lambda));
  _lcf36c237c86946f19a0dd7d71b03ed2b->fn = &lcf36c237c86946f19a0dd7d71b03ed2b;
  ecf36c237c86946f19a0dd7d71b03ed2b *_ecf36c237c86946f19a0dd7d71b03ed2b =
      malloc(sizeof(ecf36c237c86946f19a0dd7d71b03ed2b));
  _lcf36c237c86946f19a0dd7d71b03ed2b->env = _ecf36c237c86946f19a0dd7d71b03ed2b;
  if (da613d34528c242ca9ad659abae3aa9f4cached == NULL)
    da613d34528c242ca9ad659abae3aa9f4cached =
        _lcf36c237c86946f19a0dd7d71b03ed2b;
  return da613d34528c242ca9ad659abae3aa9f4cached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} e6a8461c8d5b340b38180082dbb0a2a93;
Lambda *l6a8461c8d5b340b38180082dbb0a2a93(void *raw_env, Lambda *arg) {
  e6a8461c8d5b340b38180082dbb0a2a93 *env = raw_env;
  env->v0 = arg;
  return invoke(
      invoke(d842d9baced154df0bd09e4286f51e40e(),
             invoke(invoke(d63663e3eb7374533a6a1335bbc2025c9(), env->v1),
                    env->v0)),
      invoke(d0ce8331506aa43f8accacea259f4ac98(),
             invoke(invoke(dc6c6534274fe4dd099ff2eba6b94544c(), env->v1),
                    env->v0)));
}
typedef struct {
  Lambda *v0;
} e873815feadb048808963349cb87c25eb;
Lambda *l873815feadb048808963349cb87c25eb(void *raw_env, Lambda *arg) {
  e873815feadb048808963349cb87c25eb *env = raw_env;
  env->v0 = arg;
  Lambda *_l6a8461c8d5b340b38180082dbb0a2a93 = malloc(sizeof(Lambda));
  _l6a8461c8d5b340b38180082dbb0a2a93->fn = &l6a8461c8d5b340b38180082dbb0a2a93;
  e6a8461c8d5b340b38180082dbb0a2a93 *_e6a8461c8d5b340b38180082dbb0a2a93 =
      malloc(sizeof(e6a8461c8d5b340b38180082dbb0a2a93));
  _e6a8461c8d5b340b38180082dbb0a2a93->v1 = env->v0;
  _l6a8461c8d5b340b38180082dbb0a2a93->env = _e6a8461c8d5b340b38180082dbb0a2a93;
  return _l6a8461c8d5b340b38180082dbb0a2a93;
}
// ltc
Lambda *d269055d3a92a451cb998e089bc865b89cached = NULL;
Lambda *d269055d3a92a451cb998e089bc865b89() {
  Lambda *_l873815feadb048808963349cb87c25eb = malloc(sizeof(Lambda));
  _l873815feadb048808963349cb87c25eb->fn = &l873815feadb048808963349cb87c25eb;
  e873815feadb048808963349cb87c25eb *_e873815feadb048808963349cb87c25eb =
      malloc(sizeof(e873815feadb048808963349cb87c25eb));
  _l873815feadb048808963349cb87c25eb->env = _e873815feadb048808963349cb87c25eb;
  if (d269055d3a92a451cb998e089bc865b89cached == NULL)
    d269055d3a92a451cb998e089bc865b89cached =
        _l873815feadb048808963349cb87c25eb;
  return d269055d3a92a451cb998e089bc865b89cached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} ea36f8143ace648c5bdd684780799f964;
Lambda *la36f8143ace648c5bdd684780799f964(void *raw_env, Lambda *arg) {
  ea36f8143ace648c5bdd684780799f964 *env = raw_env;
  env->v0 = arg;
  return env->v1;
}
typedef struct {
  Lambda *v0;
} e01c1a0e3660e41cc8810d181ce87943b;
Lambda *l01c1a0e3660e41cc8810d181ce87943b(void *raw_env, Lambda *arg) {
  e01c1a0e3660e41cc8810d181ce87943b *env = raw_env;
  env->v0 = arg;
  Lambda *_la36f8143ace648c5bdd684780799f964 = malloc(sizeof(Lambda));
  _la36f8143ace648c5bdd684780799f964->fn = &la36f8143ace648c5bdd684780799f964;
  ea36f8143ace648c5bdd684780799f964 *_ea36f8143ace648c5bdd684780799f964 =
      malloc(sizeof(ea36f8143ace648c5bdd684780799f964));
  _ea36f8143ace648c5bdd684780799f964->v1 = env->v0;
  _la36f8143ace648c5bdd684780799f964->env = _ea36f8143ace648c5bdd684780799f964;
  return _la36f8143ace648c5bdd684780799f964;
}
// nil
Lambda *df4a995c3cfa24389b80d91383ca0602acached = NULL;
Lambda *df4a995c3cfa24389b80d91383ca0602a() {
  Lambda *_l01c1a0e3660e41cc8810d181ce87943b = malloc(sizeof(Lambda));
  _l01c1a0e3660e41cc8810d181ce87943b->fn = &l01c1a0e3660e41cc8810d181ce87943b;
  e01c1a0e3660e41cc8810d181ce87943b *_e01c1a0e3660e41cc8810d181ce87943b =
      malloc(sizeof(e01c1a0e3660e41cc8810d181ce87943b));
  _l01c1a0e3660e41cc8810d181ce87943b->env = _e01c1a0e3660e41cc8810d181ce87943b;
  if (df4a995c3cfa24389b80d91383ca0602acached == NULL)
    df4a995c3cfa24389b80d91383ca0602acached =
        _l01c1a0e3660e41cc8810d181ce87943b;
  return df4a995c3cfa24389b80d91383ca0602acached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
  Lambda *v3;
} ebc166c1073f941268787cbbff161ec73;
Lambda *lbc166c1073f941268787cbbff161ec73(void *raw_env, Lambda *arg) {
  ebc166c1073f941268787cbbff161ec73 *env = raw_env;
  env->v0 = arg;
  return invoke(invoke(env->v0, env->v3), env->v2);
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
} ea6eeed2533944c43aa3d8a7e14aa4fc6;
Lambda *la6eeed2533944c43aa3d8a7e14aa4fc6(void *raw_env, Lambda *arg) {
  ea6eeed2533944c43aa3d8a7e14aa4fc6 *env = raw_env;
  env->v0 = arg;
  Lambda *_lbc166c1073f941268787cbbff161ec73 = malloc(sizeof(Lambda));
  _lbc166c1073f941268787cbbff161ec73->fn = &lbc166c1073f941268787cbbff161ec73;
  ebc166c1073f941268787cbbff161ec73 *_ebc166c1073f941268787cbbff161ec73 =
      malloc(sizeof(ebc166c1073f941268787cbbff161ec73));
  _ebc166c1073f941268787cbbff161ec73->v1 = env->v0;
  _ebc166c1073f941268787cbbff161ec73->v2 = env->v1;
  _ebc166c1073f941268787cbbff161ec73->v3 = env->v2;
  _lbc166c1073f941268787cbbff161ec73->env = _ebc166c1073f941268787cbbff161ec73;
  return _lbc166c1073f941268787cbbff161ec73;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} e1a07b1a147f2457c949a98de3e2104a5;
Lambda *l1a07b1a147f2457c949a98de3e2104a5(void *raw_env, Lambda *arg) {
  e1a07b1a147f2457c949a98de3e2104a5 *env = raw_env;
  env->v0 = arg;
  Lambda *_la6eeed2533944c43aa3d8a7e14aa4fc6 = malloc(sizeof(Lambda));
  _la6eeed2533944c43aa3d8a7e14aa4fc6->fn = &la6eeed2533944c43aa3d8a7e14aa4fc6;
  ea6eeed2533944c43aa3d8a7e14aa4fc6 *_ea6eeed2533944c43aa3d8a7e14aa4fc6 =
      malloc(sizeof(ea6eeed2533944c43aa3d8a7e14aa4fc6));
  _ea6eeed2533944c43aa3d8a7e14aa4fc6->v1 = env->v0;
  _ea6eeed2533944c43aa3d8a7e14aa4fc6->v2 = env->v1;
  _la6eeed2533944c43aa3d8a7e14aa4fc6->env = _ea6eeed2533944c43aa3d8a7e14aa4fc6;
  return _la6eeed2533944c43aa3d8a7e14aa4fc6;
}
typedef struct {
  Lambda *v0;
} e23aa1b04c86e4f288ea94ca7691294c5;
Lambda *l23aa1b04c86e4f288ea94ca7691294c5(void *raw_env, Lambda *arg) {
  e23aa1b04c86e4f288ea94ca7691294c5 *env = raw_env;
  env->v0 = arg;
  Lambda *_l1a07b1a147f2457c949a98de3e2104a5 = malloc(sizeof(Lambda));
  _l1a07b1a147f2457c949a98de3e2104a5->fn = &l1a07b1a147f2457c949a98de3e2104a5;
  e1a07b1a147f2457c949a98de3e2104a5 *_e1a07b1a147f2457c949a98de3e2104a5 =
      malloc(sizeof(e1a07b1a147f2457c949a98de3e2104a5));
  _e1a07b1a147f2457c949a98de3e2104a5->v1 = env->v0;
  _l1a07b1a147f2457c949a98de3e2104a5->env = _e1a07b1a147f2457c949a98de3e2104a5;
  return _l1a07b1a147f2457c949a98de3e2104a5;
}
// cons
Lambda *d033f7bee8d2f4f98899cf5563ae71826cached = NULL;
Lambda *d033f7bee8d2f4f98899cf5563ae71826() {
  Lambda *_l23aa1b04c86e4f288ea94ca7691294c5 = malloc(sizeof(Lambda));
  _l23aa1b04c86e4f288ea94ca7691294c5->fn = &l23aa1b04c86e4f288ea94ca7691294c5;
  e23aa1b04c86e4f288ea94ca7691294c5 *_e23aa1b04c86e4f288ea94ca7691294c5 =
      malloc(sizeof(e23aa1b04c86e4f288ea94ca7691294c5));
  _l23aa1b04c86e4f288ea94ca7691294c5->env = _e23aa1b04c86e4f288ea94ca7691294c5;
  if (d033f7bee8d2f4f98899cf5563ae71826cached == NULL)
    d033f7bee8d2f4f98899cf5563ae71826cached =
        _l23aa1b04c86e4f288ea94ca7691294c5;
  return d033f7bee8d2f4f98899cf5563ae71826cached;
}
// :
Lambda *df48066a732c9422bbe753225198f4c25cached = NULL;
Lambda *df48066a732c9422bbe753225198f4c25() {
  if (df48066a732c9422bbe753225198f4c25cached == NULL)
    df48066a732c9422bbe753225198f4c25cached =
        d033f7bee8d2f4f98899cf5563ae71826();
  return df48066a732c9422bbe753225198f4c25cached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
} e3044e71c0de4462cac5bb2c6b4c3c00a;
Lambda *l3044e71c0de4462cac5bb2c6b4c3c00a(void *raw_env, Lambda *arg) {
  e3044e71c0de4462cac5bb2c6b4c3c00a *env = raw_env;
  env->v0 = arg;
  return env->v1;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} ec521eeec825e405dbb457e1f8769c74b;
Lambda *lc521eeec825e405dbb457e1f8769c74b(void *raw_env, Lambda *arg) {
  ec521eeec825e405dbb457e1f8769c74b *env = raw_env;
  env->v0 = arg;
  Lambda *_l3044e71c0de4462cac5bb2c6b4c3c00a = malloc(sizeof(Lambda));
  _l3044e71c0de4462cac5bb2c6b4c3c00a->fn = &l3044e71c0de4462cac5bb2c6b4c3c00a;
  e3044e71c0de4462cac5bb2c6b4c3c00a *_e3044e71c0de4462cac5bb2c6b4c3c00a =
      malloc(sizeof(e3044e71c0de4462cac5bb2c6b4c3c00a));
  _e3044e71c0de4462cac5bb2c6b4c3c00a->v1 = env->v0;
  _e3044e71c0de4462cac5bb2c6b4c3c00a->v2 = env->v1;
  _l3044e71c0de4462cac5bb2c6b4c3c00a->env = _e3044e71c0de4462cac5bb2c6b4c3c00a;
  return _l3044e71c0de4462cac5bb2c6b4c3c00a;
}
typedef struct {
  Lambda *v0;
} ea1ba581ab13e4eb18efc7ab970607738;
Lambda *la1ba581ab13e4eb18efc7ab970607738(void *raw_env, Lambda *arg) {
  ea1ba581ab13e4eb18efc7ab970607738 *env = raw_env;
  env->v0 = arg;
  Lambda *_lc521eeec825e405dbb457e1f8769c74b = malloc(sizeof(Lambda));
  _lc521eeec825e405dbb457e1f8769c74b->fn = &lc521eeec825e405dbb457e1f8769c74b;
  ec521eeec825e405dbb457e1f8769c74b *_ec521eeec825e405dbb457e1f8769c74b =
      malloc(sizeof(ec521eeec825e405dbb457e1f8769c74b));
  _ec521eeec825e405dbb457e1f8769c74b->v1 = env->v0;
  _lc521eeec825e405dbb457e1f8769c74b->env = _ec521eeec825e405dbb457e1f8769c74b;
  return invoke(invoke(env->v0, df4a995c3cfa24389b80d91383ca0602a()),
                _lc521eeec825e405dbb457e1f8769c74b);
}
// head
Lambda *d9e02bbcb524e45b289f4afccea59ba38cached = NULL;
Lambda *d9e02bbcb524e45b289f4afccea59ba38() {
  Lambda *_la1ba581ab13e4eb18efc7ab970607738 = malloc(sizeof(Lambda));
  _la1ba581ab13e4eb18efc7ab970607738->fn = &la1ba581ab13e4eb18efc7ab970607738;
  ea1ba581ab13e4eb18efc7ab970607738 *_ea1ba581ab13e4eb18efc7ab970607738 =
      malloc(sizeof(ea1ba581ab13e4eb18efc7ab970607738));
  _la1ba581ab13e4eb18efc7ab970607738->env = _ea1ba581ab13e4eb18efc7ab970607738;
  if (d9e02bbcb524e45b289f4afccea59ba38cached == NULL)
    d9e02bbcb524e45b289f4afccea59ba38cached =
        _la1ba581ab13e4eb18efc7ab970607738;
  return d9e02bbcb524e45b289f4afccea59ba38cached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
} e552e6938a9784145bcbc9806e7da5ea3;
Lambda *l552e6938a9784145bcbc9806e7da5ea3(void *raw_env, Lambda *arg) {
  e552e6938a9784145bcbc9806e7da5ea3 *env = raw_env;
  env->v0 = arg;
  return env->v0;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} ea8348c3c49634d5cabf7cd097c36ab20;
Lambda *la8348c3c49634d5cabf7cd097c36ab20(void *raw_env, Lambda *arg) {
  ea8348c3c49634d5cabf7cd097c36ab20 *env = raw_env;
  env->v0 = arg;
  Lambda *_l552e6938a9784145bcbc9806e7da5ea3 = malloc(sizeof(Lambda));
  _l552e6938a9784145bcbc9806e7da5ea3->fn = &l552e6938a9784145bcbc9806e7da5ea3;
  e552e6938a9784145bcbc9806e7da5ea3 *_e552e6938a9784145bcbc9806e7da5ea3 =
      malloc(sizeof(e552e6938a9784145bcbc9806e7da5ea3));
  _e552e6938a9784145bcbc9806e7da5ea3->v1 = env->v0;
  _e552e6938a9784145bcbc9806e7da5ea3->v2 = env->v1;
  _l552e6938a9784145bcbc9806e7da5ea3->env = _e552e6938a9784145bcbc9806e7da5ea3;
  return _l552e6938a9784145bcbc9806e7da5ea3;
}
typedef struct {
  Lambda *v0;
} e74ff80e53db843a3bae2294cce5f67d5;
Lambda *l74ff80e53db843a3bae2294cce5f67d5(void *raw_env, Lambda *arg) {
  e74ff80e53db843a3bae2294cce5f67d5 *env = raw_env;
  env->v0 = arg;
  Lambda *_la8348c3c49634d5cabf7cd097c36ab20 = malloc(sizeof(Lambda));
  _la8348c3c49634d5cabf7cd097c36ab20->fn = &la8348c3c49634d5cabf7cd097c36ab20;
  ea8348c3c49634d5cabf7cd097c36ab20 *_ea8348c3c49634d5cabf7cd097c36ab20 =
      malloc(sizeof(ea8348c3c49634d5cabf7cd097c36ab20));
  _ea8348c3c49634d5cabf7cd097c36ab20->v1 = env->v0;
  _la8348c3c49634d5cabf7cd097c36ab20->env = _ea8348c3c49634d5cabf7cd097c36ab20;
  return invoke(invoke(env->v0, df4a995c3cfa24389b80d91383ca0602a()),
                _la8348c3c49634d5cabf7cd097c36ab20);
}
// tail
Lambda *ddf7155994d664901b243a66b9354ef02cached = NULL;
Lambda *ddf7155994d664901b243a66b9354ef02() {
  Lambda *_l74ff80e53db843a3bae2294cce5f67d5 = malloc(sizeof(Lambda));
  _l74ff80e53db843a3bae2294cce5f67d5->fn = &l74ff80e53db843a3bae2294cce5f67d5;
  e74ff80e53db843a3bae2294cce5f67d5 *_e74ff80e53db843a3bae2294cce5f67d5 =
      malloc(sizeof(e74ff80e53db843a3bae2294cce5f67d5));
  _l74ff80e53db843a3bae2294cce5f67d5->env = _e74ff80e53db843a3bae2294cce5f67d5;
  if (ddf7155994d664901b243a66b9354ef02cached == NULL)
    ddf7155994d664901b243a66b9354ef02cached =
        _l74ff80e53db843a3bae2294cce5f67d5;
  return ddf7155994d664901b243a66b9354ef02cached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
  Lambda *v3;
} efca4d4b3b384493998f6157750aecebb;
Lambda *lfca4d4b3b384493998f6157750aecebb(void *raw_env, Lambda *arg) {
  efca4d4b3b384493998f6157750aecebb *env = raw_env;
  env->v0 = arg;
  return invoke(dbf9642fd339242ea87f46fefe2ea20af(), invoke(env->v3, env->v0));
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
} ecf6b75d198cf4bb58586b9c7103be579;
Lambda *lcf6b75d198cf4bb58586b9c7103be579(void *raw_env, Lambda *arg) {
  ecf6b75d198cf4bb58586b9c7103be579 *env = raw_env;
  env->v0 = arg;
  Lambda *_lfca4d4b3b384493998f6157750aecebb = malloc(sizeof(Lambda));
  _lfca4d4b3b384493998f6157750aecebb->fn = &lfca4d4b3b384493998f6157750aecebb;
  efca4d4b3b384493998f6157750aecebb *_efca4d4b3b384493998f6157750aecebb =
      malloc(sizeof(efca4d4b3b384493998f6157750aecebb));
  _efca4d4b3b384493998f6157750aecebb->v1 = env->v0;
  _efca4d4b3b384493998f6157750aecebb->v2 = env->v1;
  _efca4d4b3b384493998f6157750aecebb->v3 = env->v2;
  _lfca4d4b3b384493998f6157750aecebb->env = _efca4d4b3b384493998f6157750aecebb;
  return _lfca4d4b3b384493998f6157750aecebb;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} e583edac73204465c8c68d92581aa4939;
Lambda *l583edac73204465c8c68d92581aa4939(void *raw_env, Lambda *arg) {
  e583edac73204465c8c68d92581aa4939 *env = raw_env;
  env->v0 = arg;
  Lambda *_lcf6b75d198cf4bb58586b9c7103be579 = malloc(sizeof(Lambda));
  _lcf6b75d198cf4bb58586b9c7103be579->fn = &lcf6b75d198cf4bb58586b9c7103be579;
  ecf6b75d198cf4bb58586b9c7103be579 *_ecf6b75d198cf4bb58586b9c7103be579 =
      malloc(sizeof(ecf6b75d198cf4bb58586b9c7103be579));
  _ecf6b75d198cf4bb58586b9c7103be579->v1 = env->v0;
  _ecf6b75d198cf4bb58586b9c7103be579->v2 = env->v1;
  _lcf6b75d198cf4bb58586b9c7103be579->env = _ecf6b75d198cf4bb58586b9c7103be579;
  return invoke(invoke(env->v0, scottNumeral(0)),
                _lcf6b75d198cf4bb58586b9c7103be579);
}
typedef struct {
  Lambda *v0;
} ef98a458bb688455b805b53995ceb83f4;
Lambda *lf98a458bb688455b805b53995ceb83f4(void *raw_env, Lambda *arg) {
  ef98a458bb688455b805b53995ceb83f4 *env = raw_env;
  env->v0 = arg;
  Lambda *_l583edac73204465c8c68d92581aa4939 = malloc(sizeof(Lambda));
  _l583edac73204465c8c68d92581aa4939->fn = &l583edac73204465c8c68d92581aa4939;
  e583edac73204465c8c68d92581aa4939 *_e583edac73204465c8c68d92581aa4939 =
      malloc(sizeof(e583edac73204465c8c68d92581aa4939));
  _e583edac73204465c8c68d92581aa4939->v1 = env->v0;
  _l583edac73204465c8c68d92581aa4939->env = _e583edac73204465c8c68d92581aa4939;
  return _l583edac73204465c8c68d92581aa4939;
}
// length'
Lambda *d07650d2fed7e498396a9278a75847f6fcached = NULL;
Lambda *d07650d2fed7e498396a9278a75847f6f() {
  Lambda *_lf98a458bb688455b805b53995ceb83f4 = malloc(sizeof(Lambda));
  _lf98a458bb688455b805b53995ceb83f4->fn = &lf98a458bb688455b805b53995ceb83f4;
  ef98a458bb688455b805b53995ceb83f4 *_ef98a458bb688455b805b53995ceb83f4 =
      malloc(sizeof(ef98a458bb688455b805b53995ceb83f4));
  _lf98a458bb688455b805b53995ceb83f4->env = _ef98a458bb688455b805b53995ceb83f4;
  if (d07650d2fed7e498396a9278a75847f6fcached == NULL)
    d07650d2fed7e498396a9278a75847f6fcached =
        _lf98a458bb688455b805b53995ceb83f4;
  return d07650d2fed7e498396a9278a75847f6fcached;
}
// length
Lambda *dd704558522674cd5a4e40228dd48c331cached = NULL;
Lambda *dd704558522674cd5a4e40228dd48c331() {
  if (dd704558522674cd5a4e40228dd48c331cached == NULL)
    dd704558522674cd5a4e40228dd48c331cached =
        invoke(d5e7283a86e4845b6aff9a4990c4bd70b(),
               d07650d2fed7e498396a9278a75847f6f());
  return dd704558522674cd5a4e40228dd48c331cached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
  Lambda *v3;
  Lambda *v4;
} e99d780e8c6f1458aab54c365b23636f5;
Lambda *l99d780e8c6f1458aab54c365b23636f5(void *raw_env, Lambda *arg) {
  e99d780e8c6f1458aab54c365b23636f5 *env = raw_env;
  env->v0 = arg;
  return invoke(invoke(d033f7bee8d2f4f98899cf5563ae71826(), env->v1),
                invoke(invoke(env->v4, env->v0), env->v2));
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
  Lambda *v3;
} eb49f985436074f8a9be7aec15e64d4f1;
Lambda *lb49f985436074f8a9be7aec15e64d4f1(void *raw_env, Lambda *arg) {
  eb49f985436074f8a9be7aec15e64d4f1 *env = raw_env;
  env->v0 = arg;
  Lambda *_l99d780e8c6f1458aab54c365b23636f5 = malloc(sizeof(Lambda));
  _l99d780e8c6f1458aab54c365b23636f5->fn = &l99d780e8c6f1458aab54c365b23636f5;
  e99d780e8c6f1458aab54c365b23636f5 *_e99d780e8c6f1458aab54c365b23636f5 =
      malloc(sizeof(e99d780e8c6f1458aab54c365b23636f5));
  _e99d780e8c6f1458aab54c365b23636f5->v1 = env->v0;
  _e99d780e8c6f1458aab54c365b23636f5->v2 = env->v1;
  _e99d780e8c6f1458aab54c365b23636f5->v3 = env->v2;
  _e99d780e8c6f1458aab54c365b23636f5->v4 = env->v3;
  _l99d780e8c6f1458aab54c365b23636f5->env = _e99d780e8c6f1458aab54c365b23636f5;
  return _l99d780e8c6f1458aab54c365b23636f5;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
} ef5835f2658094359a66d87a57f138536;
Lambda *lf5835f2658094359a66d87a57f138536(void *raw_env, Lambda *arg) {
  ef5835f2658094359a66d87a57f138536 *env = raw_env;
  env->v0 = arg;
  Lambda *_lb49f985436074f8a9be7aec15e64d4f1 = malloc(sizeof(Lambda));
  _lb49f985436074f8a9be7aec15e64d4f1->fn = &lb49f985436074f8a9be7aec15e64d4f1;
  eb49f985436074f8a9be7aec15e64d4f1 *_eb49f985436074f8a9be7aec15e64d4f1 =
      malloc(sizeof(eb49f985436074f8a9be7aec15e64d4f1));
  _eb49f985436074f8a9be7aec15e64d4f1->v1 = env->v0;
  _eb49f985436074f8a9be7aec15e64d4f1->v2 = env->v1;
  _eb49f985436074f8a9be7aec15e64d4f1->v3 = env->v2;
  _lb49f985436074f8a9be7aec15e64d4f1->env = _eb49f985436074f8a9be7aec15e64d4f1;
  return invoke(invoke(env->v1, env->v0), _lb49f985436074f8a9be7aec15e64d4f1);
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} e882ef129be7346eab531c813cab703de;
Lambda *l882ef129be7346eab531c813cab703de(void *raw_env, Lambda *arg) {
  e882ef129be7346eab531c813cab703de *env = raw_env;
  env->v0 = arg;
  Lambda *_lf5835f2658094359a66d87a57f138536 = malloc(sizeof(Lambda));
  _lf5835f2658094359a66d87a57f138536->fn = &lf5835f2658094359a66d87a57f138536;
  ef5835f2658094359a66d87a57f138536 *_ef5835f2658094359a66d87a57f138536 =
      malloc(sizeof(ef5835f2658094359a66d87a57f138536));
  _ef5835f2658094359a66d87a57f138536->v1 = env->v0;
  _ef5835f2658094359a66d87a57f138536->v2 = env->v1;
  _lf5835f2658094359a66d87a57f138536->env = _ef5835f2658094359a66d87a57f138536;
  return _lf5835f2658094359a66d87a57f138536;
}
typedef struct {
  Lambda *v0;
} e3d6d3551d6c1477382392d8df9154503;
Lambda *l3d6d3551d6c1477382392d8df9154503(void *raw_env, Lambda *arg) {
  e3d6d3551d6c1477382392d8df9154503 *env = raw_env;
  env->v0 = arg;
  Lambda *_l882ef129be7346eab531c813cab703de = malloc(sizeof(Lambda));
  _l882ef129be7346eab531c813cab703de->fn = &l882ef129be7346eab531c813cab703de;
  e882ef129be7346eab531c813cab703de *_e882ef129be7346eab531c813cab703de =
      malloc(sizeof(e882ef129be7346eab531c813cab703de));
  _e882ef129be7346eab531c813cab703de->v1 = env->v0;
  _l882ef129be7346eab531c813cab703de->env = _e882ef129be7346eab531c813cab703de;
  return _l882ef129be7346eab531c813cab703de;
}
// append'
Lambda *d41d0c473f062455791c5b8fbad3bb10fcached = NULL;
Lambda *d41d0c473f062455791c5b8fbad3bb10f() {
  Lambda *_l3d6d3551d6c1477382392d8df9154503 = malloc(sizeof(Lambda));
  _l3d6d3551d6c1477382392d8df9154503->fn = &l3d6d3551d6c1477382392d8df9154503;
  e3d6d3551d6c1477382392d8df9154503 *_e3d6d3551d6c1477382392d8df9154503 =
      malloc(sizeof(e3d6d3551d6c1477382392d8df9154503));
  _l3d6d3551d6c1477382392d8df9154503->env = _e3d6d3551d6c1477382392d8df9154503;
  if (d41d0c473f062455791c5b8fbad3bb10fcached == NULL)
    d41d0c473f062455791c5b8fbad3bb10fcached =
        _l3d6d3551d6c1477382392d8df9154503;
  return d41d0c473f062455791c5b8fbad3bb10fcached;
}
// append
Lambda *dfdd7d0aecab0494ead0c1735e83333ddcached = NULL;
Lambda *dfdd7d0aecab0494ead0c1735e83333dd() {
  if (dfdd7d0aecab0494ead0c1735e83333ddcached == NULL)
    dfdd7d0aecab0494ead0c1735e83333ddcached =
        invoke(d5e7283a86e4845b6aff9a4990c4bd70b(),
               d41d0c473f062455791c5b8fbad3bb10f());
  return dfdd7d0aecab0494ead0c1735e83333ddcached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
  Lambda *v3;
} ed2a0990f55bb485e8b3a477896b32ef9;
Lambda *ld2a0990f55bb485e8b3a477896b32ef9(void *raw_env, Lambda *arg) {
  ed2a0990f55bb485e8b3a477896b32ef9 *env = raw_env;
  env->v0 = arg;
  return invoke(
      invoke(dfdd7d0aecab0494ead0c1735e83333dd(), invoke(env->v3, env->v0)),
      invoke(invoke(d033f7bee8d2f4f98899cf5563ae71826(), env->v1),
             df4a995c3cfa24389b80d91383ca0602a()));
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
} e33d1c91a51fe45098eb427bcc35aed1b;
Lambda *l33d1c91a51fe45098eb427bcc35aed1b(void *raw_env, Lambda *arg) {
  e33d1c91a51fe45098eb427bcc35aed1b *env = raw_env;
  env->v0 = arg;
  Lambda *_ld2a0990f55bb485e8b3a477896b32ef9 = malloc(sizeof(Lambda));
  _ld2a0990f55bb485e8b3a477896b32ef9->fn = &ld2a0990f55bb485e8b3a477896b32ef9;
  ed2a0990f55bb485e8b3a477896b32ef9 *_ed2a0990f55bb485e8b3a477896b32ef9 =
      malloc(sizeof(ed2a0990f55bb485e8b3a477896b32ef9));
  _ed2a0990f55bb485e8b3a477896b32ef9->v1 = env->v0;
  _ed2a0990f55bb485e8b3a477896b32ef9->v2 = env->v1;
  _ed2a0990f55bb485e8b3a477896b32ef9->v3 = env->v2;
  _ld2a0990f55bb485e8b3a477896b32ef9->env = _ed2a0990f55bb485e8b3a477896b32ef9;
  return _ld2a0990f55bb485e8b3a477896b32ef9;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} ea8a43c1d655b489f9072876203a57109;
Lambda *la8a43c1d655b489f9072876203a57109(void *raw_env, Lambda *arg) {
  ea8a43c1d655b489f9072876203a57109 *env = raw_env;
  env->v0 = arg;
  Lambda *_l33d1c91a51fe45098eb427bcc35aed1b = malloc(sizeof(Lambda));
  _l33d1c91a51fe45098eb427bcc35aed1b->fn = &l33d1c91a51fe45098eb427bcc35aed1b;
  e33d1c91a51fe45098eb427bcc35aed1b *_e33d1c91a51fe45098eb427bcc35aed1b =
      malloc(sizeof(e33d1c91a51fe45098eb427bcc35aed1b));
  _e33d1c91a51fe45098eb427bcc35aed1b->v1 = env->v0;
  _e33d1c91a51fe45098eb427bcc35aed1b->v2 = env->v1;
  _l33d1c91a51fe45098eb427bcc35aed1b->env = _e33d1c91a51fe45098eb427bcc35aed1b;
  return invoke(invoke(env->v0, df4a995c3cfa24389b80d91383ca0602a()),
                _l33d1c91a51fe45098eb427bcc35aed1b);
}
typedef struct {
  Lambda *v0;
} eb6e5319fd2d941ada02cca2e32cc3d5d;
Lambda *lb6e5319fd2d941ada02cca2e32cc3d5d(void *raw_env, Lambda *arg) {
  eb6e5319fd2d941ada02cca2e32cc3d5d *env = raw_env;
  env->v0 = arg;
  Lambda *_la8a43c1d655b489f9072876203a57109 = malloc(sizeof(Lambda));
  _la8a43c1d655b489f9072876203a57109->fn = &la8a43c1d655b489f9072876203a57109;
  ea8a43c1d655b489f9072876203a57109 *_ea8a43c1d655b489f9072876203a57109 =
      malloc(sizeof(ea8a43c1d655b489f9072876203a57109));
  _ea8a43c1d655b489f9072876203a57109->v1 = env->v0;
  _la8a43c1d655b489f9072876203a57109->env = _ea8a43c1d655b489f9072876203a57109;
  return _la8a43c1d655b489f9072876203a57109;
}
// reverse'
Lambda *df0fc2f8efc8c4bbe9ceb9e7f3826de22cached = NULL;
Lambda *df0fc2f8efc8c4bbe9ceb9e7f3826de22() {
  Lambda *_lb6e5319fd2d941ada02cca2e32cc3d5d = malloc(sizeof(Lambda));
  _lb6e5319fd2d941ada02cca2e32cc3d5d->fn = &lb6e5319fd2d941ada02cca2e32cc3d5d;
  eb6e5319fd2d941ada02cca2e32cc3d5d *_eb6e5319fd2d941ada02cca2e32cc3d5d =
      malloc(sizeof(eb6e5319fd2d941ada02cca2e32cc3d5d));
  _lb6e5319fd2d941ada02cca2e32cc3d5d->env = _eb6e5319fd2d941ada02cca2e32cc3d5d;
  if (df0fc2f8efc8c4bbe9ceb9e7f3826de22cached == NULL)
    df0fc2f8efc8c4bbe9ceb9e7f3826de22cached =
        _lb6e5319fd2d941ada02cca2e32cc3d5d;
  return df0fc2f8efc8c4bbe9ceb9e7f3826de22cached;
}
// reverse
Lambda *d370a391815fb467e8b1d4e5e5afe6542cached = NULL;
Lambda *d370a391815fb467e8b1d4e5e5afe6542() {
  if (d370a391815fb467e8b1d4e5e5afe6542cached == NULL)
    d370a391815fb467e8b1d4e5e5afe6542cached =
        invoke(d5e7283a86e4845b6aff9a4990c4bd70b(),
               df0fc2f8efc8c4bbe9ceb9e7f3826de22());
  return d370a391815fb467e8b1d4e5e5afe6542cached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
  Lambda *v3;
  Lambda *v4;
} e1f0c3c193386487f80e585cc508ba099;
Lambda *l1f0c3c193386487f80e585cc508ba099(void *raw_env, Lambda *arg) {
  e1f0c3c193386487f80e585cc508ba099 *env = raw_env;
  env->v0 = arg;
  return invoke(
      invoke(d033f7bee8d2f4f98899cf5563ae71826(), invoke(env->v3, env->v1)),
      invoke(invoke(env->v4, env->v3), env->v0));
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
  Lambda *v3;
} ee4cd806261644c38b9982605b36d49c4;
Lambda *le4cd806261644c38b9982605b36d49c4(void *raw_env, Lambda *arg) {
  ee4cd806261644c38b9982605b36d49c4 *env = raw_env;
  env->v0 = arg;
  Lambda *_l1f0c3c193386487f80e585cc508ba099 = malloc(sizeof(Lambda));
  _l1f0c3c193386487f80e585cc508ba099->fn = &l1f0c3c193386487f80e585cc508ba099;
  e1f0c3c193386487f80e585cc508ba099 *_e1f0c3c193386487f80e585cc508ba099 =
      malloc(sizeof(e1f0c3c193386487f80e585cc508ba099));
  _e1f0c3c193386487f80e585cc508ba099->v1 = env->v0;
  _e1f0c3c193386487f80e585cc508ba099->v2 = env->v1;
  _e1f0c3c193386487f80e585cc508ba099->v3 = env->v2;
  _e1f0c3c193386487f80e585cc508ba099->v4 = env->v3;
  _l1f0c3c193386487f80e585cc508ba099->env = _e1f0c3c193386487f80e585cc508ba099;
  return _l1f0c3c193386487f80e585cc508ba099;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
} e010db86837304277bf851a268f1b275a;
Lambda *l010db86837304277bf851a268f1b275a(void *raw_env, Lambda *arg) {
  e010db86837304277bf851a268f1b275a *env = raw_env;
  env->v0 = arg;
  Lambda *_le4cd806261644c38b9982605b36d49c4 = malloc(sizeof(Lambda));
  _le4cd806261644c38b9982605b36d49c4->fn = &le4cd806261644c38b9982605b36d49c4;
  ee4cd806261644c38b9982605b36d49c4 *_ee4cd806261644c38b9982605b36d49c4 =
      malloc(sizeof(ee4cd806261644c38b9982605b36d49c4));
  _ee4cd806261644c38b9982605b36d49c4->v1 = env->v0;
  _ee4cd806261644c38b9982605b36d49c4->v2 = env->v1;
  _ee4cd806261644c38b9982605b36d49c4->v3 = env->v2;
  _le4cd806261644c38b9982605b36d49c4->env = _ee4cd806261644c38b9982605b36d49c4;
  return invoke(invoke(env->v0, df4a995c3cfa24389b80d91383ca0602a()),
                _le4cd806261644c38b9982605b36d49c4);
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} e37d730c140814c6bb1123925d66d5889;
Lambda *l37d730c140814c6bb1123925d66d5889(void *raw_env, Lambda *arg) {
  e37d730c140814c6bb1123925d66d5889 *env = raw_env;
  env->v0 = arg;
  Lambda *_l010db86837304277bf851a268f1b275a = malloc(sizeof(Lambda));
  _l010db86837304277bf851a268f1b275a->fn = &l010db86837304277bf851a268f1b275a;
  e010db86837304277bf851a268f1b275a *_e010db86837304277bf851a268f1b275a =
      malloc(sizeof(e010db86837304277bf851a268f1b275a));
  _e010db86837304277bf851a268f1b275a->v1 = env->v0;
  _e010db86837304277bf851a268f1b275a->v2 = env->v1;
  _l010db86837304277bf851a268f1b275a->env = _e010db86837304277bf851a268f1b275a;
  return _l010db86837304277bf851a268f1b275a;
}
typedef struct {
  Lambda *v0;
} edc1563b79580447e9052ece9e21f8da8;
Lambda *ldc1563b79580447e9052ece9e21f8da8(void *raw_env, Lambda *arg) {
  edc1563b79580447e9052ece9e21f8da8 *env = raw_env;
  env->v0 = arg;
  Lambda *_l37d730c140814c6bb1123925d66d5889 = malloc(sizeof(Lambda));
  _l37d730c140814c6bb1123925d66d5889->fn = &l37d730c140814c6bb1123925d66d5889;
  e37d730c140814c6bb1123925d66d5889 *_e37d730c140814c6bb1123925d66d5889 =
      malloc(sizeof(e37d730c140814c6bb1123925d66d5889));
  _e37d730c140814c6bb1123925d66d5889->v1 = env->v0;
  _l37d730c140814c6bb1123925d66d5889->env = _e37d730c140814c6bb1123925d66d5889;
  return _l37d730c140814c6bb1123925d66d5889;
}
// map'
Lambda *d7ce482884d3c4db792658e4dbc2840e5cached = NULL;
Lambda *d7ce482884d3c4db792658e4dbc2840e5() {
  Lambda *_ldc1563b79580447e9052ece9e21f8da8 = malloc(sizeof(Lambda));
  _ldc1563b79580447e9052ece9e21f8da8->fn = &ldc1563b79580447e9052ece9e21f8da8;
  edc1563b79580447e9052ece9e21f8da8 *_edc1563b79580447e9052ece9e21f8da8 =
      malloc(sizeof(edc1563b79580447e9052ece9e21f8da8));
  _ldc1563b79580447e9052ece9e21f8da8->env = _edc1563b79580447e9052ece9e21f8da8;
  if (d7ce482884d3c4db792658e4dbc2840e5cached == NULL)
    d7ce482884d3c4db792658e4dbc2840e5cached =
        _ldc1563b79580447e9052ece9e21f8da8;
  return d7ce482884d3c4db792658e4dbc2840e5cached;
}
// map
Lambda *de236a7b4c4874c90808003039f31b08bcached = NULL;
Lambda *de236a7b4c4874c90808003039f31b08b() {
  if (de236a7b4c4874c90808003039f31b08bcached == NULL)
    de236a7b4c4874c90808003039f31b08bcached =
        invoke(d5e7283a86e4845b6aff9a4990c4bd70b(),
               d7ce482884d3c4db792658e4dbc2840e5());
  return de236a7b4c4874c90808003039f31b08bcached;
}
typedef struct {
  Lambda *v0;
} eb0da4490b1b64e3b9782da4043f1f7be;
Lambda *lb0da4490b1b64e3b9782da4043f1f7be(void *raw_env, Lambda *arg) {
  eb0da4490b1b64e3b9782da4043f1f7be *env = raw_env;
  env->v0 = arg;
  return invoke(invoke(env->v0, dbf9642fd339242ea87f46fefe2ea20af()),
                scottNumeral(0));
}
// churchToScott
Lambda *d1cb3ea153b3041d3b021688d0afe5d81cached = NULL;
Lambda *d1cb3ea153b3041d3b021688d0afe5d81() {
  Lambda *_lb0da4490b1b64e3b9782da4043f1f7be = malloc(sizeof(Lambda));
  _lb0da4490b1b64e3b9782da4043f1f7be->fn = &lb0da4490b1b64e3b9782da4043f1f7be;
  eb0da4490b1b64e3b9782da4043f1f7be *_eb0da4490b1b64e3b9782da4043f1f7be =
      malloc(sizeof(eb0da4490b1b64e3b9782da4043f1f7be));
  _lb0da4490b1b64e3b9782da4043f1f7be->env = _eb0da4490b1b64e3b9782da4043f1f7be;
  if (d1cb3ea153b3041d3b021688d0afe5d81cached == NULL)
    d1cb3ea153b3041d3b021688d0afe5d81cached =
        _lb0da4490b1b64e3b9782da4043f1f7be;
  return d1cb3ea153b3041d3b021688d0afe5d81cached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
  Lambda *v3;
  Lambda *v4;
} e4f30ca03ceb84021b7a04330b133c44e;
Lambda *l4f30ca03ceb84021b7a04330b133c44e(void *raw_env, Lambda *arg) {
  e4f30ca03ceb84021b7a04330b133c44e *env = raw_env;
  env->v0 = arg;
  return invoke(env->v2,
                invoke(invoke(invoke(env->v4, env->v0), env->v2), env->v1));
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
  Lambda *v3;
} ede4a1d0da72f409ea29042e8b49b10a2;
Lambda *lde4a1d0da72f409ea29042e8b49b10a2(void *raw_env, Lambda *arg) {
  ede4a1d0da72f409ea29042e8b49b10a2 *env = raw_env;
  env->v0 = arg;
  Lambda *_l4f30ca03ceb84021b7a04330b133c44e = malloc(sizeof(Lambda));
  _l4f30ca03ceb84021b7a04330b133c44e->fn = &l4f30ca03ceb84021b7a04330b133c44e;
  e4f30ca03ceb84021b7a04330b133c44e *_e4f30ca03ceb84021b7a04330b133c44e =
      malloc(sizeof(e4f30ca03ceb84021b7a04330b133c44e));
  _e4f30ca03ceb84021b7a04330b133c44e->v1 = env->v0;
  _e4f30ca03ceb84021b7a04330b133c44e->v2 = env->v1;
  _e4f30ca03ceb84021b7a04330b133c44e->v3 = env->v2;
  _e4f30ca03ceb84021b7a04330b133c44e->v4 = env->v3;
  _l4f30ca03ceb84021b7a04330b133c44e->env = _e4f30ca03ceb84021b7a04330b133c44e;
  return invoke(invoke(env->v2, env->v0), _l4f30ca03ceb84021b7a04330b133c44e);
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
} e996d56723a02429889bfc268d86a5a47;
Lambda *l996d56723a02429889bfc268d86a5a47(void *raw_env, Lambda *arg) {
  e996d56723a02429889bfc268d86a5a47 *env = raw_env;
  env->v0 = arg;
  Lambda *_lde4a1d0da72f409ea29042e8b49b10a2 = malloc(sizeof(Lambda));
  _lde4a1d0da72f409ea29042e8b49b10a2->fn = &lde4a1d0da72f409ea29042e8b49b10a2;
  ede4a1d0da72f409ea29042e8b49b10a2 *_ede4a1d0da72f409ea29042e8b49b10a2 =
      malloc(sizeof(ede4a1d0da72f409ea29042e8b49b10a2));
  _ede4a1d0da72f409ea29042e8b49b10a2->v1 = env->v0;
  _ede4a1d0da72f409ea29042e8b49b10a2->v2 = env->v1;
  _ede4a1d0da72f409ea29042e8b49b10a2->v3 = env->v2;
  _lde4a1d0da72f409ea29042e8b49b10a2->env = _ede4a1d0da72f409ea29042e8b49b10a2;
  return _lde4a1d0da72f409ea29042e8b49b10a2;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} eb12f6b52c6cc4f10b12be2f8afc50b20;
Lambda *lb12f6b52c6cc4f10b12be2f8afc50b20(void *raw_env, Lambda *arg) {
  eb12f6b52c6cc4f10b12be2f8afc50b20 *env = raw_env;
  env->v0 = arg;
  Lambda *_l996d56723a02429889bfc268d86a5a47 = malloc(sizeof(Lambda));
  _l996d56723a02429889bfc268d86a5a47->fn = &l996d56723a02429889bfc268d86a5a47;
  e996d56723a02429889bfc268d86a5a47 *_e996d56723a02429889bfc268d86a5a47 =
      malloc(sizeof(e996d56723a02429889bfc268d86a5a47));
  _e996d56723a02429889bfc268d86a5a47->v1 = env->v0;
  _e996d56723a02429889bfc268d86a5a47->v2 = env->v1;
  _l996d56723a02429889bfc268d86a5a47->env = _e996d56723a02429889bfc268d86a5a47;
  return _l996d56723a02429889bfc268d86a5a47;
}
typedef struct {
  Lambda *v0;
} ecdf6db96e5a948b1912b1f31f18dc656;
Lambda *lcdf6db96e5a948b1912b1f31f18dc656(void *raw_env, Lambda *arg) {
  ecdf6db96e5a948b1912b1f31f18dc656 *env = raw_env;
  env->v0 = arg;
  Lambda *_lb12f6b52c6cc4f10b12be2f8afc50b20 = malloc(sizeof(Lambda));
  _lb12f6b52c6cc4f10b12be2f8afc50b20->fn = &lb12f6b52c6cc4f10b12be2f8afc50b20;
  eb12f6b52c6cc4f10b12be2f8afc50b20 *_eb12f6b52c6cc4f10b12be2f8afc50b20 =
      malloc(sizeof(eb12f6b52c6cc4f10b12be2f8afc50b20));
  _eb12f6b52c6cc4f10b12be2f8afc50b20->v1 = env->v0;
  _lb12f6b52c6cc4f10b12be2f8afc50b20->env = _eb12f6b52c6cc4f10b12be2f8afc50b20;
  return _lb12f6b52c6cc4f10b12be2f8afc50b20;
}
// scottToChurch'
Lambda *d8a54a478898b426e969ad60c877e0b7fcached = NULL;
Lambda *d8a54a478898b426e969ad60c877e0b7f() {
  Lambda *_lcdf6db96e5a948b1912b1f31f18dc656 = malloc(sizeof(Lambda));
  _lcdf6db96e5a948b1912b1f31f18dc656->fn = &lcdf6db96e5a948b1912b1f31f18dc656;
  ecdf6db96e5a948b1912b1f31f18dc656 *_ecdf6db96e5a948b1912b1f31f18dc656 =
      malloc(sizeof(ecdf6db96e5a948b1912b1f31f18dc656));
  _lcdf6db96e5a948b1912b1f31f18dc656->env = _ecdf6db96e5a948b1912b1f31f18dc656;
  if (d8a54a478898b426e969ad60c877e0b7fcached == NULL)
    d8a54a478898b426e969ad60c877e0b7fcached =
        _lcdf6db96e5a948b1912b1f31f18dc656;
  return d8a54a478898b426e969ad60c877e0b7fcached;
}
// scottToChurch
Lambda *d1c57f8caa7bf4cbd808ebfe14da4dadfcached = NULL;
Lambda *d1c57f8caa7bf4cbd808ebfe14da4dadf() {
  if (d1c57f8caa7bf4cbd808ebfe14da4dadfcached == NULL)
    d1c57f8caa7bf4cbd808ebfe14da4dadfcached =
        invoke(d5e7283a86e4845b6aff9a4990c4bd70b(),
               d8a54a478898b426e969ad60c877e0b7f());
  return d1c57f8caa7bf4cbd808ebfe14da4dadfcached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
  Lambda *v2;
} eee44a77fdde346c0844224b16264fbeb;
Lambda *lee44a77fdde346c0844224b16264fbeb(void *raw_env, Lambda *arg) {
  eee44a77fdde346c0844224b16264fbeb *env = raw_env;
  env->v0 = arg;
  return invoke(
      invoke(dfdd7d0aecab0494ead0c1735e83333dd(),
             invoke(env->v2,
                    invoke(invoke(dbb0d090f95a640339f32ddf768fb7fd1(), env->v1),
                           scottNumeral(10)))),
      invoke(invoke(d033f7bee8d2f4f98899cf5563ae71826(),
                    invoke(invoke(d4f6b7e65d84c44ddbbad75049f3cea15(),
                                  scottNumeral(48)),
                           invoke(invoke(dc592046349064f87b6e8aa282ad44d89(),
                                         env->v1),
                                  scottNumeral(10)))),
             df4a995c3cfa24389b80d91383ca0602a()));
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} efc7c1cb2187f459db6d5a0a8d8b674a1;
Lambda *lfc7c1cb2187f459db6d5a0a8d8b674a1(void *raw_env, Lambda *arg) {
  efc7c1cb2187f459db6d5a0a8d8b674a1 *env = raw_env;
  env->v0 = arg;
  Lambda *_lee44a77fdde346c0844224b16264fbeb = malloc(sizeof(Lambda));
  _lee44a77fdde346c0844224b16264fbeb->fn = &lee44a77fdde346c0844224b16264fbeb;
  eee44a77fdde346c0844224b16264fbeb *_eee44a77fdde346c0844224b16264fbeb =
      malloc(sizeof(eee44a77fdde346c0844224b16264fbeb));
  _eee44a77fdde346c0844224b16264fbeb->v1 = env->v0;
  _eee44a77fdde346c0844224b16264fbeb->v2 = env->v1;
  _lee44a77fdde346c0844224b16264fbeb->env = _eee44a77fdde346c0844224b16264fbeb;
  return invoke(invoke(env->v0, df4a995c3cfa24389b80d91383ca0602a()),
                _lee44a77fdde346c0844224b16264fbeb);
}
typedef struct {
  Lambda *v0;
} ec2a2d1637d36446b882c1f1d09f0ee6a;
Lambda *lc2a2d1637d36446b882c1f1d09f0ee6a(void *raw_env, Lambda *arg) {
  ec2a2d1637d36446b882c1f1d09f0ee6a *env = raw_env;
  env->v0 = arg;
  Lambda *_lfc7c1cb2187f459db6d5a0a8d8b674a1 = malloc(sizeof(Lambda));
  _lfc7c1cb2187f459db6d5a0a8d8b674a1->fn = &lfc7c1cb2187f459db6d5a0a8d8b674a1;
  efc7c1cb2187f459db6d5a0a8d8b674a1 *_efc7c1cb2187f459db6d5a0a8d8b674a1 =
      malloc(sizeof(efc7c1cb2187f459db6d5a0a8d8b674a1));
  _efc7c1cb2187f459db6d5a0a8d8b674a1->v1 = env->v0;
  _lfc7c1cb2187f459db6d5a0a8d8b674a1->env = _efc7c1cb2187f459db6d5a0a8d8b674a1;
  return _lfc7c1cb2187f459db6d5a0a8d8b674a1;
}
// scottToString'
Lambda *dec4c9206fddc44ddbe63660ecd5a1dafcached = NULL;
Lambda *dec4c9206fddc44ddbe63660ecd5a1daf() {
  Lambda *_lc2a2d1637d36446b882c1f1d09f0ee6a = malloc(sizeof(Lambda));
  _lc2a2d1637d36446b882c1f1d09f0ee6a->fn = &lc2a2d1637d36446b882c1f1d09f0ee6a;
  ec2a2d1637d36446b882c1f1d09f0ee6a *_ec2a2d1637d36446b882c1f1d09f0ee6a =
      malloc(sizeof(ec2a2d1637d36446b882c1f1d09f0ee6a));
  _lc2a2d1637d36446b882c1f1d09f0ee6a->env = _ec2a2d1637d36446b882c1f1d09f0ee6a;
  if (dec4c9206fddc44ddbe63660ecd5a1dafcached == NULL)
    dec4c9206fddc44ddbe63660ecd5a1dafcached =
        invoke(d5e7283a86e4845b6aff9a4990c4bd70b(),
               _lc2a2d1637d36446b882c1f1d09f0ee6a);
  return dec4c9206fddc44ddbe63660ecd5a1dafcached;
}
typedef struct {
  Lambda *v0;
  Lambda *v1;
} ea1acb25c4cdd4841811dc450f792e344;
Lambda *la1acb25c4cdd4841811dc450f792e344(void *raw_env, Lambda *arg) {
  ea1acb25c4cdd4841811dc450f792e344 *env = raw_env;
  env->v0 = arg;
  return invoke(dec4c9206fddc44ddbe63660ecd5a1daf(), env->v1);
}
typedef struct {
  Lambda *v0;
} e24dc4e1a2e0848ef96c96015ec711767;
Lambda *l24dc4e1a2e0848ef96c96015ec711767(void *raw_env, Lambda *arg) {
  e24dc4e1a2e0848ef96c96015ec711767 *env = raw_env;
  env->v0 = arg;
  Lambda *_la1acb25c4cdd4841811dc450f792e344 = malloc(sizeof(Lambda));
  _la1acb25c4cdd4841811dc450f792e344->fn = &la1acb25c4cdd4841811dc450f792e344;
  ea1acb25c4cdd4841811dc450f792e344 *_ea1acb25c4cdd4841811dc450f792e344 =
      malloc(sizeof(ea1acb25c4cdd4841811dc450f792e344));
  _ea1acb25c4cdd4841811dc450f792e344->v1 = env->v0;
  _la1acb25c4cdd4841811dc450f792e344->env = _ea1acb25c4cdd4841811dc450f792e344;
  return invoke(
      invoke(env->v0, invoke(invoke(d033f7bee8d2f4f98899cf5563ae71826(),
                                    scottNumeral(48)),
                             df4a995c3cfa24389b80d91383ca0602a())),
      _la1acb25c4cdd4841811dc450f792e344);
}
// scottToString
Lambda *d9459d3b461c04e2a86849cc99ab2b1c6cached = NULL;
Lambda *d9459d3b461c04e2a86849cc99ab2b1c6() {
  Lambda *_l24dc4e1a2e0848ef96c96015ec711767 = malloc(sizeof(Lambda));
  _l24dc4e1a2e0848ef96c96015ec711767->fn = &l24dc4e1a2e0848ef96c96015ec711767;
  e24dc4e1a2e0848ef96c96015ec711767 *_e24dc4e1a2e0848ef96c96015ec711767 =
      malloc(sizeof(e24dc4e1a2e0848ef96c96015ec711767));
  _l24dc4e1a2e0848ef96c96015ec711767->env = _e24dc4e1a2e0848ef96c96015ec711767;
  if (d9459d3b461c04e2a86849cc99ab2b1c6cached == NULL)
    d9459d3b461c04e2a86849cc99ab2b1c6cached =
        _l24dc4e1a2e0848ef96c96015ec711767;
  return d9459d3b461c04e2a86849cc99ab2b1c6cached;
}
int main() {
  printScottString(invoke(
      invoke(d033f7bee8d2f4f98899cf5563ae71826(), scottNumeral(72)),
      invoke(
          invoke(d033f7bee8d2f4f98899cf5563ae71826(), scottNumeral(101)),
          invoke(
              invoke(d033f7bee8d2f4f98899cf5563ae71826(), scottNumeral(108)),
              invoke(
                  invoke(d033f7bee8d2f4f98899cf5563ae71826(),
                         scottNumeral(108)),
                  invoke(
                      invoke(d033f7bee8d2f4f98899cf5563ae71826(),
                             scottNumeral(111)),
                      invoke(
                          invoke(d033f7bee8d2f4f98899cf5563ae71826(),
                                 scottNumeral(32)),
                          invoke(
                              invoke(d033f7bee8d2f4f98899cf5563ae71826(),
                                     scottNumeral(87)),
                              invoke(
                                  invoke(d033f7bee8d2f4f98899cf5563ae71826(),
                                         scottNumeral(111)),
                                  invoke(
                                      invoke(
                                          d033f7bee8d2f4f98899cf5563ae71826(),
                                          scottNumeral(114)),
                                      invoke(
                                          invoke(
                                              d033f7bee8d2f4f98899cf5563ae71826(),
                                              scottNumeral(108)),
                                          invoke(
                                              invoke(
                                                  d033f7bee8d2f4f98899cf5563ae71826(),
                                                  scottNumeral(100)),
                                              invoke(
                                                  invoke(
                                                      d033f7bee8d2f4f98899cf5563ae71826(),
                                                      scottNumeral(33)),
                                                  df4a995c3cfa24389b80d91383ca0602a())))))))))))));
}
