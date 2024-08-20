#include <round.h>
double round_double(double f )
{
        double multiplier = 100;
        double ff = abs(f);
        if (ff == 0 ) return f;
        if (ff < 0.01)
                for ( ;ff < 1; )
                {
                        multiplier *= 10;
                        ff *= 10;
                }
        return round(f*multiplier) / multiplier;
}
