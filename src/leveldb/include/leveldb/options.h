// Copyright (c) 2015, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_INCLUDE_OPTIONS_H_
#define STORAGE_LEVELDB_INCLUDE_OPTIONS_H_

#include <stddef.h>
#include <stdint.h>

#include <string>
#include <set>
#include <map>
#include <vector>

namespace leveldb {

// We leave eight bits empty at the bottom so a type and sequence#
// can be packed together into 64-bits.
static const uint64_t kMaxSequenceNumber = ((0x1ull << 56) - 1);

static const size_t kDefaultBlockSize = 4096;
static const size_t kDefaultSstSize = 8 * 1024 * 1024; // 8 MB
class Cache;
class TableCache;
class CompactStrategyFactory;
class Comparator;
class Env;
class FilterPolicy;
class Logger;

// DB contents are stored in a set of blocks, each of which holds a
// sequence of key,value pairs.  Each block may be compressed before
// being stored in a file.  The following enum describes which
// compression method (if any) is used to compress a block.
enum CompressionType {
  // NOTE: do not change the values of existing entries, as these are
  // part of the persistent format on disk.
  kNoCompression     = 0x0,
  kSnappyCompression = 0x1,
  kBmzCompression    = 0x2,
  kLZ4Compression    = 0x3
};

enum RawKeyFormat {
  kReadable,
  kBinary,
  kTTLKv,
};

// struct for LG properties
struct LG_info {
  // ID for LG informaction structure
  uint32_t lg_id;

  // The storage medium type for LG
  Env* env;

  // compress type
  CompressionType compression;

  // block size
  size_t block_size;

  bool use_memtable_on_leveldb;

  size_t memtable_ldb_write_buffer_size;

  size_t memtable_ldb_block_size;

  int32_t sst_size;

  // Other LG properties
  // ...

  LG_info(uint32_t id = 0, Env* custom_env = NULL)
      : lg_id(id),
        env(custom_env),
        compression(kNoCompression),
        block_size(kDefaultBlockSize),
        use_memtable_on_leveldb(false),
        memtable_ldb_write_buffer_size(1 << 20),
        memtable_ldb_block_size(kDefaultBlockSize),
        sst_size(kDefaultSstSize) {}
};

// Options to control the behavior of a database (passed to DB::Open)
struct Options {
  // -------------------
  // Parameters that affect behavior

  // Comparator used to define the order of keys in the table.
  // Default: a comparator that uses lexicographic byte-wise ordering
  //
  // REQUIRES: The client must ensure that the comparator supplied
  // here has the same name and orders keys *exactly* the same as the
  // comparator provided to previous open calls on the same DB.
  const Comparator* comparator;

  // If true, the database will be created if it is missing.
  // Default: false
  bool create_if_missing;

  // If true, an error is raised if the database already exists.
  // Default: false
  bool error_if_exists;

  // If true, the implementation will do aggressive checking of the
  // data it is processing and will stop early if it detects any
  // errors.  This may have unforeseen ramifications: for example, a
  // corruption of one DB entry may cause a large number of entries to
  // become unreadable or for the entire DB to become unopenable.
  // Default: false
  bool paranoid_checks;

  // Use the specified object to interact with the environment,
  // e.g. to read/write files, schedule background work, etc.
  // Default: Env::Default()
  Env* env;

  // Any internal progress/error information generated by the db will
  // be written to info_log if it is non-NULL, or to a file stored
  // in the same directory as the DB contents if info_log is NULL.
  // Default: NULL
  Logger* info_log;

  // -------------------
  // Parameters that affect performance

  // Amount of data to build up in memory (backed by an unsorted log
  // on disk) before converting to a sorted on-disk file.
  //
  // Larger values increase performance, especially during bulk loads.
  // Up to two write buffers may be held in memory at the same time,
  // so you may wish to adjust this parameter to control memory usage.
  // Also, a larger write buffer will result in a longer recovery time
  // the next time the database is opened.
  //
  // Default: 4MB
  size_t write_buffer_size;

  // Soft limit on number of level-0 files.
  // BusyWrite return true at this point.
  int l0_slowdown_writes_trigger;

  // Number of open files that can be used by the DB.  You may need to
  // increase this if your database has a large working set (budget
  // one open file per 2MB of working set).
  //
  // Default: 1000
  int max_open_files;

  // Table cache handle
  // If non-NULL, use the specified table cache.
  // If NULL, create a new table cache with max_open_files.
  TableCache* table_cache;

  // Control over blocks (user data is stored in a set of blocks, and
  // a block is the unit of reading from disk).

  // If non-NULL, use the specified cache for blocks.
  // If NULL, leveldb will automatically create and use an 8MB internal cache.
  // Default: NULL
  Cache* block_cache;

  // Approximate size of user data packed per block.  Note that the
  // block size specified here corresponds to uncompressed data.  The
  // actual size of the unit read from disk may be smaller if
  // compression is enabled.  This parameter can be changed dynamically.
  //
  // Default: 4K
  size_t block_size;

  // Number of keys between restart points for delta encoding of keys.
  // This parameter can be changed dynamically.  Most clients should
  // leave this parameter alone.
  //
  // Default: 16
  int block_restart_interval;

  // Compress blocks using the specified compression algorithm.  This
  // parameter can be changed dynamically.
  //
  // Default: kSnappyCompression, which gives lightweight but fast
  // compression.
  //
  // Typical speeds of kSnappyCompression on an Intel(R) Core(TM)2 2.4GHz:
  //    ~200-500MB/s compression
  //    ~400-800MB/s decompression
  // Note that these speeds are significantly faster than most
  // persistent storage speeds, and therefore it is typically never
  // worth switching to kNoCompression.  Even if the input data is
  // incompressible, the kSnappyCompression implementation will
  // efficiently detect that and will switch to uncompressed mode.
  CompressionType compression;

  // If non-NULL, use the specified filter policy to reduce disk reads.
  // Many applications will benefit from passing the result of
  // NewBloomFilterPolicy() here.
  //
  // Default: NULL
  const FilterPolicy* filter_policy;

  // tera-specific
  std::string key_start;
  std::string key_end;

  std::vector<uint64_t> snapshots_sequence;

  std::set<uint32_t>* exist_lg_list;
  std::map<uint32_t, LG_info*>* lg_info_list;

  // compaction strategy to determine how to
  // drop the obsoleted kv records
  bool enable_strategy_when_get;
  CompactStrategyFactory* compact_strategy_factory;

  // split size for log file
  // default: 2MB
  size_t log_file_size;

  // AddRecord and Sync will be apllied asynchronously
  bool log_async_mode;

  // max number of unsed log files produced by switching log
  // default: 50
  int max_block_log_number;

  // max time to wait for log writing or sync
  // wait forever if this is negative
  // default: 5
  uint64_t write_log_time_out;

  // Max Log num before flush memtable
  // default: 100000
  uint64_t flush_triggered_log_num;

  // Max Log size before flush memtable
  // default: 40MB
  uint64_t flush_triggered_log_size;

  // Create new manifest to avoid dfs failure
  // default: 3600(seconds)
  uint64_t manifest_switch_interval;

  // tera raw key encoding format
  RawKeyFormat raw_key_format;

  // seek latency(in ns)
  int seek_latency;

  // dump imm & mem table at db close, and unlink all log file
  bool dump_mem_on_shutdown;

  // parent tablets num, used to find parent tablets' CURRENT&MANIFEST
  // if size == 0, new tablet
  // if size == 1, this was generated by splitting
  // if size == 2, this was generated by merging
  std::vector<uint64_t> parent_tablets;

  bool use_memtable_on_leveldb;

  size_t memtable_ldb_write_buffer_size;

  size_t memtable_ldb_block_size;

  bool drop_base_level_del_in_compaction;

  // if true: do not allow fs error, stop serving when error occured.
  // if false: auto-process fs error,
  //        e.g. skipping lost file, use backup MANIFEST, etc.
  //        caution: this may cause data error.
  bool filesystem_error_sensitive;

  // sst file size, in bytes
  int32_t sst_size;

  // Create an Options object with default values for all fields.
  Options();
};

// Options that control read operations
struct ReadOptions {
  // If true, all data read from underlying storage will be
  // verified against corresponding checksums.
  // Default: false
  bool verify_checksums;

  // Should the data read for this iteration be cached in memory?
  // Callers may wish to set this field to false for bulk scans.
  // Default: true
  bool fill_cache;

  // If "snapshot" is non-NULL, read as of the supplied snapshot
  // (which must belong to the DB that is being read and which must
  // not have been released).  If "snapshot" is NULL, use an impliicit
  // snapshot of the state at the beginning of this read operation.
  // Default: NULL
  uint64_t snapshot;

  // The "target_lgs" specifies the target locality groups
  // which is hit during the read operation. If NULL, all
  // the locality groups will be hit.
  // Default: NULL
  std::set<uint32_t>* target_lgs;

  // db option
  const Options* db_opt;

  ReadOptions(const Options* db_option)
      : verify_checksums(false),
        fill_cache(true),
        snapshot(kMaxSequenceNumber),
        target_lgs(NULL),
        db_opt(db_option) {
  }
  ReadOptions() {
    *this = ReadOptions(NULL);
  }
};

// Options that control write operations
struct WriteOptions {
  // If true, the write will be flushed from the operating system
  // buffer cache (by calling WritableFile::Sync()) before the write
  // is considered complete.  If this flag is true, writes will be
  // slower.
  //
  // If this flag is false, and the machine crashes, some recent
  // writes may be lost.  Note that if it is just the process that
  // crashes (i.e., the machine does not reboot), no writes will be
  // lost even if sync==false.
  //
  // In other words, a DB write with sync==false has similar
  // crash semantics as the "write()" system call.  A DB write
  // with sync==true has similar crash semantics to a "write()"
  // system call followed by "fsync()".
  //
  // Default: false
  bool sync;

  bool disable_wal;

  WriteOptions()
      : sync(false),
        disable_wal(false) {
  }
};

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_INCLUDE_OPTIONS_H_
