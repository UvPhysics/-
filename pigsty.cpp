#include "pigsty.h"

PigSty::PigSty(QObject *parent)
    : QObject{parent}
{
    ghostFirstPig->lastPig = NULL;
    ghostFirstPig->nextPig = ghostEndPig;
    ghostEndPig->nextPig = NULL;
    ghostEndPig->lastPig = ghostFirstPig;
}

Pig* PigSty::removePig(Pig* p){
    if(this->styTotalPigNum == 1){
        this->styInfected = 0;
        emit styIsCleared();
    }
    this->styTotalPigNum--;
    this->styCertainPigNum[p->pigType]--;
    p->lastPig->nextPig=p->nextPig;
    p->nextPig->lastPig=p->lastPig;

    return p;
}

void PigSty::clearPigSty()
{
    //把上次清理的数据清空
    this->styCertainDeathNum[0]=0;
    this->styCertainDeathNum[1]=0;
    this->styCertainDeathNum[2]=0;
    //开始执行函数
    if(!this->styInfected){
        return;
    }
    Pig* pp;
    for(Pig* p=this->styFirstPig();p!=this->ghostEndPig;p=p->nextPig){
        if(p->infected){
            pp=p->lastPig;
            this->styCertainDeathNum[p->pigType]++;
            this->removePig(p);
            delete p;
            p=pp;
        }        
    }
    if(this->styTotalPigNum){
        this->styInfected=0;
        emit styHaveHealthPig();
    }
}

void PigSty::inPig(Pig * p){
    if((this->styTotalPigNum==0)&&(!p->infected)){
        this->styInfected=0;
        emit styHaveHealthPig();
    }
    if((p->infected)&&(!this->styInfected)){
        this->styInfected=1;
        emit styIsInfected();
    }
    p->nextPig=this->ghostEndPig;
    p->lastPig=this->styEndPig();
    this->styEndPig()->nextPig = p;
    this->ghostEndPig->lastPig =p;
    this->styTotalPigNum++;
    this->styCertainPigNum[p->pigType]++;
    return;
}

void PigSty::inPig(int pigTypee){
    Pig * newPig= new Pig(pigTypee);
    this->inPig(newPig);
    return;
}

void PigSty::styGrow(){
    for(Pig* i = this->styFirstPig();i!=this->ghostEndPig;i=i->nextPig){
        i->pigGrow();
        //猪感染了七天就死亡, 百度上猪瘟平均7天死亡一个猪
        if(i->infectedDay == 8){
            Pig* t=i;
            i=i->lastPig;
            this->removePig(t);
            delete t;
            //如果刚好最后的感染的猪死了,触发健康信号(还剩下其他猪,不考虑死光,死光了在removepig里面有相应触发信号功能
            //删除完这个猪后,循环判断一下其他的猪是健康还是感染
            if(this->styTotalPigNum){//至少这个猪圈要还有其他猪
                int tt=0;
                for(Pig* j = this->styFirstPig();j!=this->ghostEndPig;j=j->nextPig){
                    tt=j->infected;
                }
                if(!tt){
                    this->styInfected=0;
                    emit styHaveHealthPig();
                }
            }

        }
    }   
    return;
}

QString PigSty::styPrint(){
    QString info;
    int t=0;
    for(Pig* i = this->styFirstPig();i!=this->ghostEndPig;i=i->nextPig){
        info.append(QString("%1号猪:\n").arg(t++));
        info.append(i->pigPrint());
    }
    return info;
}

void PigSty::styInfectPig(){
    this->styFirstPig()->infected=1;
    this->styInfected = 1;
    emit styIsInfected();
}
