/*
移动鹿鼎记SQL脚本
*/
  
create database if not exists mtlbbdb_databasename_forreplace DEFAULT CHARACTER SET utf8mb4;
use mtlbbdb_databasename_forreplace;

/*==============================================================*/
/* Table: t_char                                               */
/*==============================================================*/
drop table if exists t_char;
create table if not exists t_char               
(
   aid                               bigint                       not null AUTO_INCREMENT,
   accname                           varchar(80) binary           not null default '',
   charname                          varchar(80) binary           not null default '',
   charguid                          bigint                       not null default -1, 
   isvalid                           tinyint                      not null default -1,     
   hp                                int                          not null default -1,
   mp                                int                          not null default -1,
   xp                                int                          not null default -1,
   level                             smallint                     not null default 0,
   profession                        tinyint                      not null default -1, 
   exp                               int                          not null default -1,  
   sceneid                           int                          not null default -1,
   instsceneid                       int                          not null default -1,
   sceneposx                         int                          not null default -1,
   sceneposz                         int                          not null default -1,
   ncpsceneid                        int                          not null default -1,
   ncpinstsceneid                    int                          not null default -1,
   ncpsceneposx                      int                          not null default -1,
   ncpsceneposz                      int                          not null default -1,
   lastdbexptime                     int                          not null default 0,
   curdbexpdura                      int                          not null default 0,
   logouttime                        bigint                       not null default 0,
   rankpos                           int                          not null default 0,
   chtimes                           int                          not null default 0,
   chbuytimes                        int                          not null default 0,
   mertimes                          int                          not null default 0,
   pkvalue                           int                          not null default 0,
   pkmodel                           tinyint                      not null default 0,
   pkcdtime                          int                          not null default 0,                     
   pkrecoverpass                     bigint                       not null default 0,
   activetitle                       smallint                     not null default -1,
   moneycoin                         int                          not null default 0,
   moneyyb                           int                          not null default 0,
   moneyybbind                       int                          not null default 0,
   stamina                           smallint                     not null default 0,
   staminacdpass                     bigint                       not null default 0,
   skillexp                          int                          not null default 0,
   mtturn                            int                          not null default 0,
   mtcdtime                          int                          not null default 0,
   bclotimes                         int                          not null default 0,
   bmatrixactive                     int                          not null default 0,
   blasttime                         int                          not null default 0,
   bmatrix                           varchar(9)                   not null, 
   mountflag                         varchar(11)                  not null, 
   awdid                             int                          not null default 0,
   awdlefttime                       int                          not null default 0,
   nsactivity                        varchar(3)                   not null,
   lastmailguid                      bigint                       not null default -1,
   backpackopencount                 tinyint                      not null default 0,         
   reachedsceneid                    varchar(33)                  not null,
   fellowcoingaincount               int                          not null default 0,
   fellowyuanbaogaincount            int                          not null default 0,
   dailymissioncount                 int                          not null default 0,
   activeness                        int                          not null default 0,
   activenessflag                    varchar(7)                   not null,
   fashionid                         int                          not null default -1,
   showfashion                       tinyint                      not null default 0,
   dldtime                           int                          not null default 0,
   guildleavetime                    int                          not null default 0,
   rlevel                            smallint                     not null default 0,
   rexp                              int                          not null default 0,
   rfriendcount                      smallint                     not null default 0,
   torchvalue                        int                          not null default 0,
   zhanji                            int                          not null default 0,
   jinyaodai                         int                          not null default 0,
   activefellowguid                  bigint                       not null default -1,
   vipcost                           int                          not null default 0,
   offlineexp                        int                          not null default 0,
   dailyexp                          int                          not null default 0,
   firstloginlevel                   int                          not null default 0,
   firstlogintime                    bigint                       not null default 0,
   reputation                        int                          not null default 0,
   swordstate                        int                          not null default 0,
   swordscore                        int                          not null default 0,
   costyuanbao                       int                          not null default 0,
   createtime                        bigint                       not null default 0,
   gender                            tinyint                      not null default -1,
   fellowfreegaincount               smallint                     not null default 0,
   payflag                           varchar(5)                   not null,
   monthcardbegin                    bigint                       not null default 0,
   monthcardyb                       int                          not null default 0,
   storageopencount                  smallint                     not null default 0,
   masterskillname1                  varchar(50)                  not null,
   masterskillname2                  varchar(50)                  not null,
   masterskillname3                  varchar(50)                  not null,
   recguid                           bigint                       not null default -1,
   torchlimit                        int                          not null default 0,
   czstarttime                       bigint                       not null default 0, 
   czendtime                         bigint                       not null default 0,
   czvalue                           int                          not null default 0,
   loverguid                         bigint                       not null default -1,
   loverstat                         tinyint                      not null default -1,
   lovertime                         bigint                       not null default 0,
   ngshareguid                       bigint                       not null default -1,
   onlinetime                        int                          not null default 0,
   unblocktime                       bigint                       not null default 0,
   unforbidtalktime                  bigint                       not null default 0,
   lightskilllevel                   int                          not null default 0,
   crc                               int                          not null default 0,
   signinmonthid                     tinyint                      not null default 0,
   mergebonusflag                    tinyint                      not null default 0,
   herosenderguid                    bigint                       not null default -1,
   juyisenderguid                    bigint                       not null default -1,
   bclosestep                        tinyint                      not null default 0,
   pworldid                          int                          not null default -1,
   pworldname                        varchar(32)                  not null default '',
   suiyu                             int                          not null default 0,
   totalexp                          bigint                       not null default 0,
   bpstep                            tinyint                      not null default 0,
   bpnum                             varchar(12)                  not null default '',
   bpcodetime                        int                          not null default 0,
   bpcode                            varchar(5)                   not null default '',
   bpmessagecount                    smallint                     not null default 0,
   bpbindtime                        int                          not null default 0,
   bpgetrewarded                     tinyint                      not null default 0,
   cybpstep                          tinyint                      not null default 0,
   cybpnum                           varchar(12)                  not null default '',
   cybpgetfirst                      tinyint                      not null default 0,
   cybpgetweekly                     tinyint                      not null default 0,
   secpwd                            varchar(7)                   not null default '',
   bpprenum                          varchar(12)                  not null default '',
   unforbidtradetime                 bigint                       not null default 0,
   activepskillid   		     int  			  not null default -1,
   kejucuridx                        int                          not null default -1,
   kejucurquestionid                 int                          not null default 0,
   kejutodayrightcount               int                          not null default 0,
   kejutotalrightcount               int                          not null default 0,
   kejutodaystarttime                bigint                       not null default 0,
   kejutotaltime                     bigint                       not null default 0,
   soonpkmodel                       tinyint                      not null default 0,
   kejuweekstarttime                 bigint                       not null default 0,
   fumoshuijingcount                 int                          not null default 0,
   realsex                           tinyint                      not null default -1,
   onlinetimetype                    tinyint                      not null default -1,
   areanid                           int                          not null default -1,
   tutorleavetime                    bigint                       not null default 0,
   chushitime                        bigint                       not null default 0,
   teachexp                          int                          not null default 0,
   teachlevel                        smallint                     not null default 0,
   markidaspupil                     int                          not null default -1,
   intenttutorpro                    int                          not null default -1,
   markidastutor                     int                          not null default -1,
   intentpupilpro                    int                          not null default -1,
   primary key (aid)
)ENGINE = INNODB;                            

create unique index Index_char_charguid on t_char
(
   charguid
);

create index Index_char_accname on t_char
(
   accname
);

create unique index Index_char_charname on t_char
(
   charname
);

create index Index_char_level on t_char
(
   level,
   exp
);
/*============================================================================*/
/* Table: t_char_ext1; 
/*此表已不能再增加新列，注意每行最大8000字节，预留空间为以后已有列可能需要扩展空间                                              */
/*============================================================================*/
drop table if exists t_char_ext1;
create table if not exists t_char_ext1               
(
   aid                               bigint                       not null AUTO_INCREMENT,
   charguid                          bigint                       not null default -1,
   isvalid                           tinyint                      not null default -1,      
   pklist                            varchar(513)                 not null,
   cdlist                            varchar(513)                 not null,
   commonflag                        varchar(41)                  not null,
   commondata                        varchar(1025)                not null,
   mflag                             varchar(257)                 not null,
   systemtitle                       varchar(1025)                not null,
   userdeftitle                      varchar(1025)                not null, 
   rfriendlist                       varchar(1585)                not null,
   cpnumber                          varchar(1281)                not null,
   gemid                             varchar(481)                 not null,
   primary key (aid)
)ENGINE = INNODB; 
create unique index Index_char_ext1_charguid on t_char_ext1
(
   charguid
);
/*============================================================================*/
/* Table: t_char_ext2; 
/*此表新增数据列或扩展已有列，注意每行最大空间8000字节限制                      */
/*============================================================================*/

drop table if exists t_char_ext2;
create table if not exists t_char_ext2               
(
   aid                               bigint                       not null AUTO_INCREMENT,
   charguid                          bigint                       not null default -1,
   isvalid                           tinyint                      not null default -1,     
   activefellowskill                 varchar(257)                 not null,
   skilllist                         varchar(121)                 not null,
   dailymission                      varchar(161)                 not null,
   fashiondeadline                   varchar(321)                 not null,
   dlduniquebonus                    varchar(241)                 not null,
   limitgoodscount                   varchar(81)                  not null,
   mountdeadline                     varchar(641)                 not null,
   signinfo                          varchar(113)                 not null default '',
   rewardlist                        varchar(81)                  not null default '',
   cpnumberex                        varchar(1025)                not null,
   juyireceiver                      varchar(241)                 not null default '',
   commonprdata                      varchar(513)                 not null default '',
   commondata                        varchar(2049)                not null default '',
   weekdiscountshopver               int                          not null default -1,
   weekdiscountgoods                 varchar(49)                  not null,
   specialdiscountshopver            int                          not null default -1,
   specialdiscountgoods              varchar(49)                  not null,
   jieyigoods              			 varchar(161)                 not null,
   primary key (aid)
)ENGINE = INNODB; 
create unique index Index_char_ext2_charguid on t_char_ext2
(
   charguid
);

DELIMITER //
DROP PROCEDURE IF EXISTS load_char_info//
CREATE PROCEDURE load_char_info(pcharguid BIGINT)
BEGIN
DECLARE taccname VARCHAR(80) BINARY DEFAULT ''; 
DECLARE tcharguid BIGINT  DEFAULT 0xffffffffffffffff;
DECLARE tcharname VARCHAR(80) BINARY DEFAULT '';
DECLARE tprofession TINYINT DEFAULT -1; 
DECLARE tlevel SMALLINT DEFAULT 0;
DECLARE texp INT DEFAULT -1; 
DECLARE thp INT DEFAULT -1;
DECLARE tmp INT DEFAULT -1; 
DECLARE txp INT DEFAULT -1;
DECLARE tisvalid TINYINT DEFAULT -1;
DECLARE tsceneid INT DEFAULT -1;
DECLARE tinstsceneid INT DEFAULT -1;
DECLARE tsceneposx INT DEFAULT -1;
DECLARE tsceneposz INT DEFAULT -1;
DECLARE tncpsceneid INT DEFAULT -1;
DECLARE tncpinstsceneid INT DEFAULT -1;
DECLARE tncpsceneposx INT DEFAULT -1;
DECLARE tncpsceneposz INT DEFAULT -1;
DECLARE tlastdbexptime INT DEFAULT 0;
DECLARE tcurdbexpdura INT DEFAULT 0;
DECLARE tlogouttime BIGINT DEFAULT 0;
DECLARE trankpos INT DEFAULT 0;
DECLARE tchtimes INT DEFAULT 0;
DECLARE tchbuytimes INT DEFAULT 0;
DECLARE tmertimes INT DEFAULT 0;
DECLARE tpkvalue INT DEFAULT 0;
DECLARE tpkmodel TINYINT DEFAULT 0;
DECLARE tpkcdtime INT DEFAULT 0;
DECLARE tpkrecoverpass BIGINT DEFAULT 0; 
DECLARE tpklist VARCHAR(513) DEFAULT '';
DECLARE tcdlist VARCHAR(513) DEFAULT '';
DECLARE tcommondata VARCHAR(1025) DEFAULT '';
DECLARE tcommonflag VARCHAR(41) DEFAULT '';
DECLARE tmflag VARCHAR(257) DEFAULT '';
DECLARE tsystemtitle VARCHAR(1025) DEFAULT '';
DECLARE tuserdeftitle VARCHAR(1025) DEFAULT '';
DECLARE tactivetitle SMALLINT DEFAULT -1;
DECLARE tmoneycoin INT DEFAULT 0;
DECLARE tmoneyyb INT DEFAULT 0;
DECLARE tmoneyybbind INT DEFAULT 0;
DECLARE tstamina SMALLINT DEFAULT 0;
DECLARE tstaminacdpass BIGINT DEFAULT 0;
DECLARE tskillexp INT DEFAULT 0;
DECLARE tmtturn INT DEFAULT 0;
DECLARE tmtcdtime INT DEFAULT 0;
DECLARE tbclotimes INT DEFAULT 0;
DECLARE tbmatrixactive INT DEFAULT 0;
DECLARE tblasttime INT DEFAULT 0;
DECLARE tbmatrix VARCHAR(9) DEFAULT '';
DECLARE tmountflag VARCHAR(11) DEFAULT '';
DECLARE tawdid INT DEFAULT 0;
DECLARE tawdlefttime INT DEFAULT 0;
DECLARE tnsactivity VARCHAR(3) DEFAULT '';
DECLARE tlastmailguid BIGINT  DEFAULT 0xffffffffffffffff;
DECLARE tskilllist VARCHAR(121) DEFAULT '';
DECLARE tbackpackopencount TINYINT DEFAULT 0;
DECLARE treachedsceneid VARCHAR(33) DEFAULT '';
DECLARE tactivefellowskill VARCHAR(257) DEFAULT '';
DECLARE tfellowcoingaincount INT DEFAULT 0;
DECLARE tfellowyuanbaogaincount INT DEFAULT 0;
DECLARE tgemid VARCHAR(481) DEFAULT '';
DECLARE tdailymission VARCHAR(161) DEFAULT '';
DECLARE tdailymissioncount INT DEFAULT 0;
DECLARE tactiveness INT DEFAULT 0;
DECLARE tactivenessflag VARCHAR(7) DEFAULT '';
DECLARE tfashionid INT DEFAULT -1;
DECLARE tshowfashion TINYINT DEFAULT 0;
DECLARE tfashiondeadline VARCHAR(321) DEFAULT '';
DECLARE tdldtime INT DEFAULT 0;
DECLARE tdlduniquebonus VARCHAR(241) DEFAULT '';
DECLARE tguildleavetime INT DEFAULT 0;
DECLARE tcpnumber VARCHAR(1281) DEFAULT '';
DECLARE trlevel SMALLINT DEFAULT 0;
DECLARE trexp INT DEFAULT 0;
DECLARE trfriendcount SMALLINT DEFAULT 0;
DECLARE trfriendlist VARCHAR(1585) DEFAULT '';
DECLARE ttorchvalue INT DEFAULT 0;
DECLARE tzhanji INT DEFAULT 0;
DECLARE tjinyaodai INT DEFAULT 0;
DECLARE tlimitgoodscount VARCHAR(81) DEFAULT 0;
DECLARE tactivefellowguid BIGINT  DEFAULT 0xffffffffffffffff;
DECLARE tvipcost INT DEFAULT 0;
DECLARE tofflineexp INT DEFAULT 0;
DECLARE tdailyexp INT DEFAULT 0;
DECLARE tfirstloginlevel INT DEFAULT 0;
DECLARE tfirstlogintime BIGINT DEFAULT 0;
DECLARE treputation INT DEFAULT 0;
DECLARE tswordstate INT DEFAULT 0;
DECLARE tswordscore INT DEFAULT 0;
DECLARE tcostyuanbao INT DEFAULT 0;
DECLARE tcreatetime BIGINT DEFAULT 0;
DECLARE tgender TINYINT DEFAULT -1;
DECLARE tfellowfreegaincount SMALLINT DEFAULT 0;
DECLARE tpayflag VARCHAR(5) DEFAULT '';
DECLARE tmonthcardbegin BIGINT DEFAULT 0;
DECLARE tmonthcardyb INT DEFAULT 0;
DECLARE tmountdeadline VARCHAR(641) DEFAULT '';
DECLARE tstorageopencount SMALLINT DEFAULT 0;
DECLARE tmasterskillname1 VARCHAR(50) DEFAULT '';
DECLARE tmasterskillname2 VARCHAR(50) DEFAULT '';
DECLARE tmasterskillname3 VARCHAR(50) DEFAULT '';
DECLARE trecguid BIGINT  DEFAULT 0xffffffffffffffff;
DECLARE ttorchlimit INT DEFAULT 0;
DECLARE tczstarttime BIGINT DEFAULT 0;
DECLARE tczendtime BIGINT DEFAULT 0;
DECLARE tczvalue INT DEFAULT 0;
DECLARE tloverguid BIGINT  DEFAULT 0xffffffffffffffff;
DECLARE tloverstat TINYINT DEFAULT -1;
DECLARE tlovertime BIGINT DEFAULT 0;
DECLARE tngshareguid BIGINT DEFAULT 0xffffffffffffffff;
DECLARE tonlinetime INT DEFAULT 0;
DECLARE tunblocktime BIGINT DEFAULT 0;
DECLARE tunforbidtalktime BIGINT DEFAULT 0;
DECLARE tlightskilllevel INT DEFAULT 0;
DECLARE tcrc INT DEFAULT 0;
DECLARE tsigninfo VARCHAR(113) DEFAULT '';
DECLARE tsigninmonthid TINYINT DEFAULT 0;
DECLARE trewardlist VARCHAR(81) DEFAULT '';
DECLARE tmergebonusflag TINYINT DEFAULT 0;
DECLARE tcpnumberex VARCHAR(1025) DEFAULT '';
DECLARE therosenderguid BIGINT DEFAULT 0xffffffffffffffff;
DECLARE tjuyisenderguid BIGINT DEFAULT 0xffffffffffffffff;
DECLARE tjuyireceiver VARCHAR(241) DEFAULT '';
DECLARE tcommonprdata VARCHAR(513) DEFAULT '';
DECLARE tbclosestep TINYINT DEFAULT 0;
DECLARE tpworldid INT DEFAULT -1;
DECLARE tpworldname VARCHAR(32) DEFAULT '';
DECLARE tsuiyu INT DEFAULT 0;
DECLARE ttotalexp BIGINT DEFAULT 0;
DECLARE t2commondata VARCHAR(2049) DEFAULT '';
DECLARE tbpstep TINYINT DEFAULT 0;
DECLARE tbpnum VARCHAR(12) DEFAULT '';
DECLARE tbpcodetime INT DEFAULT 0;
DECLARE tbpcode VARCHAR(5) DEFAULT '';
DECLARE tbpmessagecount SMALLINT DEFAULT 0;
DECLARE tbpbindtime INT DEFAULT 0;
DECLARE tbpgetrewarded TINYINT DEFAULT 0;
DECLARE tcybpstep TINYINT DEFAULT 0;
DECLARE tcybpnum VARCHAR(12) DEFAULT '';
DECLARE tcybpgetfirst TINYINT DEFAULT 0;
DECLARE tcybpgetweekly TINYINT DEFAULT 0;
DECLARE tsecpwd VARCHAR(7) DEFAULT '';
DECLARE tbpprenum VARCHAR(12) DEFAULT '';
DECLARE tunforbidtradektime BIGINT DEFAULT 0;
DECLARE tactivepskillid INT DEFAULT -1;
DECLARE tweekdiscountshopver INT DEFAULT -1;
DECLARE tweekdiscountgoods VARCHAR(49) DEFAULT '';
DECLARE tspecialdiscountshopver INT DEFAULT -1;
DECLARE tspecialdiscountgoods VARCHAR(49) DEFAULT '';
DECLARE tjieyigoods VARCHAR(161) DEFAULT '';
DECLARE tkejucuridx INT DEFAULT -1;
DECLARE tkejucurquestionid INT DEFAULT 0;
DECLARE tkejutodayrightcount INT DEFAULT 0;
DECLARE tkejutotalrightcount INT DEFAULT 0;
DECLARE tkejutodaystarttime BIGINT DEFAULT 0;
DECLARE tkejutotaltime BIGINT DEFAULT 0;
DECLARE tsoonpkmodel TINYINT DEFAULT 0;
DECLARE tkejuweekstarttime BIGINT DEFAULT 0;
DECLARE tfumoshuijingcount INT DEFAULT 0;
DECLARE trealsex TINYINT DEFAULT -1;
DECLARE tonlinetimetype TINYINT DEFAULT -1;
DECLARE tareanid INT DEFAULT -1;
DECLARE ttutorleavetime BIGINT DEFAULT 0;
DECLARE tchushitime BIGINT DEFAULT 0;
DECLARE tteachexp INT DEFAULT 0;
DECLARE tteachlevel SMALLINT DEFAULT 0;
DECLARE tmarkidaspupil INT DEFAULT -1;
DECLARE tintenttutorpro INT DEFAULT -1;
DECLARE tmarkidastutor INT DEFAULT -1;
DECLARE tintentpupilpro INT DEFAULT -1;

START TRANSACTION;
SELECT 
c.accname, 
c.charguid, 
c.charname,
c.profession, 
c.level, 
c.exp, 
c.hp, 
c.mp, 
c.xp,
c.isvalid,
c.sceneid,
c.instsceneid,
c.sceneposx,
c.sceneposz,
c.ncpsceneid,
c.ncpinstsceneid,
c.ncpsceneposx,
c.ncpsceneposz,
c.lastdbexptime,
c.curdbexpdura,
c.logouttime,
c.rankpos,
c.chtimes,
c.chbuytimes,
c.mertimes,
c.pkvalue,
c.pkmodel,
c.pkcdtime,
c.pkrecoverpass, 
c.activetitle,
c.moneycoin,
c.moneyyb,
c.moneyybbind,
c.stamina,
c.staminacdpass,
c.skillexp,
c.mtturn,
c.mtcdtime,
c.bclotimes,
c.bmatrixactive,
c.blasttime,
c.bmatrix,
c.mountflag,
c.awdid,
c.awdlefttime,
c.nsactivity,
c.lastmailguid,
c.backpackopencount,
c.reachedsceneid,
c.fellowcoingaincount,
c.fellowyuanbaogaincount,
c.dailymissioncount,
c.activeness,
c.activenessflag,
c.fashionid,
c.showfashion,
c.dldtime,
c.guildleavetime,
c.rlevel,
c.rexp,
c.rfriendcount,
c.torchvalue,
c.zhanji,
c.jinyaodai,
c.activefellowguid,
c.vipcost, 
c.offlineexp,
c.dailyexp,
c.firstloginlevel, 
c.firstlogintime,
c.reputation,
c.swordstate,
c.swordscore,
c.costyuanbao,
c.createtime,
c.gender,
c.fellowfreegaincount,
c.payflag,
c.monthcardbegin,
c.monthcardyb,
c.storageopencount,
c.masterskillname1,
c.masterskillname2,
c.masterskillname3,
c.recguid,
c.torchlimit,
c.czstarttime,
c.czendtime,
c.czvalue,
c.loverguid,
c.loverstat,
c.lovertime,
c.ngshareguid,
c.onlinetime,
c.unblocktime,
c.unforbidtalktime,
c.lightskilllevel,
c.crc,
c.signinmonthid,
c.mergebonusflag,
c.herosenderguid,
c.juyisenderguid,
c.bclosestep,
c.pworldid,
c.pworldname,
c.suiyu,
c.totalexp,
c.bpstep,
c.bpnum,
c.bpcodetime,
c.bpcode,
c.bpmessagecount,
c.bpbindtime,
c.bpgetrewarded,
c.cybpstep,
c.cybpnum,
c.cybpgetfirst,
c.cybpgetweekly,
c.secpwd,
c.bpprenum,
c.unforbidtradetime,
c.activepskillid,
c.kejucuridx,
c.kejucurquestionid,
c.kejutodayrightcount,
c.kejutotalrightcount,
c.kejutodaystarttime,
c.kejutotaltime,
c.soonpkmodel,
c.kejuweekstarttime,
c.fumoshuijingcount,
c.realsex,
c.onlinetimetype,
c.areanid,
c.tutorleavetime,
c.chushitime,
c.teachexp,
c.teachlevel,
c.markidaspupil,
c.intenttutorpro,
c.markidastutor,
c.intentpupilpro
INTO taccname,tcharguid,tcharname,tprofession,tlevel,texp,thp,tmp,txp,tisvalid,tsceneid,tinstsceneid,tsceneposx,tsceneposz,tncpsceneid,tncpinstsceneid,tncpsceneposx,
tncpsceneposz,tlastdbexptime,tcurdbexpdura,tlogouttime,trankpos,tchtimes,tchbuytimes,tmertimes,tpkvalue,tpkmodel,tpkcdtime,tpkrecoverpass,tactivetitle,tmoneycoin,
tmoneyyb,tmoneyybbind,tstamina,tstaminacdpass,tskillexp,tmtturn,tmtcdtime,tbclotimes,tbmatrixactive,tblasttime,tbmatrix,tmountflag,tawdid,tawdlefttime,tnsactivity,
tlastmailguid,tbackpackopencount,treachedsceneid,tfellowcoingaincount,tfellowyuanbaogaincount,tdailymissioncount,tactiveness,tactivenessflag,tfashionid,tshowfashion,tdldtime,
tguildleavetime,trlevel,trexp,trfriendcount,ttorchvalue,tzhanji,tjinyaodai,tactivefellowguid,tvipcost,tofflineexp,tdailyexp,tfirstloginlevel,tfirstlogintime,treputation,
tswordstate,tswordscore,tcostyuanbao,tcreatetime,tgender,tfellowfreegaincount,tpayflag,tmonthcardbegin,tmonthcardyb,tstorageopencount,tmasterskillname1,tmasterskillname2,
tmasterskillname3,trecguid,ttorchlimit,tczstarttime,tczendtime,tczvalue,tloverguid,tloverstat,tlovertime,tngshareguid,tonlinetime,tunblocktime,tunforbidtalktime,tlightskilllevel,
tcrc,tsigninmonthid,tmergebonusflag,therosenderguid,tjuyisenderguid,tbclosestep,tpworldid,tpworldname,tsuiyu,ttotalexp,tbpstep,tbpnum,tbpcodetime,tbpcode,tbpmessagecount,
tbpbindtime,tbpgetrewarded,tcybpstep,tcybpnum,tcybpgetfirst,tcybpgetweekly,tsecpwd,tbpprenum,tunforbidtradektime,tactivepskillid,tkejucuridx,tkejucurquestionid,tkejutodayrightcount,tkejutotalrightcount,tkejutodaystarttime,tkejutotaltime,tsoonpkmodel,tkejuweekstarttime,tfumoshuijingcount,
trealsex,tonlinetimetype,tareanid,ttutorleavetime,tchushitime,tteachexp,tteachlevel,tmarkidaspupil,tintenttutorpro,tmarkidastutor,tintentpupilpro
FROM t_char AS c  \
WHERE c.charguid=pcharguid AND c.isvalid=1;

SELECT 
c1.pklist,
c1.cdlist,
c1.commondata,
c1.commonflag,
c1.mflag,
c1.systemtitle,
c1.userdeftitle,
c1.gemid,
c1.cpnumber,
c1.rfriendlist
INTO tpklist,tcdlist,tcommondata,tcommonflag,tmflag,tsystemtitle,tuserdeftitle,tgemid,tcpnumber,trfriendlist
FROM t_char_ext1 AS c1 WHERE c1.charguid=pcharguid ;

SELECT 
c2.skilllist,
c2.activefellowskill,
c2.dailymission,
c2.fashiondeadline,
c2.dlduniquebonus,
c2.limitgoodscount,
c2.mountdeadline,
c2.signinfo,
c2.rewardlist,
c2.cpnumberex,
c2.juyireceiver,
c2.commonprdata,
c2.commondata,
c2.weekdiscountshopver,
c2.weekdiscountgoods,
c2.specialdiscountshopver,                  
c2.specialdiscountgoods,
c2.jieyigoods
INTO tskilllist,
tactivefellowskill,
tdailymission,
tfashiondeadline,
tdlduniquebonus,
tlimitgoodscount,
tmountdeadline,
tsigninfo,
trewardlist,
tcpnumberex,
tjuyireceiver,
tcommonprdata,
t2commondata,
tweekdiscountshopver,
tweekdiscountgoods,
tspecialdiscountshopver,
tspecialdiscountgoods,
tjieyigoods
FROM t_char_ext2 AS c2 WHERE c2.charguid=pcharguid;
IF FOUND_ROWS() > 0 THEN 
SELECT taccname,tcharguid,tcharname,tprofession,tlevel,texp,thp,tmp,txp,tisvalid,tsceneid,tinstsceneid,tsceneposx,tsceneposz,tncpsceneid,tncpinstsceneid,tncpsceneposx,
tncpsceneposz,tlastdbexptime,tcurdbexpdura,tlogouttime,trankpos,tchtimes,tchbuytimes,tmertimes,tpkvalue,tpkmodel,tpkcdtime,tpkrecoverpass,
tpklist,tcdlist,tcommondata,tcommonflag,tmflag,tsystemtitle,tuserdeftitle,tactivetitle,tmoneycoin,tmoneyyb,tmoneyybbind,tstamina,tstaminacdpass,tskillexp,
tmtturn,tmtcdtime,tbclotimes,tbmatrixactive,tblasttime,tbmatrix,tmountflag,tawdid,tawdlefttime,tnsactivity,tlastmailguid,tskilllist,tbackpackopencount,
treachedsceneid,tactivefellowskill,tfellowcoingaincount,tfellowyuanbaogaincount,tgemid,tdailymission,tdailymissioncount,tactiveness,tactivenessflag,tfashionid,
tshowfashion,tfashiondeadline,tdldtime,tdlduniquebonus,tguildleavetime,tcpnumber,trlevel,trexp,trfriendcount,trfriendlist,ttorchvalue,tzhanji,tjinyaodai,
tlimitgoodscount,tactivefellowguid,tvipcost,tofflineexp,tdailyexp,tfirstloginlevel,tfirstlogintime,treputation,tswordstate,tswordscore,tcostyuanbao,tcreatetime,tgender,
tfellowfreegaincount,tpayflag,tmonthcardbegin,tmonthcardyb,tmountdeadline,tstorageopencount,tmasterskillname1,tmasterskillname2,tmasterskillname3,
trecguid,ttorchlimit,tczstarttime,tczendtime,tczvalue,tloverguid,tloverstat,tlovertime,tngshareguid,tonlinetime,tunblocktime,tunforbidtalktime,tlightskilllevel,
tcrc,tsigninfo,tsigninmonthid,trewardlist,tmergebonusflag,tcpnumberex,therosenderguid,tjuyisenderguid,tjuyireceiver,tcommonprdata,tbclosestep,tpworldid,
tpworldname,tsuiyu,ttotalexp,t2commondata,tbpstep,tbpnum,tbpcodetime,tbpcode,tbpmessagecount,tbpbindtime,tbpgetrewarded,tcybpstep,tcybpnum,tcybpgetfirst,tcybpgetweekly,tsecpwd,tbpprenum,
tunforbidtradektime,tactivepskillid,tweekdiscountshopver,tweekdiscountgoods,tspecialdiscountshopver,tspecialdiscountgoods,tjieyigoods,tkejucuridx,tkejucurquestionid,tkejutodayrightcount,tkejutotalrightcount,tkejutodaystarttime,tkejutotaltime,tsoonpkmodel,tkejuweekstarttime,tfumoshuijingcount,
trealsex,tonlinetimetype,tareanid,ttutorleavetime,tchushitime,tteachexp,tteachlevel,tmarkidaspupil,tintenttutorpro,tmarkidastutor,tintentpupilpro;
END IF;
COMMIT;
END;//
DELIMITER ;


DELIMITER //
DROP PROCEDURE IF EXISTS update_char_info//
CREATE PROCEDURE update_char_info(
           pcharguid BIGINT,
           pcharname VARCHAR(80) BINARY,
           paccname VARCHAR(80) BINARY,
           pprofession TINYINT,
           plevel SMALLINT,
           pexp INT,
           php INT,
           pmp INT,
           pxp INT,
           pisvalid TINYINT,
           psceneid INT,
           pinstsceneid INT,
           psceneposx INT,
           psceneposz INT,
           pncpsceneid INT,
           pncpinstsceneid INT,
           pncpsceneposx INT,
           pncpsceneposz INT,
           plastdbexptime INT,
           pcurdbexpdura INT,
           plogouttime BIGINT,
           prankpos INT,
           pchtimes INT,
           pchbuytimes INT,
           pmertimes INT,
           ppkvalue INT,
           ppkmodel TINYINT,
           ppkcdtime INT, 
           ppkrecoverpass BIGINT,
           ppklist VARCHAR(513),
           pcdlist VARCHAR(513),
           pcommondataext1 VARCHAR(1025),
           pcommonflag VARCHAR(41),
           pmflag VARCHAR(257),
           psystemtitle VARCHAR(1025),
           puserdeftitle VARCHAR(1025),
           pactivetitle SMALLINT,
           pmoneycoin INT,
           pmoneyyb INT,
           pmoneyybbind INT,
           pstamina SMALLINT,
           pstaminacdpass BIGINT,
           pskillexp INT,
           pmtturn INT,
           pmtcdtime INT,
           pbclotimes INT,
           pbmatrixactive INT,
           pblasttime INT,
           pbmatrix VARCHAR(9),
           pmountflag VARCHAR(11),
           pawdid INT,
           pawdlefttime INT,
           pnsactivity VARCHAR(3),
           plastmailguid BIGINT,
           pskilllist VARCHAR(121),
           pbackpackopencount TINYINT,
           preachedsceneid VARCHAR(33),
           pactivefellowskill VARCHAR(257),
           pfellowcoingaincount INT,
           pfellowyuanbaogaincount INT,
           pgemid VARCHAR(481),
           pdailymission VARCHAR(161),
           pdailymissioncount INT,
           pactiveness INT,
           pactivenessflag VARCHAR(7),
           pfashionid INT,
           pshowfashion TINYINT,
           pfashiondeadline VARCHAR(321),
           pdldtime INT,
           pdlduniquebonus VARCHAR(241),
           pguildleavetime INT,
           pcpnumber VARCHAR(1281),
           prlevel SMALLINT,
           prexp INT,
           prfriendcount SMALLINT,
           prfriendlist VARCHAR(1585),
           ptorchvalue INT,
           pzhanji INT,
           pjinyaodai INT,
           plimitgoodscount VARCHAR(81),
           pactivefellowguid BIGINT,
           pvipcost INT,
           pofflineexp INT,
           pdailyexp INT,
           pfirstloginlevel INT,
           pfirstlogintime BIGINT,
           preputation INT,
           pswordstate INT,
           pswordscore INT,
           pcostyuanbao INT,
           pgender TINYINT,
           pfellowfreegaincount SMALLINT,
           ppayflag VARCHAR(5),
           pmonthcardbegin BIGINT,
           pmonthcardyb INT,
           pmountdeadline VARCHAR(641),
           pstorageopencount SMALLINT,
           pmasterskillname1 VARCHAR(50),
           pmasterskillname2 VARCHAR(50),
           pmasterskillname3 VARCHAR(50),
           precguid BIGINT,
           ptorchlimit INT,
           pczstarttime BIGINT,
           pczendtime BIGINT,
           pczvalue INT,
           ploverguid BIGINT,
           ploverstat TINYINT,
           plovertime BIGINT,
           pngshareguid BIGINT,
           ponlinetime INT,
           punblocktime BIGINT,
           punforbidtalktime BIGINT,
           plightskilllevel INT,
           pcrc INT,
           psigninfo VARCHAR(113),
           psigninmonthid TINYINT,
           prewardlist VARCHAR(81),
           pmergebonusflag TINYINT,
           pcpnumberex VARCHAR(1025),
           pherosenderguid BIGINT,
           pjuyisenderguid BIGINT,
           pjuyireceiver VARCHAR(241),
           pcommonprdata VARCHAR(513),
           pbclosestep TINYINT,
           ppworldid INT,
           ppworldname VARCHAR(32),
           psuiyu INT,
           ptotalexp BIGINT,
           pcommondataext2 VARCHAR(2049),
           pbpstep TINYINT,pbpnum VARCHAR(12),pbpcodetime INT,pbpcode VARCHAR(5),pbpmessagecount SMALLINT,
		   pbpbindtime INT,pbpgetrewarded TINYINT,pcybpstep TINYINT,pcybpnum VARCHAR(12),pcybpgetfirst TINYINT,pcybpgetweekly TINYINT,
           psecpwd VARCHAR(7),pbpprenum VARCHAR(12),punforbidtradetime BIGINT,pactivepskillid INT,
		   pweekdiscountshopver INT, pweekdiscountgoods VARCHAR(49),pspecialdiscountshopver INT,pspecialdiscountgoods VARCHAR(49),
		   pjieyigoods VARCHAR(161),
           pkejucuridx INT,
	   pkejucurquestionid INT,
	   pkejutodayrightcount INT,
	   pkejutotalrightcount INT,
	   pkejutodaystarttime BIGINT,
	   pkejutotaltime BIGINT,
	   psoonpkmodel TINYINT,
	   pkejuweekstarttime BIGINT,
	   pfumoshuijingcount INT,
	   prealsex tinyint,
	   ponlinetimetype tinyint,
	   pareanid int,
	   ptutorleavetime bigint,
	   pchushitime bigint,
	   pteachexp int,
	   pteachlevel smallint,
	   pmarkidaspupil int,
	   pintenttutorpro int,
	   pmarkidastutor int,
	   pintentpupilpro int)
BEGIN

DECLARE tempcharguid BIGINT DEFAULT -1;
START TRANSACTION;
SELECT charguid INTO tempcharguid FROM t_char WHERE charguid = pcharguid AND isvalid=1 LIMIT 1;

IF tempcharguid = pcharguid THEN

       UPDATE t_char AS c SET 
           c.charname=pcharname, 
           c.profession=pprofession, 
           c.level=plevel, 
           c.exp=pexp, 
           c.hp=php, 
           c.mp=pmp,
           c.xp=pxp,
           c.isvalid=pisvalid,
           c.sceneid=psceneid,
           c.instsceneid=pinstsceneid,
           c.sceneposx=psceneposx,
           c.sceneposz=psceneposz,
           c.ncpsceneid=pncpsceneid,
           c.ncpinstsceneid=pncpinstsceneid,
           c.ncpsceneposx=pncpsceneposx,
           c.ncpsceneposz=pncpsceneposz,
           c.lastdbexptime=plastdbexptime,
           c.curdbexpdura=pcurdbexpdura,
           c.logouttime=plogouttime,
           c.rankpos=prankpos,
           c.chtimes=pchtimes,
           c.chbuytimes=pchbuytimes,
           c.mertimes=pmertimes,
           c.pkvalue=ppkvalue,
           c.pkmodel=ppkmodel,
           c.pkcdtime=ppkcdtime,
           c.pkrecoverpass=ppkrecoverpass, 
           c.activetitle=pactivetitle,
           c.moneycoin=pmoneycoin,
           c.moneyyb=pmoneyyb,
           c.moneyybbind=pmoneyybbind,
           c.stamina=pstamina,
           c.staminacdpass=pstaminacdpass,
           c.skillexp=pskillexp,
           c.mtturn=pmtturn,
           c.mtcdtime=pmtcdtime,
           c.bclotimes=pbclotimes,
           c.bmatrixactive=pbmatrixactive,
           c.blasttime=pblasttime,
           c.bmatrix=pbmatrix,
           c.mountflag=pmountflag,
           c.awdid=pawdid,
           c.awdlefttime=pawdlefttime,
           c.nsactivity=pnsactivity,
           c.lastmailguid=plastmailguid,       
           c.backpackopencount=pbackpackopencount,
           c.reachedsceneid=preachedsceneid,        
           c.fellowcoingaincount=pfellowcoingaincount,
           c.fellowyuanbaogaincount=pfellowyuanbaogaincount,
           c.dailymissioncount=pdailymissioncount,
           c.activeness=pactiveness,
           c.activenessflag=pactivenessflag,
           c.fashionid=pfashionid,
           c.showfashion=pshowfashion,
           c.dldtime=pdldtime,        
           c.guildleavetime=pguildleavetime,        
           c.rlevel=prlevel,
           c.rexp=prexp,
           c.rfriendcount=prfriendcount,        
           c.torchvalue=ptorchvalue,
           c.zhanji=pzhanji,
           c.jinyaodai=pjinyaodai,       
           c.activefellowguid=pactivefellowguid,
           c.vipcost=pvipcost,
           c.offlineexp=pofflineexp,
           c.dailyexp=pdailyexp,
           c.firstloginlevel=pfirstloginlevel,
           c.firstlogintime=pfirstlogintime,
           c.reputation=preputation,
           c.swordstate=pswordstate,
           c.swordscore=pswordscore,
           c.costyuanbao=pcostyuanbao,
           c.gender=pgender,
           c.fellowfreegaincount=pfellowfreegaincount,
           c.payflag=ppayflag,
           c.monthcardbegin=pmonthcardbegin,
           c.monthcardyb=pmonthcardyb,       
           c.storageopencount=pstorageopencount,
           c.masterskillname1=pmasterskillname1,
           c.masterskillname2=pmasterskillname2,
           c.masterskillname3=pmasterskillname3,
           c.recguid=precguid,
           c.torchlimit=ptorchlimit,
           c.czstarttime = pczstarttime,
           c.czendtime = pczendtime,
           c.czvalue = pczvalue,
           c.loverguid = ploverguid,
           c.loverstat = ploverstat,
           c.lovertime = plovertime,
           c.ngshareguid = pngshareguid,
           c.onlinetime = ponlinetime,
           c.unblocktime = punblocktime,
           c.unforbidtalktime = punforbidtalktime,
           c.lightskilllevel = plightskilllevel,
           c.crc = pcrc,        
           c.signinmonthid = psigninmonthid,       
           c.mergebonusflag = pmergebonusflag,        
           c.herosenderguid = pherosenderguid,
           c.juyisenderguid = pjuyisenderguid,
           c.bclosestep = pbclosestep,
           c.pworldid = ppworldid,
           c.pworldname = ppworldname,
           c.suiyu = psuiyu,
           c.totalexp = ptotalexp,    
           c.bpstep = pbpstep,
           c.bpnum = pbpnum,
           c.bpcodetime = pbpcodetime,
           c.bpcode = pbpcode,
           c.bpmessagecount = pbpmessagecount,
           c.bpbindtime = pbpbindtime,
           c.bpgetrewarded = pbpgetrewarded,
           c.cybpstep=pcybpstep,
           c.cybpnum=pcybpnum,
           c.cybpgetfirst=pcybpgetfirst,
           c.cybpgetweekly=pcybpgetweekly,
           c.secpwd=psecpwd,
           c.bpprenum=pbpprenum,
	   c.unforbidtradetime=punforbidtradetime,
	   c.activepskillid=pactivepskillid,
	   c.kejucuridx=pkejucuridx,
	   c.kejucurquestionid=pkejucurquestionid,
	   c.kejutodayrightcount=pkejutodayrightcount,
	   c.kejutotalrightcount=pkejutotalrightcount,
	   c.kejutodaystarttime=pkejutodaystarttime,
	   c.kejutotaltime=pkejutotaltime,
	   c.soonpkmodel=psoonpkmodel,
	   c.kejuweekstarttime=pkejuweekstarttime,
	   c.fumoshuijingcount=pfumoshuijingcount,
	   c.realsex=prealsex,
           c.onlinetimetype=ponlinetimetype,
           c.areanid=pareanid,
           c.tutorleavetime=ptutorleavetime,
           c.chushitime=pchushitime,
           c.teachexp=pteachexp,
           c.teachlevel=pteachlevel,
           c.markidaspupil=pmarkidaspupil,
           c.intenttutorpro=pintenttutorpro,
           c.markidastutor=pmarkidastutor,
           c.intentpupilpro=pintentpupilpro
       WHERE c.charguid=pcharguid AND c.isvalid=1;
       
       UPDATE t_char_ext1 AS c1 SET 
           c1.pklist=ppklist,
           c1.cdlist=pcdlist,
           c1.commondata=pcommondataext1,
           c1.commonflag=pcommonflag,
           c1.mflag=pmflag,
           c1.systemtitle=psystemtitle,
           c1.userdeftitle=puserdeftitle,
           c1.gemid=pgemid,
           c1.cpnumber=pcpnumber,
           c1.rfriendlist=prfriendlist              
       WHERE c1.charguid=pcharguid ;
       
	UPDATE t_char_ext2 AS c2 SET 
           c2.skilllist=pskilllist,
           c2.activefellowskill=pactivefellowskill,
           c2.dailymission=pdailymission,
           c2.fashiondeadline=pfashiondeadline,
           c2.dlduniquebonus=pdlduniquebonus,
           c2.limitgoodscount = plimitgoodscount,
           c2.mountdeadline=pmountdeadline,
           c2.signinfo = psigninfo,       
           c2.rewardlist = prewardlist,        
           c2.cpnumberex = pcpnumberex,        
           c2.juyireceiver = pjuyireceiver,
           c2.commonprdata = pcommonprdata,       
           c2.commondata = pcommondataext2,
		   c2.weekdiscountshopver = pweekdiscountshopver,
		   c2.weekdiscountgoods = pweekdiscountgoods,
		   c2.specialdiscountshopver = pspecialdiscountshopver,                  
		   c2.specialdiscountgoods = pspecialdiscountgoods,
		   c2.jieyigoods = pjieyigoods
       WHERE c2.charguid=pcharguid;
END IF;      
COMMIT;
END;//
DELIMITER ;


DELIMITER //
DROP PROCEDURE IF EXISTS createchar//
CREATE PROCEDURE createchar(
           pcharguid BIGINT,
           pcharname VARCHAR(80) BINARY,
           paccname VARCHAR(80) BINARY,
           pprofession TINYINT,
           plevel SMALLINT,
           pexp INT,
           php INT,
           pmp INT,
           pxp INT,
           pisvalid TINYINT,
           psceneid INT,
           pinstsceneid INT,
           psceneposx INT,
           psceneposz INT,
           pncpsceneid INT,
           pncpinstsceneid INT,
           pncpsceneposx INT,
           pncpsceneposz INT,
           plastdbexptime INT,
           pcurdbexpdura INT,
           plogouttime BIGINT,
           prankpos INT,
           pchtimes INT,
           pchbuytimes INT,
           pmertimes INT,
           ppkvalue INT,
           ppkmodel TINYINT,
           ppkcdtime INT, 
           ppkrecoverpass BIGINT,
           ppklist VARCHAR(513),
           pcdlist VARCHAR(513),
           pcommondataext1 VARCHAR(1025),
           pcommonflag VARCHAR(41),
           pmflag VARCHAR(257),
           psystemtitle VARCHAR(1025),
           puserdeftitle VARCHAR(1025),
           pactivetitle SMALLINT,
           pmoneycoin INT,
           pmoneyyb INT,
           pmoneyybbind INT,
           pstamina SMALLINT,
           pstaminacdpass BIGINT,
           pskillexp INT,
           pmtturn INT,
           pmtcdtime INT,
           pbclotimes INT,
           pbmatrixactive INT,
           pblasttime INT,
           pbmatrix VARCHAR(9),
           pmountflag VARCHAR(11),
           pawdid INT,
           pawdlefttime INT,
           pnsactivity VARCHAR(3),
           plastmailguid BIGINT,
           pskilllist VARCHAR(121),
           pbackpackopencount TINYINT,
           preachedsceneid VARCHAR(33),
           pactivefellowskill VARCHAR(257),
           pfellowcoingaincount INT,
           pfellowyuanbaogaincount INT,
           pgemid VARCHAR(481),
           pdailymission VARCHAR(161),
           pdailymissioncount INT,
           pactiveness INT,
           pactivenessflag VARCHAR(7),
           pfashionid INT,
           pshowfashion TINYINT,
           pfashiondeadline VARCHAR(321),
           pdldtime INT,
           pdlduniquebonus VARCHAR(241),
           pguildleavetime INT,
           pcpnumber VARCHAR(1281),
           prlevel SMALLINT,
           prexp INT,
           prfriendcount SMALLINT,
           prfriendlist VARCHAR(1585),
           ptorchvalue INT,
           pzhanji INT,
           pjinyaodai INT,
           plimitgoodscount VARCHAR(81),
           pactivefellowguid BIGINT,
           pvipcost INT,
           pofflineexp INT,
           pdailyexp INT,
           pfirstloginlevel INT,
           pfirstlogintime BIGINT,
           preputation INT,
           pswordstate INT,
           pswordscore INT,
           pcostyuanbao INT,
           pcreatetime BIGINT,
           pgender TINYINT,
           pfellowfreegaincount SMALLINT,
           ppayflag VARCHAR(5),
           pmonthcardbegin BIGINT,
           pmonthcardyb INT,
           pstorageopencount SMALLINT,
           pmasterskillname1 VARCHAR(50),
           pmasterskillname2 VARCHAR(50),
           pmasterskillname3 VARCHAR(50),
           ptorchlimit INT,
           pczstarttime BIGINT,
           pczendtime BIGINT,
           pczvalue INT,
           ploverguid BIGINT,
           ploverstat TINYINT,
           plovertime BIGINT,
           pngshareguid BIGINT,
           ponlinetime INT,
           punblocktime BIGINT,
           punforbidtalktime BIGINT,
           plightskilllevel INT,
           pcrc INT,
           psigninfo VARCHAR(113),
           psigninmonthid TINYINT,
           prewardlist VARCHAR(81),
           pmergebonusflag TINYINT,
           pcpnumberex VARCHAR(1025),
           pherosenderguid BIGINT,
           pjuyisenderguid BIGINT,
           pjuyireceiver VARCHAR(241),
           pcommonprdata VARCHAR(513),
           pbclosestep TINYINT,
           ppworldid INT,
           ppworldname VARCHAR(32),
           psuiyu INT,
           ptotalexp BIGINT,
           pcommondataext2 VARCHAR(2049),
           pbpstep TINYINT,pbpnum VARCHAR(12),pbpcodetime INT,pbpcode VARCHAR(5),pbpmessagecount SMALLINT,
		   pbpbindtime INT,pbpgetrewarded TINYINT,pcybpstep TINYINT,pcybpnum VARCHAR(12),pcybpgetfirst TINYINT,pcybpgetweekly TINYINT,
           psecpwd VARCHAR(7),pbpprenum VARCHAR(12),punforbidtradetime BIGINT, pactivepskillid INT,
		   pweekdiscountshopver INT, pweekdiscountgoods VARCHAR(49),pspecialdiscountshopver INT,pspecialdiscountgoods VARCHAR(49),
	   pjieyigoods VARCHAR(161),
	   pkejucuridx INT,
	   pkejucurquestionid INT,
	   pkejutodayrightcount INT,
	   pkejutotalrightcount INT,
	   pkejutodaystarttime BIGINT,
	   pkejutotaltime BIGINT,
	   psoonpkmodel TINYINT,
	   pkejuweekstarttime BIGINT,
	   pfumoshuijingcount INT,
	   prealsex tinyint,
	   ponlinetimetype tinyint,
	   pareanid int,
	   ptutorleavetime bigint,
	   pchushitime bigint,
	   pteachexp int,
	   pteachlevel smallint,
	   pmarkidaspupil int,
	   pintenttutorpro int,
	   pmarkidastutor int,
	   pintentpupilpro int)
BEGIN
DECLARE rguid  BIGINT DEFAULT -1;
DECLARE result INT DEFAULT -1;
DECLARE rcount INT DEFAULT 0;
DECLARE nindex INT DEFAULT -1;

START TRANSACTION;
SELECT COUNT(*) INTO rcount FROM t_char WHERE isvalid=1 AND accname=paccname;
IF rcount < 4 THEN
    SELECT charguid INTO rguid FROM t_char WHERE charname=pcharname LIMIT 1;
    IF FOUND_ROWS() = 0 THEN
        SELECT charguid INTO rguid FROM t_char WHERE charguid=pcharguid LIMIT 1;
        IF FOUND_ROWS() = 0 THEN
        INSERT INTO t_char(
                      charguid,
                      charname, 
                      accname,
                      profession, 
                      level, 
                      exp, 
                      hp, 
                      mp, 
                      xp,
                      isvalid,
                      sceneid,
                      instsceneid,
                      sceneposx,
                      sceneposz,
                      ncpsceneid,
                      ncpinstsceneid,
                      ncpsceneposx,
                      ncpsceneposz,
                      lastdbexptime,
                      curdbexpdura,
                      logouttime,
                      rankpos,
                      chtimes,
                      chbuytimes,
                      mertimes,
                      pkvalue,
                      pkmodel,
                      pkcdtime,
                      pkrecoverpass, 
                      activetitle,
                      moneycoin,
                      moneyyb,
                      moneyybbind,
                      stamina,
                      staminacdpass,
                      skillexp,
                      mtturn,
                      mtcdtime,
                      bclotimes,
                      bmatrixactive,
                      blasttime,
                      bmatrix,
                      mountflag,
                      awdid,
                      awdlefttime,
                      nsactivity,
                      lastmailguid,
                      backpackopencount,
                      reachedsceneid,
                      fellowcoingaincount,
                      fellowyuanbaogaincount,
                      dailymissioncount,
                      activeness,
                      activenessflag,
                      fashionid,
                      showfashion,
                      dldtime,
                      guildleavetime,
                      rlevel,
                      rexp,
                      rfriendcount,
                      torchvalue,
                      zhanji,
                      jinyaodai, 
                      activefellowguid,
                      vipcost,
                      offlineexp,
                      dailyexp,
                      firstloginlevel,
                      firstlogintime,
                      reputation,
                      swordstate,
                      swordscore,
                      costyuanbao,
                      createtime,
                      gender,
                      fellowfreegaincount,
                      payflag,
                      monthcardbegin,
                      monthcardyb,
                      storageopencount,
                      masterskillname1,
                      masterskillname2,
                      masterskillname3,
                      torchlimit,
                      czstarttime,
                      czendtime,
                      czvalue,
                      loverguid,
                      loverstat,
                      lovertime,
                      ngshareguid,
                      onlinetime,
                      unblocktime,
                      unforbidtalktime,
                      lightskilllevel,
                      crc,
                      signinmonthid,
                      mergebonusflag,
                      herosenderguid,
                      juyisenderguid,
                      bclosestep,
                      pworldid,
                      pworldname,
                      suiyu,
                      totalexp,
                      bpstep,bpnum,bpcodetime,
					  bpcode,bpmessagecount,bpbindtime,
					  bpgetrewarded,cybpstep,cybpnum ,cybpgetfirst ,cybpgetweekly,secpwd,bpprenum,unforbidtradetime,activepskillid,
					  kejucuridx,
					  kejucurquestionid,
					  kejutodayrightcount,
					  kejutotalrightcount,
					  kejutodaystarttime,
					  kejutotaltime,soonpkmodel,kejuweekstarttime,fumoshuijingcount,
					  realsex,onlinetimetype,areanid,tutorleavetime,chushitime,teachexp,teachlevel,markidaspupil,intenttutorpro,markidasTutor,intentpupilpro)values(
                      pcharguid,
                      pcharname, 
                      paccname,
                      pprofession, 
                      plevel, 
                      pexp, 
                      php, 
                      pmp, 
                      pxp,
                      pisvalid,
                      psceneid,
                      pinstsceneid,
                      psceneposx,
                      psceneposz,
                      pncpsceneid,
                      pncpinstsceneid,
                      pncpsceneposx,
                      pncpsceneposz,
                      plastdbexptime,
                      pcurdbexpdura,
                      plogouttime,
                      prankpos,
                      pchtimes,
                      pchbuytimes,
                      pmertimes,
                      ppkvalue,
                      ppkmodel,
                      ppkcdtime,
                      ppkrecoverpass, 
                      pactivetitle,
                      pmoneycoin,
                      pmoneyyb,
                      pmoneyybbind,
                      pstamina,
                      pstaminacdpass,
                      pskillexp,
                      pmtturn,
                      pmtcdtime,
                      pbclotimes,
                      pbmatrixactive,
                      pblasttime,
                      pbmatrix,
                      pmountflag,
                      pawdid,
                      pawdlefttime,
                      pnsactivity,
                      plastmailguid,
                      pbackpackopencount,
                      preachedsceneid,
                      pfellowcoingaincount,
                      pfellowyuanbaogaincount,
                      pdailymissioncount,
                      pactiveness,
                      pactivenessflag,
                      pfashionid,
                      pshowfashion,
                      pdldtime,
                      pguildleavetime,
                      prlevel,
                      prexp,
                      prfriendcount,
                      ptorchvalue,
                      pzhanji,
                      pjinyaodai,
                      pactivefellowguid,
                      pvipcost,
                      pofflineexp,
                      pdailyexp,
                      pfirstloginlevel,
                      pfirstlogintime,
                      preputation,
                      pswordstate,
                      pswordscore,
                      pcostyuanbao,
                      pcreatetime,
                      pgender,
                      pfellowfreegaincount,
                      ppayflag,
                      pmonthcardbegin,
                      pmonthcardyb,
                      pstorageopencount,
                      pmasterskillname1,
                      pmasterskillname2,
                      pmasterskillname3,
                      ptorchlimit,
                      pczstarttime,
                      pczendtime,
                      pczvalue,
                      ploverguid,
                      ploverstat,
                      plovertime,
                      pngshareguid,
                      ponlinetime,
                      punblocktime,
                      punforbidtalktime,
                      plightskilllevel,
                      pcrc,
                      psigninmonthid,
                      pmergebonusflag,
                      pherosenderguid,
                      pjuyisenderguid,
                      pbclosestep,
                      ppworldid,
                      ppworldname,
                      psuiyu,
                      ptotalexp,
                      pbpstep,
                      pbpnum,pbpcodetime,pbpcode,pbpmessagecount,pbpbindtime,pbpgetrewarded,pcybpstep,pcybpnum,
		  pcybpgetfirst,pcybpgetweekly,psecpwd,pbpprenum,punforbidtradetime,pactivepskillid,
		  pkejucuridx,
		  pkejucurquestionid,
		  pkejutodayrightcount,
		  pkejutotalrightcount,
		  pkejutodaystarttime,
		  pkejutotaltime,
		  psoonpkmodel,
		  pkejuweekstarttime,
		  pfumoshuijingcount,
		  prealsex,
		  ponlinetimetype,
		  pareanid,
		  ptutorleavetime,
		  pchushitime,
		  pteachexp,
		  pteachlevel,
		  pmarkidaspupil,
		  pintenttutorpro,
		  pmarkidastutor,
		  pintentpupilpro);
                      
        SELECT ROW_COUNT() INTO result;    
        
        INSERT INTO t_char_ext1(charguid,pklist,cdlist,commonflag,commondata,mflag,systemtitle,\
                                userdeftitle,rfriendlist,cpnumber,gemid)\
                         VALUES(pcharguid, ppklist,pcdlist,pcommonflag,pcommondataext1,pmflag,psystemtitle,\
                                puserdeftitle,prfriendlist,pcpnumber,pgemid);
                                
        INSERT INTO t_char_ext2(charguid,activefellowskill,skilllist,dailymission,fashiondeadline,\
                                dlduniquebonus,limitgoodscount,signinfo,rewardlist,cpnumberex,\
                                juyireceiver,commonprdata,commondata,weekdiscountshopver,\
								weekdiscountgoods,specialdiscountshopver,specialdiscountgoods,jieyigoods)
                         VALUES(pcharguid,pactivefellowskill,pskilllist,pdailymission,pfashiondeadline,\
                                pdlduniquebonus,plimitgoodscount,psigninfo,prewardlist,pcpnumberex,\
                                pjuyireceiver,pcommonprdata,pcommondataext2,pweekdiscountshopver,\
								pweekdiscountgoods,pspecialdiscountshopver,pspecialdiscountgoods,pjieyigoods);

	UPDATE t_randomname SET exist = 1 WHERE NAME = pcharname;

        ELSE SET result = -3;
        END IF;
    ELSE SET result = -2;
    END IF;
ELSE SET result = -4;
END IF;                           
COMMIT;
SELECT result;
END//
DELIMITER ;

DELIMITER //
DROP PROCEDURE IF EXISTS save_char_info_battle//
CREATE PROCEDURE save_char_info_battle(
           pcharguid BIGINT,
           pcharname VARCHAR(80) BINARY,
           paccname VARCHAR(80) BINARY,
           pprofession TINYINT,
           plevel SMALLINT,
           pexp INT,
           php INT,
           pmp INT,
           pxp INT,
           pisvalid TINYINT,
           psceneid INT,
           pinstsceneid INT,
           psceneposx INT,
           psceneposz INT,
           pncpsceneid INT,
           pncpinstsceneid INT,
           pncpsceneposx INT,
           pncpsceneposz INT,
           plastdbexptime INT,
           pcurdbexpdura INT,
           plogouttime BIGINT,
           prankpos INT,
           pchtimes INT,
           pchbuytimes INT,
           pmertimes INT,
           ppkvalue INT,
           ppkmodel TINYINT,
           ppkcdtime INT, 
           ppkrecoverpass BIGINT,
           ppklist VARCHAR(513),
           pcdlist VARCHAR(513),
           pcommondataext1 VARCHAR(1025),
           pcommonflag VARCHAR(41),
           pmflag VARCHAR(257),
           psystemtitle VARCHAR(1025),
           puserdeftitle VARCHAR(1025),
           pactivetitle SMALLINT,
           pmoneycoin INT,
           pmoneyyb INT,
           pmoneyybbind INT,
           pstamina SMALLINT,
           pstaminacdpass BIGINT,
           pskillexp INT,
           pmtturn INT,
           pmtcdtime INT,
           pbclotimes INT,
           pbmatrixactive INT,
           pblasttime INT,
           pbmatrix VARCHAR(9),
           pmountflag VARCHAR(11),
           pawdid INT,
           pawdlefttime INT,
           pnsactivity VARCHAR(3),
           plastmailguid BIGINT,
           pskilllist VARCHAR(121),
           pbackpackopencount TINYINT,
           preachedsceneid VARCHAR(33),
           pactivefellowskill VARCHAR(257),
           pfellowcoingaincount INT,
           pfellowyuanbaogaincount INT,
           pgemid VARCHAR(481),
           pdailymission VARCHAR(161),
           pdailymissioncount INT,
           pactiveness INT,
           pactivenessflag VARCHAR(7),
           pfashionid INT,
           pshowfashion TINYINT,
           pfashiondeadline VARCHAR(321),
           pdldtime INT,
           pdlduniquebonus VARCHAR(241),
           pguildleavetime INT,
           pcpnumber VARCHAR(1281),
           prlevel SMALLINT,
           prexp INT,
           prfriendcount SMALLINT,
           prfriendlist VARCHAR(1585),
           ptorchvalue INT,
           pzhanji INT,
           pjinyaodai INT,
           plimitgoodscount VARCHAR(81),
           pactivefellowguid BIGINT,
           pvipcost INT,
           pofflineexp INT,
           pdailyexp INT,
           pfirstloginlevel INT,
           pfirstlogintime BIGINT,
           preputation INT,
           pswordstate INT,
           pswordscore INT,
           pcostyuanbao INT,
           pcreatetime BIGINT,
           pgender TINYINT,
           pfellowfreegaincount SMALLINT,
           ppayflag VARCHAR(5),
           pmonthcardbegin BIGINT,
           pmonthcardyb INT,
           pmountdeadline VARCHAR(641),
           pstorageopencount SMALLINT,
           pmasterskillname1 VARCHAR(50),
           pmasterskillname2 VARCHAR(50),
           pmasterskillname3 VARCHAR(50),
           precguid BIGINT,
           ptorchlimit INT,
           pczstarttime BIGINT,
           pczendtime BIGINT,
           pczvalue INT,
           ploverguid BIGINT,
           ploverstat TINYINT,
           plovertime BIGINT,
           pngshareguid BIGINT,
           ponlinetime INT,
           punblocktime BIGINT,
           punforbidtalktime BIGINT,
           plightskilllevel INT,
           pcrc INT,
           psigninfo VARCHAR(113),
           psigninmonthid TINYINT,
           prewardlist VARCHAR(81),
           pmergebonusflag TINYINT,
           pcpnumberex VARCHAR(1025),
           pherosenderguid BIGINT,
           pjuyisenderguid BIGINT,
           pjuyireceiver VARCHAR(241),
           pcommonprdata VARCHAR(513),
           pbclosestep TINYINT,
           ppworldid INT,
           ppworldname VARCHAR(32),
           psuiyu INT,
           ptotalexp BIGINT,
           pcommondataext2 VARCHAR(2049),
           pbpstep TINYINT,pbpnum VARCHAR(12),pbpcodetime INT,pbpcode VARCHAR(5),pbpmessagecount SMALLINT,
		   pbpbindtime INT,pbpgetrewarded TINYINT,pcybpstep TINYINT,pcybpnum VARCHAR(12),pcybpgetfirst TINYINT,pcybpgetweekly TINYINT,
           psecpwd VARCHAR(7),pbpprenum VARCHAR(12),punforbidtradetime BIGINT, pactivepskillid INT,
		   pweekdiscountshopver INT, pweekdiscountgoods VARCHAR(49),pspecialdiscountshopver INT,pspecialdiscountgoods VARCHAR(49),
	   pjieyigoods VARCHAR(161),
	   pkejucuridx INT,
	   pkejucurquestionid INT,
	   pkejutodayrightcount INT,
	   pkejutotalrightcount INT,
	   pkejutodaystarttime BIGINT,
	   pkejutotaltime BIGINT,
	   psoonpkmodel TINYINT,
	   pkejuweekstarttime BIGINT,
	   pfumoshuijingcount INT,
	   prealsex tinyint,
	   ponlinetimetype tinyint,
	   pareanid int,
	   ptutorleavetime bigint,
	   pchushitime bigint,
	   pteachexp int,
	   pteachlevel smallint,
	   pmarkidaspupil int,
	   pintenttutorpro int,
	   pmarkidastutor int,
	   pintentpupilpro int)
BEGIN

DECLARE tempcharguid BIGINT DEFAULT -1;

START TRANSACTION;

SELECT charguid INTO tempcharguid FROM t_char WHERE charname = pcharname LIMIT 1;

IF tempcharguid <> -1 AND tempcharguid <> pcharguid THEN
    DELETE FROM t_char WHERE charguid=tempcharguid;
    DELETE FROM t_char_ext1 WHERE charguid=tempcharguid;
    DELETE FROM t_char_ext2 WHERE charguid=tempcharguid;
END IF;

SET tempcharguid = -1;

SELECT charguid INTO tempcharguid FROM t_char WHERE charguid = pcharguid LIMIT 1;

IF tempcharguid = pcharguid THEN

    UPDATE t_char AS c SET 
           c.charname=pcharname, 
           c.profession=pprofession, 
           c.level=plevel, 
           c.exp=pexp, 
           c.hp=php, 
           c.mp=pmp,
           c.xp=pxp,
           c.isvalid=pisvalid,
           c.sceneid=psceneid,
           c.instsceneid=pinstsceneid,
           c.sceneposx=psceneposx,
           c.sceneposz=psceneposz,
           c.ncpsceneid=pncpsceneid,
           c.ncpinstsceneid=pncpinstsceneid,
           c.ncpsceneposx=pncpsceneposx,
           c.ncpsceneposz=pncpsceneposz,
           c.lastdbexptime=plastdbexptime,
           c.curdbexpdura=pcurdbexpdura,
           c.logouttime=plogouttime,
           c.rankpos=prankpos,
           c.chtimes=pchtimes,
           c.chbuytimes=pchbuytimes,
           c.mertimes=pmertimes,
           c.pkvalue=ppkvalue,
           c.pkmodel=ppkmodel,
           c.pkcdtime=ppkcdtime,
           c.pkrecoverpass=ppkrecoverpass, 
           c.activetitle=pactivetitle,
           c.moneycoin=pmoneycoin,
           c.moneyyb=pmoneyyb,
           c.moneyybbind=pmoneyybbind,
           c.stamina=pstamina,
           c.staminacdpass=pstaminacdpass,
           c.skillexp=pskillexp,
           c.mtturn=pmtturn,
           c.mtcdtime=pmtcdtime,
           c.bclotimes=pbclotimes,
           c.bmatrixactive=pbmatrixactive,
           c.blasttime=pblasttime,
           c.bmatrix=pbmatrix,
           c.mountflag=pmountflag,
           c.awdid=pawdid,
           c.awdlefttime=pawdlefttime,
           c.nsactivity=pnsactivity,
           c.lastmailguid=plastmailguid,
           c.backpackopencount=pbackpackopencount,
           c.reachedsceneid=preachedsceneid,
           c.fellowcoingaincount=pfellowcoingaincount,
           c.fellowyuanbaogaincount=pfellowyuanbaogaincount,
           c.dailymissioncount=pdailymissioncount,
           c.activeness=pactiveness,
           c.activenessflag=pactivenessflag,
           c.fashionid=pfashionid,
           c.showfashion=pshowfashion,
           c.dldtime=pdldtime, 
           c.guildleavetime=pguildleavetime,        
           c.rlevel=prlevel,
           c.rexp=prexp,
           c.rfriendcount=prfriendcount,        
           c.torchvalue=ptorchvalue,
           c.zhanji=pzhanji,
           c.jinyaodai=pjinyaodai,        
           c.activefellowguid=pactivefellowguid,
           c.vipcost=pvipcost,
           c.offlineexp=pofflineexp,
           c.dailyexp=pdailyexp,
           c.firstloginlevel=pfirstloginlevel,
           c.firstlogintime=pfirstlogintime,
           c.reputation=preputation,
           c.swordstate=pswordstate,
           c.swordscore=pswordscore,
           c.costyuanbao=pcostyuanbao,
           c.gender=pgender,
           c.fellowfreegaincount=pfellowfreegaincount,
           c.payflag=ppayflag,
           c.monthcardbegin=pmonthcardbegin,
           c.monthcardyb=pmonthcardyb,
           c.storageopencount=pstorageopencount,
           c.masterskillname1=pmasterskillname1,
           c.masterskillname2=pmasterskillname2,
           c.masterskillname3=pmasterskillname3,
           c.recguid=precguid,
           c.torchlimit=ptorchlimit,
           c.czstarttime = pczstarttime,
           c.czendtime = pczendtime,
           c.czvalue = pczvalue,
           c.loverguid = ploverguid,
           c.loverstat = ploverstat,
           c.lovertime = plovertime,
           c.ngshareguid = pngshareguid,
           c.onlinetime = ponlinetime,
           c.unblocktime = punblocktime,
           c.unforbidtalktime = punforbidtalktime,
           c.lightskilllevel = plightskilllevel,
           c.crc = pcrc,
           c.signinmonthid = psigninmonthid,
           c.mergebonusflag = pmergebonusflag,
           c.herosenderguid = pherosenderguid,
           c.juyisenderguid = pjuyisenderguid,
           c.bclosestep = pbclosestep,
           c.pworldid = ppworldid,
           c.pworldname = ppworldname,
           c.suiyu = psuiyu,
           c.totalexp = ptotalexp,
           c.bpstep = pbpstep,
           c.bpnum = pbpnum,
           c.bpcodetime = pbpcodetime,
           c.bpcode = pbpcode,
           c.bpmessagecount = pbpmessagecount,
           c.bpbindtime = pbpbindtime,
           c.bpgetrewarded = pbpgetrewarded,
           c.cybpstep=pcybpstep,
           c.cybpnum=pcybpnum,
           c.cybpgetfirst=pcybpgetfirst,
           c.cybpgetweekly=pcybpgetweekly,
           c.secpwd=psecpwd,c.bpprenum=pbpprenum,
		   c.unforbidtradetime=punforbidtradetime,
		   c.activepskillid=pactivepskillid,
	   c.kejucuridx=pkejucuridx,
	   c.kejucurquestionid=pkejucurquestionid,
	   c.kejutodayrightcount=pkejutodayrightcount,
	   c.kejutotalrightcount=pkejutotalrightcount,
	   c.kejutodaystarttime=pkejutodaystarttime,
	   c.kejutotaltime=pkejutotaltime,
	   c.soonpkmodel=psoonpkmodel,
	   c.kejuweekstarttime=pkejuweekstarttime,
	   c.fumoshuijingcount=pfumoshuijingcount,
	   c.realsex=prealsex,
           c.onlinetimetype=ponlinetimetype,
           c.areanid=pareanid,
           c.tutorleavetime=ptutorleavetime,
           c.chushitime=pchushitime,
           c.teachexp=pteachexp,
           c.teachlevel=pteachlevel,
           c.markidaspupil=pmarkidaspupil,
           c.intenttutorpro=pintenttutorpro,
           c.markidastutor=pmarkidastutor,
           c.intentpupilpro=pintentpupilpro
       WHERE c.charguid=pcharguid AND c.isvalid=1;
       
 
       UPDATE t_char_ext1 AS c1 SET 
           c1.pklist=ppklist,
           c1.cdlist=pcdlist,
           c1.commondata=pcommondataext1,
           c1.commonflag=pcommonflag,
           c1.mflag=pmflag,
           c1.systemtitle=psystemtitle,
           c1.userdeftitle=puserdeftitle,
           c1.gemid=pgemid,
           c1.cpnumber=pcpnumber,
           c1.rfriendlist=prfriendlist
       WHERE c1.charguid=pcharguid ;
       

       UPDATE t_char_ext2 AS c2 SET 
           c2.skilllist=pskilllist,
           c2.activefellowskill=pactivefellowskill,      
           c2.dailymission=pdailymission,
           c2.fashiondeadline=pfashiondeadline,
           c2.dlduniquebonus=pdlduniquebonus,
           c2.limitgoodscount = plimitgoodscount,
           c2.mountdeadline=pmountdeadline,
           c2.signinfo = psigninfo,
           c2.rewardlist = prewardlist,
           c2.cpnumberex = pcpnumberex,     
           c2.juyireceiver = pjuyireceiver,
           c2.commonprdata = pcommonprdata,
           c2.commondata = pcommondataext2,
		   c2.weekdiscountshopver = pweekdiscountshopver,
		   c2.weekdiscountgoods = pweekdiscountgoods,
		   c2.specialdiscountshopver = pspecialdiscountshopver,
		   c2.specialdiscountgoods = pspecialdiscountgoods,
		   c2.jieyigoods = pjieyigoods
		   WHERE c2.charguid=pcharguid ;
       
       
ELSE
    INSERT INTO t_char(
                      charguid,
                      charname, 
                      accname,
                      profession, 
                      level, 
                      exp, 
                      hp, 
                      mp, 
                      xp,
                      isvalid,
                      sceneid,
                      instsceneid,
                      sceneposx,
                      sceneposz,
                      ncpsceneid,
                      ncpinstsceneid,
                      ncpsceneposx,
                      ncpsceneposz,
                      lastdbexptime,
                      curdbexpdura,
                      logouttime,
                      rankpos,
                      chtimes,
                      chbuytimes,
                      mertimes,
                      pkvalue,
                      pkmodel,
                      pkcdtime,
                      pkrecoverpass, 
                      activetitle,
                      moneycoin,
                      moneyyb,
                      moneyybbind,
                      stamina,
                      staminacdpass,
                      skillexp,
                      mtturn,
                      mtcdtime,
                      bclotimes,
                      bmatrixactive,
                      blasttime,
                      bmatrix,
                      mountflag,
                      awdid,
                      awdlefttime,
                      nsactivity,
                      lastmailguid,
                      backpackopencount,
                      reachedsceneid,
                      fellowcoingaincount,
                      fellowyuanbaogaincount,
                      dailymissioncount,
                      activeness,
                      activenessflag,
                      fashionid,
                      showfashion,
                      dldtime,
                      guildleavetime,
                      rlevel,
                      rexp,
                      rfriendcount,
                      torchvalue,
                      zhanji,
                      jinyaodai, 
                      activefellowguid,
                      vipcost,
                      offlineexp,
                      dailyexp,
                      firstloginlevel,
                      firstlogintime,
                      reputation,
                      swordstate,
                      swordscore,
                      costyuanbao,
                      createtime,
                      gender,
                      fellowfreegaincount,
                      payflag,
                      monthcardbegin,
                      monthcardyb,
                      storageopencount,
                      masterskillname1,
                      masterskillname2,
                      masterskillname3,
                      torchlimit,
                      czstarttime,
                      czendtime,
                      czvalue,
                      loverguid,
                      loverstat,
                      lovertime,
                      ngshareguid,
                      onlinetime,
                      unblocktime,
                      unforbidtalktime,
                      lightskilllevel,
                      crc,
                      signinmonthid,
                      mergebonusflag,
                      herosenderguid,
                      juyisenderguid,
                      bclosestep,
                      pworldid,
                      pworldname,
                      suiyu,
                      totalexp,
                      bpstep,bpnum,bpcodetime,bpcode,
					  bpmessagecount,bpbindtime,bpgetrewarded,cybpstep,cybpnum,cybpgetfirst,cybpgetweekly,secpwd,bpprenum,unforbidtradetime,activepskillid,
		      kejucuridx,
		      kejucurquestionid,
		      kejutodayrightcount,
		      kejutotalrightcount,
		      kejutodaystarttime,
		      kejutotaltime,soonpkmodel,kejuweekstarttime,fumoshuijingcount,
		      realsex,onlinetimetype,areanid,tutorleavetime,chushitime,teachexp,teachlevel,markidaspupil,intenttutorpro,markidastutor,intentpupilpro)values(
                      pcharguid,
                      pcharname, 
                      paccname,
                      pprofession, 
                      plevel, 
                      pexp, 
                      php, 
                      pmp, 
                      pxp,
                      pisvalid,
                      psceneid,
                      pinstsceneid,
                      psceneposx,
                      psceneposz,
                      pncpsceneid,
                      pncpinstsceneid,
                      pncpsceneposx,
                      pncpsceneposz,
                      plastdbexptime,
                      pcurdbexpdura,
                      plogouttime,
                      prankpos,
                      pchtimes,
                      pchbuytimes,
                      pmertimes,
                      ppkvalue,
                      ppkmodel,
                      ppkcdtime,
                      ppkrecoverpass, 
                      pactivetitle,
                      pmoneycoin,
                      pmoneyyb,
                      pmoneyybbind,
                      pstamina,
                      pstaminacdpass,
                      pskillexp,
                      pmtturn,
                      pmtcdtime,
                      pbclotimes,
                      pbmatrixactive,
                      pblasttime,
                      pbmatrix,
                      pmountflag,
                      pawdid,
                      pawdlefttime,
                      pnsactivity,
                      plastmailguid,
                      pbackpackopencount,
                      preachedsceneid,
                      pfellowcoingaincount,
                      pfellowyuanbaogaincount,
                      pdailymissioncount,
                      pactiveness,
                      pactivenessflag,
                      pfashionid,
                      pshowfashion,
                      pdldtime,
                      pguildleavetime,
                      prlevel,
                      prexp,
                      prfriendcount,
                      ptorchvalue,
                      pzhanji,
                      pjinyaodai,
                      pactivefellowguid,
                      pvipcost,
                      pofflineexp,
                      pdailyexp,
                      pfirstloginlevel,
                      pfirstlogintime,
                      preputation,
                      pswordstate,
                      pswordscore,
                      pcostyuanbao,
                      pcreatetime,
                      pgender,
                      pfellowfreegaincount,
                      ppayflag,
                      pmonthcardbegin,
                      pmonthcardyb,
                      pstorageopencount,
                      pmasterskillname1,
                      pmasterskillname2,
                      pmasterskillname3,
                      ptorchlimit,
                      pczstarttime,
                      pczendtime,
                      pczvalue,
                      ploverguid,
                      ploverstat,
                      plovertime,
                      pngshareguid,
                      ponlinetime,
                      punblocktime,
                      punforbidtalktime,
                      plightskilllevel,
                      pcrc,
                      psigninmonthid,
                      pmergebonusflag,
                      pherosenderguid,
                      pjuyisenderguid,
                      pbclosestep,
                      ppworldid,
                      ppworldname,
                      psuiyu,
                      ptotalexp,
                      pbpstep,pbpnum,pbpcodetime,pbpcode,
					  pbpmessagecount,pbpbindtime,pbpgetrewarded,pcybpstep,pcybpnum,pcybpgetfirst,pcybpgetweekly,psecpwd,pbpprenum,punforbidtradetime,pactivepskillid,
		      pkejucuridx,
		      pkejucurquestionid,
		      pkejutodayrightcount,
		      pkejutotalrightcount,
		      pkejutodaystarttime,
		      pkejutotaltime,
		      psoonpkmodel,
		      pkejuweekstarttime,
		      pfumoshuijingcount,
		      prealsex,
	              ponlinetimetype,
		      pareanid,
		      ptutorleavetime,
		      pchushitime,
		      pteachexp,
		      pteachlevel,
		      pmarkidaspupil,
		      pintenttutorpro,
		      pmarkidastutor,
		      pintentpupilpro);
        
        insert into t_char_ext1(charguid,pklist,cdlist,commonflag,commondata,mflag,systemtitle,\
                                userdeftitle,rfriendlist,cpnumber,gemid)\
                         values(pcharguid, ppklist,pcdlist,pcommonflag,pcommondataext1,pmflag,psystemtitle,\
                                puserdeftitle,prfriendlist,pcpnumber,pgemid);
                                
        insert into t_char_ext2(charguid,activefellowskill,skilllist,dailymission,fashiondeadline,\
                                dlduniquebonus,limitgoodscount,signinfo,rewardlist,cpnumberex,\
                                juyireceiver,commonprdata,commondata,weekdiscountshopver,
								weekdiscountgoods,specialdiscountshopver,specialdiscountgoods,jieyigoods)
                         values(pcharguid,pactivefellowskill,pskilllist,pdailymission,pfashiondeadline,\
                                pdlduniquebonus,plimitgoodscount,psigninfo,prewardlist,pcpnumberex,\
                                pjuyireceiver,pcommonprdata,pcommondataext2,pweekdiscountshopver,
								pweekdiscountgoods,pspecialdiscountshopver,pspecialdiscountgoods,pjieyigoods);
end if;
commit;
end;//
delimiter ;

delimiter //
drop procedure if exists loadcharlist//
create procedure loadcharlist(paccname varchar(80) binary)
begin
select 
   c.charguid, 
   c.charname,
   c.profession, 
   c.level,
   c.fashionid,
   c.showfashion,
   c.gender,
   c1.gemid,
   c.unblocktime
from t_char as c,t_char_ext1 as c1 where c.accname=paccname and c.isvalid=1 and c.charguid=c1.charguid order by c.level desc limit 4;
end;//
delimiter ;

delimiter //
drop procedure if exists loadcharlist_battle//
create procedure loadcharlist_battle(paccname varchar(80) binary)
begin
select 
   c.charguid, 
   c.charname,
   c.profession, 
   c.level,
   c.fashionid,
   c.showfashion,
   c.gender,
   c1.gemid,
   c.unblocktime
from t_char as c,t_char_ext1 as c1 where c.accname=paccname and c.isvalid=1 and c.charguid=c1.charguid order by c.logouttime desc limit 4;
end;//
delimiter ;

delimiter //
drop procedure if exists loadrestaurantonuser//
create procedure loadrestaurantonuser(pcharguid bigint)
begin
select 
   c.rlevel, 
   c.rexp,
   c.rfriendcount, 
   c1.rfriendlist  
from t_char as c,t_char_ext1 as c1 where c.charguid=pcharguid and c1.charguid=pcharguid and c.isvalid=1;
end;//
delimiter ;

delimiter //
drop procedure if exists loadcharvip//
create procedure loadcharvip(paccname varchar(80) binary)
begin
select 
   vipcost 
from t_char where accname=paccname and isvalid=1;
end;//
delimiter ;

delimiter //
drop procedure if exists savecharvip//
create procedure savecharvip(pcharguid bigint,
                             pvipcost int)
begin
update t_char set vipcost= pvipcost where charguid=pcharguid;
end;//
delimiter ;

delimiter //
drop procedure if exists savecharunblocktime//
create procedure savecharunblocktime(pcharguid bigint,
                                     punblocktime bigint)
begin
update t_char set unblocktime= punblocktime where charguid=pcharguid;
end;//
delimiter ;

delimiter //
drop procedure if exists savecharunforbidtalktime//
create procedure savecharunforbidtalktime(pcharguid bigint,
                                          punforbidtalktime bigint)
begin
update t_char set unforbidtalktime= punforbidtalktime where charguid=pcharguid;
end;//
delimiter ;

/*==============================================================*/
/* Table: t_mission;             */
/*==============================================================*/
drop table if exists t_mission;
create table if not exists t_mission
(
   aid                            bigint                         not null AUTO_INCREMENT,
   charguid                       bigint                         not null default -1,
   missionid                      int                            not null default -1,
   quality                        tinyint                        not null default 0,
   status                         tinyint                        not null default 0,
   flag                           tinyint                        not null default 0,
   p1                             int                            not null default 0,
   p2                             int                            not null default 0,
   p3                             int                            not null default 0,
   p4                             int                            not null default 0,
   p5                             int                            not null default 0,
   p6                             int                            not null default 0,
   p7                             int                            not null default 0,
   p8                             int                            not null default 0,
   isvalid                        tinyint                        not null default 1,
   primary key (aid)
)ENGINE = INNODB;

create index Index_mi_charguid on t_mission
(
   charguid
);

delimiter //
drop procedure if exists loadusermission//
create procedure loadusermission(
        pcharguid       bigint)
begin
select  charguid,missionid,quality,status,flag,p1,p2,p3,p4,p5,p6,p7,p8\
from t_mission where charguid=pcharguid and isvalid=1;
end;//
delimiter ;

delimiter //
drop procedure if exists deleteusermission//
create procedure deleteusermission(
        pcharguid       bigint)
begin
delete from t_mission where charguid=pcharguid;
end;//
delimiter ;

delimiter //
drop procedure if exists saveusermission //
create procedure saveusermission(
in pcharguid bigint,
in pdatas mediumtext
)
begin
declare stmt mediumtext default '';
declare rowcount int default 0;
declare textlength int default 0;

start transaction;

select count(*) into rowcount from t_mission where charguid=pcharguid;
if rowcount > 0 then
    delete from t_mission where charguid=pcharguid;
end if;

select LENGTH(pdatas) into textlength;

if textlength > 0 then

    set @sqlstr = "insert into t_mission(charguid,missionid,quality,status,flag,p1,p2,p3,p4,p5,p6,p7,p8) values";
    set @sqlstr = concat(@sqlstr,pdatas);

    prepare stmt from @sqlstr;
    execute stmt;
    deallocate prepare stmt;

end if;

commit;
end;//
delimiter ;

/*==============================================================*/
/* Table: t_item;             */
/* t_mail t_consignsale表包含t_item表相关列，t_item表有修改时，*/
/* 辛苦确认是否需要同步修改t_mail t_consignsale相关列*/
/*==============================================================*/
drop table if exists t_item;
create table if not exists t_item
(
  aid                             bigint                        not null AUTO_INCREMENT,
  charguid                        bigint                        not null default -1,
  guid                            bigint                        not null default -1,
  dataid                          int                           not null default -1,
  binded                          tinyint                       not null default -1,
  stackcount                      smallint                      not null default -1,
  createtime                      bigint                        not null default -1,
  enchancelevel                   smallint                      not null default -1,
  enchanceexp                     int                           not null default -1,
  enchancetotalexp                bigint                        not null default -1,
  starlevel                       smallint                      not null default -1,
  startimes                       smallint                      not null default -1,
  dynamicdata1                    int                           not null default -1,
  dynamicdata2                    int                           not null default -1,
  dynamicdata3                    int                           not null default -1,
  dynamicdata4                    int                           not null default -1,
  dynamicdata5                    int                           not null default -1,
  dynamicdata6                    int                           not null default -1,
  dynamicdata7                    int                           not null default -1,
  dynamicdata8                    int                           not null default -1,
  packtype                        smallint                      not null default -1,
  packindex                       smallint                      not null default -1,
  isvalid                         tinyint                       not null default 1,
  primary key (aid)              
)ENGINE = INNODB;

create unique index Index_item_charguid on t_item
( 
  charguid,
  packtype,
  packindex 
);

create index Index_item_itemguid on t_item
(
  guid,
  isvalid 
);

delimiter //
drop procedure if exists loaditempack //
create procedure loaditempack(pcharguid   bigint)
begin
select charguid,guid,dataid,binded,stackcount,createtime,enchancelevel,enchanceexp,enchancetotalexp,starlevel,startimes,dynamicdata1,dynamicdata2,dynamicdata3,dynamicdata4,dynamicdata5,dynamicdata6,dynamicdata7,dynamicdata8,packtype,packindex\
from t_item where charguid=pcharguid and isvalid=1;
end; //
delimiter ;

delimiter //
drop procedure if exists deleteitempack //
create procedure deleteitempack(pcharguid   bigint)
begin
update t_item set isvalid=0
where charguid=pcharguid and isvalid=1;
end; //
delimiter ;

delimiter //
drop procedure if exists saveitempack //
create procedure saveitempack(
in pcharguid bigint,
in pdatas mediumtext
)
begin
declare stmt mediumtext default '';
declare rowcount int default 0;
declare textlength int default 0;

start transaction;

select count(*) into rowcount from t_item where charguid=pcharguid;
if rowcount > 0 then
    delete from t_item where charguid=pcharguid;
end if;

select LENGTH(pdatas) into textlength;

if textlength > 0 then 

    set @sqlstr = "insert into t_item(charguid,guid,dataid,binded,stackcount,createtime,enchancelevel,enchanceexp,enchancetotalexp,starlevel,startimes,dynamicdata1,dynamicdata2,dynamicdata3,dynamicdata4,dynamicdata5,dynamicdata6,dynamicdata7,dynamicdata8,packtype,packindex,isvalid) values";
    set @sqlstr = concat(@sqlstr,pdatas);

    prepare stmt from @sqlstr;
    execute stmt;
    deallocate prepare stmt;

end if;

commit;
end;//
delimiter ;

delimiter //
drop procedure if exists loadcharlistequip //
create procedure loadcharlistequip(pcharguid   bigint)
begin
select packindex,dataid
from t_item where charguid=pcharguid and packtype=1 and isvalid=1;
end; //
delimiter ;
/*==============================================================*/
/* Table: t_general_set;             */
/*==============================================================*/
drop table if exists t_general_set;
create table if not exists t_general_set
(
   aid                            bigint                         not null AUTO_INCREMENT,
   sKey                           varchar(50)                    not null,
   nVal                           int                            not null default 0,
   primary key (aid)
)ENGINE = INNODB;

create unique index Index_general_set_Index on t_general_set
(
   sKey
);

delimiter //
drop procedure if exists save_general_set//
create procedure save_general_set(
          psKey                 varchar(50),
          pnVal                 int)
begin
declare _sKey varchar(50) default NULL;
start transaction;
select sKey into _sKey from t_general_set where sKey = psKey;
if _sKey = psKey then
                 update t_general_set set sKey=psKey,
                                          nVal=pnVal 
                 where  sKey = psKey;
else
                 insert into t_general_set(sKey,      
                                           nVal) values (psKey,       
                                           pnVal);
end if;
commit;
end;//
delimiter ;

delimiter //
drop procedure if exists load_general_set//
create procedure load_general_set(psKey varchar(50))
begin
select nVal from t_general_set where sKey=psKey;
end;//
delimiter ;
/*==============================================================*/
/* Table: t_belle;             */
/*==============================================================*/
drop table if exists t_belle;
create table if not exists t_belle
(
  aid                 bigint             not null AUTO_INCREMENT,
  charguid            bigint             not null default -1,
  belleindex          smallint           not null default -1,
  level               smallint           not null default 0,
  matrixid            tinyint            not null default -1,
  matirxindex         tinyint            not null default -1,
  lastevoltime        int                not null default 0,
  favourval           int                not null default 0,
  isvalid             tinyint            not null default 0,
  closeattr           int                not null default 0,
  primary key (aid)              
)ENGINE = INNODB;

create unique index Index_belle_charguid on t_belle
( 
  charguid,
  belleindex
);

delimiter //
drop procedure if exists loadbelle //
create procedure loadbelle(pcharguid   bigint)
begin
select charguid,belleindex,level,matrixid,matirxindex,lastevoltime,favourval,closeattr\
from t_belle where charguid=pcharguid and isvalid=1;
end; //
delimiter ;

delimiter //
drop procedure if exists deletebelle //
create procedure deletebelle(pcharguid   bigint)
begin
update t_belle set isvalid=0
where charguid=pcharguid and isvalid=1;
end; //
delimiter ;

delimiter //
drop procedure if exists savebelle //
create procedure savebelle(
in pcharguid bigint,
in pdatas mediumtext
)
begin
declare stmt mediumtext default '';
declare rowcount int default 0;
declare textlength int default 0;

start transaction;

select count(*) into rowcount from t_belle where charguid=pcharguid;
if rowcount > 0 then
    delete from t_belle where charguid=pcharguid;
end if;

select LENGTH(pdatas) into textlength;

if textlength > 0 then
    set @sqlstr = "insert into t_belle (charguid,belleindex,level,matrixid,matirxindex,lastevoltime,favourval,isvalid,closeattr) values";
    set @sqlstr = concat(@sqlstr,pdatas);

    prepare stmt from @sqlstr;
    execute stmt;
    deallocate prepare stmt;
end if;

commit;
end;//
delimiter ;

/*==============================================================*/
/* Table: t_relation;             */
/*==============================================================*/
drop table if exists t_relation;
create table if not exists t_relation
(
  aid                             bigint                        not null AUTO_INCREMENT,
  charguid                        bigint                        not null default -1,
  guid                            bigint                        not null default -1,
  name                            varchar(80) binary            not null default '',
  level                           smallint                      not null default -1,
  profession                      tinyint                       not null default -1,
  combatnum                       int                           not null default -1,
  listtype                        tinyint                       not null default -1,
  isvalid                         tinyint                       not null default 1,
  timeinfo                        bigint                        not null default 0,
  declarewar                      tinyint                       not null default 0,
  declarewartime                  bigint                        not null default 0,
  attentionstate                  tinyint                       not null default 0,
  primary key (aid)              
)ENGINE = INNODB;

create index Index_relation_charguid on t_relation
( 
  charguid
);

delimiter //
drop procedure if exists loadrelation //
create procedure loadrelation(pcharguid   bigint)
begin
select charguid,guid,name,level,profession,combatnum,listtype,isvalid,timeinfo,declarewar,declarewartime,attentionstate\
from t_relation where charguid=pcharguid and isvalid=1;
end; //
delimiter ;

delimiter //
drop procedure if exists deleterelation //
create procedure deleterelation(pcharguid   bigint)
begin
delete from t_relation where charguid=pcharguid;
end; //
delimiter ;


delimiter //
drop procedure if exists saverelation //
create procedure saverelation(
in pcharguid bigint,
in pdatas mediumtext
)
begin
declare stmt mediumtext default '';
declare rowcount int default 0;
declare textlength int default 0;

start transaction;

select count(*) into rowcount from t_relation where charguid=pcharguid;
if rowcount > 0 then
    delete from t_relation where charguid=pcharguid;
end if;

select LENGTH(pdatas) into textlength;

if textlength > 0 then

    set @sqlstr = "insert into t_relation(charguid,guid,name,level,profession,combatnum,listtype,isvalid,timeinfo,declarewar,declarewartime,attentionstate) values";
    set @sqlstr = concat(@sqlstr,pdatas);

    prepare stmt from @sqlstr;
    execute stmt;
    deallocate prepare stmt;

end if;

commit;
end;//
delimiter ;
/*==============================================================*/
/* Table: t_impact;             */
/*==============================================================*/
drop table if exists t_impact;
create table if not exists t_impact
(
  aid                             bigint                        not null AUTO_INCREMENT,
  charguid                        bigint                        not null default -1,
  id                              int                           not null default -1,
  skillid                         int                           not null default -1,
  sendtype                        int                           not null default -1,
  senderguid                      bigint                        not null default -1,
  sceneclassid                    int                           not null default -1,
  sceneinstid                     int                           not null default -1,
  sendobjid                       int                           not null default -1,
  contime                         int                           not null default 0,
  efftime                         bigint                        not null default 0,
  inttime                         int                           not null default 0,
  intelapsed                      int                           not null default 0,
  delaytime                       int                           not null default 0,
  wrapcnt                         tinyint                       not null default 0,
  param                           varchar(65)                   not null,
  isvalid                         tinyint                       not null default 0,
  primary key (aid)              
)ENGINE = INNODB;

create index Index_impact_charguid on t_impact
( 
  charguid
);

delimiter //
drop procedure if exists loadimpact //
create procedure loadimpact(pcharguid   bigint)
begin
select \
    charguid,
    id,
    skillid,
    sendtype,
    senderguid,
    sceneclassid,
    sceneinstid,
    sendobjid,
    contime,
    efftime,
    inttime,
    intelapsed,
    delaytime,
    wrapcnt,
    param
from t_impact where charguid=pcharguid and isvalid=1;
end; //
delimiter ;

delimiter //
drop procedure if exists deleteimpact //
create procedure deleteimpact(pcharguid   bigint)
begin
delete from t_impact where charguid=pcharguid;
end; //
delimiter ;

delimiter //
drop procedure if exists saveimpact //
create procedure saveimpact(
in pcharguid bigint,
in pdatas mediumtext
)
begin
declare stmt mediumtext default '';
declare rowcount int default 0;
declare textlength int default 0;

start transaction;

select count(*) into rowcount from t_impact where charguid=pcharguid;
if rowcount > 0 then
    delete from t_impact where charguid=pcharguid;
end if;

select LENGTH(pdatas) into textlength;

if textlength > 0 then 

    set @sqlstr = "insert into t_impact(charguid,id,skillid,sendtype,senderguid,sceneclassid,sceneinstid,sendobjid,contime,efftime,inttime,intelapsed,delaytime,wrapcnt,param,isvalid) values";
    set @sqlstr = concat(@sqlstr,pdatas);

    prepare stmt from @sqlstr;
    execute stmt;
    deallocate prepare stmt;
    
end if;

commit;
end;//
delimiter ;

/*==============================================================*/
/* Table: t_mail;             */
/* t_mail表包含t_item表相关列，t_item表有修改时，*/
/* 辛苦确认是否需要同步修改t_mail相关列*/
/* t_mail表不能再增加唯一索引*/
/*==============================================================*/
drop table if exists t_mail;
create table if not exists t_mail
(
  aid                             bigint                        not null AUTO_INCREMENT,
  guid                            bigint                        not null default -1,
  sendguid                        bigint                        not null default -1,
  sendname                        varchar(80) binary            not null,
  writetime                       bigint                        not null default  0,
  receiveguid                     bigint                        not null default -1,
  readtime                        bigint                        not null default  0,
  mailtype                        smallint                      not null default  0,
  content                         varchar(192)                  not null,
  moneytype                       smallint                      not null default  0,
  moneyvalue                      int                           not null default  0,  
  boxtype                         tinyint                       not null default  0,
  isvalid                         tinyint                       not null default  0,
  itemguid                        bigint                        not null default -1,
  dataid                          int                           not null default -1,
  binded                          tinyint                       not null default -1,
  stackcount                      smallint                      not null default -1,
  createtime                      bigint                        not null default -1,
  enchancelevel                   smallint                      not null default -1,
  enchanceexp                     int                           not null default -1,
  enchancetotalexp                bigint                        not null default -1,
  starlevel                       smallint                      not null default -1,
  startimes                       smallint                      not null default -1,
  dynamicdata1                    int                           not null default -1,
  dynamicdata2                    int                           not null default -1,
  dynamicdata3                    int                           not null default -1,
  dynamicdata4                    int                           not null default -1,
  dynamicdata5                    int                           not null default -1,
  dynamicdata6                    int                           not null default -1,
  dynamicdata7                    int                           not null default -1,
  dynamicdata8                    int                           not null default -1,
  origin                          int                           not null default 0,
  primary key (aid)              
)ENGINE = INNODB;

create unique index Index_mail_guid on t_mail
( 
  guid,
  boxtype
);
create index Index_mail_charguid on t_mail
( 
  receiveguid,
  boxtype
);


delimiter //
drop procedure if exists loadmail //
create procedure loadmail(pcharguid bigint,pboxtype tinyint)
begin
if pboxtype = 1 then
select guid,sendguid,sendname,writetime,receiveguid,readtime,mailtype,content,moneytype,moneyvalue,\
  boxtype,isvalid,itemguid,dataid,binded,stackcount,createtime,enchancelevel,enchanceexp,enchancetotalexp,starlevel,\
  startimes,dynamicdata1,dynamicdata2,dynamicdata3,dynamicdata4,dynamicdata5,dynamicdata6,dynamicdata7,dynamicdata8,origin\
from t_mail where boxtype=pboxtype and isvalid=1;
elseif pboxtype = 2 then 
  select guid,sendguid,sendname,writetime,receiveguid,readtime,mailtype,content,moneytype,moneyvalue,\
  boxtype,isvalid,itemguid,dataid,binded,stackcount,createtime,enchancelevel,enchanceexp,enchancetotalexp,starlevel,\
  startimes,dynamicdata1,dynamicdata2,dynamicdata3,dynamicdata4,dynamicdata5,dynamicdata6,dynamicdata7,dynamicdata8,origin\
from t_mail where receiveguid = pcharguid and boxtype=pboxtype and isvalid=1;
end if;
end; //
delimiter ;


delimiter //
drop procedure if exists deletemail //
create procedure deletemail(pcharguid bigint,pboxtype tinyint)
begin
if pboxtype = 1 then
  update t_mail set isvalid=0 where boxtype=pboxtype;
elseif pboxtype = 2 then 
  update t_mail set isvalid=0 where receiveguid = pcharguid and boxtype=pboxtype;
end if;
end; //
delimiter ;

delimiter //
drop procedure if exists savemail //
create procedure savemail(pdatas mediumtext)
begin
declare stmt mediumtext default '';

start transaction;

set @sqlstr = concat("insert into t_mail(guid,sendguid,sendname,writetime,receiveguid,readtime,mailtype,content,moneytype,moneyvalue,boxtype,isvalid,itemguid,dataid,binded,stackcount,createtime,enchancelevel,enchanceexp,enchancetotalexp,starlevel,startimes,dynamicdata1,dynamicdata2,dynamicdata3,dynamicdata4,dynamicdata5,dynamicdata6,dynamicdata7,dynamicdata8,origin) values ", pdatas, "on duplicate key update sendguid=values(sendguid),sendname=values(sendname),writetime=values(writetime),receiveguid=values(receiveguid),readtime=values(readtime),mailtype=values(mailtype),content=values(content),moneytype=values(moneytype),moneyvalue=values(moneyvalue),isvalid=values(isvalid),itemguid=values(itemguid),dataid=values(dataid),binded=values(binded),stackcount=values(stackcount),createtime=values(createtime),enchancelevel=values(enchancelevel),enchanceexp=values(enchanceexp),enchancetotalexp=values(enchancetotalexp),starlevel=values(starlevel),startimes=values(startimes),dynamicdata1=values(dynamicdata1),dynamicdata2=values(dynamicdata2),dynamicdata3=values(dynamicdata3),dynamicdata4=values(dynamicdata4),dynamicdata5=values(dynamicdata5),dynamicdata6=values(dynamicdata6),dynamicdata7=values(dynamicdata7),dynamicdata8=values(dynamicdata8),origin=values(origin)");

prepare stmt from @sqlstr;
execute stmt;
deallocate prepare stmt;

commit;
end; //
delimiter ;

delimiter //
drop procedure if exists saveusermail //
create procedure saveusermail(
in pcharguid bigint,
in pdatas mediumtext
)
begin
declare stmt mediumtext default '';
declare rowcount int default 0;
declare textlength int default 0;

start transaction;

select count(*) into rowcount from t_usermail where receiveguid=pcharguid and boxtype=2;
if rowcount > 0 then
    delete from t_usermail where receiveguid=pcharguid and boxtype=2;
end if;

select LENGTH(pdatas) into textlength;

if textlength > 0 then 

    set @sqlstr = "insert into t_usermail(guid,sendguid,sendname,writetime,receiveguid,readtime,mailtype,content,moneytype,moneyvalue,boxtype,isvalid,itemguid,dataid,binded,stackcount,createtime,enchancelevel,enchanceexp,enchancetotalexp,starlevel,startimes,dynamicdata1,dynamicdata2,dynamicdata3,dynamicdata4,dynamicdata5,dynamicdata6,dynamicdata7,dynamicdata8,origin) values";
    set @sqlstr = concat(@sqlstr,pdatas);

    prepare stmt from @sqlstr;
    execute stmt;
    deallocate prepare stmt;

end if;

commit;
end;//
delimiter ;

/*==============================================================*/
/* Table: t_chrank;             */
/*==============================================================*/
drop table if exists t_chrank;
create table if not exists t_chrank
(
  aid                             bigint                        not null AUTO_INCREMENT,
  guid                            bigint                        not null default -1,
  level                           smallint                      not null default 0,
  profession                      tinyint                       not null default -1,
  combatnum                       int                           not null default 0,
  rankpos                         smallint                      not null default -1,
  fashionid                       int                           not null default -1,
  weapon                          int                           not null default -1,
  name                            varchar(80) binary            not null,
  refixval                        varchar(217)                  not null,
  refixper                        varchar(217)                  not null,
  skilllist                       varchar(121)                  not null,
  fguid                           bigint                        not null default -1,
  fdataid                         int                           not null default -1,
  fname                           varchar(50) binary            not null default '',
  fexp                            int                           not null default 0,
  flevel                          smallint                      not null default 0,
  flocked                         tinyint                       not null default -1,
  fquality                        tinyint                       not null default 0,
  fstarlevel                      tinyint                       not null default 0,
  fzzpoint                        int                      			not null default 0,
  fzizhi_attack                   int                           not null default 0,
  fzizhi_hit                      int                           not null default 0,
  fzizhi_critical                 int                           not null default 0,
  fzizhi_attackspeed              int                           not null default 0,
  fzizhi_bless                    int                           not null default 0,
  fcdlist                         varchar(513)                  not null,
  fskilllist                      varchar(73)                   not null,
  isvalid                         tinyint                       not null default -1,
  shenyiid                   	  int                        	not null default -1,
  shenyiquality                   int                        	not null default  0,  
  primary key (aid)              
)ENGINE = INNODB;

create unique index Index_rank_pos on t_chrank
( 
  rankpos
);
delimiter //
drop procedure if exists loadchrank //
create procedure loadchrank()
begin
select 
      guid,
      level,
      profession,
      combatnum,
      rankpos,
      fashionid,
      weapon,
      name,
      refixval,
      refixper,
      skilllist,
      fguid,
      fdataid,
      fname,
      fexp,
      flevel,
      flocked,
      fquality,
      fstarlevel,
      fzzpoint,
      fzizhi_attack,
      fzizhi_hit,
      fzizhi_critical,
      fzizhi_attackspeed,
      fzizhi_bless,
      fcdlist,
      fskilllist,
	  shenyiid,
	  shenyiquality
from t_chrank where isvalid=1 order by rankpos;
end; //
delimiter ;

delimiter //
drop procedure if exists deletechrank //
create procedure deletechrank()
begin
  update t_chrank set isvalid=0;
end; //
delimiter ;

delimiter //
drop procedure if exists savechrank //
create procedure savechrank( pguid          bigint,
                             plevel         smallint,
                             pprofession    tinyint,
                             pcombatnum     int,
                             prankpos       smallint,
                             pfashionid     int,
                             pweapon        int,
                             pname          varchar(80) binary,
                             prefixval      varchar(217),
                             prefixper      varchar(217),
                             pskilllist     varchar(121),
                             pfguid          bigint,
                             pfdataid        int,
                             pfname          varchar(50) binary,
                             pfexp           int,
                             pflevel         smallint,
                             pflocked        tinyint,
                             pfquality       tinyint,
                             pfstarlevel     tinyint,
                             pfzzpoint       				int,
                             pfzizhi_attack         int,
                             pfzizhi_hit            int,
                             pfzizhi_critical       int,
                             pfzizhi_attackspeed    int,
                             pfzizhi_bless          int,
                             pfcdlist               varchar(513),
                             pfskilllist            varchar(73),
                             pisvalid               tinyint,
							 pshenyiid         		int,
							 pshenyiquality         int)
begin
declare rrankpos  smallint default -1;
start transaction;
select rankpos into rrankpos from t_chrank where rankpos=prankpos;
if rrankpos = prankpos then
    update t_chrank set guid=pguid,
                        level=plevel,
                        profession=pprofession,
                        combatnum=pcombatnum,
                        rankpos=prankpos,
                        fashionid=pfashionid,
                        weapon=pweapon,
                        name=pname,
                        refixval=prefixval,
                        refixper=prefixper,
                        skilllist=pskilllist,
                        fguid=pfguid,
                        fdataid=pfdataid,
                        fname=pfname,
                        fexp=pfexp,
                        flevel=pflevel,
                        flocked=pflocked,
                        fquality=pfquality,
                        fstarlevel=pfstarlevel,
                        fzzpoint=pfzzpoint,
                        fzizhi_attack=pfzizhi_attack,
                        fzizhi_hit=pfzizhi_hit,
                        fzizhi_critical=pfzizhi_critical,
                        fzizhi_attackspeed=pfzizhi_attackspeed,
                        fzizhi_bless=pfzizhi_bless,
                        fcdlist=pfcdlist,
                        fskilllist=pfskilllist,                       
                        isvalid=pisvalid,
						shenyiid=pshenyiid,
						shenyiquality=pshenyiquality
    where rankpos=prankpos;
else
    insert into t_chrank(guid,
                        level,
                        profession,
                        combatnum,
                        rankpos,
                        fashionid,
                        weapon,
                        name,
                        refixval,
                        refixper,
                        skilllist,
                        fguid,
                        fdataid,
                        fname,
                        fexp,
                        flevel,
                        flocked,
                        fquality,
                        fstarlevel,
                        fzzpoint,
                        fzizhi_attack,
                        fzizhi_hit,
                        fzizhi_critical,
                        fzizhi_attackspeed,
                        fzizhi_bless,
                        fcdlist,
                        fskilllist,
                        isvalid,
						shenyiid,
						shenyiquality)values(pguid,
                        plevel,
                        pprofession,
                        pcombatnum,
                        prankpos,
                        pfashionid,
                        pweapon,
                        pname,
                        prefixval,
                        prefixper,
                        pskilllist,
                        pfguid,
                        pfdataid,
                        pfname,
                        pfexp,
                        pflevel,
                        pflocked,
                        pfquality,
                        pfstarlevel,
                        pfzzpoint,
                        pfzizhi_attack,
                        pfzizhi_hit,
                        pfzizhi_critical,
                        pfzizhi_attackspeed,
                        pfzizhi_bless,
                        pfcdlist,
                        pfskilllist,                   
                        pisvalid,
						pshenyiid,
						pshenyiquality);                                    
end if;
commit;
end; //
delimiter ;

delimiter //
drop procedure if exists gm_querycharinfo//
create procedure gm_querycharinfo(pcharname varchar(80) binary, pcharguid bigint, pIsFullMode int)
begin
if length(pcharname) > 0 then
 select 
    accname, 
    charguid, 
    charname,
    profession, 
    level, 
    exp, 
    hp, 
    mp, 
    xp,
    moneycoin,
    moneyyb,
    moneyybbind,
    logouttime,
    createtime,
    sceneid,
    sceneposx,
    sceneposz,
    activefellowguid,
    vipcost,
    unblocktime,
    unforbidtalktime
 from t_char where charname = pcharname and isvalid=1;
else
 select 
    accname, 
    charguid, 
    charname,
    profession, 
    level, 
    exp, 
    hp, 
    mp, 
    xp,
    moneycoin,
    moneyyb,
    moneyybbind,
    logouttime,
    createtime,
    sceneid,
    sceneposx,
    sceneposz,
    activefellowguid,
    vipcost,
    unblocktime,
    unforbidtalktime
  from t_char where charguid=pcharguid and isvalid=1;
end if;
end;//
delimiter ;

/*==============================================================*/
/* Table: t_goldmine;             */
/*==============================================================*/
drop table if exists t_goldmine;
create table if not exists t_goldmine
(
  aid                             bigint                        not null AUTO_INCREMENT,
  mineindex                       int                           not null default -1,
  bindyuanbao                     int                           not null default 0,
  coin                            int                           not null default 0,
  guid                            bigint                        not null default -1,
  level                           smallint                      not null default 0,
  profession                      tinyint                       not null default -1,
  combatnum                       int                           not null default 0,
  fashionid                       int                           not null default -1,
  weapon                          int                           not null default -1,
  name                            varchar(80) binary            not null,
  refixval                        varchar(217)                  not null,
  refixper                        varchar(217)                  not null,
  skilllist                       varchar(121)                  not null,
  fguid                           bigint                        not null default -1,
  fdataid                         int                           not null default -1,
  fname                           varchar(50) binary            not null default '',
  fexp                            int                           not null default 0,
  flevel                          smallint                      not null default 0,
  flocked                         tinyint                       not null default -1,
  fquality                        tinyint                       not null default 0,
  fstarlevel                      tinyint                       not null default 0,
  fzzpoint                        int                      			not null default 0,
  fzizhi_attack                   int                           not null default 0,
  fzizhi_hit                      int                           not null default 0,
  fzizhi_critical                 int                           not null default 0,
  fzizhi_attackspeed              int                           not null default 0,
  fzizhi_bless                    int                           not null default 0,
  fcdlist                         varchar(513)                  not null,
  fskilllist                      varchar(73)                   not null,
  isvalid                         tinyint                       not null default -1,
  shenyiid                   	  int                        	not null default -1,
  shenyiquality                   int                        	not null default  0, 
  holdtime                        int                        	not null default  0,  
  primary key (aid)              
)ENGINE = INNODB;

create unique index Index_mine on t_goldmine
( 
  mineindex
);

delimiter //
drop procedure if exists loadgoldmine //
create procedure loadgoldmine()
begin
select 
      mineindex,
      bindyuanbao,
      coin,
      guid,
      level,
      profession,
      combatnum,
      fashionid,
      weapon,
      name,
      refixval,
      refixper,
      skilllist,
      fguid,
      fdataid,
      fname,
      fexp,
      flevel,
      flocked,
      fquality,
      fstarlevel,
      fzzpoint,
      fzizhi_attack,
      fzizhi_hit,
      fzizhi_critical,
      fzizhi_attackspeed,
      fzizhi_bless,
      fcdlist,
      fskilllist,
      isvalid,
      shenyiid,
      shenyiquality,
      holdtime
from t_goldmine where isvalid=1;
end; //
delimiter ;

delimiter //
drop procedure if exists deletegoldmine //
create procedure deletegoldmine()
begin
  update t_goldmine set isvalid=0;
end; //
delimiter ;

delimiter //
drop procedure if exists savegoldmine //
create procedure savegoldmine( pmineindex      int,
                             pbindyuanbao   int,
                             pcoin          int,
                             pguid          bigint,
                             plevel         smallint,
                             pprofession    tinyint,
                             pcombatnum     int,
                             pfashionid     int,
                             pweapon        int,
                             pname          varchar(80) binary,
                             prefixval      varchar(217),
                             prefixper      varchar(217),
                             pskilllist     varchar(121),
                             pfguid          bigint,
                             pfdataid        int,
                             pfname          varchar(50) binary,
                             pfexp           int,
                             pflevel         smallint,
                             pflocked        tinyint,
                             pfquality       tinyint,
                             pfstarlevel     tinyint,
                             pfzzpoint       				int,
                             pfzizhi_attack         int,
                             pfzizhi_hit            int,
                             pfzizhi_critical       int,
                             pfzizhi_attackspeed    int,
                             pfzizhi_bless          int,
                             pfcdlist               varchar(513),
                             pfskilllist            varchar(73),
                             pisvalid               tinyint,
			     pshenyiid         		int,
			     pshenyiquality         int,
			     pholdtime         int)
begin
declare rmineindex  int default -1;
start transaction;
select mineindex into rmineindex from t_goldmine where mineindex=pmineindex;
if rmineindex = pmineindex then
    update t_goldmine set mineindex=pmineindex,
                        bindyuanbao=pbindyuanbao,
                        coin=pcoin,
                        guid=pguid,
                        level=plevel,
                        profession=pprofession,
                        combatnum=pcombatnum,
                        fashionid=pfashionid,
                        weapon=pweapon,
                        name=pname,
                        refixval=prefixval,
                        refixper=prefixper,
                        skilllist=pskilllist,
                        fguid=pfguid,
                        fdataid=pfdataid,
                        fname=pfname,
                        fexp=pfexp,
                        flevel=pflevel,
                        flocked=pflocked,
                        fquality=pfquality,
                        fstarlevel=pfstarlevel,
                        fzzpoint=pfzzpoint,
                        fzizhi_attack=pfzizhi_attack,
                        fzizhi_hit=pfzizhi_hit,
                        fzizhi_critical=pfzizhi_critical,
                        fzizhi_attackspeed=pfzizhi_attackspeed,
                        fzizhi_bless=pfzizhi_bless,
                        fcdlist=pfcdlist,
                        fskilllist=pfskilllist,                       
                        isvalid=pisvalid,
			shenyiid=pshenyiid,
			shenyiquality=pshenyiquality,
			holdtime=pholdtime
    where mineindex=pmineindex;
else
    insert into t_goldmine(mineindex,
                        bindyuanbao,
                        coin,
                        guid,
                        level,
                        profession,
                        combatnum,
                        fashionid,
                        weapon,
                        name,
                        refixval,
                        refixper,
                        skilllist,
                        fguid,
                        fdataid,
                        fname,
                        fexp,
                        flevel,
                        flocked,
                        fquality,
                        fstarlevel,
                        fzzpoint,
                        fzizhi_attack,
                        fzizhi_hit,
                        fzizhi_critical,
                        fzizhi_attackspeed,
                        fzizhi_bless,
                        fcdlist,
                        fskilllist,
                        isvalid,
			shenyiid,
			shenyiquality,holdtime)values(pmineindex,pbindyuanbao,pcoin,pguid,
                        plevel,
                        pprofession,
                        pcombatnum,
                        pfashionid,
                        pweapon,
                        pname,
                        prefixval,
                        prefixper,
                        pskilllist,
                        pfguid,
                        pfdataid,
                        pfname,
                        pfexp,
                        pflevel,
                        pflocked,
                        pfquality,
                        pfstarlevel,
                        pfzzpoint,
                        pfzizhi_attack,
                        pfzizhi_hit,
                        pfzizhi_critical,
                        pfzizhi_attackspeed,
                        pfzizhi_bless,
                        pfcdlist,
                        pfskilllist,                   
                        pisvalid,
			pshenyiid,
			pshenyiquality,pholdtime);                                    
end if;
commit;
end; //
delimiter ;

drop table if exists t_world_globaldata;
create table if not exists t_world_globaldata
(
    aid                              bigint                                         not null AUTO_INCREMENT,
    mergedtimes                      bigint                                         not null default 0,
    activitydata                     varchar(1025)                                  not null default '',
    primary key (aid)
)ENGINE = INNODB;

delimiter //
drop procedure if exists loadworldglobaldata //
create procedure loadworldglobaldata()
begin
     select activitydata from t_world_globaldata;
end; //
delimiter ;

delimiter //
drop procedure if exists saveactivityglobaldata //
create procedure saveactivityglobaldata(pactivitydata varchar(1025))
begin
    update t_world_globaldata set activitydata = pactivitydata;
end; //
delimiter ;
/*=======================================================================*/
/* Table: t_fellow;             */
/*t_chrank表中有fellow数据，修改t_fellow表时，辛苦确认是否同步修改t_chrank*/
/*======================================================================*/
drop table if exists t_fellow;
create table if not exists t_fellow
(
  aid                             bigint                        not null AUTO_INCREMENT,
  charguid                        bigint                        not null default -1,
  guid                            bigint                        not null default -1,
  dataid                          int                           not null default -1,
  name                            varchar(50) binary            not null default '',
  exp                             int                           not null default 0,
  level                           smallint                      not null default 0,
  locked                          tinyint                       not null default -1,
  quality                         tinyint                       not null default 0,
  starlevel                       tinyint                       not null default 0,
  zzpoint                         int                      			not null default 0,
  zizhi_attack                    int                           not null default 0,
  zizhi_hit                       int                           not null default 0,
  zizhi_critical                  int                           not null default 0,
  zizhi_attackspeed               int                           not null default 0,
  zizhi_bless                     int                           not null default 0,
  cdlist                          varchar(513)                  not null,
  skilllist                       varchar(73)                   not null,
  packindex                       smallint                      not null default -1,
  isvalid                         tinyint                       not null default 1,
  primary key (aid)              
)ENGINE = INNODB;

create unique index Index_fellow_charguid on t_fellow
( 
  charguid,
  packindex 
);

create index Index_fellow_itemguid on t_fellow
(
  guid,
  isvalid 
);

delimiter //
drop procedure if exists loadfellowpack //
create procedure loadfellowpack(pcharguid   bigint)
begin
select charguid,
        guid,
        dataid,
        name,
        exp,
        level,
        locked,
        quality,
        starlevel,
        zzpoint,
        zizhi_attack,
        zizhi_hit,
        zizhi_critical,
        zizhi_attackspeed,
        zizhi_bless,
        cdlist,
        skilllist,
        packindex
from t_fellow where charguid=pcharguid and isvalid=1;
end; //
delimiter ;

delimiter //
drop procedure if exists deletefellowpack //
create procedure deletefellowpack(pcharguid   bigint)
begin
update t_fellow set isvalid=0
where charguid=pcharguid and isvalid=1;
end; //
delimiter ;

delimiter //
drop procedure if exists savefellowpack //
create procedure savefellowpack(
in pcharguid bigint,
in pdatas mediumtext
)
begin
declare stmt mediumtext default '';
declare rowcount int default 0;
declare textlength int default 0;

start transaction;

select count(*) into rowcount from t_fellow where charguid=pcharguid;
if rowcount > 0 then
    delete from t_fellow where charguid=pcharguid;
end if;

select LENGTH(pdatas) into textlength;

if textlength > 0 then
   
    set @sqlstr = "insert into t_fellow(charguid,guid,dataid,name,exp,level,locked,quality,starlevel,zzpoint,zizhi_attack,zizhi_hit,zizhi_critical,zizhi_attackspeed,zizhi_bless,cdlist,skilllist,packindex,isvalid) values";
    set @sqlstr = concat(@sqlstr,pdatas);

    prepare stmt from @sqlstr;
    execute stmt;
    deallocate prepare stmt;
    
end if;
commit;
end;//
delimiter ;
/*==============================================================*/
/* Table: t_consignsale;             */
/* t_consignsale表包含t_item表相关列，t_item表有修改时，*/
/* 辛苦确认是否需要同步修改t_consignsale相关列*/
/*==============================================================*/
drop table if exists t_consignsale;
create table if not exists t_consignsale
(
  aid                             bigint                        not null AUTO_INCREMENT,
  itemguid                        bigint                        not null default -1,
  dataid                          int                           not null default -1,
  binded                          tinyint                       not null default -1,
  stackcount                      smallint                      not null default -1,
  createtime                      bigint                        not null default -1,
  enchancelevel                   smallint                      not null default -1,
  enchanceexp                     int                           not null default -1,
  enchancetotalexp                bigint                        not null default -1,
  starlevel                       smallint                      not null default -1,
  startimes                       smallint                      not null default -1,
  dynamicdata1                    int                           not null default -1,
  dynamicdata2                    int                           not null default -1,
  dynamicdata3                    int                           not null default -1,
  dynamicdata4                    int                           not null default -1,
  dynamicdata5                    int                           not null default -1,
  dynamicdata6                    int                           not null default -1,
  dynamicdata7                    int                           not null default -1,
  dynamicdata8                    int                           not null default -1,
  itemprice                       int                           not null default 0,
  charguid                        bigint                        not null default 0,
  charname                        varchar(80) binary            not null,
  shangjiatime                    bigint                        not null default 0,
  isvalid                         tinyint                       not null default -1,
  primary key (aid)              
)ENGINE = INNODB;

create unique index Index_consignsale_guid on t_consignsale
( 
  itemguid
);
delimiter //
drop procedure if exists loadconsignsale //
create procedure loadconsignsale()
begin
  select itemguid,dataid,binded,stackcount,createtime,enchancelevel,enchanceexp,enchancetotalexp,\
  starlevel,startimes,dynamicdata1,dynamicdata2,dynamicdata3,dynamicdata4,dynamicdata5,dynamicdata6,dynamicdata7,dynamicdata8,itemprice,charguid,\
  charname,shangjiatime
from t_consignsale where isvalid=1;
end; //
delimiter ;

delimiter //
drop procedure if exists deleteconsignsale //
create procedure deleteconsignsale()
begin
  update t_consignsale set isvalid=0;
end; //
delimiter ;

delimiter //
drop procedure if exists saveconsignsale //
create procedure saveconsignsale(
                          pitemguid     bigint,
                          pdataid       int,
                          pbinded       tinyint,
                          pstackcount   smallint,
                          pcreatetime   bigint,
                          penchancelevel smallint,
                          penchanceexp   int,
                          penchancetotalexp   bigint,
                          pstarlevel     smallint,
                          pstartimes     smallint,
                          pdynamicdata1  int,
                          pdynamicdata2  int,
                          pdynamicdata3  int,
                          pdynamicdata4  int,
                          pdynamicdata5  int,
                          pdynamicdata6  int,
                          pdynamicdata7  int,
                          pdynamicdata8  int,
                          pitemprice int,
                          pcharguid  bigint,
                          pcharname  varchar(80) binary,
                          pshangjiatime bigint,
                          pisvalid tinyint)
begin
declare rguid        bigint  default -1;
start transaction;
select itemguid into rguid from t_consignsale where itemguid=pitemguid;
if rguid = pitemguid then
    update t_consignsale set 
                  itemguid     = pitemguid,
                  dataid       = pdataid,
                  binded       = pbinded,
                  stackcount   = pstackcount,
                  createtime   = pcreatetime,
                  enchancelevel = penchancelevel,
                  enchanceexp   = penchanceexp,
                  enchancetotalexp   = penchancetotalexp,
                  starlevel     = pstarlevel,
                  startimes     = pstartimes,
                  dynamicdata1  = pdynamicdata1,
                  dynamicdata2  = pdynamicdata2,
                  dynamicdata3  = pdynamicdata3,
                  dynamicdata4  = pdynamicdata4,
                  dynamicdata5  = pdynamicdata5,
                  dynamicdata6  = pdynamicdata6,
                  dynamicdata7  = pdynamicdata7,
                  dynamicdata8  = pdynamicdata8,
                  itemprice     = pitemprice,
                  charguid      = pcharguid,
                  charname      = pcharname,
                  shangjiatime  = pshangjiatime,
                  isvalid       = pisvalid                  
    where  itemguid = pitemguid;
else
    insert into t_consignsale(
                  itemguid,
                  dataid,
                  binded,
                  stackcount,
                  createtime,
                  enchancelevel,
                  enchanceexp,
                  enchancetotalexp,
                  starlevel,
                  startimes,
                  dynamicdata1,
                  dynamicdata2,
                  dynamicdata3,
                  dynamicdata4,
                  dynamicdata5,
                  dynamicdata6,
                  dynamicdata7,
                  dynamicdata8,
                  itemprice,
                  charguid,
                  charname,
                  shangjiatime,
                  isvalid)values(pitemguid,
                  pdataid,
                  pbinded,
                  pstackcount,
                  pcreatetime,
                  penchancelevel,
                  penchanceexp,
                  penchancetotalexp,
                  pstarlevel,
                  pstartimes,
                  pdynamicdata1,
                  pdynamicdata2,
                  pdynamicdata3,
                  pdynamicdata4,
                  pdynamicdata5,
                  pdynamicdata6,
                  pdynamicdata7,
                  pdynamicdata8,
                  pitemprice,
                  pcharguid,
                  pcharname,
                  pshangjiatime,
                  pisvalid);                                    
end if;
commit;
end; //
delimiter ;
/*==============================================================*/
/* Table: t_guild;             */
/*==============================================================*/
drop table if exists t_guild;
create table if not exists t_guild
(
  aid                             bigint                        not null AUTO_INCREMENT,
  guid                            bigint                        not null default -1,
  name                            varchar(50)                   not null,
  level                           smallint                      not null default 0,
  chiefguid                       bigint                        not null default -1,
  exp                             int                           not null default 0,
  notice                          varchar(80)                   not null,
  warscore                        int                           not null default 0,
  warfinalscore                   int                           not null default 0,
  finalsortweek                   int                           not null default -1,
  warfinalsort                    int                           not null default -1, 
  isvalid                         tinyint                       not null default 0,
  isfirst                         tinyint                       not null default -1,
  createtime                      int  unsigned                 not null default 0,
  wilddays                        tinyint                       not null default 0,
  combatvalue                     int                           not null default 0,
  wildwarfight                    varchar(289)                  not null default '',
  wealth                          int                           not null default 0,
  psassigntimes                   int                           not null default 0,
  psaccepttimes                   int                           not null default 0,
  autoassignpstimes               tinyint                       not null default 0,
  shopitem                        varchar(257)                  not null default '',
  misassigntimes                  smallint                      not null default 0,
  misaccepttimes                  smallint                      not null default 0,
  battledata                      int                           not null default 0,
  battlescore                     bigint                        not null default -1,
  dayaddedexp                     int                           not null default 0,
  parallelworldid                 int                           not null default 0,
  parallelworldname               varchar(50)                   not null,
  primary key (aid)              
)ENGINE = INNODB;

create unique index Index_guild_guid on t_guild
( 
  guid
);
delimiter //
drop procedure if exists loadguild //
create procedure loadguild()
begin
  select guid,
         name,
         level,
         chiefguid,
         exp,
         notice,
         warscore,
         warfinalscore,
         finalsortweek,
         warfinalsort,
         isfirst,
         createtime,
         wilddays,
         combatvalue,
         wildwarfight,
         wealth,
         psassigntimes,
         psaccepttimes,
         autoassignpstimes,
         shopitem,
         misassigntimes,
         misaccepttimes,
         battledata,
         battlescore,
         dayaddedexp,
	 parallelworldid,
	 parallelworldname
from t_guild where isvalid=1;
end; //
delimiter ;

delimiter //
drop procedure if exists deleteguild //
create procedure deleteguild()
begin
  update t_guild set isvalid=0;
end; //
delimiter ;

delimiter //
drop procedure if exists saveguild //
create procedure saveguild(
                          pguid       bigint,
                          pname       varchar(50),
                          plevel      smallint,
                          pchiefguid  bigint,
                          pexp        int,
                          pnotice     varchar(80),
                          pwarscore   int,
                          pwarfinalscore int,
                          pfinalsortweek int,
                          pwarfinalsort  int, 
                          pisvalid    tinyint,
                          pisfirst    tinyint,
                          pcreatetime int unsigned,
                          pwilddays tinyint,
                          pcombatvalue int,
                          pwildwarfight varchar(289),
                          pwealth int,
                          ppsassigntimes int,
                          ppsaccepttimes int,
                          pautoassignpstimes tinyint,
                          pshopitem varchar(257),
                          pmisassigntimes smallint,
                          pmisaccepttimes smallint,
                          pbattledata int,
                          pbattlescore bigint,
                          pdayaddedexp int,
			  pparallelworldid int,
			  pparallelworldname varchar(50))
begin
declare rguid        bigint  default -1;
start transaction;
select pguid into rguid from t_guild where guid=pguid;
if rguid = pguid then
    update t_guild set 
                  guid=pguid,
                  name=pname,
                  level=plevel,
                  chiefguid=pchiefguid,
                  exp=pexp,
                  notice=pnotice,
                  warscore=pwarscore,
                  warfinalscore=pwarfinalscore,
                  finalsortweek=pfinalsortweek,
                  warfinalsort=pwarfinalsort,
                  isvalid=pisvalid,
                  isfirst=pisfirst,
                  createtime=pcreatetime,
                  wilddays=pwilddays,
                  combatvalue=pcombatvalue,
                  wildwarfight=pwildwarfight,
                  wealth=pwealth,
                  psassigntimes=ppsassigntimes,
                  psaccepttimes=ppsaccepttimes,
                  autoassignpstimes=pautoassignpstimes,
                  shopitem=pshopitem,
                  misassigntimes=pmisassigntimes,
                  misaccepttimes=pmisaccepttimes,
                  battledata=pbattledata,
                  battlescore=pbattlescore,
                  dayaddedexp=pdayaddedexp,
		  parallelworldid=pparallelworldid,
		  parallelworldname=pparallelworldname
    where  guid = pguid;
else
    insert into t_guild(
                  guid,
                  name,
                  level,
                  chiefguid,
                  exp,
                  notice,
                  warscore,
                  warfinalscore,
                  finalsortweek,
                  warfinalsort,
                  isvalid,
                  isfirst,
                  createtime,
                  wilddays,
                  combatvalue,
                  wildwarfight,
                  wealth,
                  psassigntimes,
                  psaccepttimes,
                  autoassignpstimes,
                  shopitem,
                  misassigntimes,
                  misaccepttimes,
                  battledata,
                  battlescore,
                  dayaddedexp,
		  parallelworldid,
		  parallelworldname)values(pguid,
                  pname,
                  plevel,
                  pchiefguid,
                  pexp,
                  pnotice,
                  pwarscore,
                  pwarfinalscore,
                  pfinalsortweek,
                  pwarfinalsort,
                  pisvalid,
                  pisfirst,
                  pcreatetime,
                  pwilddays,
                  pcombatvalue,
                  pwildwarfight,
                  pwealth,
                  ppsassigntimes,
                  ppsaccepttimes,
                  pautoassignpstimes,
                  pshopitem,
                  pmisassigntimes,
                  pmisaccepttimes,
                  pbattledata,
                  pbattlescore,
                  pdayaddedexp,
		  pparallelworldid,
		  pparallelworldname);                                    
end if;
commit;
end; //
delimiter ;
/*==============================================================*/
/* Table: t_guild_user;             */
/*==============================================================*/
drop table if exists t_guild_user;
create table if not exists t_guild_user
(
  aid                             bigint                        not null AUTO_INCREMENT,
  guildguid                       bigint                        not null default -1,
  guid                            bigint                        not null default -1,
  name                            varchar(80) binary            not null,
  vip                             int                           not null default -1,
  profession                      tinyint                       not null default -1,
  level                           smallint                      not null default 0,
  contribute                      int                           not null default 0,
  lastlogin                       int                           not null default 0,
  job                             tinyint                       not null default -1,
  combatnum                       int                           not null default  0,
  type                            tinyint                       not null default -1,
  isvalid                         tinyint                       not null default -1,
  joinwar                         tinyint                       not null default -1,
  lastlogout                      bigint                        not null default 0,
  jointime                        bigint                        not null default 0,
  exchange                        tinyint                       not null default 0,
  primary key (aid)              
)ENGINE = INNODB;

create unique index Index_guild_user_guid on t_guild_user
( 
  guid
);
delimiter //
drop procedure if exists loadguilduser //
create procedure loadguilduser(pguildguid bigint)
begin
  select guid,
         name,
         vip,
         profession,
         level,
         contribute,
         lastlogin,
         job,
         combatnum,
         type,
         joinwar,
         lastlogout,
         jointime,
         exchange
from t_guild_user where guildguid=pguildguid and isvalid=1;
end; //
delimiter ;

delimiter //
drop procedure if exists deleteguilduser //
create procedure deleteguilduser(pguildguid bigint)
begin
  update t_guild_user set isvalid=0 where guildguid=pguildguid;
end; //
delimiter ;

delimiter //
drop procedure if exists saveguilduser //
create procedure saveguilduser(
                         pguildguid   bigint,
                         pguid        bigint,
                         pname        varchar(80) binary,
                         pvip         int,
                         pprofession  tinyint,
                         plevel       smallint,
                         pcontribute  int,
                         plastlogin   int,
                         pjob         tinyint,
                         pcombatnum   int,
                         ptype        tinyint,
                         pisvalid     tinyint,
                         pjoinwar     tinyint,
                         plastlogout  bigint,
                         pjointime    bigint,
                         pexchange    tinyint)
begin
declare rguid        bigint  default -1;
start transaction;
select pguid into rguid from t_guild_user where guid=pguid;
if rguid = pguid then
    update t_guild_user set 
                  guildguid=pguildguid,
                  guid=pguid,
                  name=pname,
                  vip=pvip,
                  profession=pprofession,
                  level=plevel,
                  contribute=pcontribute,
                  lastlogin=plastlogin,
                  job=pjob,
                  combatnum=pcombatnum,
                  type=ptype,
                  isvalid=pisvalid,
                  joinwar=pjoinwar,
                  lastlogout=plastlogout,
                  jointime=pjointime,
                  exchange=pexchange
    where  guid = pguid;
else
    insert into t_guild_user(
                   guildguid,
                   guid,
                   name,
                   vip,
                   profession,
                   level,
                   contribute,
                   lastlogin,
                   job,
                   combatnum,
                   type,
                   isvalid,
                   joinwar,
                   lastlogout,
                   jointime,
                   exchange)
                   values( 
                   pguildguid,
                   pguid,
                   pname,
                   pvip,
                   pprofession,
                   plevel,
                   pcontribute,
                   plastlogin,
                   pjob,
                   pcombatnum,      
                   ptype,       
                   pisvalid,
                   pjoinwar,
                   plastlogout,
                   pjointime,
                   pexchange);                                    
end if;
commit;
end; //
delimiter ;
/*==============================================================*/
/* Table: t_master;             */
/*==============================================================*/
drop table if exists t_master;
create table if not exists t_master
(
  aid                             bigint                        not null AUTO_INCREMENT,
  guid                            bigint                        not null default -1,
  name                            varchar(50)                   not null,
  notice                          varchar(180)                  not null,
  chiefguid                       bigint                        not null default -1,
  torch                           int                           not null default 0,
  createtime                      bigint                        not null default 0,
  skillid1                        int                           not null default -1,
  skillid2                        int                           not null default -1,
  skillid3                        int                           not null default -1,
  skillid4                        int                           not null default -1,
  skillid5                        int                           not null default -1,
  skillid6                        int                           not null default -1,
  skillname1                      varchar(50)                   not null,
  skillname2                      varchar(50)                   not null,
  skillname3                      varchar(50)                   not null,
  skillname4                      varchar(50)                   not null,
  skillname5                      varchar(50)                   not null,
  skillname6                      varchar(50)                   not null,
  combatvalue                     int                           not null default 0,
  dieoutday                       smallint                      not null default 0,
  isvalid                         tinyint                       not null default 0,
  primary key (aid)              
)ENGINE = INNODB;

create unique index Index_master_guid on t_master
( 
  guid
);
delimiter //
drop procedure if exists loadmaster //
create procedure loadmaster()
begin
  select guid,
         name,
         notice,
         chiefguid,
         torch,
         createtime,
         skillid1,
         skillid2,
         skillid3,
         skillid4,
         skillid5,
         skillid6,
         skillname1,
         skillname2,
         skillname3,
         skillname4,
         skillname5,
         skillname6,
         combatvalue,
         dieoutday
  from t_master where isvalid=1;
end; //
delimiter ;

delimiter //
drop procedure if exists deletemaster //
create procedure deletemaster()
begin
  update t_master set isvalid=0;
end; //
delimiter ;

delimiter //
drop procedure if exists savemaster //
create procedure savemaster(
                          pguid       bigint,
                          pname       varchar(50),
                          pnotice     varchar(180),
                          pchiefguid  bigint,
                          ptorch      int,
                          pcreatetime bigint,
                          pskillid1   int,
                          pskillid2   int,
                          pskillid3   int,
                          pskillid4   int,
                          pskillid5   int,
                          pskillid6   int,
                          pskillname1 varchar(50),
                          pskillname2 varchar(50),
                          pskillname3 varchar(50),
                          pskillname4 varchar(50),
                          pskillname5 varchar(50),
                          pskillname6 varchar(50),
                          pcombatvalue int,
                          pdieoutday   smallint,
                          pisvalid    tinyint)
begin
declare rguid        bigint  default -1;
start transaction;
  select pguid into rguid from t_master where guid=pguid;
    if rguid = pguid then
      update t_master set 
                  guid=pguid,
                  name=pname,
                  notice=pnotice,
                  chiefguid=pchiefguid,
                  torch=ptorch,
                  createtime=pcreatetime,
                  skillid1=pskillid1,
                  skillid2=pskillid2,
                  skillid3=pskillid3,
                  skillid4=pskillid4,
                  skillid5=pskillid5,
                  skillid6=pskillid6,
                  skillname1=pskillname1,
                  skillname2=pskillname2,
                  skillname3=pskillname3,
                  skillname4=pskillname4,
                  skillname5=pskillname5,
                  skillname6=pskillname6,
                  combatvalue=pcombatvalue,
                  dieoutday=pdieoutday,
                  isvalid=pisvalid       
      where  guid = pguid;
    else
      insert into t_master(
                  guid,
                  name,
                  notice,
                  chiefguid,
                  torch,
                  createtime,
                  skillid1,
                  skillid2,
                  skillid3,
                  skillid4,
                  skillid5,
                  skillid6,
                  skillname1,
                  skillname2,
                  skillname3,
                  skillname4,
                  skillname5,
                  skillname6,
                  combatvalue,
                  dieoutday,
                  isvalid)
                  values(
                    pguid,
                    pname,
                    pnotice,
                    pchiefguid,
                    ptorch,
                    pcreatetime,
                    pskillid1,
                    pskillid2,
                    pskillid3,
                    pskillid4,
                    pskillid5,
                    pskillid6,
                    pskillname1,
                    pskillname2,
                    pskillname3,
                    pskillname4,
                    pskillname5,
                    pskillname6,
                    pcombatvalue,
                    pdieoutday,
                    pisvalid);                                    
    end if;
  commit;
end; //
delimiter ;
/*==============================================================*/
/* Table: t_master_user;             */
/*==============================================================*/
drop table if exists t_master_user;
create table if not exists t_master_user
(
  aid                             bigint                        not null AUTO_INCREMENT,
  masterguid                      bigint                        not null default -1,
  guid                            bigint                        not null default -1,
  name                            varchar(80) binary            not null,
  guildname                       varchar(50)                   not null,
  vip                             int                           not null default -1,
  profession                      tinyint                       not null default -1,
  level                           smallint                      not null default 0,
  torchvalue                      int                           not null default 0,
  combatvalue                     int                           not null default 0,
  lastlogin                       bigint                        not null default 0,
  type                            tinyint                       not null default 0,
  isvalid                         tinyint                       not null default -1,
  primary key (aid)              
)ENGINE = INNODB;

create unique index Index_master_user_guid on t_master_user
( 
  guid
);
delimiter //
drop procedure if exists loadmasteruser //
create procedure loadmasteruser(pmasterguid bigint)
begin
  select guid,
         name,
         guildname,
         vip,
         profession,
         level,
         torchvalue,
         lastlogin,
         combatvalue,
         type
  from t_master_user where isvalid=1 and masterguid=pmasterguid;
end; //
delimiter ;

delimiter //
drop procedure if exists deletemasteruser //
create procedure deletemasteruser(pmasterguid bigint)
begin
  update t_master_user set isvalid=0 where masterguid=pmasterguid;
end; //
delimiter ;

delimiter //
drop procedure if exists savemasteruser //
create procedure savemasteruser(
                         pmasterguid   bigint,
                         pguid        bigint,
                         pname        varchar(80) binary,
                         pguildname   varchar(50),
                         pvip         int,
                         pprofession  tinyint,
                         plevel       smallint,
                         ptorchvalue  int,
                         plastlogin   bigint,
                         pcombatvalue int,
                         ptype   tinyint,
                         pisvalid     tinyint)
begin
declare rguid        bigint  default -1;
start transaction;
select pguid into rguid from t_master_user where guid=pguid;
  if rguid = pguid then
    update t_master_user set 
                  masterguid=pmasterguid,
                  guid=pguid,
                  name=pname,
                  guildname=pguildname,
                  vip=pvip,
                  profession=pprofession,
                  level=plevel,
                  torchvalue=ptorchvalue,
                  lastlogin=plastlogin,
                  combatvalue=pcombatvalue,
                  type=ptype,
                  isvalid=pisvalid              
    where  guid = pguid;
  else
    insert into t_master_user(
                   masterguid,
                   guid,
                   name,
                   guildname,
                   vip,
                   profession,
                   level,
                   torchvalue,
                   lastlogin,
                   combatvalue,
                   type,
                   isvalid)
                   values( 
                   pmasterguid,
                   pguid,
                   pname,
                   pguildname,
                   pvip,
                   pprofession,
                   plevel,
                   ptorchvalue,
                   plastlogin,
                   pcombatvalue,
                   ptype,
                   pisvalid);                                 
  end if;
commit;
end; //
delimiter ;
/*==============================================================*/
/* Table: t_desk;             */
/*==============================================================*/
drop table if exists t_desk;
create table if not exists t_desk
(
  aid                 bigint             not null AUTO_INCREMENT,
  charguid            bigint             not null default -1,
  deskindex           tinyint            not null default -1,
  state               tinyint            not null default 0,
  ftime               int                not null default 0,
  curfood             int                not null default -1,
  guest               varchar(33)        not null,
  isvalid             tinyint            not null default 0,
  primary key (aid)              
)ENGINE = INNODB;

create unique index Index_desk_charguid on t_desk
( 
  charguid,
  deskindex
);

delimiter //
drop procedure if exists loadrestaurantdesk //
create procedure loadrestaurantdesk(pcharguid   bigint)
begin
select charguid,deskindex,state,ftime,curfood,guest\
from t_desk where charguid=pcharguid and isvalid=1;
end; //
delimiter ;

delimiter //
drop procedure if exists deleterestaurantdesk //
create procedure deleterestaurantdesk(pcharguid   bigint)
begin
update t_desk set isvalid=0
where charguid=pcharguid and isvalid=1;
end; //
delimiter ;


delimiter //
drop procedure if exists saverestaurantdesk //
create procedure saverestaurantdesk(
in pcharguid bigint,
in pdatas mediumtext
)
begin
declare stmt mediumtext default '';
declare rowcount int default 0;
declare textlength int default 0;

start transaction;

select count(*) into rowcount from t_desk where charguid=pcharguid;
if rowcount > 0 then
    delete from t_desk where charguid=pcharguid;
end if;

select LENGTH(pdatas) into textlength;

if textlength > 0 then

    set @sqlstr = "insert into t_desk( charguid,deskindex,state,ftime,curfood,guest,isvalid) values";
    set @sqlstr = concat(@sqlstr,pdatas);

    prepare stmt from @sqlstr;
    execute stmt;
    deallocate prepare stmt;

end if;

commit;
end;//
delimiter ;

/*==============================================================*/
/* Table: t_rank;             */
/*==============================================================*/
drop table if exists t_rank;
create table if not exists t_rank
(
  aid                             bigint                        not null AUTO_INCREMENT,
  ranktype                        smallint                      not null default -1,
  pos                             smallint                      not null default -1,
  guid1                           bigint                        not null default -1,
  szvalue1                        varchar(80)                   not null default '',
  n64value1                       bigint                        not null default -1,
  n32value1                       int                           not null default -1,
  n32value2                       int                           not null default -1,
  n32value3                       int                           not null default -1,
  isvalid                         tinyint                       not null default -1,
  szvalue2                        varchar(80)                   not null default '',
  n64value2                       bigint                        not null default -1,
  primary key (aid)              
)ENGINE = INNODB;

create unique index Index_rank_type_pos on t_rank
( 
  ranktype,
  pos
);
delimiter //
drop procedure if exists loadrankdata //
create procedure loadrankdata(pranktype smallint)
begin
  select ranktype,
         guid1,
         szvalue1,
         n64value1,
         n32value1,
         n32value2,
         n32value3,
         szvalue2,
         n64value2
from t_rank where ranktype=pranktype and isvalid=1;
end; //
delimiter ;

delimiter //
drop procedure if exists deleterankdata //
create procedure deleterankdata(pranktype smallint)
begin
  update t_rank set isvalid=0 where ranktype=pranktype;
end; //
delimiter ;

delimiter //
drop procedure if exists saverankdata //
create procedure saverankdata(
                         pranktype smallint,
                         ppos smallint,
                         pguid1     bigint,
                         pszvalue1  varchar(80),
                         pn64value1 bigint,
                         pn32value1 int,
                         pn32value2 int,
                         pn32value3 int,
                         pisvalid   tinyint,
                         pszvalue2 varchar(80),
                         pn64value2 bigint)
begin
  declare rpos             int    default -1;

start transaction;

 select pos into rpos from t_rank where  ranktype = pranktype and pos = ppos ;

 if rpos = ppos then
             update t_rank set
                         ranktype=pranktype,
                         pos=ppos,
                         guid1=pguid1,
                         szvalue1=pszvalue1,
                         n64value1=pn64value1,
                         n32value1=pn32value1,
                         n32value2=pn32value2,
                         n32value3=pn32value3,
                         isvalid=pisvalid,
                         szvalue2=pszvalue2,
                         n64value2=pn64value2
                       
           where ranktype = pranktype and pos = ppos;
else 
           insert into t_rank(
                           ranktype,
                           pos,
                           guid1,
                           szvalue1,
                           n64value1,
                           n32value1,
                           n32value2,
                           n32value3,
                           isvalid,
                           szvalue2,
                           n64value2)values (
                           pranktype,
                           ppos,
                           pguid1,
                           pszvalue1,
                           pn64value1,
                           pn32value1,
                           pn32value2,
                           pn32value3,
                           pisvalid,
                           pszvalue2,
                           pn64value2);
end if;
commit;
end;//
delimiter ;
/*==============================================================*/
/* Table: t_wordsman;             */
/*==============================================================*/
drop table if exists t_wordsman;
create table if not exists t_wordsman
(
  aid                             bigint                        not null AUTO_INCREMENT,
  charguid                        bigint                        not null default -1,
  guid                            bigint                        not null default -1,
  packtype                        tinyint                       not null default -1,
  packindex                       smallint                      not null default -1,
  dataid                          int                           not null default -1,
  level                           smallint                      not null default 0,
  exp                             int                           not null default 0,
  isvalid                         tinyint                       not null default -1,
  locked                          tinyint                       not null default 0,
  primary key (aid)              
)ENGINE = INNODB;

create unique index Index_wordsman_charguid on t_wordsman
( 
  charguid,
  packtype,
  packindex 
);

delimiter //
drop procedure if exists loadwordsmanpack //
create procedure loadwordsmanpack(pcharguid   bigint)
begin
select charguid,guid,packtype,packindex,dataid,level,exp,locked\
from t_wordsman where charguid=pcharguid and isvalid=1;
end; //
delimiter ;

delimiter //
drop procedure if exists savewordsmanpack //
create procedure savewordsmanpack(
in pcharguid bigint,
in pdatas mediumtext
)
begin
declare stmt mediumtext default '';
declare rowcount int default 0;
declare textlength int default 0;

start transaction;

select count(*) into rowcount from t_wordsman where charguid=pcharguid;
if rowcount > 0 then
    delete from t_wordsman where charguid=pcharguid;
end if;

select LENGTH(pdatas) into textlength;

if textlength > 0 then 
	
    set @sqlstr = "insert into t_wordsman(charguid,guid,packtype,packindex,dataid,level,exp,isvalid,locked) values";
    set @sqlstr = concat(@sqlstr,pdatas);

    prepare stmt from @sqlstr;
    execute stmt;
    deallocate prepare stmt;

end if;

commit;
end;//
delimiter ;
/*==============================================================*/
/* Table: t_guid;             */
/*==============================================================*/

DROP TABLE IF EXISTS t_guid;
CREATE TABLE t_guid (
  type int not null default 0,
  carry int not null default 0,
  serial int not null default 0,
  unique key (type)
) ENGINE=InnoDB;

delimiter //
drop procedure if exists saveguid//
create procedure saveguid(
           ptype int,
           pcarry int,
           pserial int)
begin
declare rcarry    int   default 0;
declare rserial   int   default 0;

select carry into rcarry from t_guid where type=ptype;
select serial into rserial from t_guid where type=ptype;

if rcarry <= pcarry and rserial < pserial then
    update t_guid set 
           carry=pcarry,
           serial=pserial
    where type=ptype;
end if;
end;//
delimiter ;

/*==============================================================*/
/* Table: t_randomname;             */
/*==============================================================*/
DROP TABLE IF EXISTS `t_randomname`;
CREATE TABLE `t_randomname` (
  `Id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(80) binary NOT NULL DEFAULT '',
  `exist` tinyint(3) NOT NULL DEFAULT '0',
  PRIMARY KEY (`Id`)
) ENGINE=InnoDB;
create index Index_randomname_name on t_randomname (
    name
);
/*==============================================================*/
/* Table: t_guid;             */
/*==============================================================*/
delimiter //
drop procedure if exists loadrandomname//
create procedure loadrandomname()
begin
	select name
	from t_randomname
	where exist = 0
	limit 100000;
end;//
delimiter ;
/*==============================================================*/
/* Table: t_shoppinglist;             */
/*==============================================================*/
drop table if exists t_shoppinglist;
create table if not exists t_shoppinglist
(
  aid                                 bigint                       not null AUTO_INCREMENT,
  guid                                bigint                       not null default -1,
  sendguid                            bigint                       not null default -1,
  sendname                            varchar(80) binary           not null default '',
  receiveguid                         bigint                       not null default -1,
  receivename                         varchar(80) binary           not null default '',
  createtime                          bigint                       not null default 0,
  goodsid                             varchar(21)                  not null default '',
  goodscount                          varchar(21)                  not null default '',
  listtype                            tinyint                      not null default -1,
  isvalid                             tinyint                      not null default 0,
  primary key (aid)              
)ENGINE = INNODB;

create unique index Index_shoppinglist_guid on t_shoppinglist
( 
  guid 
);

delimiter //
drop procedure if exists loadshoppinglist //
create procedure loadshoppinglist()
begin
  select   guid,
           sendguid,
           sendname,
           receiveguid,
           receivename,
           createtime,
           goodsid,
           goodscount,
           listtype
  from t_shoppinglist where isvalid=1;
end; //
delimiter ;

delimiter //
drop procedure if exists deleteshoppinglist //
create procedure deleteshoppinglist()
begin
  update t_shoppinglist set isvalid=0;
end; //
delimiter ;

delimiter //
drop procedure if exists saveshoppinglist //
create procedure saveshoppinglist(
                         pguid          bigint,
                         psendguid      bigint,
                         psendname      varchar(80) binary,
                         preceiveguid   bigint,
                         preceivename   varchar(80) binary,
                         pcreatetime    bigint,
                         pgoodsid       varchar(21),
                         pgoodscount    varchar(21),
                         plisttype      tinyint,
                         pisvalid       tinyint)
begin
declare rguid        bigint  default -1;
start transaction;
  select pguid into rguid from t_shoppinglist where guid=pguid;
    if rguid = pguid then
      update t_shoppinglist set 
                   guid=pguid,
                   sendguid=psendguid,
                   sendname=psendname,
                   receiveguid=preceiveguid,
                   receivename=preceivename,
                   createtime=pcreatetime,
                   goodsid=pgoodsid,
                   goodscount=pgoodscount,
                   listtype=plisttype,
                   isvalid=pisvalid
      where  guid = pguid;
    else
      insert into t_shoppinglist(
                   guid,
                   sendguid,
                   sendname,
                   receiveguid,
                   receivename,
                   createtime,
                   goodsid,
                   goodscount,
                   listtype,
                   isvalid)
                  values(
                   pguid,
                   psendguid,
                   psendname,
                   preceiveguid,
                   preceivename,
                   pcreatetime,
                   pgoodsid,
                   pgoodscount,
                   plisttype,
                   pisvalid);                                    
    end if;
  commit;
end; //
delimiter ;
/*==============================================================*/
/* Table: t_hongbao;             */
/*==============================================================*/
drop table if exists t_hongbao;
create table if not exists t_hongbao
(
  aid                                 bigint                       not null AUTO_INCREMENT,
  guid                                bigint                       not null default -1,
  sendguid                            bigint                       not null default -1,
  sendname                            varchar(80) binary           not null default '',
  type                                smallint                     not null default 0,
  channel                             smallint                     not null default 0,
  yuanbao                             int                          not null default 0,
  param                               bigint                       not null default -1,
  robbers                             varchar(161)                 not null default '',
  isvalid                             tinyint                      not null default 0,
  primary key (aid)              
)ENGINE = INNODB;

create unique index Index_hongbao_guid on t_hongbao
( 
  guid 
);

delimiter //
drop procedure if exists loadhongbao //
create procedure loadhongbao()
begin
  select   guid,
           sendguid,
           sendname,
           type,
           channel,
           yuanbao,
           param,
           robbers
  from t_hongbao where isvalid=1;
end; //
delimiter ;

delimiter //
drop procedure if exists deletehongbao //
create procedure deletehongbao()
begin
  update t_hongbao set isvalid=0;
end; //
delimiter ;

delimiter //
drop procedure if exists savehongbao //
create procedure savehongbao(
                         pguid   bigint,
                         psendguid   bigint,
                         psendname   varchar(80) binary,
                         ptype       smallint,
                         pchannel    smallint,
                         pyuanbao    int,
                         pparam      bigint,
                         probbers    varchar(161),
                         pisvalid    tinyint)
begin
declare rguid        bigint  default -1;
start transaction;
  select pguid into rguid from t_hongbao where guid=pguid;
    if rguid = pguid then
      update t_hongbao set 
                         guid=pguid,
                         sendguid=psendguid,
                         sendname=psendname,
                         type=ptype,
                         channel=pchannel,
                         yuanbao=pyuanbao,
                         param=pparam,
                         robbers=probbers,
                         isvalid=pisvalid
      where  guid = pguid;
    else
      insert into t_hongbao(
                   guid,
                   sendguid,
                   sendname,
                   type,
                   channel,
                   yuanbao,
                   param,
                   robbers,
                   isvalid)
                  values(
                   pguid,
                   psendguid,
                   psendname,
                   ptype,
                   pchannel,
                   pyuanbao,
                   pparam,
                   probbers,
                   pisvalid);                                    
    end if;
  commit;
end; //
delimiter ;
/*==============================================================*/
/* Table: t_herocode;             */
/*==============================================================*/
drop table if exists t_herocode;
create table if not exists t_herocode
(
  aid                                 bigint                       not null AUTO_INCREMENT,
  senderguid                          bigint                       not null default -1,
  guidlistsize                        smallint                     not null default 0,
  guidlist                            varchar(161)                 not null default '',
  isvalid                             tinyint                      not null default 0,
  primary key (aid)              
)ENGINE = INNODB;

create unique index Index_herocode_guid on t_herocode
( 
  senderguid 
);

delimiter //
drop procedure if exists loadherocode //
create procedure loadherocode()
begin
  select   senderguid,
           guidlistsize,
           guidlist
  from t_herocode where isvalid=1;
end; //
delimiter ;

delimiter //
drop procedure if exists deleteherocode //
create procedure deleteherocode()
begin
  update t_herocode set isvalid=0;
end; //
delimiter ;

delimiter //
drop procedure if exists saveherocode //
create procedure saveherocode(
                         psenderguid   bigint,
                         pguidlistsize smallint,
                         pguidlist     varchar(161),
                         pisvalid      tinyint)
begin
declare rguid        bigint  default -1;
start transaction;
  select psenderguid into rguid from t_herocode where senderguid=psenderguid;
    if rguid = psenderguid then
      update t_herocode set 
                         senderguid=psenderguid,
                         guidlistsize=pguidlistsize,
                         guidlist=pguidlist,
                         isvalid=pisvalid
      where  senderguid = psenderguid;
    else
      insert into t_herocode(
                   senderguid,
                   guidlistsize,
                   guidlist,
                   isvalid)
                  values(
                   psenderguid,
                   pguidlistsize,
                   pguidlist,
                   pisvalid);                                    
    end if;
  commit;
end; //
delimiter ;
/*==============================================================*/
/* Table: t_auctionprice;             */
/*==============================================================*/
drop table if exists t_auctionprice;
create table if not exists t_auctionprice
(
  aid                                 bigint                       not null AUTO_INCREMENT,
  itemindex                           smallint                     not null default -1,
  itemid                              int                          not null default -1,
  charguid                            bigint                       not null default -1,
  charname                            varchar(80) binary           not null default '',
  time                                bigint                       not null default 0,
  price                               int                          not null default 0,
  iswinner                            tinyint                      not null default 0,
  primary key (aid)              
)ENGINE = INNODB;

create index Index_auctionprice_itemindex on t_auctionprice
( 
  itemindex 
);

delimiter //
drop procedure if exists loadauthionprice //
create procedure loadauthionprice()
begin
    select itemindex,itemid,charguid,charname,time,price,iswinner from t_auctionprice;
end; //
delimiter ;

delimiter //
drop procedure if exists deleteauthionprice //
create procedure deleteauthionprice(pitemindex smallint)
begin
    delete from t_auctionprice where itemindex = pitemindex;
end; //
delimiter ;

delimiter //
drop procedure if exists saveauthionprice //
create procedure saveauthionprice(pdatas mediumtext)
begin
declare stmt mediumtext default '';
declare textlength int default 0;

start transaction;

select LENGTH(pdatas) into textlength;

if textlength > 0 then 

    set @sqlstr = "insert into t_auctionprice(itemindex,itemid,charguid,charname,time,price,iswinner) values";
    set @sqlstr = concat(@sqlstr,pdatas);

    prepare stmt from @sqlstr;
    execute stmt;
    deallocate prepare stmt;

end if;

commit;
end;//
delimiter ;
/*************************************************/
/*      GM工具脚本v0-01-600120                     */
/*************************************************/
delimiter //

drop procedure if exists gm_get_version//
create procedure gm_get_version()
begin
    select 600120;
end;//

drop function if exists gm_hexstr2uint_f;//
create function gm_hexstr2uint_f(h varchar(8))
returns int unsigned
DETERMINISTIC
begin
    declare result int unsigned default 0;
    declare len int default 0;
    declare i int default 1;
    declare base bigint default 1;
    declare tmp char(2);

    set len = length(h);
    while i <= len do
        set tmp = substring(h, i, 2);
        set result = result + conv(tmp, 16, 10) * base;
        set i = i + 2;
        set base = base * 256;
    end while;

    return result;
end;//

drop function if exists gm_get_uint_from_hexstr_at_f;//
create function gm_get_uint_from_hexstr_at_f(hexstr varchar(255), pos int)
returns int
DETERMINISTIC
begin
    return gm_hexstr2uint_f(substring(hexstr, pos * 8 + 1, 8));
end;//

drop function if exists pro_hex2int;//
drop function if exists gm_hexstr2int_f;//
create function gm_hexstr2int_f(h varchar(8))
returns int
DETERMINISTIC
begin
    declare result int unsigned default 0;
    declare len int default 0;
    declare i int default 1;
    declare base bigint default 1;
    declare tmp char(2);

    set len = length(h);
    while i <= len do
        set tmp = substring(h, i, 2);
        set result = result + conv(tmp, 16, 10) * base;
        set i = i + 2;
        set base = base * 256;
    end while;

    if result > 2147483647 then
        return -(4294967296 - result);
    else
        return result;
    end if;
end;//

drop function if exists gm_get_int_from_hexstr_at_f;//
create function gm_get_int_from_hexstr_at_f(hexstr varchar(255), pos int)
returns int
DETERMINISTIC
begin
    return gm_hexstr2int_f(substring(hexstr, pos * 8 + 1, 8));
end;//

drop function if exists pro_int2hex;//
drop function if exists gm_int2hexstr_f;//
create function gm_int2hexstr_f(i int)
returns varchar(60) 
DETERMINISTIC
begin
    declare result varchar(60);
    declare result_1 varchar(60);
    declare result_2 varchar(60);
    declare result_3 varchar(60);
    declare result_4 varchar(60);
    declare result_default varchar(60) default "FFFFFFFF";
    declare varshang int default 1;
    declare base int default 1;
    declare varyu int default 1;
    declare varyutmp varchar(1);
    declare varyutmp_1 varchar(8);
    declare varyutmp_2 varchar(8);
    declare varcount int default 0;
    declare varzero_num int default 0;
    declare varzero_ch char(8);
    declare j int default 0;

    if i=-1 then 
        return result_default;
    end if;

    set varyutmp_2 = '';
    while varshang>0 do
        set varshang = i div 16;  
        set varyu = i mod 16;
        case varyu
            when 1 then set varyutmp='1';
            when 2 then set varyutmp='2';
            when 3 then set varyutmp='3';
            when 4 then set varyutmp='4';
            when 5 then set varyutmp='5';
            when 6 then set varyutmp='6';
            when 7 then set varyutmp='7';
            when 8 then set varyutmp='8';
            when 9 then set varyutmp='9';
            when 10 then set varyutmp='A';
            when 11 then set varyutmp='B';
            when 12 then set varyutmp='C';
            when 13 then set varyutmp='D';
            when 14 then set varyutmp='E';
            when 15 then set varyutmp='F';
            else set varyutmp = '0';
        end case;
        set varyutmp_2 = concat(varyutmp,varyutmp_2);
        set i = varshang;
        set varcount = varcount + 1;
    end while;
    set varzero_num = 8 - varcount;
    case varzero_num
        when 1 then set varzero_ch='0';
        when 2 then set varzero_ch='00';
        when 3 then set varzero_ch='000';
        when 4 then set varzero_ch='0000';
        when 5 then set varzero_ch='00000';
        when 6 then set varzero_ch='000000';
        when 7 then set varzero_ch='0000000';
        when 8 then set varzero_ch='00000000';
        else set varzero_ch = '0';
    end case;
    set varyutmp_2 = concat(varzero_ch,varyutmp_2);
    set result_1 = substr(varyutmp_2, 7, 2);
    set result_2 = substr(varyutmp_2, 5, 2);
    set result_3 = substr(varyutmp_2, 3, 2);
    set result_4 = substr(varyutmp_2, 1, 2);
    set result = concat(result_1,result_2,result_3,result_4);
    RETURN result;
end;//
delimiter ;

delimiter //
drop procedure if exists gm_querycharitem;//
create procedure gm_querycharitem(nCharGuid bigint)
	begin
	select dataid,packtype,stackcount,guid,packindex from t_item WHERE charguid = nCharGuid and isvalid =1;
end;//
delimiter ;

delimiter //
drop procedure if exists gm_querycharpets;//
create procedure gm_querycharpets(nCharGuid bigint)
begin
        select guid, dataid,name,level,exp,locked,quality,starlevel,zzpoint,zizhi_attack,zizhi_hit,zizhi_critical,zizhi_attackspeed,zizhi_bless,packindex
            from t_fellow where charguid = nCharGuid and isvalid=1;
end;//
delimiter ;

delimiter //
drop procedure if exists gm_querycharbelle;//
create procedure gm_querycharbelle(nCharGuid bigint)
begin
        select belleindex,level,matrixid,lastevoltime,favourval from t_belle where charguid = nCharGuid and isvalid=1; 
end;//
delimiter ;

#================================================================
#酒楼桌子
#================================================================
delimiter //
drop procedure if exists gm_queryrestaurantdesk;//
create procedure gm_queryrestaurantdesk(pcharguid bigint)
begin
     select charguid,deskindex,state,ftime,curfood,guest
         from t_desk where charguid=pcharguid and isvalid=1;
end;//
delimiter ;

#================================================================
#酒楼信息
#================================================================
delimiter //
drop procedure if exists gm_queryrestaurantinfo;//
create procedure gm_queryrestaurantinfo(pcharguid bigint)
begin
     select 
     c.rlevel, 
     c.rexp,
     c.rfriendcount, 
     c1.rfriendlist  
     from t_char AS c,t_char_ext1 AS c1 where c.charguid=pcharguid and c1.charguid=pcharguid and c.isvalid=1;
end;//
delimiter ;

#================================================================
#排行榜
#================================================================
delimiter //
drop procedure if exists gm_queryrankdata;//
create procedure gm_queryrankdata(pranktype smallint)
BEGIN
  select ranktype,
         guid1,
         szvalue1,
         n64value1,
         n32value1,
         n32value2,
         n32value3,
         szvalue2,
         n64value2
				from t_rank where ranktype=pranktype and isvalid=1;
end;//
delimiter ;
#======================================================================
#扣除金钱
#======================================================================

delimiter //
drop procedure if exists gm_DeleteRoleCoin;//
create procedure gm_DeleteRoleCoin(pcharguid bigint, pmoneycoin int)
begin
	update t_char set moneycoin=if(moneycoin-pmoneycoin < 0, 0, moneycoin-pmoneycoin) where charguid=pcharguid;
end;//
delimiter ;

#======================================================================
#扣除元宝
#======================================================================
delimiter //
drop procedure if exists gm_DeleteRoleYuanBao;//
create procedure gm_DeleteRoleYuanBao(pcharguid bigint, pmoneyyuanbao int)
begin
	update t_char set moneyyb=if(moneyyb-pmoneyyuanbao < 0, 0, moneyyb-pmoneyyuanbao) where charguid=pcharguid;
end;//
delimiter ;
#======================================================================
#扣除绑定元宝
#======================================================================
delimiter //
drop procedure if exists gm_DeleteRoleYuanBaoBind;//
create procedure gm_DeleteRoleYuanBaoBind(pcharguid bigint, pmoneyyuanbaobind int)
begin
	update t_char set moneyybbind=if(moneyybbind-pmoneyyuanbaobind < 0, 0, moneyybbind-pmoneyyuanbaobind) where charguid=pcharguid;
end;//
delimiter ;

#======================================================================
#设置vipcost
#======================================================================

delimiter //
drop procedure if exists gm_SetVipCost;//
create procedure gm_SetVipCost(pcharguid bigint, pvipconst int)
begin
	update t_char set vipcost=pvipconst where charguid=pcharguid;
end;//
delimiter ;

#======================================================================
#扣除物品
#======================================================================

delimiter //
drop procedure if exists gm_DeleteRoleItem;//
create procedure gm_DeleteRoleItem(pcharguid bigint, pitemguid bigint, itemcount int)
begin
	update t_item set stackcount = if(stackcount-itemcount < 0, 0, stackcount-itemcount), isvalid = if(stackcount = 0, 0, 1) where charguid=pcharguid and guid=pitemguid;
end;//
delimiter ;

#======================================================================
#查询招财进宝和月卡
#======================================================================

delimiter //
drop procedure if exists gm_GetPayFlag;//
create procedure gm_GetPayFlag(pcharguid bigint)
begin
	  select payflag,monthcardbegin,monthcardyb from t_char where charguid=pcharguid and isvalid=1;
end;//
delimiter ;
#======================================================================
#设置招财进宝和月卡
#======================================================================

delimiter //
drop procedure if exists gm_SetPayFlag;//
create procedure gm_SetPayFlag(pcharguid bigint, ppayflag varchar(5),pmonthcardbegin bigint, pmonthcardyb int)
begin
	update t_char set payflag=ppayflag,monthcardbegin=pmonthcardbegin,monthcardyb=pmonthcardyb where charguid=pcharguid;
end;//
delimiter ;
#================================================================
#删档脚本相关
#================================================================
drop table if exists t_backup_tmp_guid;
create table if not exists t_backup_tmp_guid(charguid bigint not null default 0,primary key (charguid));

#======================================================================
# 玩家补偿表
#======================================================================
drop table if exists t_redeem;
create table if not exists t_redeem
(
   aid                            bigint                         not null AUTO_INCREMENT,
   charguid                       bigint                         not null default -1,
   type                           int                            not null default -1,
   param1                         int                            not null default 0,
   param2                         int                            not null default 0,
   param3                         int                            not null default 0,
   param4                         int                            not null default 0,
   param5                         int                            not null default 0,
   param6                         int                            not null default 0,
   primary key (aid)
)ENGINE = INNODB;
#======================================================================
# 玩家补偿表 Index
#======================================================================
create index Index_Redeem on t_redeem
(
   charguid,type
);
#======================================================================
# 玩家补偿表 loadredeem
#======================================================================
delimiter //
drop procedure if exists loadredeem//
create procedure loadredeem( pcharguid bigint )
begin
  select type,param1,param2,param3,param4,param5,param6
  from t_redeem 
  where charguid=pcharguid;
end;//
delimiter ;
#======================================================================
# 玩家补偿表 saveredeem
#======================================================================
delimiter //
drop procedure if exists saveredeem //
create procedure saveredeem(
in pcharguid bigint,
in pdatas mediumtext
)
begin
  declare stmt mediumtext default '';
  declare rowcount int default 0;
  declare textlength int default 0;

  start transaction;

  select count(*) into rowcount from t_redeem where charguid=pcharguid;

  if rowcount > 0 then
      delete from t_redeem where charguid=pcharguid;
  end if;

  select LENGTH(pdatas) into textlength;

  if textlength > 0 then 
  
    set @sqlstr = "insert into t_redeem(charguid,type,param1,param2,param3,param4,param5,param6) values";
    set @sqlstr = concat(@sqlstr,pdatas);

    prepare stmt from @sqlstr;
    execute stmt;
    deallocate prepare stmt;
    
  end if; 

  commit;
end;//
delimiter ;

#======================================================================
# 离线命令表
#======================================================================
drop table if exists t_offlinecommand;
create table if not exists t_offlinecommand
(
   aid                          bigint                         not null AUTO_INCREMENT,
   charguid                     bigint                         not null default -1,
   sln                          int                            not null default -1,
   guid1                        bigint                         not null default -1,
   guid2                        bigint                         not null default -1,
   integer1                     int                            not null default -1,
   integer2                     int                            not null default -1,
   integer3                     int                            not null default -1,
   integer4                     int                            not null default -1,
   char1                        varchar(80)                    not null default '',
   primary key (aid)
)ENGINE = INNODB;
#======================================================================
# 离线命令表 Index
#======================================================================
create index Index_OfflineCommand on t_offlinecommand
(
   charguid
);
#======================================================================
# 离线命令表 loadofflinecmd
#======================================================================
delimiter //
drop procedure if exists loadofflinecmd//
create procedure loadofflinecmd( pcharguid bigint )
begin
  select aid, charguid, sln, guid1, guid2, integer1, integer2, integer3, integer4, char1
  from t_offlinecommand
  where charguid = pcharguid order by aid;
end;//
delimiter ;
#======================================================================
# 离线命令表 deleteofflinecmd
#======================================================================
delimiter //
drop procedure if exists deleteofflinecmd//
create procedure deleteofflinecmd( pcharguid bigint ,pindex int )
begin
  delete from t_offlinecommand
  where charguid = pcharguid and aid = pindex;
end;//
delimiter ;
#======================================================================
# 离线命令表 saveofflinecmd
#======================================================================
delimiter //
drop procedure if exists saveofflinecmd //
create procedure saveofflinecmd(
  pcharguid bigint,
  psln int,
  pguid1 bigint,
  pguid2 bigint,
  pint1 int,
  pint2 int,
  pint3 int,
  pint4 int,
  pchar1 varchar(80)
)
begin
  declare rcount int default 0;

  start transaction;
  select count(*) into rcount from t_char where charguid = pcharguid;
  if rcount > 0 then
    insert into t_offlinecommand
    ( charguid, 
      sln, 
      guid1, 
      guid2, 
      integer1, 
      integer2, 
      integer3, 
      integer4,
      char1 )
    values
    ( pcharguid, 
      psln, 
      pguid1, 
      pguid2, 
      pint1, 
      pint2, 
      pint3, 
      pint4,
      pchar1);
  end if;
  commit;
  
end;//
delimiter ;
#======================================================================
# 查找重复名称 findrepeatname
#======================================================================
delimiter //
drop procedure if exists findrepeatname //
create procedure findrepeatname(
  pcharname varchar(80) binary
)
begin  
  select aid from t_char where charname = pcharname;  
end;//
delimiter ;
#======================================================================
# 统计DB金钱数量 calctotalmoney
#======================================================================
delimiter //
drop procedure if exists calctotalmoney //
create procedure calctotalmoney()
begin  
  select sum(moneycoin),sum(moneyyb),sum(moneyybbind) from t_char;
end;//
delimiter ;

/*==============================================================*/
/* Table: t_usermail;             */
/* t_mail表包含t_item表相关列，t_item表有修改时，*/
/* 辛苦确认是否需要同步修改t_usermail相关列*/
/* t_usermail表不能再增加唯一索引*/
/*==============================================================*/
drop table if exists t_usermail;
create table if not exists t_usermail
(
  aid                             bigint                        not null AUTO_INCREMENT,
  guid                            bigint                        not null default -1,
  sendguid                        bigint                        not null default -1,
  sendname                        varchar(80) binary            not null,
  writetime                       bigint                        not null default  0,
  receiveguid                     bigint                        not null default -1,
  readtime                        bigint                        not null default  0,
  mailtype                        smallint                      not null default  0,
  content                         varchar(192)                  not null,
  moneytype                       smallint                      not null default  0,
  moneyvalue                      int                           not null default  0,  
  boxtype                         tinyint                       not null default  0,
  isvalid                         tinyint                       not null default  0,
  itemguid                        bigint                        not null default -1,
  dataid                          int                           not null default -1,
  binded                          tinyint                       not null default -1,
  stackcount                      smallint                      not null default -1,
  createtime                      bigint                        not null default -1,
  enchancelevel                   smallint                      not null default -1,
  enchanceexp                     int                           not null default -1,
  enchancetotalexp                bigint                        not null default -1,
  starlevel                       smallint                      not null default -1,
  startimes                       smallint                      not null default -1,
  dynamicdata1                    int                           not null default -1,
  dynamicdata2                    int                           not null default -1,
  dynamicdata3                    int                           not null default -1,
  dynamicdata4                    int                           not null default -1,
  dynamicdata5                    int                           not null default -1,
  dynamicdata6                    int                           not null default -1,
  dynamicdata7                    int                           not null default -1,
  dynamicdata8                    int                           not null default -1,
  origin                          int                           not null default 0,
  primary key (aid)              
)ENGINE = INNODB;

create index Index_usermail_guid on t_usermail
( 
  receiveguid,
  guid,
  boxtype
);
delimiter //
drop procedure if exists loadusermail //
create procedure loadusermail(pcharguid bigint,pboxtype tinyint)
begin
  select guid,sendguid,sendname,writetime,receiveguid,readtime,mailtype,content,moneytype,moneyvalue,\
  boxtype,isvalid,itemguid,dataid,binded,stackcount,createtime,enchancelevel,enchanceexp,enchancetotalexp,starlevel,\
  startimes,dynamicdata1,dynamicdata2,dynamicdata3,dynamicdata4,dynamicdata5,dynamicdata6,dynamicdata7,dynamicdata8,origin\
from t_usermail where receiveguid = pcharguid and boxtype=pboxtype and isvalid=1;
end; //
delimiter ;

/*==============================================================*/
/* Table: t_shenyi;             */
/*==============================================================*/
	
drop table if exists t_shenyi;
create table if not exists t_shenyi
(
  aid                             bigint                        not null AUTO_INCREMENT,
  charguid                        bigint                        not null default -1,
  shenyiid                   	  int                        	not null default -1,
  shenyiquality                   int                        	not null default  0,  
  shenyiqualityexp                int                        	not null default  0,  
  shenyilevel                     int                        	not null default  0,  
  shenyiexp                       int                        	not null default  0,  
  shenyinormalcyct                int                        	not null default  0,  
  shenyivipcyct                   int                        	not null default  0,  
  attrlist                        varchar(257) binary           not null default '',
  isvalid                         tinyint                       not null default -1,      
  primary key (aid)              
)ENGINE = INNODB;

create unique index Index_shenyi_guid on t_shenyi
( 
  charguid,
  shenyiid
);

delimiter //
drop procedure if exists loadshengyi //
create procedure loadshengyi(pcharguid bigint)
begin
  select charguid,shenyiid,shenyiquality,shenyiqualityexp,shenyilevel,shenyiexp,shenyinormalcyct,shenyivipcyct,attrlist\
	from t_shenyi where charguid = pcharguid and isvalid=1;
end; //
delimiter ;

delimiter //
drop procedure if exists deleteshenyi //
create procedure deleteshenyi(pcharguid   bigint)
begin
delete from t_shenyi where charguid=pcharguid;
end; //
delimiter ;


delimiter //
drop procedure if exists saveshenyi //
create procedure saveshenyi( pcharguid          	bigint,
                             pshenyiid         		int,
							 pshenyiquality         int,
							 pshenyiqualityexp      int,
							 pshenyilevel           int,
							 pshenyiexp             int,
							 pshenyinormalcyct      int,
							 pshenyivipcyct         int,                         
                             pattrlist            	varchar(257),
                             pisvalid               tinyint)
begin
declare rshenyiid  int default -1;
start transaction;
select shenyiid into rshenyiid from t_shenyi where pcharguid=charguid and shenyiid=pshenyiid;
if FOUND_ROWS() > 0 then
    update t_shenyi set charguid=pcharguid,
                        shenyiid=pshenyiid,
                        shenyiquality=pshenyiquality,
                        shenyiqualityexp=pshenyiqualityexp,
                        shenyilevel=pshenyilevel,
                        shenyiexp=pshenyiexp,
                        shenyinormalcyct=pshenyinormalcyct,
                        shenyivipcyct=pshenyivipcyct,
                        attrlist=pattrlist,                                                       
                        isvalid=pisvalid
    where charguid=pcharguid and shenyiid=pshenyiid;
else
    insert into t_shenyi (charguid,shenyiid,shenyiquality,shenyiqualityexp,shenyilevel,shenyiexp,shenyinormalcyct,shenyivipcyct,attrlist,isvalid)\
							values(pcharguid,
                             pshenyiid,
							 pshenyiquality,
							 pshenyiqualityexp,
							 pshenyilevel,
							 pshenyiexp,
							 pshenyinormalcyct,
							 pshenyivipcyct,                         
                             pattrlist,
                             pisvalid);                                    
end if;
commit;
end; //
delimiter ;

delimiter //
drop procedure if exists savecharunforbidtradetime//
create procedure savecharunforbidtradetime(pcharguid bigint,
                                          punforbidtradetime bigint)
begin
update t_char set unforbidtradetime= punforbidtradetime where charguid=pcharguid;
end;//
delimiter ;

delimiter //
drop procedure if exists loadcharlistshenyi //
create procedure loadcharlistshenyi(pcharguid   bigint)
begin
select shenyiid,shenyiquality from t_shenyi where charguid = pcharguid and isvalid=1;
end; //
delimiter ;

/*==============================================================*/
/* Table: t_jieyi;             */
/*==============================================================*/
drop table if exists t_jieyi;
create table if not exists t_jieyi
(
  aid                             bigint                        not null AUTO_INCREMENT,
  guid                            bigint                        not null default -1,
  name                            varchar(50)                   not null,
  notice                          varchar(100)                  not null,
  createtime                      int  unsigned                 not null default 0,
  dieoutday                       tinyint                      	not null default 0,
  combatvalue                     int                           not null default 0,
  isvalid                         tinyint                       not null default 0,
  primary key (aid)              
)ENGINE = INNODB;

create unique index Index_jieyi_guid on t_jieyi
( 
  guid
);
delimiter //
drop procedure if exists loadjieyi //
create procedure loadjieyi()
begin
  select guid,
         name,
         notice,   
         createtime,        
         dieoutday,
		 combatvalue
  from t_jieyi where isvalid=1;
end; //
delimiter ;

delimiter //
drop procedure if exists deletejieyi //
create procedure deletejieyi()
begin
  update t_jieyi set isvalid=0;
end; //
delimiter ;

delimiter //
drop procedure if exists savejieyi //
create procedure savejieyi(
                          pguid       	bigint,
                          pname       	varchar(50),
                          pnotice     	varchar(100),                             
                          pcreatetime 	int unsigned,                                             
                          pdieoutday  	tinyint,
						  pcombatvalue 	int,
                          pisvalid    	tinyint)
						  
begin
declare rguid        bigint  default -1;
start transaction;
  select pguid into rguid from t_jieyi where guid=pguid;
    if rguid = pguid then
      update t_jieyi set 
                  guid=pguid,
                  name=pname,
                  notice=pnotice,                            
                  createtime=pcreatetime,                
                  dieoutday=pdieoutday,
				  combatvalue=pcombatvalue,
                  isvalid=pisvalid       
      where  guid = pguid;
    else
      insert into t_jieyi(
                  guid,
                  name,
                  notice,                         
                  createtime,
                  dieoutday,
				  combatvalue,
                  isvalid)
                  values(
                    pguid,
                    pname,
                    pnotice,                              
                    pcreatetime,
                    pdieoutday,
					pcombatvalue,
                    pisvalid);                                    
    end if;
  commit;
end; //
delimiter ;
/*==============================================================*/
/* Table: t_jieyi_user;             */
/*==============================================================*/
drop table if exists t_jieyi_user;
create table if not exists t_jieyi_user
(
  aid                             bigint                        not null AUTO_INCREMENT,
  jieyiguid                       bigint                        not null default -1,
  guid                            bigint                        not null default -1,
  name                            varchar(80) binary            not null,
  profession                      tinyint                       not null default -1,
  level                           smallint                      not null default 0,
  job                      	  	  tinyint                       not null default -1,
  lastlogin                       bigint                        not null default 0,
  jointime                        bigint                        not null default 0,
  isvalid                         tinyint                       not null default 0,
  combatvalue                     int                        	not null default 0,
  primary key (aid)              
)ENGINE = INNODB;

create unique index Index_jieyi_user_guid on t_jieyi_user
( 
  guid
);

delimiter //
drop procedure if exists loadjieyiuser //
create procedure loadjieyiuser(pjieyiguid bigint)
begin
  select guid,
         name,      
         profession,
         level,               
         job,
		 lastlogin,
		 jointime,
		 combatvalue
  from t_jieyi_user where isvalid=1 and jieyiguid=pjieyiguid;
end; //
delimiter ;

delimiter //
drop procedure if exists deletejieyiuser //
create procedure deletejieyiuser(pjieyiguid bigint)
begin
  update t_jieyi_user set isvalid=0 where jieyiguid=pjieyiguid;
end; //
delimiter ;

delimiter //
drop procedure if exists savejieyiuser //
create procedure savejieyiuser(
                         pjieyiguid   bigint,
                         pguid        bigint,
                         pname        varchar(80) binary,                                          
                         pprofession  tinyint,
                         plevel       smallint,                                                         
                         pjob   	  tinyint,
						 plastlogin   bigint,   
						 pjointime    bigint, 
						 pcombatvalue int,						 
                         pisvalid     tinyint)
begin
declare rguid        bigint  default -1;
start transaction;
select pguid into rguid from t_jieyi_user where guid=pguid;
  if rguid = pguid then
    update t_jieyi_user set 
                  jieyiguid=pjieyiguid,
                  guid=pguid,
                  name=pname,             
                  profession=pprofession,
                  level=plevel,  
				  job=pjob,
                  lastlogin=plastlogin,               
                  jointime=pjointime, 
				  combatvalue=pcombatvalue,
                  isvalid=pisvalid              
    where  guid = pguid;
  else
    insert into t_jieyi_user(
                   jieyiguid,
                   guid,
                   name,                             
                   profession,
                   level,                                                
                   job,
				   lastlogin, 
				   jointime,
				   combatvalue,
                   isvalid)
                   values( 
                   pjieyiguid,
                   pguid,
                   pname,
                   pprofession,
                   plevel,
				   pjob,
                   plastlogin,
                   pjointime,
				   pcombatvalue,
                   pisvalid);                                 
  end if;
commit;
end; //
delimiter ;

/*==============================================================*/
/* Table: t_achievement;             */
/*==============================================================*/
	
drop table if exists t_achievement;
create table if not exists t_achievement
(
  aid                             bigint                        not null AUTO_INCREMENT,
  charguid                        bigint                        not null default -1,
  achilist                        varchar(6145) binary           not null default '',
  isvalid                         tinyint                       not null default -1,      
  primary key (aid)              
)ENGINE = INNODB;

create unique index Index_achievement_guid on t_achievement
( 
  charguid
);

delimiter //
drop procedure if exists loadachievement //
create procedure loadachievement(pcharguid bigint)
begin
  select charguid,achilist\
	from t_achievement where charguid = pcharguid and isvalid=1;
end; //
delimiter ;

delimiter //
drop procedure if exists deleteachievement //
create procedure deleteachievement(pcharguid   bigint)
begin
delete from t_achievement where charguid=pcharguid;
end; //
delimiter ;


delimiter //
drop procedure if exists saveachievement //
create procedure saveachievement( pcharguid          	bigint,
                             pachilist            	varchar(6145),
                             pisvalid               tinyint)
begin
start transaction;
select charguid from t_achievement where pcharguid=charguid;
if FOUND_ROWS() > 0 then
    update t_achievement set charguid=pcharguid,
                        achilist=pachilist,                                                       
                        isvalid=pisvalid
    where charguid=pcharguid;
else
    insert into t_achievement (charguid,achilist,isvalid)\
							values(pcharguid,
                             pachilist,
                             pisvalid);                                    
end if;
commit;
end; //
delimiter ;

/*==============================================================*/
/* Table: t_chat;             */
/* t_chat表不能再增加唯一索引*/
/*==============================================================*/
drop table if exists t_chat;
create table if not exists t_chat
(
  aid                             bigint                        not null AUTO_INCREMENT,
  guid                            bigint                        not null default -1,
  writetime                       bigint                        not null default -1,
  channel                         int                           not null default -1,
  chatinfo                        varchar(520) binary           not null,
  receiveguid                     bigint                        not null default -1,
  receivename                     varchar(80) binary            not null,
  sendguid                        bigint                        not null default -1,
  sendname                        varchar(80) binary            not null,
  sendviplevel                    int                           not null default  0,
  isvalid                         tinyint                       not null default  0,
  primary key (aid)              
)ENGINE = INNODB;

create unique index Index_chat_guid on t_chat
( 
  guid
);

delimiter //
drop procedure if exists loadchat //
create procedure loadchat()
begin
select guid,writetime,channel,chatinfo,receiveguid,receivename,sendguid,sendname,sendviplevel,isvalid\
from t_chat where isvalid=1;
end; //
delimiter ;


delimiter //
drop procedure if exists deletechat //
create procedure deletechat()
begin
  update t_chat set isvalid=0;
end; //
delimiter ;

delimiter //
drop procedure if exists savechat //
create procedure savechat(pdatas mediumtext)
begin
declare stmt mediumtext default '';

start transaction;

set @sqlstr = concat("insert into t_chat(guid,writetime,channel,chatinfo,receiveguid,receivename,sendguid,sendname,sendviplevel,isvalid) values ", pdatas, "on duplicate key update guid=values(guid),writetime=values(writetime),channel=values(channel),chatinfo=values(chatinfo),receiveguid=values(receiveguid),receivename=values(receivename),sendguid=values(sendguid),sendname=values(sendname),sendviplevel=values(sendviplevel),isvalid=values(isvalid)");

prepare stmt from @sqlstr;
execute stmt;
deallocate prepare stmt;

commit;
end; //
delimiter ;

/*==============================================================*/
/* Table: t_liuyan;             */
/*==============================================================*/
drop table if exists t_liuyan;
create table if not exists t_liuyan
(
  aid                             bigint                        not null AUTO_INCREMENT,
  guid                            bigint                        not null default -1,
  content                         varchar(80)                   not null,
  createtime                      int  unsigned                 not null default 0,
  praise                          int                      	not null default 0,
  trample                         int                           not null default 0,
  senderguid                      bigint                        not null default -1,
  sendername                      varchar(80)                   not null,
  sendervip                       int                           not null default 0,
  senderprofession                tinyint                       not null default 0,
  senderlevel                     int                           not null default 0,
  isvalid                         tinyint                       not null default 0,
  primary key (aid)              
)ENGINE = INNODB;

create unique index Index_liuyan_guid on t_liuyan
( 
  guid
);
delimiter //
drop procedure if exists loadliuyan //
create procedure loadliuyan()
begin
  select guid,
         content,
         createtime,        
         praise,
	 trample,
	 senderguid,
	 sendername,
	 sendervip,
	 senderprofession,
	 senderlevel
  from t_liuyan where isvalid=1;
end; //
delimiter ;

delimiter //
drop procedure if exists deleteliuyan //
create procedure deleteliuyan()
begin
  update t_liuyan set isvalid=0;
end; //
delimiter ;

delimiter //
drop procedure if exists saveliuyan //
create procedure saveliuyan(
                          pguid       	bigint,
                          pcontent    	varchar(80),                             
                          pcreatetime 	int unsigned,                                             
                          ppraise  	int,
			  ptrample 	int,
			  psenderguid   bigint,
			  psendername  	varchar(80),
			  psendervip 	int,
			  psenderprofession tinyint,
			  psenderlevel 	int,
                          pisvalid    	tinyint)
						  
begin
declare rguid        bigint  default -1;
start transaction;
  select pguid into rguid from t_liuyan where guid=pguid;
    if rguid = pguid then
      update t_liuyan set 
                  guid=pguid,
                  content=pcontent,                            
                  createtime=pcreatetime,                
                  praise=ppraise,
		  trample=ptrample,
		  senderguid=psenderguid,
		  sendername=psendername,
		  sendervip=psendervip,
		  senderprofession=psenderprofession,
		  senderlevel=psenderlevel,
                  isvalid=pisvalid       
      where  guid = pguid;
    else
      insert into t_liuyan(
                  guid,
                  content,
                  createtime,        
                  praise,
	          trample,
	          senderguid,
	          sendername,
	          sendervip,
	          senderprofession,
	          senderlevel,
                  isvalid)
                  values(
                    pguid,
                    pcontent,
                    pcreatetime,                              
                    ppraise,
                    ptrample,
		    psenderguid,
		    psendername,
		    psendervip,
		    psenderprofession,
		    psenderlevel,
                    pisvalid);                                    
    end if;
  commit;
end; //
delimiter ;


/*==============================================================*/
/* Table: t_tutor;             */
/*==============================================================*/
drop table if exists t_tutor;
create table if not exists t_tutor
(
  aid                             bigint                 not null AUTO_INCREMENT,
  guid                            bigint                 not null default -1,
  createtime                      bigint                 not null default 0,
  quzhutime                       bigint                 not null default 0,
  dieoutday                       int                    not null default 0,
  shifuguid                       bigint                 not null default -1,
  shifuname                       varchar(80)            not null,
  shifuprofession                 tinyint                not null default 0,
  shifulevel                      smallint               not null default 0,
  shifuvip                        int                    not null default 0,
  shifuteachexp                   int                    not null default 0,
  shifuteachlevel                 smallint               not null default 0,
  shifulogouttime                 bigint                 not null default 0,
  isvalid                         tinyint                not null default 0,
  primary key (aid)              
)ENGINE = INNODB;

create unique index Index_tutor_guid on t_tutor
( 
  guid
);
delimiter //
drop procedure if exists loadtutor //
create procedure loadtutor()
begin
  select guid,
         createtime,
         quzhutime,   
         dieoutday,        
         shifuguid,
         shifuname,
         shifuprofession,
	 shifulevel,
	 shifuvip,
	 shifuteachexp,
	 shifuteachlevel,
	 shifulogouttime
  from t_tutor where isvalid=1;
end; //
delimiter ;

delimiter //
drop procedure if exists deletetutor //
create procedure deletetutor()
begin
  update t_tutor set isvalid=0;
end; //
delimiter ;

delimiter //
drop procedure if exists savetutor //
create procedure savetutor(
                        pguid bigint,
			pcreatetime bigint,
			pquzhutime bigint,
			pdieoutday int,
			pshifuguid bigint,
			pshifuname varchar(80),
			pshifuprofession tinyint,
			pshifulevel smallint,
			pshifuvip int,
			pshifuteachexp int,
			pshifuteachlevel smallint,
			pshifulogouttime bigint,
			pisvalid tinyint)
						  
begin
declare rguid        bigint  default -1;
start transaction;
  select pguid into rguid from t_tutor where guid=pguid;
    if rguid = pguid then
      update t_tutor set 
                guid=pguid,
                createtime=pcreatetime,
		quzhutime=pquzhutime,
		dieoutday=pdieoutday,
		shifuguid=pshifuguid,
		shifuname=pshifuname,
		shifuprofession=pshifuprofession,
		shifulevel=pshifulevel,
		shifuvip=pshifuvip,
		shifuteachexp=pshifuteachexp,
		shifuteachlevel=pshifuteachlevel,
		shifulogouttime=pshifulogouttime,
		isvalid=pisvalid      
      where  guid = pguid;
    else
      insert into t_tutor(
                  guid,
		  createtime,
		  quzhutime,
		  dieoutday,
		  shifuguid,
		  shifuname,
		  shifuprofession,
		  shifulevel,
		  shifuvip,
		  shifuteachexp,
		  shifuteachlevel,
		  shifulogouttime,
		  isvalid)
                  values(
                          pguid,
			  pcreatetime,
			  pquzhutime,
			  pdieoutday,
			  pshifuguid,
			  pshifuname,
			  pshifuprofession,
			  pshifulevel,
			  pshifuvip,
			  pshifuteachexp,
			  pshifuteachlevel,
			  pshifulogouttime,
			  pisvalid);                                    
    end if;
  commit;
end; //
delimiter ;
/*==============================================================*/
/* Table: t_tutor_user;             */
/*==============================================================*/
drop table if exists t_tutor_user;
create table if not exists t_tutor_user
(
  aid                         bigint                 not null AUTO_INCREMENT,
  tutorguid                   bigint                 not null default -1,
  guid                        bigint                 not null default -1,
  name                        varchar(80) binary           not null,
  profession                  tinyint                not null default 0,
  level                       smallint               not null default 0,
  vip                         int                    not null default 0,
  logouttime                  bigint                 not null default 0,
  jointime                    bigint                 not null default 0,
  studyexp                    int                    not null default 0,
  studylevel                  smallint               not null default 0,
  missionquality              tinyint                not null default 0,
  missionboxid                int                    not null default 0,
  missionboxstate             tinyint                not null default 0,
  missionlist                 varchar(100)    not null,
  belleid                     int                    not null default 0,
  bellestate                  tinyint                not null default 0,
  bellelist                   varchar(40)     not null,
  chuangonginfo               varchar(26)     not null,
  chuangonginfo2              varchar(26)     not null,
  chuangonginfo3              varchar(26)     not null,
  isvalid                     tinyint                not null default 0,
  primary key (aid)              
)ENGINE = INNODB;

create unique index Index_tutor_user_guid on t_tutor_user
( 
  guid
);

delimiter //
drop procedure if exists loadtutoruser //
create procedure loadtutoruser(ptutorguid bigint)
begin
  select guid,
         name,      
         profession,
         level,               
         vip,
         logouttime,
	 jointime,
         studyexp,      
         studylevel,           
         missionquality,
         missionboxid,
	 missionboxstate,
         missionlist,
         belleid,               
         bellestate,
         bellelist,
	 chuangonginfo,
	 chuangonginfo2,
	 chuangonginfo3
  from t_tutor_user where isvalid=1 and tutorguid=ptutorguid;
end; //
delimiter ;

delimiter //
drop procedure if exists deletetutoruser //
create procedure deletetutoruser(ptutorguid bigint)
begin
  update t_tutor_user set isvalid=0 where tutorguid=ptutorguid;
end; //
delimiter ;

delimiter //
drop procedure if exists savetutoruser //
create procedure savetutoruser(
                                ptutorguid bigint,
				pguid bigint,
				pname varchar(80) binary,
				pprofession tinyint,
				plevel smallint,
				pvip int,
				plogouttime bigint,
				pjointime bigint,
				pstudyexp int,
				pstudylevel smallint,
				pmissionquality tinyint,
				pmissionboxid int,
				pmissionboxstate tinyint,
				pmissionlist varchar(100),
				pbelleid int,
				pbellestate tinyint,
				pbellelist varchar(40),
				pchuangonginfo varchar(26),
				pchuangonginfo2 varchar(26),
				pchuangonginfo3 varchar(26),
				pisvalid tinyint)
begin
declare rguid        bigint  default -1;
start transaction;
select pguid into rguid from t_tutor_user where guid=pguid;
  if rguid = pguid then
    update t_tutor_user set 
                 tutorguid=ptutorguid,
                 guid=pguid,
		 name=pname,      
		 profession=pprofession,
		 level=plevel,               
		 vip=pvip,
		 logouttime=plogouttime,
		 jointime=pjointime,
		 studyexp=pstudyexp,      
		 studylevel=pstudylevel,              
		 missionquality=pmissionquality,
		 missionboxid=pmissionboxid,
		 missionboxstate=pmissionboxstate,
		 missionlist=pmissionlist,
		 belleid=pbelleid,               
		 bellestate=pbellestate,
		 bellelist=pbellelist,
		 chuangonginfo=pchuangonginfo,
		 chuangonginfo2=pchuangonginfo2,
		 chuangonginfo3=pchuangonginfo3,
                 isvalid=pisvalid          
    where  guid = pguid;
  else
    insert into t_tutor_user(
                 tutorguid,
                 guid,
		 name,      
		 profession,
		 level,               
		 vip,
		 logouttime,
		 jointime,
		 studyexp,      
		 studylevel,              
		 missionquality,
		 missionboxid,
		 missionboxstate,
		 missionlist,
		 belleid,               
		 bellestate,
		 bellelist,
		 chuangonginfo,
		 chuangonginfo2,
		 chuangonginfo3,
                 isvalid)
                   values( 
                         ptutorguid,
			 pguid,
			 pname,      
			 pprofession,
			 plevel,               
			 pvip,
			 plogouttime,
			 pjointime,
			 pstudyexp,      
			 pstudylevel,            
			 pmissionquality,
			 pmissionboxid,
			 pmissionboxstate,
			 pmissionlist,
			 pbelleid,               
			 pbellestate,
			 pbellelist,
			 pchuangonginfo,
			 pchuangonginfo2,
			 pchuangonginfo3,
			 pisvalid);                                 
  end if;
commit;
end; //
delimiter ;

/*==============================================================*/
/* Table: t_push;             */
/*==============================================================*/
drop table if exists t_push;
create table if not exists t_push
(
  aid                             bigint                        not null AUTO_INCREMENT,
  guid                            bigint                        not null default -1,
  pushkey                         varchar(50)                   not null,
  updatetime                      int  unsigned                 not null default 0,
  isvalid                         tinyint                       not null default 0,
  primary key (aid)              
)ENGINE = INNODB;

create unique index Index_push_guid on t_push
( 
  guid
);
delimiter //
drop procedure if exists loadpush //
create procedure loadpush()
begin
  select guid,
         pushkey, 
         updatetime
  from t_push where isvalid=1;
end; //
delimiter ;

delimiter //
drop procedure if exists deletepush //
create procedure deletepush()
begin
  update t_push set isvalid=0;
end; //
delimiter ;

delimiter //
drop procedure if exists savepush //
create procedure savepush(pdatas mediumtext)
begin
declare stmt mediumtext default '';

start transaction;

set @sqlstr = concat("insert into t_push(guid,pushkey,updatetime,isvalid) values ", pdatas, "on duplicate key update guid=values(guid),pushkey=values(pushkey),updatetime=values(updatetime),isvalid=values(isvalid)");

prepare stmt from @sqlstr;
execute stmt;
deallocate prepare stmt;

commit;
end; //
delimiter ;
