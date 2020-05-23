#ifndef POINTS_H
#define POINTS_H

#include <QtMath>
#include <QDebug>
#include <QAbstractTableModel>
#include <QDateTime>

struct Coordinate {
    double X;
    double Y;
    double Z;
};

struct Point {
    double lat;
    double lng;
    double alt;
    QDateTime when;
    bool eclipse;

    double distance(Point other) const {
        Coordinate c1 = transform();
        Coordinate c2 = other.transform();

        double dist = qPow(c2.X - c1.X, 2) + qPow(c2.Y - c1.Y, 2) + qPow(c2.Z - c1.Z, 2);
        return qSqrt(dist);
    }

    Coordinate transform() const {
        double Flattening   = 0.003352813178;
        double Excentricity = 0.006694385;
        double Radius       = 6378.136;

        double radian = M_PI / 180;
        double x = lat*radian;
        double y = lng*radian;

        double s = qSin(x) * qSin(x);
        double n = Radius * qPow((1-Flattening*(2-Flattening)*s), -0.5);

        Coordinate c;
        c.X = (n+alt) * qCos(x) * qCos(y);
        c.Y = (n+alt) * qCos(x) * qSin(y);
        c.Z = (n*(1-Excentricity) + alt) * qSin(x);

        return c;
    }
};

struct Area {
    double latitude;
    double longitude;
    double margin;

    QDateTime dtstart;
    QDateTime dtend;

    bool day;
    bool night;

    QString label;

    double north()
    {
        return latitude + margin;
    }
    double south()
    {
        return latitude - margin;
    }
    double east()
    {
        return longitude + margin;
    }
    double west()
    {
        return longitude - margin;
    }

    bool accept(Point p)
    {
        bool pd = true; //p.when >= dtstart && p.when <= dtend;
        if (p.when.isValid())
        {
            pd = p.when >= dtstart && p.when <= dtend;
        }
        bool ns = p.lat >= south() && p.lat <= north();
        bool ew = p.lng <= east() && p.lng >= west();

        bool both = !day && !night;
        if (!both) {
            if (night) {
                both = p.eclipse;
            }
            if (day) {
                both = !p.eclipse;
            }
        }
        return pd && ns && ew && both;
    }
};

class Path {
private:
    QString label;
    Point first;
    Point last;

public:

    Path() = default;
    Path(const Path& other) = default;
    ~Path() = default;

    Path(Point p1, Point p2): first(p1), last(p2) {}

    Point getFirst()
    {
        return first;
    }

    Point getLast()
    {
        return last;
    }

    qint64 elapsed() const
    {
        return first.when.secsTo(last.when);
    };

    double distance() const
    {
        return last.distance(first);
    };
};

QString toDMS(double pt);

Q_DECLARE_METATYPE(Path);

#endif // POINTS_H
