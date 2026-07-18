#include "ben_afk.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Ben_AFK w;
    w.show();
    return QApplication::exec();
}
