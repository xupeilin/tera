# schema更新（schema update）

tera支持表格创建之后更新其schema，已有的数据仍然有效（如果对应列没有在新schema中被删除）。

## schema更新的类型以及特点

1. 表格属性更新

    包括splitsize、mergesize. 这些属性由master负责维护，tabletnode无感知。
    表格在服务时可以在线更新以上属性，对读写服务没有任何影响。

2. lg更新

    增加、删除lg，lg属性（storage、blocksize、sst_size）的更新。
    这些更新操作涉及存储引擎，依赖很多分布式文件系统IO操作。
    当前，lg更新需要disable表格。

3. cf更新

    增加、删除cf，cf属性（最大版本数、最小版本数、ttl）的更新。
    这些更新操作主要涉及tabletnode内存状态的变更。
    当前，cf更新需要disable表格。

## 在线schema更新（online schema change）

### 背景

当前，cf和lg更新时需要先将表格disable掉，这意味着停服，对于用户来说，是不能接收的。

"在线schema更新"技术允许用户在不停服的情况下更新表格schema。

## 实现

1. 表格属性更新

    splitsize和mergesize是master在内存维护的状态，更新时命令master修改内存状态之后，
    新的splitsize和mergesize立即生效。

1. lg更新

  * master先将新schema持久化到meta表中
  * 然后依次重新load每个tablet，SDK有重试机制，可以实现用户无感知
  整个过程完成的时间与数据量（tablet数量）正相关。

1. cf更新

  * client发起更新操作，master写meta表后通知所有相关的tabletnode（load了对应tablet的ts）
  * 每个tabletnode更新内存状态，应用新的cf属性
  * master确认所有tabletnode成功后向client返回成功
  * 如果中途master挂掉或者rpc超时，client需要调用IsSchemaUpdateOk(schema)来获取结果

  tera保证只有在全部tablet都已生效新schema之后，才会向用户返回此次更新操作成功。
  在得到更新操作“OK”的结果之前，对新schema的任何假设都是未定义的。
  类似put一个值，在得到明确的返回值“OK”之前去读取，可能读到，也可能读不到，结果是无法预测的。

  在所依赖的底层系统（如文件系统）稳定可用（容忍偶尔抖动）时，tera保证在线schema更新在确定有限的时间内一定返回成功。
  否则，更新操作可能持续失败，一直不能返回成功，直到底层系统恢复稳定。

  通用性的模块：cf更新中用到的“master通知tabletnode执行命令（修改内存状态），master确保所有tabletnode执行成功后返回成功”的逻辑
  也可以抽象出来，用于更多类似的用途（例如部分配置变更等）。
