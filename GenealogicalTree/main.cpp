#include "genealogicaltree.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GenealogicalTree w;
    w.show();

    return a.exec();
}
