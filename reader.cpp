#include "reader.h"

namespace csv
{
    Reader::Reader(QString file, QObject *parent) : QObject(parent), fin(file)
    {
        fin.open(QIODevice::Text|QIODevice::ReadOnly);
        inner = new QTextStream(&fin);
    }

    Reader::~Reader() {
        fin.close();
    }

    QStringList Reader::read()
    {
        QString line = readLine();
        return line.split(",");
    }

    QVector<QStringList> Reader::readAll()
    {
        QVector<QStringList> vs;
        while(!done())
        {
            QStringList es = read();
            vs.append(es);
        }
        return vs;
    }

    bool Reader::done()
    {
        return inner->atEnd();
    }

    QString Reader::readLine() {
        if (done())
        {
            return "";
        }
        QString line;
        while(!done())
        {
            line = inner->readLine();
            if (line.isEmpty() || line.startsWith("#"))
            {
                continue;
            } else {
                break;
            }
        }
        return line;
    }
}

