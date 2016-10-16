#ifndef XML_WRITER_H
#define XML_WRITER_H

#include "JukeboxPlaylist.h"

char writeXml(const char* path, FileList* fileList, AlbumList* albumList);

#endif /* XML_WRITER_H */
