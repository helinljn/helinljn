-- 创建test数据库
CREATE DATABASE IF NOT EXISTS test DEFAULT CHARSET utf8 COLLATE utf8_general_ci;

-- 创建升级数据库的存储过程
DROP PROCEDURE IF EXISTS `proc_update_test_db`;
DELIMITER //
CREATE PROCEDURE `proc_update_test_db` ()
EXIT_LABEL:BEGIN
  DECLARE cur_version        int(10) unsigned DEFAULT 0;
  DECLARE new_db_version     int(10) unsigned DEFAULT 0;
  DECLARE new_db_update_time timestamp        DEFAULT CURRENT_TIMESTAMP;
  DECLARE new_db_comment     varchar(255)     DEFAULT '';

  -- 判断数据库版本升级记录表是否存在，如果不存在则创建之
  CREATE TABLE IF NOT EXISTS `db_versions` (
    `version_id` int(10) unsigned NOT NULL DEFAULT 0                 COMMENT '版本ID',
    `upate_time` timestamp        NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '升级时间',
    `db_comment` varchar(255)     NOT NULL DEFAULT ''                COMMENT '升级内容',
    PRIMARY KEY (`version_id`)
  ) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='数据库升级记录表';

  -- 获取当前最大的版本ID
  IF EXISTS (SELECT * FROM db_versions) THEN
    SELECT MAX(version_id) INTO cur_version FROM db_versions;
  END IF;

  /******************************************************************
   * 设置版本号、升级时间和升级内容开始
   ******************************************************************
   */
  -- 版本1
  SET new_db_version     = 1;
  SET new_db_update_time = '2016-08-12 19:48:17';
  SET new_db_comment     = '创建数据测试表';
  -- 维护旧版本的更新日志，如果是最新的版本则不需要在此处插入，会在操作成功之后自动插入
  IF (cur_version < 1) THEN
    IF NOT EXISTS (SELECT * FROM db_versions WHERE version_id = new_db_version) THEN
      INSERT INTO db_versions VALUES(new_db_version, new_db_update_time, new_db_comment);
    END IF;
  END IF;

  -- 版本2
  SET new_db_version     = 2;
  SET new_db_update_time = '2016-08-12 20:34:01';
  SET new_db_comment     = '一些常用的表操作的语法';
  -- 维护旧版本的更新日志，如果是最新的版本则不需要在此处插入，会在操作成功之后自动插入
  IF (cur_version < 1) THEN
    IF NOT EXISTS (SELECT * FROM db_versions WHERE version_id = new_db_version) THEN
      INSERT INTO db_versions VALUES(new_db_version, new_db_update_time, new_db_comment);
    END IF;
  END IF;

  -- 版本3
  SET new_db_version     = 3;
  SET new_db_update_time = '2016-08-12 20:38:11';
  SET new_db_comment     = '本次版本更新的内容';
  /******************************************************************
   * 设置版本号、升级时间和升级内容结束
   ******************************************************************
   */

  /******************************************************************
   * 数据库升级开始
   ******************************************************************
   */
  -- 版本1
  IF (cur_version < 1) THEN
    -- 创建数据测试表
    DROP TABLE IF EXISTS `types_test`;
    CREATE TABLE `types_test` (
      `type_auto_increment`  bigint(20)  unsigned NOT NULL AUTO_INCREMENT COMMENT 'type_auto_increment',
      `tinyint_like_bool`    tinyint(1)  unsigned NOT NULL DEFAULT 0      COMMENT 'tinyint_like_bool value is 0 or 1',
      `tinyint_like_int8`    tinyint(4)           NOT NULL DEFAULT 0      COMMENT 'tinyint_like_int8 [-128 ~ 127]',
      `tinyint_like_uint8`   tinyint(3)  unsigned NOT NULL DEFAULT 0      COMMENT 'tinyint_like_uint8 [0 ~ 255]',
      `smallint_like_int16`  smallint(6)          NOT NULL DEFAULT 0      COMMENT 'smallint_like_int16 [-32768 ~ 32767]',
      `smallint_like_uint16` smallint(5) unsigned NOT NULL DEFAULT 0      COMMENT 'smallint_like_uint16 [0 ~ 65535]',
      `int_like_int32`       int(11)              NOT NULL DEFAULT 0      COMMENT 'int_like_int32',
      `int_like_uint32`      int(10)     unsigned NOT NULL DEFAULT 0      COMMENT 'int_like_uint32',
      `bigint_like_int64`    bigint(20)           NOT NULL DEFAULT 0      COMMENT 'bigint_like_int64',
      `bigint_like_uint64`   bigint(20)  unsigned NOT NULL DEFAULT 0      COMMENT 'bigint_like_uint64',
      `type_char`            char(255)            NOT NULL DEFAULT ''     COMMENT 'type_char max length 255',
      `type_varchar`         varchar(1024)        NOT NULL DEFAULT ''     COMMENT 'type_varchar max length 65535',
      PRIMARY KEY (`type_auto_increment`)
    ) ENGINE=InnoDB AUTO_INCREMENT=1000 DEFAULT CHARSET=utf8 COMMENT='types_test';
  END IF;

  -- 版本2
  IF (cur_version < 2) THEN
    -- 一些常用的表操作的语法
    -- add column in types_test after column 'int_like_int32'
    ALTER TABLE `types_test`
    ADD COLUMN `add_int32_column` int(11) NOT NULL DEFAULT 0 COMMENT 'add_int_column' AFTER `int_like_int32`;

    -- change column `add_int32_column`
    ALTER TABLE `types_test`
    CHANGE COLUMN `add_int32_column` `change_varchar_column` varchar(1024) NOT NULL DEFAULT '' COMMENT 'change_varchar_column';

    -- change column `change_varchar_column`
    ALTER TABLE `types_test`
    CHANGE COLUMN `change_varchar_column` `change_char_column` char(32) NOT NULL DEFAULT '' COMMENT 'change_char_column' AFTER `type_char`;

    -- modify column `change_char_column`
    ALTER TABLE `types_test`
    MODIFY COLUMN `change_char_column` char(128) NOT NULL DEFAULT '' COMMENT 'change_char_column';

    -- modify column `change_char_column`
    ALTER TABLE `types_test`
    MODIFY COLUMN `change_char_column` char(255) NOT NULL DEFAULT '' COMMENT 'change_char_column' AFTER `type_varchar`;

    -- drop column
    ALTER TABLE `types_test`
    DROP COLUMN `change_char_column`;

    -- add column `type_date` after `type_varchar`
    ALTER TABLE `types_test`
    ADD COLUMN `type_date` date NOT NULL COMMENT 'default is 0000-00-00' AFTER `type_varchar`;

    -- add column `type_timestamp` after `type_date`
    ALTER TABLE `types_test`
    ADD COLUMN `type_timestamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT 'type_timestamp' AFTER `type_date`;

    -- add column `test_drop` after `type_timestamp`
    ALTER TABLE `types_test`
    ADD COLUMN `test_drop` int(11) NOT NULL DEFAULT 0 COMMENT 'test_drop' AFTER `type_timestamp`;
  END IF;

  -- 版本3
  IF (cur_version < 3) THEN
    -- 本次版本更新的内容
    SELECT * FROM types_test;
  END IF;
  /******************************************************************
   * 数据库升级结束
   ******************************************************************
   */

  -- 升级成功，将最新的版本号、升级时间和升级内容存入数据库升级表中
  IF (cur_version < new_db_version) THEN
    INSERT INTO db_versions VALUES(new_db_version, new_db_update_time, new_db_comment);
  END IF;
END//
DELIMITER ;

-- 创建测试存储过程
DROP PROCEDURE IF EXISTS `proc_test_procedure`;
DELIMITER //
CREATE PROCEDURE `proc_test_procedure` (
  IN  in_param1  int(11),
  IN  in_param2  varchar(32),
  OUT out_param1 varchar(32)
)
EXIT_LABEL:BEGIN
  DECLARE return_value1 varchar(32) DEFAULT '';
  DECLARE return_value2 int(11)     DEFAULT 0;
  DECLARE temp_count    int(11)     DEFAULT 0;
  DECLARE repeat_times  int(11)     DEFAULT 0;
  DECLARE while_times   int(11)     DEFAULT 10;

  -- 表格判断
  IF EXISTS (SELECT * FROM information_schema.columns WHERE table_name = 'types_test' AND column_name = 'test_drop') THEN
    ALTER TABLE `types_test`
    DROP COLUMN `test_drop`;
  END IF;

  -- REPEAT循环
  REPEAT
    INSERT INTO types_test(
      tinyint_like_bool,
      tinyint_like_int8,
      tinyint_like_uint8,
      smallint_like_int16,
      smallint_like_uint16,
      int_like_int32,
      int_like_uint32,
      bigint_like_int64,
      bigint_like_uint64,
      type_char,
      type_varchar)
    VALUES(
      0,
      repeat_times,
      repeat_times,
      repeat_times,
      repeat_times,
      repeat_times,
      repeat_times,
      repeat_times,
      repeat_times,
      'Shmilyl',
      'Milan'
    );

    SET repeat_times = repeat_times + 1;
  UNTIL (repeat_times >= 10) END REPEAT;

  -- WHILE循环
  WHILE (while_times < 20) DO
    INSERT INTO types_test(
      tinyint_like_bool,
      tinyint_like_int8,
      tinyint_like_uint8,
      smallint_like_int16,
      smallint_like_uint16,
      int_like_int32,
      int_like_uint32,
      bigint_like_int64,
      bigint_like_uint64,
      type_char,
      type_varchar,
      type_date,
      type_timestamp)
    VALUES(
      1,
      while_times,
      while_times,
      while_times,
      while_times,
      while_times,
      while_times,
      while_times,
      while_times,
      'Shmilyl',
      'Milan',
      CURDATE(),
      NOW()
    );

    SET while_times = while_times + 1;
  END WHILE;

  SELECT COUNT(*) INTO temp_count FROM types_test;
  IF (temp_count = 0) THEN
    SET return_value1 = 'empty';
    SET return_value2 = temp_count;
    -- LEAVE EXIT_LABEL;
  ELSE
    SET return_value1 = 'not empty';
    SET return_value2 = temp_count;
  END IF;

  -- 测试输出
  SET out_param1 = 'output text!';
  SELECT in_param1, in_param2, out_param1, return_value1, return_value2;
  SELECT CURRENT_TIME, CURRENT_DATE, CURRENT_TIMESTAMP, CURTIME(), CURDATE(), NOW();
END//
DELIMITER ;

-- 调用升级数据库的存储过程
CALL proc_update_test_db();