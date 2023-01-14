#ifndef SFX_ZIP_H
#define SFX_ZIP_H

#include <filesystem>
#include <functional>
#include <string>
#include <vector>

#include "mz.h"
#include "mz_os.h"
#include "mz_strm.h"
#include "mz_strm_os.h"
#include "mz_strm_buf.h"
#include "mz_strm_split.h"
#include "mz_zip.h"
#include "mz_zip_rw.h"

#include <Object.h>

class Zip : public giri::Object<Zip> {
public:
    Zip(const std::filesystem::path& file);
    ~Zip();

    void seek(unsigned int offset);

    std::string getTextFileContent(const std::string& fileName);

    void extract(const std::filesystem::path& to);

    void setProgressCb(const std::function<void(const std::string&, double)>& fun);

    static int32_t progress_cb(void *handle, void *userdata, mz_zip_file *file_info, int64_t position);

    std::vector<std::string> getFileList();

private:
    void* m_Stream;
    void* m_Reader;
    std::function<void(const std::string&, double)> m_Fun = [](const std::string&, double){};
};

#endif // SFX_ZIP_H