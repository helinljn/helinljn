-- ------------------------------------------------------------------------
-- 存储GUID
drop procedure if exists `proc_save_guid`;
delimiter ;;
create procedure `proc_save_guid` (
	in in_type   int(10) unsigned,
	in in_carry  int(10) unsigned,
	in in_serial int(10) unsigned
)
exit_label:begin
	declare val_carry  int(10) unsigned default 0;
	declare val_serial int(10) unsigned default 0;
	declare exit handler for sqlexception
	begin
		rollback;
	end;

	start transaction;
		select guid_carry, guid_serial into val_carry, val_serial from t_guid where guid_type = in_type;
		if (val_carry <= in_carry and val_serial < in_serial) then
			update t_guid set guid_carry = in_carry, guid_serial = in_serial where guid_type = in_type;
		end if;
	commit;
end ;;
delimiter ;

-- ------------------------------------------------------------------------
-- 读取角色额外数据表1
drop procedure if exists `proc_load_charinfo_ext1`;
delimiter ;;
create procedure `proc_load_charinfo_ext1` (
	in in_character_id bigint(20) unsigned
)
exit_label:begin
	select * from t_charinfo_ext1 where character_id = in_character_id;
end ;;
delimiter ;

-- ------------------------------------------------------------------------
-- 保存角色额外数据表1
drop procedure if exists `proc_save_charinfo_ext1`;
delimiter ;;
create procedure `proc_save_charinfo_ext1` (
	in in_character_id   bigint(20) unsigned,
	in in_item_pack_data mediumtext
)
exit_label:begin
	declare exit handler for sqlexception
	begin
		rollback;
	end;

	start transaction;
		if not exists (select * from t_charinfo_ext1 where character_id = in_character_id) then
			insert into t_charinfo_ext1 values(in_character_id, in_item_pack_data);
		else
			update t_charinfo_ext1 set item_pack_data = in_item_pack_data where character_id = in_character_id;
		end if;
	commit;
end ;;
delimiter ;
