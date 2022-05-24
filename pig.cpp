#include "pig.h"

Pig::Pig(QObject *parent)
    : QObject{parent}
{
    //体重20到50的某个1位小数
    weight = ((double)(rand()%301)/10)+20;
}

void Pig::pigGrow(){
    growDay++;
    weight+= ((double)(rand()%13))/10;
    if(infected){
        infectedDay++;
    };
}

Pig::Pig(int pigTypee){
    pigType = pigTypee;
    weight = ((double)(rand()%301)/10)+20;
}

QString Pig::pigPrint(){
    QString info="品种:";
    switch(pigType){
    case 0:
        info.append(" 黑猪\n");
        break;
    case 1:
        info.append(" 大花白猪\n");
        break;
    case 2:
        info.append(" 小花猪\n");
        break;
    }
    QString info1= QString("体重: %1kg\n饲养时间: %2day\n健康状态: ").arg(weight).arg(growDay);
    info.append(info1);
    switch((int)(this->infected)){
    case 0:
        info.append("健康\n");
        break;
    case 1:
        info.append("感染\n");
        break;
    }
    return info;
}
