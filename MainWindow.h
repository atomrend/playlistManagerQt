#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include "JukeboxPlaylist.h"
class AlbumInfoWidget;
class FileInfoWidget;

class MainWindow :
    public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* parent = 0, Qt::WindowFlags flags = 0);
    ~MainWindow();

    bool openFile(const char* file);

protected slots:
    void onOpen();
    void onClose();
    void onSave();
    void onSaveAs();

private:
    FileList fileList_;
    AlbumList albumList_;
    AlbumInfoWidget *albumInfo_;
    FileInfoWidget *fileInfo_;
    QString playlistFilePath_;
};

#endif // MAIN_WINDOW_H
