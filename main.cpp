#include "MainWindow.h"
#include <QApplication>
#include <stdlib.h>
#include <time.h>

int
main(int argc, char** argv)
{
    QApplication app(argc, argv);
    MainWindow main;

    srand(time(0));

    main.show();

    if (argc > 1) {
        main.openFile(argv[1]);
    }

    return app.exec();
}
