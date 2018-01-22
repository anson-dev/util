#ifndef COMMTYPE_DEFINE_H_
#define COMMTYPE_DEFINE_H_

namespace HUYA {
    
    // use type1, 渠道号定义
    enum{
        kTransactionTypeGambling = 30,        //竞猜 use type2: game_id
        kTransactionTypeGBTransferWB = 31,    //绿豆转白豆
        kTransactionTypeUseItem = 32,         //使用道具 use type2: 道具ID
        kTransactionTypeCard = 33,            //使用卡  use type2: card type
        kTransactionTypeTreasure = 34,        //藏宝图
        kTransactionTypeGoldBTransferGB = 35,   //金币转绿豆
        kTransactionTypeGetGBAfterSettle = 36,   //消费不足返绿豆
        kTransactionTypeYBTransferGB = 37,     //YB充值绿豆
        kTransactionTypeGambleCommission = 38,   //官方竞猜佣金 use type2: game_id
        kTransactionTypePresentation = 39,   //活动赠送
        kTransactionTypeManual = 40,   //手工充值
        kTransactionTypeGoldBAutoTransferGB = 41,   //金币自动转绿豆
        kTransactionTypeBackFromDeposit = 42,   //归还押金
        kTransactionTypeGuardianPresentaton = 43, //守护赠送道具 use type2: 道具ID
        kTransactionTypeWebSpecialTopic = 44, //web专题 use type2: 0
        kTransactionTypeBindingGBTransferGB = 45, //绑定绿豆转绿豆
        kTransactionTypeGBTransferBindingGB = 46, //绿豆转绑定绿豆
        kTransactionTypeNewTask = 47, //新手任务充值白豌豆
        kTransactionTypeJackpotBuy = 48, // jackpot use type2: jackpot ID
        kTransactionTypeJackpotCharge = 49, // jackpot use type2: jackpot ID
        kTransactionTypeDealPrsente = 50, //绿豆首充返利
        kTransactionTypeGoddessVote = 51, //女神活动投票,消费白豆
        kTransactionTypeBuyCreditByGB = 52, //通过绿豆购买竞彩信用
        kTransactionTypeFansProps = 53, //fans_rank send ba, keng, meng to fans: use type2: card type in( 80,81,82)
        kTransactionTypeGoldTicket = 54, //充金豆券
        kTransactionTypeAnnualLottery = 6000, //年度盛典抽奖充值, use type:白豆 or 绿豆
        kTransactionTypeTreasureMap = 6001, //充值藏宝图， usetype2 = fromtype
        kTransactionTypeHideBullet = 6002, //珠海躲子弹游戏， usetype2 = 13
        kTransactionTypeDreamTicketPresentation = 6003, //送梦想票赠送绿豌豆, usetype2 = ItemType_Greenbean
        kTransactionTypeWangYiGameWithGB = 6004, //网易游戏合作送绿豆， usetype2 = 9
        kTransactionTypeWangYiGameWithWB = 6005, //网易游戏合作送白豆， usetype = 8
        kTransactionTypeRewardGB = 6006, //运营奖励绿豆, usetype2 = 9
        kTransactionTypePunishGB = 6007, //运营惩罚扣绿豆, usetype2 =9
        kTransactionTypeTieBaWB = 6008, //百度贴吧白豌豆奖励 usertype2 = 8
        kTransactionTypeFreezGBForCredit = 6009, //提高绿豆信用值冻结, 消费, usetype2 = 9
        kTransactionTypeThawGbForCredit = 6010, //信用值绿豆解冻, 充值, usetype2 = 9 
        kTransactionTypeBlackGoldGB = 6011, //黑金主题充值绿豆, usetype=9
        kTransactionTypeBlackGoldWB = 6012, //黑金主题充值白豆, usetype=8
        kTransactionTypeWangYi2GB = 6013, //网易二期充值绿豆, usetype=9
        kTransactionTypeWangYi2WB = 6014, //网易二期充值白豆, usetype=8
        kTransactionTypeWuHunGB = 6015, //武魂充值绿豆，usetype = 9
        kTransactionTypeWuHunWB = 6016, //武魂充值白豆，usetype = 8
        kTransactionTypeQianNvWB = 6017, //倩女活动充值白豆，usetyp2=8
        kTransactionTypeQianNvGB = 6018, //倩女活动充值绿豆，usetype2=9
        kTransactionTypeJiangHuGB = 6019, //江湖活动充值绿豆，usetype=9, 每次充值额度为1000,2000,5000 
        kTransactionTypeZiZuan = 6020, //紫钻活动充值 10个血瓶+5个蓝瓶+200个霸+25w银豆， usetype2 为充值类型
        kTransactionTypeWarWorld = 6021, //战争世界充值绿豆，usetype2=9 
        kTransactionTypeCsOnline = 6022, //cs online 活动,充值茶叶蛋，藏宝图，金豆，银豆
        kTransactionTypeRepaireFailedRecharge = 6023, //修复充值失败，重新充值给用,usetype2=0
        kTransactionTypeJiuYin = 6024, //九阴真经抽奖，奖品茶叶蛋，金豆，银豆, 藏宝图
        kTransactionTypeZuiLiu = 6025, //最6主播， 批量给用户充值， usetype2=8银豆或usetype=9金, usetype2=2//霸  usetype2=3//坑 usetype2=4//萌
        kTransactionTypeSheDiao = 6026, //射雕活动
        kTransactionTypeJianNvYouHui = 6027, //倩女幽魂2活动
        kTransactionTypeWeiTeQuan = 6028, //微特权，充值“坑”、“霸”、“萌”和“YY藏宝图”
        kTransactionTypeJinGDong = 6029, //京东资源互换活动， by李可
        kTransactionTypeFBToGB = 6030, //开心豆转金豆
        kTransactionTypeGBToFB = 6031, //金豆转开心豆
        kTransactionTypeTexasPoker = 6032, //开心德州
        kTransactionTypeLingYu1 = 6033, //灵域推广活动礼包1 
        kTransactionTypeLingYu2 = 6034, //灵域推广活动礼包2 
        kTransactionTypeLingYu3 = 6035, //灵域推广活动礼3 
        kTransactionTypeLingYu4 = 6036, //灵域推广活动礼4
        kTransactionTypeLingYu5 = 6037, //灵域推广活动礼5 
        kTransactionTypeLingYu6 = 6038, //灵域推广活动礼包6
        kTransactionTypeLingYu7 = 6040, //灵域推广活动礼包7
        kTransactionTypeRechargeFunnybean = 6041, //充值开心豆
        kTransactionTypeYearCeremony = 6042, //年终盛典 by雷雷  usetype2=8银豆或usetype=9金, usetype2=2//霸  usetype2=3//坑 usetype2=4//萌
        kTransactionTypePhoneCardBaKengMeng = 6043, //手机免费充值坑霸萌礼包
        kTransactionTypeTexasPokerInOfficial = 6044, //德州抵押豆子入官方账户
        kTransactionTypeTexasPokerOutOfficial = 6045, //德州从官方账户返回豆子
        kTransactionTypeLinYuPacket1 = 6046, //灵域礼包1-10000银豆
        kTransactionTypeLinYuPacket2 = 6047, //灵域礼包2-5000金豆
        kTransactionTypeLinYuPacket3 = 6048, //灵域礼包3-4900金豆
        kTransactionTypeLinYuPacket5 = 6049, //灵域礼包5-49000金豆
        kTransactionTypePhoneSpread = 6050, //手机推广领取
        kTransactionTypeYule = 6051, //娱乐合作领取
        kTransactionTypeLiYuZhuboPK = 6052, //灵域主播PK大赛
        kTransactionType1206ReRecharge = 6053, // 12月6日当日全部收入重新下发
        kTransactionType1206ReActConsume = 6054, // 12月6日当日错误收入系统扣除 
        kTransactionTypeHuyaApp = 6055, //虎牙直播app每日抽奖活动
        kTransactionType5153Ba = 6056, //礼包1：直播霸气礼包，霸气*10
        kTransactionType5153Meng = 6057, //礼包2：直播好萌礼包，好萌*10
        kTransactionType5153Whitebean = 6058, //礼包3：直播银豆礼包，10000银豆
        kTransactionTypeChaojiZiZuanYueFei = 6059, //超级紫钻月费开通礼包
        kTransactionTypeChaojiZiZuanJiFei = 6060, //超级紫钻季费开通礼包
        kTransactionTypeChaojiZiZuanNianFei = 6061, //超级紫钻年费开通礼包
        kTransactionTypeJingCaiFix = 6062, //竞猜修复 byleilei 
        kTransactionTypeLingYuApple = 6063, //灵域礼包充值小苹果
        kTransactionTypeJiHuoMaDuiHuan = 6064, //激活码兑换by xiaoyuan   
        kTransactionTypeHuYaTexasPoker = 6065, //虎牙德州  //use2 1:购买啤酒，赠送虎牙币 2:消费 3:转入官方账户 4:从官方账户转出 5:过期
        kTransactionTypeHero = 6066, //英雄之刃 byxiaoyun 金豆8000
        kTransactionTypeMinDaiTianXia3888 = 6067, //民贷天下合作 3888金豆 by xiaoyuan
        kTransactionTypeMinDaiTianXia2000 = 6068, //民贷天下合作 2000金豆 by xiaoyuan
        kTransactionTypeJiaJingWen =  6069, //贾静雯微信礼包 10000by黄玲
        kTransactionTypeAvengers = 6070, //曜能量复仇者联盟促销活动 by risheng 
        kTransactionTypeDailyRegister = 6071, //用户生态每日签到赠送霸坑萌 by summer 
        kTransactionTypeAppXiaoMi = 6072, //虎牙App小米渠道活动 获“萌”道具名 by chenbing   
        kTransactionTypeGoldBeanMarketing = 6073,       //金豆销售管理 扣除金豆 by liuguan  1:划扣 2:奖励
        kTransactionGaiZhangRechargeGreenBean = 6099,   // 盖章返冲金豆
    };	 	
};

#endif //#endif COMMTYPE_DEFINE_H_
