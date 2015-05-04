Tera中的Leveldb
=====

Leveldb在tera中负责表格分片的存储，一个tabletnode中可能管理几个至几千个leveldb。因此，leveldb在tera中扮演着非常重要的角色，我们对leveldb进行了大量的改变和优化。

# 功能改变

原有LevelDB的功能对Tera的需求并不是完全满足，对于不满足Tera需求的部分加以改写优化。

## 1. 多种数据模型支持

## 2. 表格分片分裂与合并

## 3. 多存储介质支持

## 4. 单元格原子操作

## 5. 嵌套LevelDB

# 性能及资源利用优化

当单机管理几十至几千个leveldb时，leveldb的性能及资源消耗就变得很可观，对资源的有效利用提出了更高的要求，针对不同的情况，我们对leveldb做出了很多性能及资源利用方面的优化。

## 1. WAL异步写入

## 2. leveldb卸载流程优化

## 3. blockcache、tablecache支持多leveldb共享

## 4. leveldb后台共享线程池

## 5. MergingIterator中排序算法优化

# 稳定性优化

tera中的leveldb持久化存储由单机变为了分布式文件系统，当leveldb运行在分布式文件系统上时，将面对分布式文件系统中不同于本地系统的大量异常，稳定性大打折扣，tera在dfs容错方面也做了大量优化。

## 1. 各类文件丢失容错

## 2. 文件操作夯住容错

## 3. 文件副本无法恢复容错
