#ifndef PIG_H
#define PIG_H

#include <QObject>
#include <ctime>
#include <QString>
class Pig : public QObject
{
    Q_OBJECT
public:
    explicit Pig(QObject *parent = nullptr);
    /////黑白花猪对应为0, 1, 2
    int pigType;
    float weight;
    int growDay=0;
    int infected=0;
    int infectedDay=0;
    static constexpr int salePrice[3]={15, 6, 7};//卖出xx每kg猪
    static constexpr int buyPrice[3]={525, 210, 245};//买入xx每个猪
    static constexpr double weightMax=150.0;
    static const int growDayMax=360;
    Pig * lastPig=NULL;
    Pig * nextPig=NULL;
    //猪成长函数
    void pigGrow();
    //打印猪信息
    QString pigPrint();
    //构造函数, 选定猪种类的构造, 能初始化猪的种类体重
    Pig(int pigTypee);



signals:

};

#endif // PIG_H
