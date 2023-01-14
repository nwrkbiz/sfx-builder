#include <Zip.h>

Zip::Zip(const std::filesystem::path& file) {
        mz_zip_reader_create(&m_Reader);
        mz_stream_os_create(&m_Stream);
        mz_stream_os_open(m_Stream, file.string().c_str(), MZ_OPEN_MODE_READ);
        mz_zip_reader_set_progress_cb(m_Reader, this, progress_cb);
};

Zip::~Zip() {
    mz_zip_reader_close(m_Reader);
    mz_zip_reader_delete(&m_Reader);
    mz_stream_os_close(m_Stream);
    mz_stream_os_delete(&m_Stream);
};


void Zip::seek(unsigned int offset) {
    mz_stream_os_seek(m_Stream, offset, MZ_SEEK_CUR);
}

std::string Zip::getTextFileContent(const std::string& f) {
    mz_zip_reader_set_pattern(m_Reader, f.c_str(), 1);
    mz_zip_reader_open(m_Reader, m_Stream);
    int32_t len = mz_zip_reader_entry_save_buffer_length(m_Reader);
    std::string s(len, ' ');
    mz_zip_reader_entry_save_buffer(m_Reader, &s[0], len);
    mz_zip_reader_close(m_Reader);
    return s;
}

void Zip::extract(const std::filesystem::path& to) {
    mz_zip_reader_set_pattern(m_Reader, nullptr, 1);
    mz_zip_reader_open(m_Reader, m_Stream);
    mz_zip_reader_save_all(m_Reader, to.string().c_str());
    mz_zip_reader_close(m_Reader);
}

int32_t Zip::progress_cb(void *handle, void *userdata, mz_zip_file *file_info, int64_t position) {
    Zip* me = static_cast<Zip*>(userdata);

    double progress = 0;
    uint8_t raw = 0;

    mz_zip_reader_get_raw(handle, &raw);

    if (raw && file_info->compressed_size > 0){
        progress = ((double)position / file_info->compressed_size) * 100;
    }
    else if (!raw && file_info->uncompressed_size > 0) {
        progress = ((double)position / file_info->uncompressed_size) * 100;
    }
    else if (!raw && file_info->uncompressed_size == 0) {
        progress = 100;
    }

    me->m_Fun(file_info->filename, progress);
    return MZ_OK;
}

void Zip::setProgressCb(const std::function<void(const std::string&, double)>& fun) {
    m_Fun = fun;
}

std::vector<std::string> Zip::getFileList() {
    mz_zip_file *file_info = nullptr;
    int32_t err = MZ_OK;

    mz_zip_reader_set_pattern(m_Reader, nullptr, 1);
    mz_zip_reader_open(m_Reader, m_Stream);
    mz_zip_reader_goto_first_entry(m_Reader);

    std::vector<std::string> f;
    do {
        mz_zip_reader_entry_get_info(m_Reader, &file_info);
        if (mz_zip_attrib_is_dir(file_info->external_fa, file_info->version_madeby) != MZ_OK) {
            f.emplace_back(file_info->filename);
        }
        err = mz_zip_reader_goto_next_entry(m_Reader);
        if (err != MZ_OK && err != MZ_END_OF_LIST) {
            break;
        }
    } while (err == MZ_OK);

    mz_zip_reader_close(m_Reader);
    return f;
}
