#include "points.h"

QString toDMS(double pt)
{
    double rest = pt - int(pt);
    if (rest < 0)
    {
        rest = -rest;
    }

    double deg = int(pt - rest);
    double min = int(rest * 60.0);
    if (min < 0)
    {
        min = -min;
    }
    double sec = (rest - (min/60.0)) * 3600;
    if (sec < 0)
    {
        sec = -sec;
    }
    return QString("%1° %2' %3''").arg(int(deg)).arg(int(min)).arg(sec, 0, 'f', 2);
}

