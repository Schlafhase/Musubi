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
typedef struct {Lambda *a;Lambda *b;} eb225df2f71e44c1a8088e82ceb4620f0;Lambda *lb225df2f71e44c1a8088e82ceb4620f0(void *raw_env, Lambda *b) {eb225df2f71e44c1a8088e82ceb4620f0 *env = raw_env;env->b = b;return env->a;}typedef struct {Lambda *a;} e9b6745e1cfd04401911e84228f71e668;Lambda *l9b6745e1cfd04401911e84228f71e668(void *raw_env, Lambda *a) {e9b6745e1cfd04401911e84228f71e668 *env = raw_env;env->a = a;Lambda *_lb225df2f71e44c1a8088e82ceb4620f0 = malloc(sizeof(Lambda));_lb225df2f71e44c1a8088e82ceb4620f0->fn = &lb225df2f71e44c1a8088e82ceb4620f0;eb225df2f71e44c1a8088e82ceb4620f0 *_eb225df2f71e44c1a8088e82ceb4620f0 = malloc(sizeof(eb225df2f71e44c1a8088e82ceb4620f0));_eb225df2f71e44c1a8088e82ceb4620f0->a = env->a;_lb225df2f71e44c1a8088e82ceb4620f0->env = _eb225df2f71e44c1a8088e82ceb4620f0;return _lb225df2f71e44c1a8088e82ceb4620f0;}Lambda *d276224b6c32b436d82d264fbab57e387() {Lambda *_l9b6745e1cfd04401911e84228f71e668 = malloc(sizeof(Lambda));_l9b6745e1cfd04401911e84228f71e668->fn = &l9b6745e1cfd04401911e84228f71e668;e9b6745e1cfd04401911e84228f71e668 *_e9b6745e1cfd04401911e84228f71e668 = malloc(sizeof(e9b6745e1cfd04401911e84228f71e668));_l9b6745e1cfd04401911e84228f71e668->env = _e9b6745e1cfd04401911e84228f71e668;return _l9b6745e1cfd04401911e84228f71e668;}typedef struct {Lambda *a;Lambda *b;} e9f64402984fa49c793c44b3786abba32;Lambda *l9f64402984fa49c793c44b3786abba32(void *raw_env, Lambda *b) {e9f64402984fa49c793c44b3786abba32 *env = raw_env;env->b = b;return env->b;}typedef struct {Lambda *a;} ecfbe1ed230a7481ba2f8f427a004de47;Lambda *lcfbe1ed230a7481ba2f8f427a004de47(void *raw_env, Lambda *a) {ecfbe1ed230a7481ba2f8f427a004de47 *env = raw_env;env->a = a;Lambda *_l9f64402984fa49c793c44b3786abba32 = malloc(sizeof(Lambda));_l9f64402984fa49c793c44b3786abba32->fn = &l9f64402984fa49c793c44b3786abba32;e9f64402984fa49c793c44b3786abba32 *_e9f64402984fa49c793c44b3786abba32 = malloc(sizeof(e9f64402984fa49c793c44b3786abba32));_e9f64402984fa49c793c44b3786abba32->a = env->a;_l9f64402984fa49c793c44b3786abba32->env = _e9f64402984fa49c793c44b3786abba32;return _l9f64402984fa49c793c44b3786abba32;}Lambda *d673faa466fdb4b68b72392afc436beff() {Lambda *_lcfbe1ed230a7481ba2f8f427a004de47 = malloc(sizeof(Lambda));_lcfbe1ed230a7481ba2f8f427a004de47->fn = &lcfbe1ed230a7481ba2f8f427a004de47;ecfbe1ed230a7481ba2f8f427a004de47 *_ecfbe1ed230a7481ba2f8f427a004de47 = malloc(sizeof(ecfbe1ed230a7481ba2f8f427a004de47));_lcfbe1ed230a7481ba2f8f427a004de47->env = _ecfbe1ed230a7481ba2f8f427a004de47;return _lcfbe1ed230a7481ba2f8f427a004de47;}typedef struct {Lambda *x;} ef7a08edff7884af79f3481a3661d5993;Lambda *lf7a08edff7884af79f3481a3661d5993(void *raw_env, Lambda *x) {ef7a08edff7884af79f3481a3661d5993 *env = raw_env;env->x = x;return env->x;}Lambda *db78ba1c68dc04c2a9e405865e7a08de6() {Lambda *_lf7a08edff7884af79f3481a3661d5993 = malloc(sizeof(Lambda));_lf7a08edff7884af79f3481a3661d5993->fn = &lf7a08edff7884af79f3481a3661d5993;ef7a08edff7884af79f3481a3661d5993 *_ef7a08edff7884af79f3481a3661d5993 = malloc(sizeof(ef7a08edff7884af79f3481a3661d5993));_lf7a08edff7884af79f3481a3661d5993->env = _ef7a08edff7884af79f3481a3661d5993;return _lf7a08edff7884af79f3481a3661d5993;}typedef struct {Lambda *a;Lambda *b;} eec39511373f54fe287af337e198cf5b9;Lambda *lec39511373f54fe287af337e198cf5b9(void *raw_env, Lambda *b) {eec39511373f54fe287af337e198cf5b9 *env = raw_env;env->b = b;return invoke(invoke(env->a, env->b), d673faa466fdb4b68b72392afc436beff());}typedef struct {Lambda *a;} eee09ac7eb4284443947e406977abe5dc;Lambda *lee09ac7eb4284443947e406977abe5dc(void *raw_env, Lambda *a) {eee09ac7eb4284443947e406977abe5dc *env = raw_env;env->a = a;Lambda *_lec39511373f54fe287af337e198cf5b9 = malloc(sizeof(Lambda));_lec39511373f54fe287af337e198cf5b9->fn = &lec39511373f54fe287af337e198cf5b9;eec39511373f54fe287af337e198cf5b9 *_eec39511373f54fe287af337e198cf5b9 = malloc(sizeof(eec39511373f54fe287af337e198cf5b9));_eec39511373f54fe287af337e198cf5b9->a = env->a;_lec39511373f54fe287af337e198cf5b9->env = _eec39511373f54fe287af337e198cf5b9;return _lec39511373f54fe287af337e198cf5b9;}Lambda *da42e68c400f64842bbaa63b40d490406() {Lambda *_lee09ac7eb4284443947e406977abe5dc = malloc(sizeof(Lambda));_lee09ac7eb4284443947e406977abe5dc->fn = &lee09ac7eb4284443947e406977abe5dc;eee09ac7eb4284443947e406977abe5dc *_eee09ac7eb4284443947e406977abe5dc = malloc(sizeof(eee09ac7eb4284443947e406977abe5dc));_lee09ac7eb4284443947e406977abe5dc->env = _eee09ac7eb4284443947e406977abe5dc;return _lee09ac7eb4284443947e406977abe5dc;}typedef struct {Lambda *a;Lambda *b;} e0a5fbf5a8fef4941b5f9877395779604;Lambda *l0a5fbf5a8fef4941b5f9877395779604(void *raw_env, Lambda *b) {e0a5fbf5a8fef4941b5f9877395779604 *env = raw_env;env->b = b;return invoke(invoke(env->a, d276224b6c32b436d82d264fbab57e387()), env->b);}typedef struct {Lambda *a;} eb60f9d342435446f889485d583877d34;Lambda *lb60f9d342435446f889485d583877d34(void *raw_env, Lambda *a) {eb60f9d342435446f889485d583877d34 *env = raw_env;env->a = a;Lambda *_l0a5fbf5a8fef4941b5f9877395779604 = malloc(sizeof(Lambda));_l0a5fbf5a8fef4941b5f9877395779604->fn = &l0a5fbf5a8fef4941b5f9877395779604;e0a5fbf5a8fef4941b5f9877395779604 *_e0a5fbf5a8fef4941b5f9877395779604 = malloc(sizeof(e0a5fbf5a8fef4941b5f9877395779604));_e0a5fbf5a8fef4941b5f9877395779604->a = env->a;_l0a5fbf5a8fef4941b5f9877395779604->env = _e0a5fbf5a8fef4941b5f9877395779604;return _l0a5fbf5a8fef4941b5f9877395779604;}Lambda *d5ac9869a9928448eba4a9db52717c9f8() {Lambda *_lb60f9d342435446f889485d583877d34 = malloc(sizeof(Lambda));_lb60f9d342435446f889485d583877d34->fn = &lb60f9d342435446f889485d583877d34;eb60f9d342435446f889485d583877d34 *_eb60f9d342435446f889485d583877d34 = malloc(sizeof(eb60f9d342435446f889485d583877d34));_lb60f9d342435446f889485d583877d34->env = _eb60f9d342435446f889485d583877d34;return _lb60f9d342435446f889485d583877d34;}typedef struct {Lambda *f;Lambda *n;Lambda *x;} e597506a0972f4f1ba2abc2acd4fb2bb8;Lambda *l597506a0972f4f1ba2abc2acd4fb2bb8(void *raw_env, Lambda *x) {e597506a0972f4f1ba2abc2acd4fb2bb8 *env = raw_env;env->x = x;return invoke(env->f, invoke(invoke(env->n, env->f), env->x));}typedef struct {Lambda *n;Lambda *f;} e4138646753694df69240f8733cd243b9;Lambda *l4138646753694df69240f8733cd243b9(void *raw_env, Lambda *f) {e4138646753694df69240f8733cd243b9 *env = raw_env;env->f = f;Lambda *_l597506a0972f4f1ba2abc2acd4fb2bb8 = malloc(sizeof(Lambda));_l597506a0972f4f1ba2abc2acd4fb2bb8->fn = &l597506a0972f4f1ba2abc2acd4fb2bb8;e597506a0972f4f1ba2abc2acd4fb2bb8 *_e597506a0972f4f1ba2abc2acd4fb2bb8 = malloc(sizeof(e597506a0972f4f1ba2abc2acd4fb2bb8));_e597506a0972f4f1ba2abc2acd4fb2bb8->f = env->f;_e597506a0972f4f1ba2abc2acd4fb2bb8->n = env->n;_l597506a0972f4f1ba2abc2acd4fb2bb8->env = _e597506a0972f4f1ba2abc2acd4fb2bb8;return _l597506a0972f4f1ba2abc2acd4fb2bb8;}typedef struct {Lambda *n;} e5051b3231f8d4f809ab43cfdc1ee4fea;Lambda *l5051b3231f8d4f809ab43cfdc1ee4fea(void *raw_env, Lambda *n) {e5051b3231f8d4f809ab43cfdc1ee4fea *env = raw_env;env->n = n;Lambda *_l4138646753694df69240f8733cd243b9 = malloc(sizeof(Lambda));_l4138646753694df69240f8733cd243b9->fn = &l4138646753694df69240f8733cd243b9;e4138646753694df69240f8733cd243b9 *_e4138646753694df69240f8733cd243b9 = malloc(sizeof(e4138646753694df69240f8733cd243b9));_e4138646753694df69240f8733cd243b9->n = env->n;_l4138646753694df69240f8733cd243b9->env = _e4138646753694df69240f8733cd243b9;return _l4138646753694df69240f8733cd243b9;}Lambda *d6a8bad4a2012417d9888faec806c44a9() {Lambda *_l5051b3231f8d4f809ab43cfdc1ee4fea = malloc(sizeof(Lambda));_l5051b3231f8d4f809ab43cfdc1ee4fea->fn = &l5051b3231f8d4f809ab43cfdc1ee4fea;e5051b3231f8d4f809ab43cfdc1ee4fea *_e5051b3231f8d4f809ab43cfdc1ee4fea = malloc(sizeof(e5051b3231f8d4f809ab43cfdc1ee4fea));_l5051b3231f8d4f809ab43cfdc1ee4fea->env = _e5051b3231f8d4f809ab43cfdc1ee4fea;return _l5051b3231f8d4f809ab43cfdc1ee4fea;}typedef struct {Lambda *g;Lambda *x;Lambda *f;Lambda *n;Lambda *h;} e9b5972790f784fb3b7006e04f368fbce;Lambda *l9b5972790f784fb3b7006e04f368fbce(void *raw_env, Lambda *h) {e9b5972790f784fb3b7006e04f368fbce *env = raw_env;env->h = h;return invoke(env->h, invoke(env->g, env->f));}typedef struct {Lambda *x;Lambda *f;Lambda *n;Lambda *g;} eda124a9db15648708377bf65cf9fac3b;Lambda *lda124a9db15648708377bf65cf9fac3b(void *raw_env, Lambda *g) {eda124a9db15648708377bf65cf9fac3b *env = raw_env;env->g = g;Lambda *_l9b5972790f784fb3b7006e04f368fbce = malloc(sizeof(Lambda));_l9b5972790f784fb3b7006e04f368fbce->fn = &l9b5972790f784fb3b7006e04f368fbce;e9b5972790f784fb3b7006e04f368fbce *_e9b5972790f784fb3b7006e04f368fbce = malloc(sizeof(e9b5972790f784fb3b7006e04f368fbce));_e9b5972790f784fb3b7006e04f368fbce->g = env->g;_e9b5972790f784fb3b7006e04f368fbce->x = env->x;_e9b5972790f784fb3b7006e04f368fbce->f = env->f;_e9b5972790f784fb3b7006e04f368fbce->n = env->n;_l9b5972790f784fb3b7006e04f368fbce->env = _e9b5972790f784fb3b7006e04f368fbce;return _l9b5972790f784fb3b7006e04f368fbce;}typedef struct {Lambda *x;Lambda *f;Lambda *n;Lambda *u;} e7126282b55bb4de381c04896dbdb55b9;Lambda *l7126282b55bb4de381c04896dbdb55b9(void *raw_env, Lambda *u) {e7126282b55bb4de381c04896dbdb55b9 *env = raw_env;env->u = u;return env->x;}typedef struct {Lambda *x;Lambda *f;Lambda *n;Lambda *u;} eb9b60afbb0334c7da146699684fb6792;Lambda *lb9b60afbb0334c7da146699684fb6792(void *raw_env, Lambda *u) {eb9b60afbb0334c7da146699684fb6792 *env = raw_env;env->u = u;return env->u;}typedef struct {Lambda *f;Lambda *n;Lambda *x;} e2956b0149c8c4eb3a209bfe10c5ce8ac;Lambda *l2956b0149c8c4eb3a209bfe10c5ce8ac(void *raw_env, Lambda *x) {e2956b0149c8c4eb3a209bfe10c5ce8ac *env = raw_env;env->x = x;Lambda *_lda124a9db15648708377bf65cf9fac3b = malloc(sizeof(Lambda));_lda124a9db15648708377bf65cf9fac3b->fn = &lda124a9db15648708377bf65cf9fac3b;eda124a9db15648708377bf65cf9fac3b *_eda124a9db15648708377bf65cf9fac3b = malloc(sizeof(eda124a9db15648708377bf65cf9fac3b));_eda124a9db15648708377bf65cf9fac3b->x = env->x;_eda124a9db15648708377bf65cf9fac3b->f = env->f;_eda124a9db15648708377bf65cf9fac3b->n = env->n;_lda124a9db15648708377bf65cf9fac3b->env = _eda124a9db15648708377bf65cf9fac3b;Lambda *_l7126282b55bb4de381c04896dbdb55b9 = malloc(sizeof(Lambda));_l7126282b55bb4de381c04896dbdb55b9->fn = &l7126282b55bb4de381c04896dbdb55b9;e7126282b55bb4de381c04896dbdb55b9 *_e7126282b55bb4de381c04896dbdb55b9 = malloc(sizeof(e7126282b55bb4de381c04896dbdb55b9));_e7126282b55bb4de381c04896dbdb55b9->x = env->x;_e7126282b55bb4de381c04896dbdb55b9->f = env->f;_e7126282b55bb4de381c04896dbdb55b9->n = env->n;_l7126282b55bb4de381c04896dbdb55b9->env = _e7126282b55bb4de381c04896dbdb55b9;Lambda *_lb9b60afbb0334c7da146699684fb6792 = malloc(sizeof(Lambda));_lb9b60afbb0334c7da146699684fb6792->fn = &lb9b60afbb0334c7da146699684fb6792;eb9b60afbb0334c7da146699684fb6792 *_eb9b60afbb0334c7da146699684fb6792 = malloc(sizeof(eb9b60afbb0334c7da146699684fb6792));_eb9b60afbb0334c7da146699684fb6792->x = env->x;_eb9b60afbb0334c7da146699684fb6792->f = env->f;_eb9b60afbb0334c7da146699684fb6792->n = env->n;_lb9b60afbb0334c7da146699684fb6792->env = _eb9b60afbb0334c7da146699684fb6792;return invoke(invoke(invoke(env->n, _lda124a9db15648708377bf65cf9fac3b), _l7126282b55bb4de381c04896dbdb55b9), _lb9b60afbb0334c7da146699684fb6792);}typedef struct {Lambda *n;Lambda *f;} e389de26c4a8541cf96041f0f55b73666;Lambda *l389de26c4a8541cf96041f0f55b73666(void *raw_env, Lambda *f) {e389de26c4a8541cf96041f0f55b73666 *env = raw_env;env->f = f;Lambda *_l2956b0149c8c4eb3a209bfe10c5ce8ac = malloc(sizeof(Lambda));_l2956b0149c8c4eb3a209bfe10c5ce8ac->fn = &l2956b0149c8c4eb3a209bfe10c5ce8ac;e2956b0149c8c4eb3a209bfe10c5ce8ac *_e2956b0149c8c4eb3a209bfe10c5ce8ac = malloc(sizeof(e2956b0149c8c4eb3a209bfe10c5ce8ac));_e2956b0149c8c4eb3a209bfe10c5ce8ac->f = env->f;_e2956b0149c8c4eb3a209bfe10c5ce8ac->n = env->n;_l2956b0149c8c4eb3a209bfe10c5ce8ac->env = _e2956b0149c8c4eb3a209bfe10c5ce8ac;return _l2956b0149c8c4eb3a209bfe10c5ce8ac;}typedef struct {Lambda *n;} ebed83ce439754896b358a4675bc0144a;Lambda *lbed83ce439754896b358a4675bc0144a(void *raw_env, Lambda *n) {ebed83ce439754896b358a4675bc0144a *env = raw_env;env->n = n;Lambda *_l389de26c4a8541cf96041f0f55b73666 = malloc(sizeof(Lambda));_l389de26c4a8541cf96041f0f55b73666->fn = &l389de26c4a8541cf96041f0f55b73666;e389de26c4a8541cf96041f0f55b73666 *_e389de26c4a8541cf96041f0f55b73666 = malloc(sizeof(e389de26c4a8541cf96041f0f55b73666));_e389de26c4a8541cf96041f0f55b73666->n = env->n;_l389de26c4a8541cf96041f0f55b73666->env = _e389de26c4a8541cf96041f0f55b73666;return _l389de26c4a8541cf96041f0f55b73666;}Lambda *d3497f90a83c946bfa52dd03d37586f7b() {Lambda *_lbed83ce439754896b358a4675bc0144a = malloc(sizeof(Lambda));_lbed83ce439754896b358a4675bc0144a->fn = &lbed83ce439754896b358a4675bc0144a;ebed83ce439754896b358a4675bc0144a *_ebed83ce439754896b358a4675bc0144a = malloc(sizeof(ebed83ce439754896b358a4675bc0144a));_lbed83ce439754896b358a4675bc0144a->env = _ebed83ce439754896b358a4675bc0144a;return _lbed83ce439754896b358a4675bc0144a;}typedef struct {Lambda *a;Lambda *b;} ec7c4916834bf439cad995f2868f82bcc;Lambda *lc7c4916834bf439cad995f2868f82bcc(void *raw_env, Lambda *b) {ec7c4916834bf439cad995f2868f82bcc *env = raw_env;env->b = b;return invoke(invoke(env->a, d6a8bad4a2012417d9888faec806c44a9()), env->b);}typedef struct {Lambda *a;} eeb90c26bbe1242a498b45fb5f6438248;Lambda *leb90c26bbe1242a498b45fb5f6438248(void *raw_env, Lambda *a) {eeb90c26bbe1242a498b45fb5f6438248 *env = raw_env;env->a = a;Lambda *_lc7c4916834bf439cad995f2868f82bcc = malloc(sizeof(Lambda));_lc7c4916834bf439cad995f2868f82bcc->fn = &lc7c4916834bf439cad995f2868f82bcc;ec7c4916834bf439cad995f2868f82bcc *_ec7c4916834bf439cad995f2868f82bcc = malloc(sizeof(ec7c4916834bf439cad995f2868f82bcc));_ec7c4916834bf439cad995f2868f82bcc->a = env->a;_lc7c4916834bf439cad995f2868f82bcc->env = _ec7c4916834bf439cad995f2868f82bcc;return _lc7c4916834bf439cad995f2868f82bcc;}Lambda *d8a6ce804e63a4311ab8d912b4757e06e() {Lambda *_leb90c26bbe1242a498b45fb5f6438248 = malloc(sizeof(Lambda));_leb90c26bbe1242a498b45fb5f6438248->fn = &leb90c26bbe1242a498b45fb5f6438248;eeb90c26bbe1242a498b45fb5f6438248 *_eeb90c26bbe1242a498b45fb5f6438248 = malloc(sizeof(eeb90c26bbe1242a498b45fb5f6438248));_leb90c26bbe1242a498b45fb5f6438248->env = _eeb90c26bbe1242a498b45fb5f6438248;return _leb90c26bbe1242a498b45fb5f6438248;}typedef struct {Lambda *a;Lambda *b;} e23d938abb4684072a00367f5accf563c;Lambda *l23d938abb4684072a00367f5accf563c(void *raw_env, Lambda *b) {e23d938abb4684072a00367f5accf563c *env = raw_env;env->b = b;return invoke(invoke(env->b, d3497f90a83c946bfa52dd03d37586f7b()), env->a);}typedef struct {Lambda *a;} ebe0dde5e7b8f45c3b3f6b9b38d7103d6;Lambda *lbe0dde5e7b8f45c3b3f6b9b38d7103d6(void *raw_env, Lambda *a) {ebe0dde5e7b8f45c3b3f6b9b38d7103d6 *env = raw_env;env->a = a;Lambda *_l23d938abb4684072a00367f5accf563c = malloc(sizeof(Lambda));_l23d938abb4684072a00367f5accf563c->fn = &l23d938abb4684072a00367f5accf563c;e23d938abb4684072a00367f5accf563c *_e23d938abb4684072a00367f5accf563c = malloc(sizeof(e23d938abb4684072a00367f5accf563c));_e23d938abb4684072a00367f5accf563c->a = env->a;_l23d938abb4684072a00367f5accf563c->env = _e23d938abb4684072a00367f5accf563c;return _l23d938abb4684072a00367f5accf563c;}Lambda *df331c0346ee14317afcbff1bebe97f2a() {Lambda *_lbe0dde5e7b8f45c3b3f6b9b38d7103d6 = malloc(sizeof(Lambda));_lbe0dde5e7b8f45c3b3f6b9b38d7103d6->fn = &lbe0dde5e7b8f45c3b3f6b9b38d7103d6;ebe0dde5e7b8f45c3b3f6b9b38d7103d6 *_ebe0dde5e7b8f45c3b3f6b9b38d7103d6 = malloc(sizeof(ebe0dde5e7b8f45c3b3f6b9b38d7103d6));_lbe0dde5e7b8f45c3b3f6b9b38d7103d6->env = _ebe0dde5e7b8f45c3b3f6b9b38d7103d6;return _lbe0dde5e7b8f45c3b3f6b9b38d7103d6;}typedef struct {Lambda *n;Lambda *x;} e106c63a33ef047a4a09883602cd7dd1b;Lambda *l106c63a33ef047a4a09883602cd7dd1b(void *raw_env, Lambda *x) {e106c63a33ef047a4a09883602cd7dd1b *env = raw_env;env->x = x;return d673faa466fdb4b68b72392afc436beff();}typedef struct {Lambda *n;} e937b244a8cd040aabb2594f4d1ad7eed;Lambda *l937b244a8cd040aabb2594f4d1ad7eed(void *raw_env, Lambda *n) {e937b244a8cd040aabb2594f4d1ad7eed *env = raw_env;env->n = n;Lambda *_l106c63a33ef047a4a09883602cd7dd1b = malloc(sizeof(Lambda));_l106c63a33ef047a4a09883602cd7dd1b->fn = &l106c63a33ef047a4a09883602cd7dd1b;e106c63a33ef047a4a09883602cd7dd1b *_e106c63a33ef047a4a09883602cd7dd1b = malloc(sizeof(e106c63a33ef047a4a09883602cd7dd1b));_e106c63a33ef047a4a09883602cd7dd1b->n = env->n;_l106c63a33ef047a4a09883602cd7dd1b->env = _e106c63a33ef047a4a09883602cd7dd1b;return invoke(invoke(env->n, _l106c63a33ef047a4a09883602cd7dd1b), d276224b6c32b436d82d264fbab57e387());}Lambda *d6bb06bc0235e4de380bf2905d78eb757() {Lambda *_l937b244a8cd040aabb2594f4d1ad7eed = malloc(sizeof(Lambda));_l937b244a8cd040aabb2594f4d1ad7eed->fn = &l937b244a8cd040aabb2594f4d1ad7eed;e937b244a8cd040aabb2594f4d1ad7eed *_e937b244a8cd040aabb2594f4d1ad7eed = malloc(sizeof(e937b244a8cd040aabb2594f4d1ad7eed));_l937b244a8cd040aabb2594f4d1ad7eed->env = _e937b244a8cd040aabb2594f4d1ad7eed;return _l937b244a8cd040aabb2594f4d1ad7eed;}typedef struct {Lambda *a;Lambda *b;} e4d0cff04c8974770908fd4e560017b38;Lambda *l4d0cff04c8974770908fd4e560017b38(void *raw_env, Lambda *b) {e4d0cff04c8974770908fd4e560017b38 *env = raw_env;env->b = b;return invoke(d6bb06bc0235e4de380bf2905d78eb757(), invoke(invoke(df331c0346ee14317afcbff1bebe97f2a(), env->a), env->b));}typedef struct {Lambda *a;} ef5a61760e2c44b5e9dda883b4cfff606;Lambda *lf5a61760e2c44b5e9dda883b4cfff606(void *raw_env, Lambda *a) {ef5a61760e2c44b5e9dda883b4cfff606 *env = raw_env;env->a = a;Lambda *_l4d0cff04c8974770908fd4e560017b38 = malloc(sizeof(Lambda));_l4d0cff04c8974770908fd4e560017b38->fn = &l4d0cff04c8974770908fd4e560017b38;e4d0cff04c8974770908fd4e560017b38 *_e4d0cff04c8974770908fd4e560017b38 = malloc(sizeof(e4d0cff04c8974770908fd4e560017b38));_e4d0cff04c8974770908fd4e560017b38->a = env->a;_l4d0cff04c8974770908fd4e560017b38->env = _e4d0cff04c8974770908fd4e560017b38;return _l4d0cff04c8974770908fd4e560017b38;}Lambda *dc359c478ecc04385b851fd08a0327946() {Lambda *_lf5a61760e2c44b5e9dda883b4cfff606 = malloc(sizeof(Lambda));_lf5a61760e2c44b5e9dda883b4cfff606->fn = &lf5a61760e2c44b5e9dda883b4cfff606;ef5a61760e2c44b5e9dda883b4cfff606 *_ef5a61760e2c44b5e9dda883b4cfff606 = malloc(sizeof(ef5a61760e2c44b5e9dda883b4cfff606));_lf5a61760e2c44b5e9dda883b4cfff606->env = _ef5a61760e2c44b5e9dda883b4cfff606;return _lf5a61760e2c44b5e9dda883b4cfff606;}typedef struct {Lambda *a;Lambda *b;} e2ff6a9f051cc4dc38bd25f4c86bb03ee;Lambda *l2ff6a9f051cc4dc38bd25f4c86bb03ee(void *raw_env, Lambda *b) {e2ff6a9f051cc4dc38bd25f4c86bb03ee *env = raw_env;env->b = b;return invoke(invoke(da42e68c400f64842bbaa63b40d490406(), invoke(invoke(dc359c478ecc04385b851fd08a0327946(), env->a), env->b)), invoke(invoke(dc359c478ecc04385b851fd08a0327946(), env->b), env->a));}typedef struct {Lambda *a;} e1d5273a235594d3ca72716f951e99995;Lambda *l1d5273a235594d3ca72716f951e99995(void *raw_env, Lambda *a) {e1d5273a235594d3ca72716f951e99995 *env = raw_env;env->a = a;Lambda *_l2ff6a9f051cc4dc38bd25f4c86bb03ee = malloc(sizeof(Lambda));_l2ff6a9f051cc4dc38bd25f4c86bb03ee->fn = &l2ff6a9f051cc4dc38bd25f4c86bb03ee;e2ff6a9f051cc4dc38bd25f4c86bb03ee *_e2ff6a9f051cc4dc38bd25f4c86bb03ee = malloc(sizeof(e2ff6a9f051cc4dc38bd25f4c86bb03ee));_e2ff6a9f051cc4dc38bd25f4c86bb03ee->a = env->a;_l2ff6a9f051cc4dc38bd25f4c86bb03ee->env = _e2ff6a9f051cc4dc38bd25f4c86bb03ee;return _l2ff6a9f051cc4dc38bd25f4c86bb03ee;}Lambda *d681b9b4ae83842d0bbfe6b4c6e344a35() {Lambda *_l1d5273a235594d3ca72716f951e99995 = malloc(sizeof(Lambda));_l1d5273a235594d3ca72716f951e99995->fn = &l1d5273a235594d3ca72716f951e99995;e1d5273a235594d3ca72716f951e99995 *_e1d5273a235594d3ca72716f951e99995 = malloc(sizeof(e1d5273a235594d3ca72716f951e99995));_l1d5273a235594d3ca72716f951e99995->env = _e1d5273a235594d3ca72716f951e99995;return _l1d5273a235594d3ca72716f951e99995;}Lambda *d2fe19696b82e44e8a0fa522b9dd0c76f() {return d681b9b4ae83842d0bbfe6b4c6e344a35();}int main() {printf("%d\n", churchToInt(invoke(invoke(invoke(db78ba1c68dc04c2a9e405865e7a08de6(), invoke(invoke(d2fe19696b82e44e8a0fa522b9dd0c76f(), churchNumeral(1)), churchNumeral(1))), churchNumeral(1)), churchNumeral(0))));}