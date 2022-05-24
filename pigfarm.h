#ifndef PIGFARM_H
#define PIGFARM_H

#include <QObject>
#include "pigsty.h"
#include "pig.h"
#include "QDebug"
#include "QString"
class PigFarm : public QObject
{
    Q_OBJECT
public:
    explicit PigFarm(QObject *parent = nullptr);
    static const int farmStyNumMax=100;
    PigSty pigStys[100];
    bool farmInfected=0;
    int farmCertainPigNum[3]={0,0,0};
    int farmTotalPigNum=0;
    int dayNum = 0;
    double farmMoney = 200000;

    //临时保存经济活动记录,10个数字依次是买入猪数量:总数,黑猪数,白猪数,花猪数,花了的钱,卖出猪数量:总数,黑猪数,白猪数,花猪数,赚的钱
    float farmLogTemp[10]={0,0,0,0,0,0,0,0,0,0};
    //临时保存提示清理了多少病猪
    int farmCertainDeathNum[3]={0,0,0};

    //随着QTimer进行,会自动更新dayNum
    void farmGrow();
    //买猪,然后顺带把猪放到猪圈里面,顺带平均分配
    void buyPig(int blackNum, int whiteNum, int flowerNum);
    //指定猪感染
    void pigInfect(Pig* p);
    //猪圈里的自动设置数量的函数影响不到农场类,要写一个函数,更新农场数量和健康信息
    void farmSetPigNum();
    //随着QTimer进行, 每天的感染进行中……只有感染的时候会执行这个函数
    void farmInfecting();
    //瘟疫预测, 计算预计多少天农场猪死光(如果不采取措施), 也随着QTimer每天必须要进行
    int farmDeathSimulation();
    //卖猪,然后把猪移除,不进行平均猪圈猪数量处理
    float salePig();
    //一键清除农场所有病猪,同时设置农场健康状态
    void clearFarm();


signals:

};

#endif // PIGFARM_H
