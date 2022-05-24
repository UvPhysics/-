#include "pigfarm.h"

PigFarm::PigFarm(QObject *parent)
    : QObject{parent}
{

}

void PigFarm::farmGrow(){//-----------------------
    for(int i=0;i<100;i++){
        pigStys[i].styGrow();
    }
    this->dayNum++;
}

void PigFarm::farmSetPigNum(){//---------------------
    this->farmCertainPigNum[0]=0;
    this->farmCertainPigNum[1]=0;
    this->farmCertainPigNum[2]=0;
    this->farmTotalPigNum=0;
    this->farmInfected=0;
    for(int i=0;i<100;i++){
        if(this->pigStys[i].styInfected){
            //只要有1个感染猪圈,农场就是感染状态
            this->farmInfected=1;
        }
        this->farmCertainPigNum[0]+=this->pigStys[i].styCertainPigNum[0];
        this->farmCertainPigNum[1]+=this->pigStys[i].styCertainPigNum[1];
        this->farmCertainPigNum[2]+=this->pigStys[i].styCertainPigNum[2];
        this->farmTotalPigNum+=this->pigStys[i].styTotalPigNum;
    }

}

void PigFarm::buyPig(int blackNum, int whiteNum, int flowerNum){//----------------------第四版
    //qDebug()<<QString("进入了买猪函数");
    this->farmSetPigNum();
    int tempBlack=this->farmCertainPigNum[0];//保留一下旧黑猪的量
    int tempWhite=this->farmCertainPigNum[1];
    int tempFlower=this->farmCertainPigNum[2];
    //由于要均分猪, 所以这里要涉及到原先猪圈的猪们的转移
    //均分猪的话就要涉及猪圈分配比例
    PigSty pigStyTemp;
    int blackStyNum;
    float fblackStyNum=((float)(this->farmCertainPigNum[0]+blackNum))/((float)(this->farmTotalPigNum+blackNum+whiteNum+flowerNum))*100.0;
    if(fblackStyNum-(int)fblackStyNum == 0.0){
        blackStyNum = (int)fblackStyNum;
    }
    else{
        blackStyNum = (int)fblackStyNum +1;
    }
    Pig* pp;
    for(int i=0; i<100; i++){
        for(Pig* p=this->pigStys[i].styFirstPig();p!=this->pigStys[i].ghostEndPig;){
            pp = p->nextPig;
            pigStyTemp.inPig(this->pigStys[i].removePig(p));
            p = pp;
        }
    }
    int blackIndex=0;
    for(int i=0;i<blackNum;i++){
        this->pigStys[blackIndex].inPig(0);
        blackIndex++;
        if(blackIndex==blackStyNum){
            blackIndex=0;
        }
    }
    int otherIndex=blackStyNum;
    for(int i=0;i<whiteNum;i++){
        this->pigStys[otherIndex].inPig(1);
        otherIndex++;
        if(otherIndex==100){
            otherIndex=blackStyNum;
        }
    }
    for(int i=0;i<flowerNum;i++){
        this->pigStys[otherIndex].inPig(2);
        otherIndex++;
        if(otherIndex == 100){
            otherIndex = blackStyNum;
        }
    }
    for(int i=0;i<tempBlack;i++){
        this->pigStys[blackIndex].inPig(pigStyTemp.removePig(pigStyTemp.styFirstPig()));
        blackIndex++;
        if(blackIndex==blackStyNum){
            blackIndex=0;
        }
    }
    for(int i=0;i<tempWhite+tempFlower;i++){
        this->pigStys[otherIndex].inPig(pigStyTemp.removePig(pigStyTemp.styFirstPig()));
        otherIndex++;
        if(otherIndex==100){
            otherIndex=blackStyNum;
        }
    }
    this->farmSetPigNum();
}

void PigFarm::pigInfect(Pig* p){//-----------------
    p->infected=1;
    return;
}

void PigFarm::farmInfecting(){
    if(!this->farmInfected){
        //如果农场健康,直接返回,不进行感染传播
        return;
    }
    //保存状态量,防止"100"->"111"这样第三个猪圈本不该感染的状况
    int tempStyInfected=this->pigStys[0].styInfected;
    for(int i=0;i<100;i++){
        //第0个猪圈和第99个猪圈没有2个相邻的猪圈,比较特殊
        if(i==0){
            if(this->pigStys[0].styInfected){//50%感染
                //tempStyInfected=this->pigStys[i].styInfected;
                for(Pig*p=this->pigStys[1].styFirstPig();p!=this->pigStys[1].ghostEndPig;p=p->nextPig){
                    if(p->infected){
                        continue;
                    }
                    //感染猪
                    int t=rand()%20;
                    if((t>=0)&&(t<=9)){
                        p->infected=1;
                    }
                }                
            }
            else if((this->pigStys[1].styInfected)&&(!this->pigStys[0].styInfected)){//15%感染
                //tempStyInfected=this->pigStys[i].styInfected;
                for(Pig*p=this->pigStys[0].styFirstPig();p!=this->pigStys[0].ghostEndPig;p=p->nextPig){
                    int t=rand()%20;
                    if((t==0)||(t==1)||(t==2)){
                        //感染猪
                        p->infected=1;
                        if(this->pigStys[0].styInfected==0){
                            //更新猪圈感染状态
                            pigStys[0].styInfected=1;
                            emit pigStys[0].styIsInfected();
                        }
                    }
                }

            }
            else{
                //tempStyInfected=this->pigStys[i].styInfected;
                continue;
            }
        }
        else if(i==99){
            if(this->pigStys[99].styInfected){//50%感染
                for(Pig*p=this->pigStys[99].styFirstPig();p!=this->pigStys[99].ghostEndPig;p=p->nextPig){
                    if(p->infected){
                        continue;
                    }
                    //感染猪
                    int t=rand()%20;
                    if((t>=0)&&(t<=9)){
                        p->infected=1;
                    }
                }
            }
            else if((tempStyInfected)&&(!this->pigStys[99].styInfected)){//15%感染
                for(Pig*p=this->pigStys[99].styFirstPig();p!=this->pigStys[99].ghostEndPig;p=p->nextPig){
                    int t=rand()%20;
                    if((t==0)||(t==1)||(t==2)){
                        p->infected=1;
                        //更新猪圈感染状态
                        if(this->pigStys[99].styInfected == 0){
                            pigStys[99].styInfected=1;
                            emit pigStys[99].styIsInfected();
                        }
                    }
                }
            }
            else{
                continue;
            }
        }
        else {
            if(this->pigStys[i].styInfected){//50%感染
                tempStyInfected=1;
                for(Pig*p=this->pigStys[i].styFirstPig();p!=this->pigStys[i].ghostEndPig;p=p->nextPig){
                    if(p->infected){
                        continue;
                    }
                    int t=rand()%20;
                    if((t>=0)&&(t<=9)){
                        p->infected=1;
                    } 
                }
            }
            else if(((tempStyInfected)||(this->pigStys[i+1].styInfected))&&(!this->pigStys[i].styInfected)){//15%感染
                tempStyInfected=0;
                for(Pig*p=this->pigStys[i].styFirstPig();p!=this->pigStys[i].ghostEndPig;p=p->nextPig){
                    int t=rand()%20;
                    if((t==0)||(t==1)||(t==2)){
                        //感染猪
                        p->infected=1;
                        //更新猪圈状态
                        if(this->pigStys[i].styInfected==0){
                            this->pigStys[i].styInfected=1;
                            emit this->pigStys[i].styIsInfected();
                        }
                    }
                }
            }
            else{
                    tempStyInfected=0;
                    continue;
            }
        }
    }
}

int PigFarm::farmDeathSimulation(){
    //死亡倒计时, 最后要返回这个数字
    if(this->farmInfected){//感染了再模拟
        int countDown=0;
        //创建临时农场,因为要模拟,如果真让原来的猪圈死了就很麻烦
        PigFarm * tempFarm=new PigFarm;
        for(int i=0;i<100;i++){
            //复制猪圈健康状态和总猪数量
            tempFarm->pigStys[i].styInfected=this->pigStys[i].styInfected;
            tempFarm->pigStys[i].styTotalPigNum=this->pigStys[i].styTotalPigNum;
            for(Pig* p=this->pigStys[i].styFirstPig();p!=this->pigStys[i].ghostEndPig;p=p->nextPig){
                //tempFarm->pigStys[i].inPig(p);//不能用这个语句,这会导致程序crash,并且p本来是原猪圈的,inpig会改变p前后的指针
                //猪种类不重要,就默认黑猪,核心要复制感染时间和感染状态
                tempFarm->pigStys[i].inPig(0);
                tempFarm->pigStys[i].styEndPig()->infected=p->infected;
                tempFarm->pigStys[i].styEndPig()->infectedDay=p->infectedDay;
            }
        }
        //复制农场状态
        tempFarm->farmInfected=this->farmInfected;
        //更新一下农场的数据
        tempFarm->farmSetPigNum();
        //好了,这回真的开始了
        while(tempFarm->farmTotalPigNum){//还有猪活着就一直模拟
            //tempFarm->farmGrow();//不能用grow函数,grow函数会让体重之类的也长,耗费多余内存
            //作为替换, 把原来的styGrow的函数搬过来稍微修改一下, infectedDay增加
            for(int i=0;i<100;i++){
                for(Pig* p=tempFarm->pigStys[i].styFirstPig();p!=tempFarm->pigStys[i].ghostEndPig;p=p->nextPig){
                    if(p->infected){
                        p->infectedDay++;//感染了的话就感染天数增加
                    }
                    /////////////////////////////////
                    //成长超过7天死亡
                    if(p->infectedDay == 8){
                        Pig* t=p;
                        p=p->lastPig;
                        tempFarm->pigStys[i].removePig(t);
                        delete t;
                        //如果还有猪并且猪都健康,设置猪圈是健康的,防止空猪圈感染其他猪圈
                        if(tempFarm->pigStys[i].styTotalPigNum){
                            int tt=0;
                            for(Pig* j = tempFarm->pigStys[i].styFirstPig();j!=tempFarm->pigStys[i].ghostEndPig;j=j->nextPig){
                                tt=j->infected;
                            }
                            if(!tt){
                                tempFarm->pigStys[i].styInfected=0;
                            }
                        }

                    }
                    //////////////////////////////
                }
            }
            //上面的成长执行完了,再进行感染扩散
            tempFarm->farmInfecting();
            //感染了1天,更新一下农场数据,(猪数量以及农场健康状态)
            tempFarm->farmSetPigNum();
            countDown++;
            //如果刚好有哪个猪圈幸存了,再模拟就会让程序无响应(泪的教训,这么一个程序耗了我17%的cpu性能
            if(!tempFarm->farmInfected){
                delete tempFarm;
                return countDown;//这里不返回-1,返回-1感觉太过分了,这个就当作这些天后病猪死光了
            }

        }
        delete tempFarm;
        return countDown;
    }
    else{
        return -1;
    }

}

float PigFarm::salePig(){
    //清空一下上次的日志
    this->farmLogTemp[5]=0;
    this->farmLogTemp[6]=0;
    this->farmLogTemp[7]=0;
    this->farmLogTemp[8]=0;
    this->farmLogTemp[9]=0;
    float getMoney=0.0;
    this->farmSetPigNum();
    Pig* pp;
    for(int i=0;i<100;i++){
        for(Pig* p=this->pigStys[i].styFirstPig();p!=this->pigStys[i].ghostEndPig;){
            if((p->growDay>360)||(p->weight>150.0)){
                pp=p->nextPig;
                getMoney+=(p->salePrice[p->pigType])*(this->pigStys[i].removePig(p)->weight);
                this->farmLogTemp[5]++;
                this->farmLogTemp[6+p->pigType]++;
                delete p;
                p=pp;
            }else{
                p=p->nextPig;
            }
        }
    }
    this->farmMoney+=getMoney;
    this->farmLogTemp[9]=getMoney;
    return getMoney;
}

void PigFarm::clearFarm(){
    //把上次清理的数据清空
    this->farmCertainDeathNum[0]=0;
    this->farmCertainDeathNum[1]=0;
    this->farmCertainDeathNum[2]=0;
    for(int i=0;i<100;i++){
        this->pigStys[i].clearPigSty();
        this->farmCertainDeathNum[0]+=pigStys[i].styCertainDeathNum[0];
        this->farmCertainDeathNum[1]+=pigStys[i].styCertainDeathNum[1];
        this->farmCertainDeathNum[2]+=pigStys[i].styCertainDeathNum[2];
    }
    this->farmInfected=0;
}
