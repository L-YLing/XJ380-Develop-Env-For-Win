#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint64_t mz_uint64;
typedef uint32_t mz_uint32;
typedef uint16_t mz_uint16;
typedef int mz_bool;

#define MZ_FALSE 0
#define MZ_TRUE 1

typedef struct mz_zip_archive_s {
    void *m_pState;
    int m_last_error;
} mz_zip_archive;

#define MZ_ZIP_MODE_READ 1
#define MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY 0x00010000

MZ_EXPORT mz_bool mz_zip_reader_init_file(mz_zip_archive *pZip, const char *pFilename, mz_uint32 flags);
MZ_EXPORT mz_bool mz_zip_reader_end(mz_zip_archive *pZip);
MZ_EXPORT mz_uint32 mz_zip_reader_get_num_files(mz_zip_archive *pZip);
MZ_EXPORT mz_bool mz_zip_reader_file_stat(mz_zip_archive *pZip, mz_uint32 file_index, void *pStat);

typedef struct mz_zip_file_stat_s {
    const char *m_pFilename;
    mz_uint64 m_uncomp_size;
    mz_uint64 m_comp_size;
    mz_uint32 m_time;
    mz_uint32 m_crc32;
    mz_uint16 m_compression_method;
    mz_uint16 m_file_comment_size;
    const char *m_pComment;
    mz_uint32 m_internal_fa;
    mz_uint32 m_external_fa;
    mz_bool m_is_directory;
} mz_zip_file_stat;

typedef void (*mz_zip_reader_extract_progress_callback)(const mz_zip_archive *pZip, mz_uint32 file_index, const char *pFilename, mz_uint64 cur_bytes, mz_uint64 total_bytes);

MZ_EXPORT mz_bool mz_zip_reader_extract_to_file(mz_zip_archive *pZip, mz_uint32 file_index, const char *pDest_filename, mz_uint32 flags);
MZ_EXPORT mz_bool mz_zip_reader_extract_file_to_heap(mz_zip_archive *pZip, const char *pFilename, void **ppBuf, size_t *pSize, mz_uint32 flags);

#ifdef __cplusplus
}
#endif