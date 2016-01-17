# 在线schema更新（online schema update)

## 背景

当前，tera在更新表格schema时需要先将表格disable掉，这意味着停服，
对于绝大多数用户特别是7*24小时在线服务的用户来说，是不能接收的。
停服不仅严重影响可用性，也会损失很多cache内容，在一段时间内影响性能。

"在线schema更新"技术允许用户在不停服的情况下更新表格schema。

## schema更新的类型以及特点

1. 表格属性更新

	主要是split和merge的size，由master负责维护，tabletnode无感知，所以天然支持在线更新。

2. lg属性更新

	lg属性的动态更新（增、删、属性变更等）涉及存储引擎，依赖很多分布式文件系统IO操作。

3. cf属性更新

	cf属性的更新（赠、删、属性变更等）主要涉及tabletnode内存状态的变更。

## 实现

1. 表格属性更新

	直接修改master内存状态即可。

1. lg属性更新

  * master控制依次重新load每个tablet，SDK有重试机制，可以实现用户无感知

1. cf属性更新

  * client发起更新操作，master写meta表后通知所有相关的tabletnode（load了对应tablet的ts）
  * 每个tabletnode更新内存状态，应用新的cf属性
  * master确认所有tabletnode成功后向client返回成功
  * 如果中途master挂掉或者rpc超时，client需要调用IsSchemaUpdateOk(schema)来获取结果
