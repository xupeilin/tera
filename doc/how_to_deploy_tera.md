如何部署分布式Tera
-----------------------

# 准备工作

布署一个全功能分布式Tera需要如下组件：

1·分布式协调服务：zookeeper/[nexus](nexus github)等，初始化路径结构（启动tera前建立好，假设根节点为/tera）:

    /tera/master-lock
    /tera/ts
    /tera/kick
    /tera/root_table

2.分布式文件系统：hdfs/nfs等

3.编译产出二进制文件：tera_main、teracli

# 部署目录结构

tera的master和tabletnode可以使用同样的部署目录进行启动，参考example/deploy目录结构：

	deploy_dir +- bin +- tera_main
			   |	  |- teracli
			   |      |- master.sh
			   |      |- tabletnode.sh
               +- conf +- tera.flag
               +- log
               +- lib +- 文件系统或其它组件需要链接的库（libhdfs.so等）

# 配置

tera的配置全部写在conf/tera.flag中，一个tera群集所有模块对应一个同样的配置文件，其中必需的配置项（以hdfs&zookeeper为例）如下：

	# master启动端口
	--tera_master_port=6678

	# tabletnode启动端口
	--tera_tabletnode_port=7778

	# 分布式文件系统类型[hdfs/hdfs2...]
	--tera_leveldb_env_dfs_type=hdfs
	# tera在文件系统上的根目录
	--tera_tabletnode_path_prefix=/tera/
	# tera允许在本地缓存数据文件，增加读性能，若设置为ssd路径，读性能会更高
	--tera_tabletnode_cache_paths=cache_path1;cache_path2...

	# zookeeper相关
	--tera_zk_enabled=true
	--tera_zk_addr_list=zk_addr1:3000,zk_addr2:3000,zk_addr3:3000
	--tera_zk_root_path=/tera

更多高级配置，参见[这里](config_url)

# 启动

需要将deploy目录分发到准备启动tera服务的服务器上。

## master

进入准务启动为master的服务器对应布署bin目录，执行master.sh即可启动一个master服务，若需要多master备份，只要在多台服务器上执行master.sh即可。

此时在bin目录下执行：

	./teracli showts

如果显示：

![master启动成功](master_pic_url)

说明master启动成功，其它结果说明启动失败，可查看log/master.INFO等日志文件追查失败原因。

## tabletnode

进入准务启动为master的服务器对应布署bin目录，执行tabletnode.sh即可启动一个tabletnode服务

此时在bin目录下执行：

	./teracli showts

可以发现些tabletnode地址及端口出现了，说明启动成功，已经可以提供服务。

![tabletnode启动成功](tabletnode_pic_url)

tabletnode服务可以随时加入或退出。

tera允许一台服务器上启动多个tera进程，也允许master与tabletnode混布，只需要配置不同的端口即可，但不推荐这样做，一台服务器一个tera进程是更好的选择。

# 启动成功以后

teracli是一个集群管理、tabletnode状态及负载显示、表格管理、表格状态负载显示、表格读写等功能于一身的客户端程序，可以通过其对集群及表格内容进行强有力的管理。使用方法参见[此处](teracli_help_url)。

# 常见问题

### master或tabletnode无法启动成功

* 检查协调服务是否可用、对应路径是否有读写权限
* 检查分布式文件系统是否可用、对应路径是否有读写权限
* 检查服务本地的cache目录是否有读写权限
* 检查服务启动所需端口是否被占用

### tabletnode删除一定数量服务器后，tera无法提供服务了

tera允许动态删除服务器，但不能一次过多，当超过10%的服务器不可用时，tera会判定为集群异常，进入safemode，可通过如下命令确认

	./teracli safemode get

如果确认是期望的下架操作，可通过如下命令强制退出safemode，使集群恢复服务（若机器数目过多，可能需要多次执行）

	./teracli safemode leave

### 可以在别的目录访问tera吗

可以，tera服务的定位通过tera.flag中的协调服务相关配置实现，tera会试图寻找tera.flag，寻找路径优先级为：当前目录 -> ../conf -> TERA_CONF环境变量指向的文件。只要被寻找到相应的tera.flag，即可连接tera服务。
