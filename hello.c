#include <stdio.h>
#include <math.h>
int main(void)
{
    printf("Hello World!\n");
    double x = 2;
    int y = 4;
    int z = 0;

    z += x * y;
    printf("%d\n", z * (- 1));
    printf("%f\n", sqrt(x));
    return 0;
}
