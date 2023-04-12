#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/// quick test code to testing averaging rotation method.
/// comments can be found in other files as it ended up workign and being used

#define PI 3.141592654

short int angles[] = {90, 360, 270};

typedef struct{
    float x;
    float y;
} Vector2;

int wrapAround(int n)
{
    while(n < 0)
    {
        n += 360;
    }

    while(n > 360)
    {
        n -= 360;
    }

    return n;
}

void main()
{
    float RAD2DEG = 180/PI;
    float DEG2RAD = 1 / RAD2DEG;
    int amount = sizeof(angles)/sizeof(angles[0]);
    Vector2 averageVector = {0,0};

    for(int i = 0; i < amount; i++)
    {
        float angle = angles[i] * DEG2RAD;
        float x = sin(angle);
        float y = cos(angle);

        averageVector.x += x / amount;
        averageVector.y += y / amount;
    }

    float averageAngle = atan2(averageVector.x, averageVector.y) * RAD2DEG;

    printf("average angle: %.2f   %i\n", averageAngle, wrapAround(averageAngle));
    printf("average position: %.2f %.2f\n", averageVector.x, averageVector.y);


}