#ifndef READER_H
#define READER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QVector>

namespace csv
{
    class Reader : public QObject
    {
        Q_OBJECT
    public:
        explicit Reader(QString file, QObject *parent = nullptr);
        ~Reader();
        QStringList read();
        QVector<QStringList> readAll();
        bool done();

    private:
        QString readLine();

        QTextStream* inner;
        QFile fin;
    };
}

#endif // READER_H
