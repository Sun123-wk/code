#include "imSynthesis.h"

void test()
{
    imSynthesis integra("norm09_13_54.raw.png", "2512res_corner", 0.3);
    integra.integrate();
}

int main()
{
    test();
    return 0;
}