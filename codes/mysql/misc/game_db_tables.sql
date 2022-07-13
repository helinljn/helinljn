-- 角色额外数据表1
create table if not exists `t_charinfo_ext1` (
	`character_id`   bigint(20) unsigned not null default 0  comment '角色ID',            -- 角色ID
	`item_pack_data` mediumtext          not null default '' comment '该角色的背包数据',  -- 该角色的背包数据
	primary key (`character_id`)
) engine=innodb default charset=utf8 comment='角色额外数据表1';

-- GUID表
create table if not exists `t_guid` (
	`guid_type`   int(10) unsigned not null default 0 comment 'GUID的类型',    -- GUID的类型
	`guid_carry`  int(10) unsigned not null default 0 comment 'GUID的进位数',  -- GUID的进位数
	`guid_serial` int(10) unsigned not null default 0 comment 'GUID的序列号',  -- GUID的序列号
	primary key (`guid_type`)
) engine=innodb default charset=utf8 comment='GUID记录表';

-- 初始化GUID，初始化后自动删除该存储过程
-- begin
drop procedure if exists `proc_init_guid`;
delimiter ;;
create procedure `proc_init_guid` (
)
exit_label:begin
	declare val_init_count   int(11) default 0;
	declare val_record_count int(11) default 0;

	select count(*) into val_record_count from t_guid;

	-- 如果还未初始化，就执行初始化操作
	if (val_record_count = 0) then
		-- 循环插入数据，进行初始化操作
		while (val_record_count < 32) do
			insert into t_guid values(val_record_count, 0, 0);
			set val_record_count = val_record_count + 1;
		end while;
	end if;
end ;;
delimiter ;

call proc_init_guid();

drop procedure if exists `proc_init_guid`;
-- end
