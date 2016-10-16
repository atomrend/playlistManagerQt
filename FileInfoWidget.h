#ifndef FILE_INFO_WIDGET_H
#define FILE_INFO_WIDGET_H

#include <QWidget>
struct FileInfo;
struct FileList;
class QCheckBox;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QSpinBox;

class FileInfoWidget :
    public QWidget
{
    Q_OBJECT
public:
    FileInfoWidget(
        struct FileList* fileList,
        QWidget* parent = 0,
        Qt::WindowFlags flags = 0);
    ~FileInfoWidget();

    void updateData();
    void clear();

private slots:
    void onItemActivated(QListWidgetItem* item);
    void onEditingFinished();
    void onYearValueChanged(int value);
    void onFavoriteStateChanged(int state);
    void onItemSelectionChanged();
    void onUpFile();
    void onDownFile();

private:
    FileInfo* getFileById(const char* id);
    void saveActiveLineEdit();

private:
    struct FileList *fileList_;
    FileInfo *currFileInfo_;
    QListWidget *list_;
    QLineEdit *id_;
    QLineEdit *filePath_;
    QLineEdit *title_;
    QLineEdit *album_;
    QLineEdit *artist_;
    QLineEdit *genre_;
    QLineEdit *comment_;
    QLineEdit *albumArt_;
    QSpinBox *year_;
    QLineEdit *frameCount_;
    QCheckBox *favorite_;
};

#endif // FILE_INFO_WIDGET_H
