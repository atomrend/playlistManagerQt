#include "MainWindow.h"
#include "AlbumInfoWidget.h"
#include "FileInfoWidget.h"
#include <QAction>
#include <QApplication>
#include <QFileDialog>
#include <QKeySequence>
#include <QMenu>
#include <QMenuBar>
#include <QTabWidget>
#include <stdlib.h>
#include "XmlReader.h"
#include "XmlWriter.h"

MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags flags) :
    QMainWindow(parent, flags),
    albumInfo_(new AlbumInfoWidget(&albumList_, &fileList_)),
    fileInfo_(new FileInfoWidget(&fileList_)),
    playlistFilePath_(QString())
{
    QAction *action;
    QTabWidget *tab = new QTabWidget;
    QMenuBar *menuBar = new QMenuBar;
    QMenu *menu;

    setWindowState(Qt::WindowMaximized);
    setWindowTitle(tr("Playlist Manager"));
    setCentralWidget(tab);
    setMenuBar(menuBar);

    albumList_.list = 0;
    albumList_.count = 0;
    fileList_.list = 0;
    fileList_.count = 0;

    tab->addTab(albumInfo_, tr("Album Info"));
    tab->addTab(fileInfo_, tr("File Info"));

    menu = menuBar->addMenu(tr("&File"));

    action = menu->addAction(tr("&Open"));
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
    connect(action, SIGNAL(triggered()), this, SLOT(onOpen()));
    addAction(action);

    action = menu->addAction(tr("&Close"));
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
    connect(action, SIGNAL(triggered()), this, SLOT(onClose()));
    addAction(action);

    menu->addSeparator();

    action = menu->addAction(tr("&Save"));
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
    connect(action, SIGNAL(triggered()), this, SLOT(onSave()));
    addAction(action);

    action = menu->addAction(tr("&SaveAs..."));
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S));
    connect(action, SIGNAL(triggered()), this, SLOT(onSaveAs()));
    addAction(action);

    menu->addSeparator();

    action = menu->addAction(tr("&Quit"));
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    connect(action, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
    addAction(action);
}

MainWindow::~MainWindow()
{
}

bool
MainWindow::openFile(const char* file)
{
    bool result = false;

    free(fileList_.list);
    free(albumList_.list);
    fileList_.count = 0;
    albumList_.count = 0;

    if (readPlaylistFile(file, &fileList_, &albumList_) == 0) {
        albumInfo_->updateData();
        fileInfo_->updateData();
        playlistFilePath_ = file;
        result = true;
    }

    return result;
}

void
MainWindow::onOpen()
{
    QString file = QFileDialog::getOpenFileName(this,
        tr("Open Jukebox Playlist"), QString(), tr("XML file (*.xml *.XML)"));

    if (file.isEmpty() == false) {
        openFile(file.toLatin1().data());
    }
}

void
MainWindow::onClose()
{
    albumInfo_->clear();
    fileInfo_->clear();
    free(fileList_.list);
    fileList_.count = 0;
    free(albumList_.list);
    albumList_.count = 0;
}

void
MainWindow::onSave()
{
    writeXml(playlistFilePath_.toLatin1().data(), &fileList_, &albumList_);
}

void
MainWindow::onSaveAs()
{
    QString file = QFileDialog::getSaveFileName(this,
        tr("Save Jukebox Playlist"), QString(), tr("XML file (*.xml *.XML)"));

    if (file.isEmpty() == false) {
        if (writeXml(file.toLatin1().data(), &fileList_, &albumList_) == 0) {
            playlistFilePath_ = file;
        }
    }
}
