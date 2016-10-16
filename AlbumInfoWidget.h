#ifndef ALBUM_INFO_WIDGET_H
#define ALBUM_INFO_WIDGET_H

#include <QWidget>
struct Album;
struct AlbumList;
struct FileInfo;
struct FileList;
class QCheckBox;
class QLineEdit;
class QListWidget;
class QListWidgetItem;

class AlbumInfoWidget :
    public QWidget
{
    Q_OBJECT
public:
    AlbumInfoWidget(
        struct AlbumList* albumList,
        struct FileList* fileList,
        QWidget* parent = 0,
        Qt::WindowFlags flags = 0);
    ~AlbumInfoWidget();

    void updateData();
    void clear();

private slots:
    void onNewAlbum();
    void onRemoveAlbum();
    void onItemActivated(QListWidgetItem* item);
    void onEditingFinished();
    void onCheckBoxStateChanged(int state);
    void onAddSongs(bool);
    void onMoveSongUp(bool);
    void onMoveSongDown(bool);
    void onRemoveSong(bool);
    void onApplyFilter(bool);
    void onUpAlbum();
    void onDownAlbum();
    void onAlbumSelectionChanged();

private:
    FileInfo* getFileById(const char* id) const;
    Album* getAlbumById(const char* id) const;
    bool verifyUniqueId(const char* id) const;
    char* generateId() const;
    void saveActiveLineEdit();

private:
    struct AlbumList *albumList_;
    struct FileList *fileList_;
    Album *currAlbum_;
    QListWidget *albums_;
    QListWidget *files_;
    QLineEdit *id_;
    QLineEdit *name_;
    QLineEdit *algorithm_;
    QLineEdit *albumArt_;
    QCheckBox *shuffle_;
    QCheckBox *repeat_;
    QListWidget *songs_;
};

#endif // ALBUM_INFO_WIDGET_H
