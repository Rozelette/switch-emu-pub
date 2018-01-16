#include "test.h"

int main(void) {
    test_func();
    test_func2(2);
    int i = test_func3(3);

    test_func4(1.1f);
    test_func5(2.2);
    float f = test_func6();
    double d = test_func7();

    test_unimp();

    float f2 = i + f + d;
    int a = 0;
    while (1) a++;

    return 1;
}