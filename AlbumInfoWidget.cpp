#include "AlbumInfoWidget.h"
#include "Filter.h"
#include "JukeboxPlaylist.h"
#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QGridLayout>
#include <QInputDialog>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QSplitter>
#include <stdlib.h>

AlbumInfoWidget::AlbumInfoWidget(
        struct AlbumList* albumList,
        struct FileList* fileList,
        QWidget* parent,
        Qt::WindowFlags flags) :
    QWidget(parent, flags),
    albumList_(albumList),
    fileList_(fileList),
    currAlbum_(0),
    albums_(new QListWidget),
    files_(new QListWidget),
    id_(new QLineEdit),
    name_(new QLineEdit),
    algorithm_(new QLineEdit),
    albumArt_(new QLineEdit),
    shuffle_(new QCheckBox),
    repeat_(new QCheckBox),
    songs_(new QListWidget)
{
    QAction *action = 0;
    QLabel *label = 0;
    QGridLayout *layout = new QGridLayout;
    QPushButton *button = 0;
    QSplitter *splitter = new QSplitter;
    QWidget *widget = 0;
    int row = 0;

    setLayout(layout);
    layout->addWidget(splitter, 0, 0, 1, 1);

    // Album list.
    widget = new QWidget;
    splitter->addWidget(widget);
    layout = new QGridLayout;
    widget->setLayout(layout);

    albums_->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(albums_, SIGNAL(itemSelectionChanged()),
        this, SLOT(onAlbumSelectionChanged()));
    layout->addWidget(albums_, 0, 0, 1, 2);

    button = new QPushButton(tr("New"));
    button->setToolTip(tr("Create a new album."));
    connect(button, SIGNAL(clicked()), this, SLOT(onNewAlbum()));
    layout->addWidget(button, 1, 0, 1, 1);

    button = new QPushButton(tr("Remove"));
    button->setToolTip(tr("Remove the currently selected album."));
    connect(button, SIGNAL(clicked()), this, SLOT(onRemoveAlbum()));
    layout->addWidget(button, 1, 1, 1, 1);

    // Album info.
    widget = new QWidget;
    splitter->addWidget(widget);
    layout = new QGridLayout;
    widget->setLayout(layout);

    label = new QLabel(tr("ID"));
    layout->addWidget(label, row, 0, 1, 1, Qt::AlignRight);
    id_->setDisabled(true);
    layout->addWidget(id_, row, 1, 1, 3);
    ++row;

    label = new QLabel(tr("Name"));
    layout->addWidget(label, row, 0, 1, 1, Qt::AlignRight);
    connect(name_, SIGNAL(editingFinished()),
        this, SLOT(onEditingFinished()));
    layout->addWidget(name_, row, 1, 1, 3);
    ++row;

    label = new QLabel(tr("Algorithm"));
    layout->addWidget(label, row, 0, 1, 1, Qt::AlignRight);
    connect(algorithm_, SIGNAL(editingFinished()),
        this, SLOT(onEditingFinished()));
    layout->addWidget(algorithm_, row, 1, 1, 2);
    button = new QPushButton(tr("Apply"));
    connect(button, SIGNAL(clicked(bool)), this, SLOT(onApplyFilter(bool)));
    layout->addWidget(button, row, 3, 1, 1);
    ++row;

    label = new QLabel(tr("Album Art"));
    layout->addWidget(label, row, 0, 1, 1, Qt::AlignRight);
    connect(albumArt_, SIGNAL(editingFinished()),
        this, SLOT(onEditingFinished()));
    layout->addWidget(albumArt_, row, 1, 1, 3);
    ++row;

    label = new QLabel(tr("Shuffle"));
    layout->addWidget(label, row, 0, 1, 1, Qt::AlignRight);
    connect(shuffle_, SIGNAL(stateChanged(int)),
        this, SLOT(onCheckBoxStateChanged(int)));
    layout->addWidget(shuffle_, row, 1, 1, 1);

    label = new QLabel(tr("Repeat"));
    layout->addWidget(label, row, 2, 1, 1, Qt::AlignRight);
    connect(repeat_, SIGNAL(stateChanged(int)),
        this, SLOT(onCheckBoxStateChanged(int)));
    layout->addWidget(repeat_, row, 3, 1, 1);
    ++row;

    songs_->setSelectionMode(QAbstractItemView::SingleSelection);
    layout->addWidget(songs_, row, 0, 1, 4);
    ++row;

    button = new QPushButton(tr("Up"));
    button->setToolTip(tr("Move the selected song up one place in the album."));
    connect(button, SIGNAL(clicked(bool)), this, SLOT(onMoveSongUp(bool)));
    layout->addWidget(button, row, 0, 1, 1);

    button = new QPushButton(tr("Down"));
    button->setToolTip(
        tr("Move the selected song down one place in the album."));
    connect(button, SIGNAL(clicked(bool)), this, SLOT(onMoveSongDown(bool)));
    layout->addWidget(button, row, 1, 1, 1);

    button = new QPushButton(tr("Remove"));
    button->setToolTip(tr("Remove the selected song from the album."));
    connect(button, SIGNAL(clicked(bool)), this, SLOT(onRemoveSong(bool)));
    layout->addWidget(button, row, 3, 1, 1);
    ++row;

    // File list.
    widget = new QWidget;
    splitter->addWidget(widget);
    layout = new QGridLayout;
    widget->setLayout(layout);

    files_->setSelectionMode(QAbstractItemView::ExtendedSelection);
    layout->addWidget(files_, 0, 0, 1, 1);

    button = new QPushButton(tr("Add files to album"));
    button->setToolTip(tr("Add selected files to the current album."));
    connect(button, SIGNAL(clicked(bool)), this, SLOT(onAddSongs(bool)));
    layout->addWidget(button, 1, 0, 1, 1);

    // Keyboard Shortcuts.
    action = new QAction(this);
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Up));
    connect(action, SIGNAL(triggered()),
        this, SLOT(onUpAlbum()));
    addAction(action);

    action = new QAction(this);
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Down));
    connect(action, SIGNAL(triggered()),
        this, SLOT(onDownAlbum()));
    addAction(action);

    action = new QAction(this);
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
    connect(action, SIGNAL(triggered()),
        this, SLOT(onNewAlbum()));
    addAction(action);
}

AlbumInfoWidget::~AlbumInfoWidget()
{
}

void
AlbumInfoWidget::updateData()
{
    unsigned int index = 0;
    Album *album = albumList_->list;
    FileInfo *file = fileList_->list;
    QListWidgetItem *item = 0;

    albums_->clear();
    songs_->clear();
    currAlbum_ = 0;

    index = 0;
    while (index < albumList_->count) {
        item = new QListWidgetItem(QString::fromUtf8(album->name), albums_);
        item->setData(Qt::UserRole, QString(album->id));

        ++album;
        ++index;
    }

    item = albums_->item(0);
    if (item != 0) {
        item->setSelected(true);
        onItemActivated(item);
    }

    index = 0;
    while (index < fileList_->count) {
        item = new QListWidgetItem(QString::fromUtf8(file->filePath), files_);
        item->setData(Qt::UserRole, QString(file->id));

        ++file;
        ++index;
    }
}

void
AlbumInfoWidget::clear()
{
    albums_->clear();
    files_->clear();
    id_->clear();
    name_->clear();
    algorithm_->clear();
    albumArt_->clear();
    songs_->clear();
    shuffle_->setChecked(false);
    repeat_->setChecked(false);
}

void
AlbumInfoWidget::onNewAlbum()
{
    QString newName = QInputDialog::getText(this, tr("Album Name"),
        tr("Name of the new album:"));
    char *id;
    Album *oldAlbum = albumList_->list;
    Album *curr;
    unsigned int index = 0;
    QListWidgetItem *item = 0;

    if (newName.isEmpty() == false) {
        id = generateId();
        while (verifyUniqueId(id) == false) {
            id = generateId();
        }

        albumList_->list =
            (Album*) malloc((albumList_->count + 1) * sizeof(Album));
        curr = albumList_->list;
        while (index < albumList_->count) {
            curr->id = oldAlbum->id;
            curr->name = oldAlbum->name;
            curr->algorithm = oldAlbum->algorithm;
            curr->songs = oldAlbum->songs;
            curr->albumArt = oldAlbum->albumArt;
            curr->count = oldAlbum->count;
            curr->shuffle = oldAlbum->shuffle;
            curr->repeat = oldAlbum->repeat;

            ++curr;
            ++oldAlbum;
            ++index;
        }
        ++albumList_->count;

        curr->id = (char*) malloc(strlen(id) + 1);
        memset(curr->id, '\0', strlen(id) + 1);
        strcpy(curr->id, id);
        curr->name = (char*) malloc(newName.toUtf8().size() + 1);
        memset(curr->name, '\0', newName.toUtf8().size() + 1);
        memcpy(curr->name, newName.toUtf8().data(), newName.toUtf8().size());
        curr->algorithm = 0;
        curr->songs = 0;
        curr->albumArt = 0;
        curr->count = 0;
        curr->shuffle = 0;
        curr->repeat = 0;

        albums_->clear();
        curr = albumList_->list;
        index = 0;
        while (index < albumList_->count) {
            item = new QListWidgetItem(QString::fromUtf8(curr->name), albums_);
            item->setData(Qt::UserRole, QString(curr->id));

            ++curr;
            ++index;
        }
        currAlbum_ = 0;

        item = albums_->item(0);
        if (item != 0) {
            item->setSelected(true);
            onItemActivated(item);
        }
    }
}

void
AlbumInfoWidget::onRemoveAlbum()
{
    // DBG
}

void
AlbumInfoWidget::onItemActivated(QListWidgetItem* item)
{
    QString variant;
    Album *album = 0;
    FileInfo *file = 0;
    unsigned int index = 0;
    QListWidgetItem *listItem;

    if (item != 0) {
        variant = item->data(Qt::UserRole).toString();
        album = getAlbumById(variant.toLatin1().data());

        if (album != 0) {
            id_->setText(QString::fromUtf8(album->id));
            name_->setText(QString::fromUtf8(album->name));
            algorithm_->setText(QString::fromUtf8(album->algorithm));
            albumArt_->setText(QString::fromUtf8(album->albumArt));
            shuffle_->blockSignals(true);
            if (album->shuffle == 1) {
                shuffle_->setChecked(true);
            } else {
                shuffle_->setChecked(false);
            }
            shuffle_->blockSignals(false);
            repeat_->blockSignals(true);
            if (album->repeat == 1) {
                repeat_->setChecked(true);
            } else {
                repeat_->setChecked(false);
            }
            repeat_->blockSignals(false);

            songs_->clear();
            while (index < album->count) {
                file = getFileById(album->songs[index]);
                if (file != 0) {
                    listItem = new QListWidgetItem(
                        QString::fromUtf8(file->filePath), songs_);
                    listItem->setData(Qt::UserRole,
                        QString::fromUtf8(file->id));
                }
                ++index;
            }

            currAlbum_ = album;
        }
    }
}

void
AlbumInfoWidget::onEditingFinished()
{
    QString id;
    QLineEdit *lineEdit = dynamic_cast<QLineEdit*>(sender());
    QListWidgetItem *item = 0;
    char **str = 0;
    int index = 0;

    if (currAlbum_ != 0) {
        if (lineEdit == name_) {
            str = &currAlbum_->name;
        } else if (lineEdit == algorithm_) {
            onApplyFilter(true);
            str = &currAlbum_->algorithm;
        } else if (lineEdit == albumArt_) {
            str = &currAlbum_->albumArt;
        }

        if (str != 0) {
            free(*str);
            index = lineEdit->text().toUtf8().size();
            if (index > 0) {
                *str = (char*) malloc(index * sizeof(char) + 1);
                memset(*str, '\0', index * sizeof(char) + 1);
                strcpy(*str, lineEdit->text().toUtf8().data());
            } else {
                *str = 0;
            }
        }

        if (lineEdit == name_) {
            id = QString::fromUtf8(currAlbum_->id);
            index = 0;
            while (index < albums_->count()) {
                item = albums_->item(index);
                if (item->data(Qt::UserRole).toString() == id) {
                    item->setText(QString::fromUtf8(currAlbum_->name));
                    break;
                }
                ++index;
            }
        }
    }
}

void
AlbumInfoWidget::onCheckBoxStateChanged(int state)
{
    QCheckBox *checkBox = dynamic_cast<QCheckBox*>(sender());

    if (currAlbum_ != 0) {
        if (checkBox == shuffle_) {
            if (state == Qt::Checked) {
                currAlbum_->shuffle = 1;
            } else {
                currAlbum_->shuffle = 0;
            }
        }
        if (checkBox == repeat_) {
            if (state == Qt::Checked) {
                currAlbum_->repeat = 1;
            } else {
                currAlbum_->repeat = 0;
            }
        }
    }
}

void
AlbumInfoWidget::onAddSongs(bool)
{
    QList<QListWidgetItem*> items = files_->selectedItems();
    char **oldSongs = 0;
    int index = 0;
    int tmpIndex = 0;
    QListWidgetItem *item = 0;
    QListWidgetItem *tmpItem = 0;
    QString id;
    FileInfo *file = 0;
    FileInfo *tmpFile = 0;

    if (currAlbum_ != 0 && items.size() > 0) {
        // Sort the selected items list.
        index = 0;
        while (index < items.size() - 1) {
            tmpIndex = index + 1;
            while (tmpIndex < items.size()) {
                item = items.at(index);
                id = item->data(Qt::UserRole).toString();
                file = getFileById(id.toLatin1().data());

                tmpItem = items.at(tmpIndex);
                id = tmpItem->data(Qt::UserRole).toString();
                tmpFile = getFileById(id.toLatin1().data());

                if (strcmp(file->filePath, tmpFile->filePath) > 0) {
                    tmpItem = items.takeAt(tmpIndex);
                    item = items.takeAt(index);

                    items.insert(index, tmpItem);
                    items.insert(tmpIndex, item);
                }

                ++tmpIndex;
            }

            ++index;
        }

        oldSongs = currAlbum_->songs;
        currAlbum_->songs =
            (char**) malloc(sizeof(char*) * (currAlbum_->count + items.size()));

        index = 0;
        while (index < (int) currAlbum_->count) {
            currAlbum_->songs[index] = oldSongs[index];
            ++index;
        }

        free(oldSongs);

        index = 0;
        while (index < items.size()) {
            item = items[index];
            id = item->data(Qt::UserRole).toString();

            file = getFileById(id.toLatin1().data());
            if (file != 0) {
                item = new QListWidgetItem(file->filePath, songs_);
                item->setData(Qt::UserRole, id);

                currAlbum_->songs[currAlbum_->count] =
                    (char*) malloc(sizeof(char) * (id.toLatin1().size() + 1));
                memset(currAlbum_->songs[currAlbum_->count], '\0',
                    id.toLatin1().size() + 1);
                strcpy(currAlbum_->songs[currAlbum_->count],
                    id.toLatin1().data());
                ++currAlbum_->count;
            }

            ++index;
        }
    }
}

void
AlbumInfoWidget::onMoveSongUp(bool)
{
    QListWidgetItem *item = 0;
    int row = songs_->currentRow();
    char *tmp;

    if (currAlbum_ != 0 && row > 0) {
        item = songs_->takeItem(row);
        songs_->insertItem(row - 1, item);
        songs_->setCurrentItem(item);

        tmp = currAlbum_->songs[row];
        currAlbum_->songs[row] = currAlbum_->songs[row - 1];
        currAlbum_->songs[row - 1] = tmp;
    }
}

void
AlbumInfoWidget::onMoveSongDown(bool)
{
    QListWidgetItem *item = 0;
    int row = songs_->currentRow();
    char *tmp;

    if (currAlbum_ != 0 && row < songs_->count() - 1) {
        item = songs_->takeItem(row);
        songs_->insertItem(row + 1, item);
        songs_->setCurrentItem(item);

        tmp = currAlbum_->songs[row];
        currAlbum_->songs[row] = currAlbum_->songs[row + 1];
        currAlbum_->songs[row + 1] = tmp;
    }
}

void
AlbumInfoWidget::onRemoveSong(bool)
{
    int row = songs_->currentRow();

    if (currAlbum_ != 0 && row < songs_->count() && row >= 0) {
        songs_->takeItem(row);

        --currAlbum_->count;
        while (row < (int) currAlbum_->count) {
            currAlbum_->songs[row] = currAlbum_->songs[row + 1];
            ++row;
        }
        currAlbum_->songs[currAlbum_->count] = 0;
    }
}

void
AlbumInfoWidget::onApplyFilter(bool)
{
    unsigned int count = 0;
    char *filtered = filterPlaylist(&count,
        algorithm_->text().toLatin1().data(), fileList_);
    QListWidgetItem *item = 0;
    unsigned int index = 0;
    FileInfo *file = 0;

    songs_->clear();

    if (count > 0 && currAlbum_ != 0) {
        while (index < count) {
            file = getFileById(filtered + (index * 17));
            if (file != 0) {
                item = new QListWidgetItem(QString::fromUtf8(file->filePath),
                    songs_);
                item->setData(Qt::UserRole, QString(file->id));
            }

            ++index;
        }

        free(filtered);
    }
}

//#22 0x00007ffff75e5d4f in QListView::updateGeometries() () from /usr/lib/x86_64-linux-gnu/libQtGui.so.4
//#23 0x00007ffff70d58d0 in QWidget::event(QEvent*) () from /usr/lib/x86_64-linux-gnu/libQtGui.so.4
//#24 0x00007ffff749ecee in QFrame::event(QEvent*) () from /usr/lib/x86_64-linux-gnu/libQtGui.so.4
//#25 0x00007ffff75c4663 in QAbstractItemView::viewportEvent(QEvent*) () from /usr/lib/x86_64-linux-gnu/libQtGui.so.4
//#26 0x00007ffff6b51a76 in QCoreApplicationPrivate::sendThroughObjectEventFilters(QObject*, QEvent*) () from /usr/lib/x86_64-linux-gnu/libQtCore.so.4
//#27 0x00007ffff707efbc in QApplicationPrivate::notify_helper(QObject*, QEvent*) () from /usr/lib/x86_64-linux-gnu/libQtGui.so.4
//#28 0x00007ffff7085f16 in QApplication::notify(QObject*, QEvent*) () from /usr/lib/x86_64-linux-gnu/libQtGui.so.4
//#29 0x00007ffff6b5190d in QCoreApplication::notifyInternal(QObject*, QEvent*) () from /usr/lib/x86_64-linux-gnu/libQtCore.so.4
//#30 0x00007ffff7121f51 in QWidgetPrivate::setGeometry_sys(int, int, int, int, bool) () from /usr/lib/x86_64-linux-gnu/libQtGui.so.4
//#31 0x00007ffff70cd315 in QWidget::setGeometry(QRect const&) () from /usr/lib/x86_64-linux-gnu/libQtGui.so.4
//#32 0x00007ffff7522fad in QAbstractScrollAreaPrivate::layoutChildren() () from /usr/lib/x86_64-linux-gnu/libQtGui.so.4

void
AlbumInfoWidget::onUpAlbum()
{
    int row = albums_->currentRow();

    saveActiveLineEdit();

    if (row > 0) {
        --row;
        albums_->setCurrentRow(row);
    }
}

void
AlbumInfoWidget::onDownAlbum()
{
    int row = albums_->currentRow();

    saveActiveLineEdit();

    if (row < albums_->count() - 1) {
        ++row;
        albums_->setCurrentRow(row);
    }
}

void
AlbumInfoWidget::onAlbumSelectionChanged()
{
    QListWidgetItem *item = albums_->currentItem();

    onItemActivated(item);
}

FileInfo*
AlbumInfoWidget::getFileById(const char* id) const
{
    FileInfo *result = 0;
    FileInfo *curr = fileList_->list;
    unsigned int index = 0;

    while (index < fileList_->count) {
        if (strcmp(id, curr->id) == 0) {
            result = curr;
            break;
        }

        ++curr;
        ++index;
    }

    return result;
}

Album*
AlbumInfoWidget::getAlbumById(const char* id) const
{
    Album *result = 0;
    Album *curr = albumList_->list;
    unsigned int index = 0;

    while (index < albumList_->count) {
        if (strcmp(id, curr->id) == 0) {
            result = curr;
            break;
        }

        ++curr;
        ++index;
    }

    return result;
}

bool
AlbumInfoWidget::verifyUniqueId(const char* id) const
{
    bool result = true;
    Album *album = getAlbumById(id);
    FileInfo *file = getFileById(id);

    if (album != 0 || file != 0) {
        result = false;
    }

    return result;
}

char*
AlbumInfoWidget::generateId() const
{
    static char *result = 0;
    unsigned char index = 0;
    char randNum;

    if (result == 0) {
        result = (char*) malloc(17 * sizeof(char));
        result[16] = '\0';
    }

    while (index < 16) {
        randNum = (char) rand() % 85 + 40;
        if ((randNum >= 40 && randNum <= 59) ||
                (randNum >= 63 && randNum <= 90) ||
                (randNum >= 95 && randNum <= 125)) {
            result[index] = randNum;
            ++index;
        }
    }
    result[16] = '\0';

    return result;
}

void
AlbumInfoWidget::saveActiveLineEdit()
{
    QString id;
    QLineEdit *lineEdit = 0;
    QListWidgetItem *item = 0;
    char **str = 0;
    int index = 0;

    if (currAlbum_ != 0) {
        lineEdit = dynamic_cast<QLineEdit*>(QApplication::focusWidget());
        if (lineEdit == name_) {
            str = &currAlbum_->name;
        } else if (lineEdit == algorithm_) {
            str = &currAlbum_->algorithm;
        } else if (lineEdit == albumArt_) {
            str = &currAlbum_->albumArt;
        }

        if (str != 0) {
            free(*str);
            index = lineEdit->text().toUtf8().size();
            if (index > 0) {
                *str = (char*) malloc(index * sizeof(char) + 1);
                memset(*str, '\0', index * sizeof(char) + 1);
                strcpy(*str, lineEdit->text().toUtf8().data());
            } else {
                *str = 0;
            }
        }

        if (lineEdit == name_) {
            id = QString::fromUtf8(currAlbum_->id);
            index = 0;
            while (index < albums_->count()) {
                item = albums_->item(index);
                if (item->data(Qt::UserRole).toString() == id) {
                    item->setText(QString::fromUtf8(currAlbum_->name));
                    break;
                }
                ++index;
            }
        }
    }
}
