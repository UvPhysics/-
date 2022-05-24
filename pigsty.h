#ifndef PIGSTY_H
#define PIGSTY_H
#include "pig.h"
#include <QObject>

class PigSty : public QObject
{
    Q_OBJECT
public:
    explicit PigSty(QObject *parent = nullptr);
    static const int styPigNumMax=10;
    //方便管理的幽灵猪,不算数,为了代码更简单而存在
    Pig* ghostFirstPig=new Pig;
    Pig* ghostEndPig=new Pig;
    int styCertainPigNum[3]={0,0,0};
    int styTotalPigNum=0;
    bool styInfected=0;
    //临时保存提示清理了多少病猪,不算病死的
    int styCertainDeathNum[3]={0,0,0};
    //下面两个函数得到这个猪圈的第一或最后的猪(真的猪)
    Pig* styFirstPig(){return ghostFirstPig->nextPig;}
    Pig* styEndPig(){return ghostEndPig->lastPig;}
    //防疫措施,清除这里的所有猪, 用于有猪感染的时候使用, 顺带重置猪圈感染状态
    void clearPigSty();
    //移除传入的猪, 返回这个猪的指针,不进行delete,自动更新当前猪圈猪的信息
    Pig* removePig(Pig* p);
    //在该猪圈插入传入的猪, 在插入老猪的时候用
    void inPig(Pig * p);
    //上个函数的选定猪种类的版本,用于新猪的函数,买的新猪用这个
    void inPig(int pigTypee);
    //让每个猪生长,同时更新疾病信息(成长总是伴随着死亡),即使刚好病猪死了也不能移除高风险猪圈标签,死没了也不行,必须去手动进行clear函数才能移除高风险标签捏
    void styGrow();
    //打印猪圈信息
    QString styPrint();
    //感染第一个猪
    void styInfectPig();

signals:
    void styIsInfected();
    void styIsCleared();
    void styHaveHealthPig();

};

#endif // PIGSTY_H
