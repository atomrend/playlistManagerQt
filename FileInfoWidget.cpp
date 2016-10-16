#include "FileInfoWidget.h"
#include "JukeboxPlaylist.h"
#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QGridLayout>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QSpinBox>
#include <stdio.h>
#include <stdlib.h>

FileInfoWidget::FileInfoWidget(
        struct FileList* fileList,
        QWidget* parent,
        Qt::WindowFlags flags) :
    QWidget(parent, flags),
    fileList_(fileList),
    currFileInfo_(0),
    list_(new QListWidget),
    id_(new QLineEdit),
    filePath_(new QLineEdit),
    title_(new QLineEdit),
    album_(new QLineEdit),
    artist_(new QLineEdit),
    genre_(new QLineEdit),
    comment_(new QLineEdit),
    albumArt_(new QLineEdit),
    year_(new QSpinBox),
    frameCount_(new QLineEdit),
    favorite_(new QCheckBox)
{
    QAction *action = 0;
    QLabel *label = 0;
    QGridLayout *layout = new QGridLayout;
    int row = 0;

    setLayout(layout);
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 0);
    layout->setColumnStretch(2, 1);
    layout->setRowStretch(11, 1);

    list_->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(list_, SIGNAL(itemSelectionChanged()),
        this, SLOT(onItemSelectionChanged()));
    layout->addWidget(list_, 0, 0, 12, 1);

    label = new QLabel(tr("ID"));
    layout->addWidget(label, row, 1, 1, 1, Qt::AlignRight);
    id_->setDisabled(true);
    layout->addWidget(id_, row, 2, 1, 1);
    ++row;

    label = new QLabel(tr("File Path"));
    layout->addWidget(label, row, 1, 1, 1, Qt::AlignRight);
    filePath_->setDisabled(true);
    layout->addWidget(filePath_, row, 2, 1, 1);
    ++row;

    label = new QLabel(tr("Title"));
    layout->addWidget(label, row, 1, 1, 1, Qt::AlignRight);
    connect(title_, SIGNAL(editingFinished()),
        this, SLOT(onEditingFinished()));
    layout->addWidget(title_, row, 2, 1, 1);
    ++row;

    label = new QLabel(tr("Album"));
    layout->addWidget(label, row, 1, 1, 1, Qt::AlignRight);
    connect(album_, SIGNAL(editingFinished()),
        this, SLOT(onEditingFinished()));
    layout->addWidget(album_, row, 2, 1, 1);
    ++row;

    label = new QLabel(tr("Artist"));
    layout->addWidget(label, row, 1, 1, 1, Qt::AlignRight);
    connect(artist_, SIGNAL(editingFinished()),
        this, SLOT(onEditingFinished()));
    layout->addWidget(artist_, row, 2, 1, 1);
    ++row;

    label = new QLabel(tr("Genre"));
    layout->addWidget(label, row, 1, 1, 1, Qt::AlignRight);
    connect(genre_, SIGNAL(editingFinished()),
        this, SLOT(onEditingFinished()));
    layout->addWidget(genre_, row, 2, 1, 1);
    ++row;

    label = new QLabel(tr("Comment"));
    layout->addWidget(label, row, 1, 1, 1, Qt::AlignRight);
    connect(comment_, SIGNAL(editingFinished()),
        this, SLOT(onEditingFinished()));
    layout->addWidget(comment_, row, 2, 1, 1);
    ++row;

    label = new QLabel(tr("Album Art"));
    layout->addWidget(label, row, 1, 1, 1, Qt::AlignRight);
    connect(albumArt_, SIGNAL(editingFinished()),
        this, SLOT(onEditingFinished()));
    layout->addWidget(albumArt_, row, 2, 1, 1);
    ++row;

    label = new QLabel(tr("Year"));
    layout->addWidget(label, row, 1, 1, 1, Qt::AlignRight);
    year_->setRange(0, 2100);
    connect(year_, SIGNAL(valueChanged(int)),
        this, SLOT(onYearValueChanged(int)));
    layout->addWidget(year_, row, 2, 1, 1);
    ++row;

    label = new QLabel(tr("Frame Count"));
    layout->addWidget(label, row, 1, 1, 1, Qt::AlignRight);
    frameCount_->setDisabled(true);
    layout->addWidget(frameCount_, row, 2, 1, 1);
    ++row;

    label = new QLabel(tr("Favorite"));
    layout->addWidget(label, row, 1, 1, 1, Qt::AlignRight);
    connect(favorite_, SIGNAL(stateChanged(int)),
        this, SLOT(onFavoriteStateChanged(int)));
    layout->addWidget(favorite_, row, 2, 1, 1);
    ++row;

    action = new QAction(this);
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Up));
    connect(action, SIGNAL(triggered()),
        this, SLOT(onUpFile()));
    addAction(action);

    action = new QAction(this);
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Down));
    connect(action, SIGNAL(triggered()),
        this, SLOT(onDownFile()));
    addAction(action);
}

FileInfoWidget::~FileInfoWidget()
{
}

void
FileInfoWidget::updateData()
{
    FileInfo *file = fileList_->list;
    unsigned int index = 0;
    QListWidgetItem *item = 0;

    list_->clear();
    currFileInfo_ = 0;

    while (index < fileList_->count) {
        item = new QListWidgetItem(QString::fromUtf8(file->filePath), list_);
        item->setData(Qt::UserRole, QString(file->id));

        ++file;
        ++index;
    }
}

void
FileInfoWidget::clear()
{
    list_->clear();
    id_->clear();
    filePath_->clear();
    title_->clear();
    album_->clear();
    artist_->clear();
    genre_->clear();
    comment_->clear();
    albumArt_->clear();
    year_->setValue(0);
    frameCount_->clear();
    favorite_->setChecked(false);
}

void
FileInfoWidget::onItemActivated(QListWidgetItem* item)
{
    QString variant;
    FileInfo *fileInfo = 0;

    if (item != 0) {
        variant = item->data(Qt::UserRole).toString();
        fileInfo = getFileById(variant.toLatin1().data());

        if (fileInfo != 0) {
            currFileInfo_ = fileInfo;

            id_->setText(QString::fromUtf8(fileInfo->id));
            filePath_->setText(QString::fromUtf8(fileInfo->filePath));
            title_->setText(QString::fromUtf8(fileInfo->title));
            album_->setText(QString::fromUtf8(fileInfo->album));
            artist_->setText(QString::fromUtf8(fileInfo->artist));
            genre_->setText(QString::fromUtf8(fileInfo->genre));
            comment_->setText(QString::fromUtf8(fileInfo->comment));
            albumArt_->setText(QString::fromUtf8(fileInfo->albumArt));
            year_->setValue(fileInfo->year);
            frameCount_->setText(QString("%1").arg(fileInfo->frameCount));
            favorite_->blockSignals(true);
            if (fileInfo->favorite == 1) {
                favorite_->setChecked(true);
            } else {
                favorite_->setChecked(false);
            }
            favorite_->blockSignals(false);
        } else {
            currFileInfo_ = 0;
        }
    } else {
        currFileInfo_ = 0;
    }
}

void
FileInfoWidget::onEditingFinished()
{
    QLineEdit *lineEdit = dynamic_cast<QLineEdit*>(sender());
    char **str = 0;
    int size = 0;

    if (currFileInfo_ != 0) {
        if (lineEdit == title_) {
            str = &currFileInfo_->title;
        } else if (lineEdit == album_) {
            str = &currFileInfo_->album;
        } else if (lineEdit == artist_) {
            str = &currFileInfo_->artist;
        } else if (lineEdit == genre_) {
            str = &currFileInfo_->genre;
        } else if (lineEdit == comment_) {
            str = &currFileInfo_->comment;
        } else if (lineEdit == albumArt_) {
            str = &currFileInfo_->albumArt;
        }

        if (str != 0) {
            free(*str);
            size = lineEdit->text().toUtf8().size();
            if (size > 0) {
                *str = (char*) malloc(size * sizeof(char) + 1);
                memset(*str, '\0', size * sizeof(char) + 1);
                strcpy(*str, lineEdit->text().toUtf8().data());
            } else {
                *str = 0;
            }
        }
    }
}

void
FileInfoWidget::onYearValueChanged(int value)
{
    if (currFileInfo_ != 0) {
        currFileInfo_->year = value;
    }
}

void
FileInfoWidget::onFavoriteStateChanged(int state)
{
    if (currFileInfo_ != 0) {
        if (state == Qt::Checked) {
            currFileInfo_->favorite = 1;
        } else {
            currFileInfo_->favorite = 0;
        }
    }
}

void
FileInfoWidget::onItemSelectionChanged()
{
    QListWidgetItem *item = list_->currentItem();

    onItemActivated(item);
}

void
FileInfoWidget::onUpFile()
{
    int row = list_->currentRow();

    saveActiveLineEdit();

    if (row > 0) {
        --row;
        list_->setCurrentRow(row);
    }
}

void
FileInfoWidget::onDownFile()
{
    int row = list_->currentRow();

    saveActiveLineEdit();

    if (row < list_->count() - 1) {
        ++row;
        list_->setCurrentRow(row);
    }
}

FileInfo*
FileInfoWidget::getFileById(const char* id)
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

void
FileInfoWidget::saveActiveLineEdit()
{
    QLineEdit *lineEdit = 0;
    char **str = 0;
    int size = 0;

    if (currFileInfo_ != 0) {
        lineEdit = dynamic_cast<QLineEdit*>(QApplication::focusWidget());

        if (lineEdit == title_) {
            str = &currFileInfo_->title;
        } else if (lineEdit == album_) {
            str = &currFileInfo_->album;
        } else if (lineEdit == artist_) {
            str = &currFileInfo_->artist;
        } else if (lineEdit == genre_) {
            str = &currFileInfo_->genre;
        } else if (lineEdit == comment_) {
            str = &currFileInfo_->comment;
        } else if (lineEdit == albumArt_) {
            str = &currFileInfo_->albumArt;
        }

        if (str != 0) {
            free(*str);
            size = lineEdit->text().toUtf8().size();
            if (size > 0) {
                *str = (char*) malloc(size * sizeof(char) + 1);
                memset(*str, '\0', size * sizeof(char) + 1);
                strcpy(*str, lineEdit->text().toUtf8().data());
            } else {
                *str = 0;
            }
        }
    }
}
