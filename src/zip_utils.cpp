//
// Created by Yuanjun Xiong on 18/11/2015.
//

#include "easylogging++.h"
#include "utils.h"
#include "zip.h"

void writeZipFile(std::vector<std::vector<uchar>> &data, std::string name_temp, std::string archive_name) {
    int err = 0;
#ifdef USE_OBSEL_LIBZIP
    struct zip *archive = zip_open(archive_name.c_str(), ZIP_CREATE, &err);
#else
    struct zip *archive = zip_open(archive_name.c_str(), ZIP_CREATE | ZIP_TRUNCATE, &err);
#endif

    CHECK_EQ(err, 0) << "Failed to open Zip file with error code: " << err;
    char name[256];
    struct zip_source *src_ptr;
    for (int i = 0; i < data.size(); ++i) {

        sprintf(name, name_temp.c_str(), i + 1);
        if ((src_ptr = zip_source_buffer(archive, data[i].data(), data[i].size(), 0)) == NULL ||
#ifdef USE_OBSEL_LIBZIP
            zip_add(archive, name, src_ptr) < 0) {
#else
            zip_file_add(archive, name, src_ptr, ZIP_FL_ENC_UTF_8) < 0) {
#endif
            zip_source_free(src_ptr);
            LOG(FATAL) << "error adding file " << name << ": " << zip_strerror(archive);
            zip_close(archive);
            exit(-1);
        }
    }
    zip_close(archive);
}

void SplitString(const std::string &s, std::vector<std::string> &v, const std::string &c) {
    std::string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while (std::string::npos != pos2) {
        v.push_back(s.substr(pos1, pos2 - pos1));

        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if (pos1 != s.length())
        v.push_back(s.substr(pos1));
}
