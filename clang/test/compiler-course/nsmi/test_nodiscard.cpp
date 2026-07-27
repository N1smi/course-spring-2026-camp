// RUN: %clang_cc1 -load %llvmshlibdir/nsmi_ClangAST%pluginext -plugin nodiscard-plugin -fsyntax-only %s 2>&1 | FileCheck %s

// CHECK-NOT: warning: function returning non-void should be marked with {{\[\[nodiscard\]\]}}
// CHECK-NOT: warning: result of call to non-void function is ignored
#include <stddef.h>

// CHECK: warning: function returning non-void should be marked with {{\[\[nodiscard\]\]}}
int basic_warn() {
    return 55;
}

// CHECK-NOT: warning: function returning non-void should be marked with {{\[\[nodiscard\]\]}}
void void_its_ok() {
    return;
}

// CHECK: warning: function returning non-void should be marked with {{\[\[nodiscard\]\]}}
template<typename T>
T template_func(T val) {
    return val;
}

// CHECK-NOT: warning: function returning non-void should be marked with {{\[\[nodiscard\]\]}}
void test_instantiations() {
    template_func(10);
    template_func(3.14);
    template_func('a');
}

// CHECK-NOT: warning: function returning non-void should be marked with {{\[\[nodiscard\]\]}}
[[nodiscard]] int already_has_attr() {
  return 100;
}

// CHECK-NOT: warning: function returning non-void should be marked with {{\[\[nodiscard\]\]}}
class SpecialMethodsTest {
public:
    SpecialMethodsTest() {}
    ~SpecialMethodsTest() {}
    operator int() const {
        return 42;
    }
};

class OperatorsTest {
  int x;

public:
  // CHECK-NOT: warning: function returning non-void should be marked with {{\[\[nodiscard\]\]}}
  OperatorsTest& operator=(const OperatorsTest& Other) { x = Other.x; return *this; }

  OperatorsTest& operator+=(int n) { x += n; return *this; }

  OperatorsTest& operator-=(int n) { x -= n; return *this; }

  OperatorsTest& operator*=(int n) { x *= n; return *this; }

  OperatorsTest& operator/=(int n) { x /= n; return *this; }

  OperatorsTest& operator%=(int n) { x %= n; return *this; }

  OperatorsTest& operator&=(int n) { x &= n; return *this; }

  OperatorsTest& operator|=(int n) { x |= n; return *this; }

  OperatorsTest& operator^=(int n) { x ^= n; return *this; }

  OperatorsTest& operator<<(int n) { x <<= n; return *this; }

  OperatorsTest& operator>>(int n) { x >>= n; return *this; }

  OperatorsTest& operator<<=(int n) { x <<= n; return *this; }

  OperatorsTest& operator>>=(int n) { x >>= n; return *this; }

  OperatorsTest& operator++() { ++x; return *this; }
  OperatorsTest operator++(int) { OperatorsTest tmp = *this; ++x; return tmp; }

  OperatorsTest& operator--() { --x; return *this; }
  OperatorsTest operator--(int) { OperatorsTest tmp = *this; --x; return tmp; }
};

// CHECK: warning: function returning non-void should be marked with {{\[\[nodiscard\]\]}}
OperatorsTest operator+(const OperatorsTest& a, const OperatorsTest& b) {
    return a;
}

struct StructWithDtor {
  ~StructWithDtor() {}
};

// CHECK: warning: function returning non-void should be marked with {{\[\[nodiscard\]\]}}
StructWithDtor return_struct_with_dtor() {
  return StructWithDtor();
}

void test_dtor() {
    // CHECK: warning: result of call to non-void function is ignored
    return_struct_with_dtor();
}

void test_ignored_calls() {
    // CHECK: warning: result of call to non-void function is ignored
    basic_warn();

    // CHECK: warning: result of call to non-void function is ignored
    OperatorsTest a, b;
    a + b;

    SpecialMethodsTest obj;
    // CHECK: warning: result of call to non-void function is ignored
    (int)obj;
}

// CHECK-NOT: warning: function returning non-void should be marked with {{\[\[nodiscard\]\]}}
void test_used_calls() {
  // CHECK-NOT: warning: result of call to non-void function is ignored
  int val = basic_warn();

  // CHECK-NOT: warning: result of call to non-void function is ignored
  if (basic_warn() == 55) {}

  // CHECK-NOT: warning: result of call to non-void function is ignored
  void_its_ok();

  // CHECK-NOT: warning: result of call to non-void function is ignored
  already_has_attr(); 

  OperatorsTest op;
  OperatorsTest other;
  // CHECK-NOT: warning: result of call to non-void function is ignored
  op = other;
  op << 5;
  op >> 5;
  op += 5;
  op -= 5;
  op *= 5;
  op /= 5;
  op %= 5;
  op &= 5;
  op |= 5;
  op ^= 5;
  op <<= 5;
  op >>= 5;
  op++;
  op--;
  --op;
  ++op;
}
