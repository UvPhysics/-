#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    this->timerDay= new QTimer(this);
    myFarm->setParent(this);
    ui->setupUi(this);
    //放置猪圈按钮并设置图标
    setStyButtons();
    //连接按钮和猪圈
    connectStyButtons();
    connectStyButtonsInfected();
    connectStyButtonsHealthy();
    connectStyButtonsNoPig();
    //链接购买按钮
    connect(ui->buyButton, &QPushButton::clicked, [=](){
        this->timerDay->stop();
        ui->timeControl->setText("继续");

        if(this->myFarm->farmInfected){
            QDialog wrong(this);
            wrong.setFixedSize(230, 70);
            wrong.setWindowTitle(QString("警告!"));
            QLabel la(&wrong);
            la.move(18,13);
            la.setText(QString("农场此时有猪瘟,购买会扩大瘟疫传播\n             建议先采取措施"));
            wrong.exec();
        }
        QDialog dialog(this);
        dialog.setWindowTitle(QString("请输入买的猪数量:"));
        dialog.setFixedWidth(180);
        QFormLayout form(&dialog);

        QSpinBox *spinbox1 = new QSpinBox(&dialog);
        form.addRow(QString("黑猪: "), spinbox1);
        QSpinBox *spinbox2 = new QSpinBox(&dialog);
        form.addRow(QString("大花白猪: "), spinbox2);
        QSpinBox *spinbox3 = new QSpinBox(&dialog);
        form.addRow(QString("小花猪: "), spinbox3);

        spinbox1->setMaximum(1000);
        spinbox2->setMaximum(1000);
        spinbox3->setMaximum(1000);

        QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,Qt::Horizontal, &dialog);
        form.addRow(&buttonBox);
        QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
        QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

        if (dialog.exec() == QDialog::Accepted){
            this->myFarm->farmSetPigNum();

            int total_blackNum=spinbox1->value()+this->myFarm->farmCertainPigNum[0];
            int total_otherNum=spinbox2->value()+spinbox3->value()+this->myFarm->farmCertainPigNum[1]+this->myFarm->farmCertainPigNum[2];
            int black_leastNum = total_blackNum/10 + 1;
            int other_leastNum = total_otherNum/10 + 1;

            if(((float)total_blackNum)/10.0 == (float)(total_blackNum/10)){
                black_leastNum = total_blackNum/10;
            }
            if(((float)total_otherNum)/10.0 == (float)(total_otherNum/10)){
                other_leastNum = total_otherNum/10;
            }

            if(black_leastNum+other_leastNum>100){
                QDialog wrong(this);
                wrong.setFixedSize(230, 70);
                wrong.setWindowTitle(QString("错误!"));
                QLabel la(&wrong);
                la.move(30,20);
                la.setText(QString("现有猪圈空间不足以存放这些猪"));
                wrong.exec();
                return;
            }
            else if(spinbox1->value()*525.0+spinbox2->value()*210.0+spinbox3->value()*245.0>this->myFarm->farmMoney){

                QDialog wrong(this);
                wrong.setFixedSize(230, 70);
                wrong.setWindowTitle(QString("错误!"));
                QLabel la(&wrong);
                la.move(30,20);
                la.setText(QString("现有金钱不足以购买这些猪"));
                wrong.exec();

                this->timerDay->start(1000);
                //ui->timeControl->setText("暂停");
            }
            else{
                //记录买入信息
                this->myFarm->farmLogTemp[0]=spinbox1->value()+spinbox2->value()+spinbox3->value();
                this->myFarm->farmLogTemp[1]=spinbox1->value();
                this->myFarm->farmLogTemp[2]=spinbox2->value();
                this->myFarm->farmLogTemp[3]=spinbox3->value();
                this->myFarm->farmLogTemp[4]=spinbox1->value()*150.0+spinbox2->value()*60.0+spinbox3->value()*70.0;
                //存入日志信息
                this->logInfo.append(QString("第%1天\n总共购入了%2个猪\n黑猪:%3个,大花白猪:%4个,小花猪:%5个\n共花费%6元\n").arg(this->myFarm->dayNum).arg(this->myFarm->farmLogTemp[0]).arg(this->myFarm->farmLogTemp[1]).arg(this->myFarm->farmLogTemp[2]).arg(this->myFarm->farmLogTemp[3]).arg(this->myFarm->farmLogTemp[4]));
                this->myFarm->farmMoney -= spinbox1->value()*525+spinbox2->value()*210+spinbox3->value()*245;
                this->myFarm->buyPig(spinbox1->value(), spinbox2->value(), spinbox3->value());
                ui->moneyValue->setText(QString::number(this->myFarm->farmMoney, 'f', 2));

                this->timerDay->start(1000);
                ui->timeControl->setText("暂停");
            }
        }
        else{
            return;
        }

    });
    //连接时间事件
    connect(timerDay, &QTimer::timeout,[=](){
        //更新下farm
        this->myFarm->farmSetPigNum();
        //进行成长,期间会有猪的死亡
        this->myFarm->farmGrow();
        //进行感染传播
        this->myFarm->farmInfecting();
        //更新一天的结果
        this->myFarm->farmSetPigNum();
        //模拟结果的显示设置
        ui->deathValue->setText(QString("%1").arg(myFarm->farmDeathSimulation()));
        ui->dayValue->setText(QString("%1").arg(this->myFarm->dayNum));
        //每90天要进行的周期活动
        if(((this->myFarm->dayNum)%90==0)&&(this->myFarm->dayNum!=0)){
            this->timerDay->stop();
            //每90天, 先自动卖猪
            if(this->myFarm->farmInfected){
                //先清理农场的病猪
                this->myFarm->clearFarm();
                //开始卖猪,并且保留信息
                QDialog dia(this);
                dia.setFixedSize(230, 70);
                dia.setWindowTitle(QString("提示:"));
                QLabel la(&dia);
                la.move(30,20);
                la.setText(QString("过去了3个月,自动清理了病猪\n这次赚到了%1元").arg(this->myFarm->salePig()));
                this->logInfo.append(QString("第%1天\n总共卖出了%2个猪\n黑猪:%3个,大花白猪:%4个,小花猪:%5个\n共赚得%6元\n").arg(this->myFarm->dayNum).arg(this->myFarm->farmLogTemp[5]).arg(this->myFarm->farmLogTemp[6]).arg(this->myFarm->farmLogTemp[7]).arg(this->myFarm->farmLogTemp[8]).arg(this->myFarm->farmLogTemp[9]));
                //ui->moneyValue->setText(QString("%1").arg(this->myFarm->farmMoney));
                ui->moneyValue->setText(QString::number(this->myFarm->farmMoney, 'f', 2));
                dia.exec();
            }
            else{
                QDialog dia(this);
                dia.setFixedSize(230, 70);
                dia.setWindowTitle(QString("提示:"));
                QLabel la(&dia);
                la.move(30,20);
                la.setText(QString("过去了3个月,这次赚到了%1元").arg(this->myFarm->salePig()));
                this->logInfo.append(QString("第%1天\n总共卖出了%2个猪\n黑猪:%3个,大花白猪:%4个,小花猪:%5个\n共赚得%6元\n").arg(this->myFarm->dayNum).arg(this->myFarm->farmLogTemp[5]).arg(this->myFarm->farmLogTemp[6]).arg(this->myFarm->farmLogTemp[7]).arg(this->myFarm->farmLogTemp[8]).arg(this->myFarm->farmLogTemp[9]));
                //ui->moneyValue->setText(QString("%1").arg(this->myFarm->farmMoney));
                ui->moneyValue->setText(QString::number(this->myFarm->farmMoney, 'f', 2));
                dia.exec();
            }
            //提示买猪
            QDialog dialog(this);
            dialog.setFixedSize(230, 70);
            dialog.setWindowTitle(QString("提示:"));
            QLabel la(&dialog);
            la.move(30,20);
            la.setText("3个月了,建议再购入一批猪仔");
            dialog.exec();
            this->timerDay->start(1000);
        }


    });
    //连接开始界面退出按钮
    connect(ui->escapeGame, &QPushButton::clicked, [=](){
        this->close();
    });
    //开始界面新游戏
    connect(ui->newGame, &QPushButton::clicked, [=](){
        //清空当前农场信息
        this->logInfo="";
        this->myFarm->dayNum=0;
        this->myFarm->farmMoney=200000;
        Pig*p;
        for(int i=0;i<100;i++){
            while(this->myFarm->pigStys[i].styTotalPigNum){
                p=this->myFarm->pigStys[i].styEndPig();
                this->myFarm->pigStys[i].removePig(this->myFarm->pigStys[i].styEndPig());
                delete p;
            }
        }
        this->myFarm->farmSetPigNum();
        ui->moneyValue->setText(QString::number(this->myFarm->farmMoney, 'f', 2));
        //转换页面
        ui->stackedWidget->setCurrentIndex(1);
    });
    //开始界面继续游戏
    connect(ui->continueGame, &QPushButton::clicked, [=](){
        //转换页面
        ui->stackedWidget->setCurrentIndex(1);
    });
    //主页面暂停继续按钮
    connect(ui->timeControl, &QPushButton::clicked, [=](){
        if(ui->timeControl->text()=="暂停"){
            this->timerDay->stop();
            ui->timeControl->setText("继续");
        }
        else{
            this->timerDay->start(1000);
            ui->timeControl->setText("暂停");
        }
    });
    //链接卖猪按钮
    connect(ui->saleButton, &QPushButton::clicked, [=](){
        this->timerDay->stop();
        ui->timeControl->setText("继续");

        if(this->myFarm->farmInfected){
            QDialog wrong(this);
            wrong.setFixedSize(230, 70);
            wrong.setWindowTitle(QString("警告!"));
            QLabel la(&wrong);
            la.move(26,12);
            la.setText(QString("            农场此时有猪瘟\n            必须先采取措施"));
            wrong.exec();
            return;
        }
        QDialog dia(this);
        dia.setFixedSize(230, 70);
        dia.setWindowTitle(QString("结果:"));
        QLabel la(&dia);
        la.move(30,20);
        la.setText(QString("这次赚到了%1元").arg(this->myFarm->salePig()));
        this->logInfo.append(QString("第%1天\n总共卖出了%2个猪\n黑猪:%3个,大花白猪:%4个,小花猪:%5个\n共赚得%6元\n").arg(this->myFarm->dayNum).arg(this->myFarm->farmLogTemp[5]).arg(this->myFarm->farmLogTemp[6]).arg(this->myFarm->farmLogTemp[7]).arg(this->myFarm->farmLogTemp[8]).arg(this->myFarm->farmLogTemp[9]));
        //ui->moneyValue->setText(QString("%1").arg(this->myFarm->farmMoney));
        ui->moneyValue->setText(QString::number(this->myFarm->farmMoney, 'f', 2));
        //运行过程中发现农场金钱过多会科学计数法,就很烦,测试了一下,float还不管用,要换成double的
        dia.exec();

    });
    //链接保存按钮
    connect(ui->saveButton, &QPushButton::clicked, [=](){
        this->timerDay->stop();
        ui->timeControl->setText("继续");

        //销售记录
        QFile file("saleRecord.txt");
        //农场信息
        QFile file1("pigData.txt");
        //保存销售记录
        if(file.open(QIODevice::WriteOnly | QIODevice::Text)){
            QTextStream out(&file);
            out<<this->logInfo;
            QDialog dia(this);
            dia.setWindowTitle("提示:");
            dia.setFixedSize(230, 70);
            QLabel la(&dia);
            la.move(30,20);
            la.setText("保存销售记录成功!");
            file.close();
            dia.exec();
        }
        else{
            QDialog dia(this);
            dia.setWindowTitle("提示:");
            dia.setFixedSize(230, 70);
            QLabel la(&dia);
            la.move(30,20);
            la.setText("保存销售记录失败!");
            dia.exec();
        }
        //保存农场信息
        if(file1.open(QIODevice::WriteOnly | QIODevice::Text)){
            QTextStream out1(&file1);
            out1<<this->myFarm->dayNum<<'\n';
            out1<<this->myFarm->farmMoney<<'\n';
            for(int i=0;i<100;i++){
                out1<<this->myFarm->pigStys[i].styTotalPigNum<<'\n';
            }
            for(int i=0;i<100;i++){
                for(Pig*p=this->myFarm->pigStys[i].styFirstPig();p!=this->myFarm->pigStys[i].ghostEndPig;p=p->nextPig){
                    out1<<p->pigType<<'\n';
                    out1<<p->weight<<'\n';
                    out1<<p->growDay<<'\n';
                    out1<<p->infectedDay<<'\n';
                    out1<<p->infected<<'\n';
                }
            }
            QDialog dia(this);
            dia.setWindowTitle("提示:");
            dia.setFixedSize(230, 70);
            QLabel la(&dia);
            la.move(30,20);
            la.setText("保存猪场信息成功!");
            dia.exec();
            file1.close();            
        }
        else{
            QDialog dia(this);
            dia.setWindowTitle("提示:");
            dia.setFixedSize(230, 70);
            QLabel la(&dia);
            la.move(30,20);
            la.setText("保存猪场信息失败!");
            dia.exec();            
        }

    });
    //链接读取按钮
    connect(ui->readButton, &QPushButton::clicked, [=](){
        this->timerDay->stop();
        ui->timeControl->setText("继续");

        //清空当前农场信息,防止等会读取结果出问题
        this->logInfo="";
        this->myFarm->dayNum=0;
        this->myFarm->farmMoney=200000;
        Pig*p;
        for(int i=0;i<100;i++){
            while(this->myFarm->pigStys[i].styTotalPigNum){
                p=this->myFarm->pigStys[i].styEndPig();
                this->myFarm->pigStys[i].removePig(this->myFarm->pigStys[i].styEndPig());
                delete p;
            }
        }
        this->myFarm->farmSetPigNum();
        //销售记录
        QFile file("saleRecord.txt");
        //农场信息
        QFile file1("pigData.txt");
        //读取销售记录
        if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
            QTextStream in(&file);
            logInfo=in.readAll();
            QDialog dia(this);
            dia.setWindowTitle("提示:");
            dia.setFixedSize(230, 70);
            QLabel la(&dia);
            la.move(30,20);
            la.setText("读取销售记录成功!");
            file.close();
            dia.exec();
        }else{
            QDialog dia(this);
            dia.setWindowTitle("提示:");
            dia.setFixedSize(230, 70);
            QLabel la(&dia);
            la.move(30,20);
            la.setText("读取销售记录失败!");
            dia.exec();
        }
        //读取农场信息
        if(file1.open(QIODevice::ReadOnly | QIODevice::Text)){
            QTextStream in1(&file1);
            in1>>this->myFarm->dayNum;
            in1>>this->myFarm->farmMoney;
            int pigNum[100];
            for(int i=0;i<100;i++){
                in1>>pigNum[i];
            }
            for(int i=0;i<100;i++){
                while(pigNum[i]--){
                    int pigTypee;
                    in1>>pigTypee;
                    Pig* p=new Pig(pigTypee);
                    in1>>p->weight;
                    in1>>p->growDay;
                    in1>>p->infectedDay;
                    in1>>p->infected;
                    this->myFarm->pigStys[i].inPig(p);
                }
            }
            this->myFarm->farmSetPigNum();
            QDialog dia(this);
            dia.setWindowTitle("提示:");
            dia.setFixedSize(230, 70);
            QLabel la(&dia);
            la.move(30,20);
            la.setText("读取猪场信息成功!");
            file1.close();
            dia.exec();

        }else{
            QDialog dia(this);
            dia.setWindowTitle("提示:");
            dia.setFixedSize(230, 70);
            QLabel la(&dia);
            la.move(30,20);
            la.setText("读取猪场信息失败!");
            dia.exec();
        }
        this->myFarm->farmSetPigNum();
        ui->moneyValue->setText(QString::number(this->myFarm->farmMoney, 'f', 2));
        ui->dayValue->setText(QString::number(this->myFarm->dayNum));
    });
    //链接记录按钮
    connect(ui->logButton, &QPushButton::clicked, [=](){
        this->timerDay->stop();
        ui->timeControl->setText("继续");
        QDialog dia(this);
        dia.setFixedSize(256,256);
        dia.setWindowTitle("销售与购买记录");
        QPlainTextEdit text(&dia);
        text.move(0,0);
        text.resize(256,256);
        text.appendPlainText(this->logInfo);
        text.setReadOnly(1);
        dia.exec();

    });
    //链接抗疫按钮
    connect(ui->clearButton, &QPushButton::clicked, [=](){
        this->timerDay->stop();
        ui->timeControl->setText("继续");

        this->myFarm->clearFarm();
        QDialog dia(this);
        dia.setFixedSize(230, 70);
        dia.setWindowTitle(QString("结果:"));
        QLabel la(&dia);
        la.move(0,0);
        la.setText(QString("这次清理了%1个猪\n").arg(this->myFarm->farmCertainDeathNum[0]+this->myFarm->farmCertainDeathNum[1]+this->myFarm->farmCertainDeathNum[2]).append(QString("黑猪:%1个\n大花白猪:%2个\n小花猪:%3个").arg(this->myFarm->farmCertainDeathNum[0]).arg(this->myFarm->farmCertainDeathNum[1]).arg(this->myFarm->farmCertainDeathNum[2])));
        dia.exec();

    });
    //链接瘟疫按钮,让随机1个猪感染
    connect(ui->illButton, &QPushButton::clicked, [=](){
        if(!this->myFarm->farmTotalPigNum){
            return;//防止没有猪而死循环
        }
        while(1){//为了防止感染失败,来个while
            int t=rand()%100;
            if((this->myFarm->pigStys[t].styFirstPig())!=(this->myFarm->pigStys[t].ghostEndPig)){
                this->myFarm->pigStys[t].styInfectPig();
                break;//感染成功就break
            }
        }

    });
    //链接统计按钮,都是重复的代码,看起来很长
    connect(ui->infoButton, &QPushButton::clicked, [=](){
        this->timerDay->stop();
        ui->timeControl->setText("继续");
        if(this->myFarm->farmTotalPigNum==0){
            ui->blackwbar0->resize(0,31);
            ui->blackwbar1->resize(0,31);
            ui->blackwbar2->resize(0,31);
            ui->blackwbar3->resize(0,31);
            ui->blacktbar0->resize(0,31);
            ui->blacktbar1->resize(0,31);
            ui->blacktbar2->resize(0,31);
            ui->blacktbar3->resize(0,31);
            ui->label_b0->setText(QString::number(0));
            ui->label_b1->setText(QString::number(0));
            ui->label_b2->setText(QString::number(0));
            ui->label_b3->setText(QString::number(0));
            ui->label_b4->setText(QString::number(0));
            ui->label_b5->setText(QString::number(0));
            ui->label_b6->setText(QString::number(0));
            ui->label_b7->setText(QString::number(0));
            //白猪
            ui->whitewbar0->resize(0,31);
            ui->whitewbar1->resize(0,31);
            ui->whitewbar2->resize(0,31);
            ui->whitewbar3->resize(0,31);
            ui->whitetbar0->resize(0,31);
            ui->whitetbar1->resize(0,31);
            ui->whitetbar2->resize(0,31);
            ui->whitetbar3->resize(0,31);
            ui->label_w0->setText(QString::number(0));
            ui->label_w1->setText(QString::number(0));
            ui->label_w2->setText(QString::number(0));
            ui->label_w3->setText(QString::number(0));
            ui->label_w4->setText(QString::number(0));
            ui->label_w5->setText(QString::number(0));
            ui->label_w6->setText(QString::number(0));
            ui->label_w7->setText(QString::number(0));
            //花猪
            ui->flowerwbar0->resize(0,31);
            ui->flowerwbar1->resize(0,31);
            ui->flowerwbar2->resize(0,31);
            ui->flowerwbar3->resize(0,31);
            ui->flowertbar0->resize(0,31);
            ui->flowertbar1->resize(0,31);
            ui->flowertbar2->resize(0,31);
            ui->flowertbar3->resize(0,31);
            ui->label_f0->setText(QString::number(0));
            ui->label_f1->setText(QString::number(0));
            ui->label_f2->setText(QString::number(0));
            ui->label_f3->setText(QString::number(0));
            ui->label_f4->setText(QString::number(0));
            ui->label_f5->setText(QString::number(0));
            ui->label_f6->setText(QString::number(0));
            ui->label_f7->setText(QString::number(0));
            /////////////////////////////
            ui->stackedWidget->setCurrentIndex(2);
            return;
        }
        ///////////////////////////
        //收集统计信息
        int blackInfo[8]={0,0,0,0,0,0,0,0};
        int whiteInfo[8]={0,0,0,0,0,0,0,0};
        int flowerInfo[8]={0,0,0,0,0,0,0,0};
        for(int i=0;i<100;i++){
            for(Pig *p=this->myFarm->pigStys[i].styFirstPig();p!=this->myFarm->pigStys[i].ghostEndPig;p=p->nextPig){
                if(p->pigType==0){
                    //体重统计
                    if(p->weight<65){
                        blackInfo[0]++;
                    }
                    else if(p->weight<105){
                        blackInfo[1]++;
                    }
                    else if(p->weight<147){
                        blackInfo[2]++;
                    }
                    else{
                        blackInfo[3]++;
                    }
                    //饲养时间统计
                    if(p->growDay<112){
                        blackInfo[4]++;
                    }
                    else if(p->growDay<225){
                        blackInfo[5]++;
                    }
                    else if(p->growDay<337){
                        blackInfo[6]++;
                    }
                    else{
                        blackInfo[7]++;
                    }
                }
                if(p->pigType==1){
                    //体重统计
                    if(p->weight<65){
                        whiteInfo[0]++;
                    }
                    else if(p->weight<105){
                        whiteInfo[1]++;
                    }
                    else if(p->weight<147){
                        whiteInfo[2]++;
                    }
                    else{
                        whiteInfo[3]++;
                    }
                    //饲养时间统计
                    if(p->growDay<112){
                        whiteInfo[4]++;
                    }
                    else if(p->growDay<225){
                        whiteInfo[5]++;
                    }
                    else if(p->growDay<337){
                        whiteInfo[6]++;
                    }
                    else{
                        whiteInfo[7]++;
                    }
                }
                if(p->pigType==2){
                    //体重统计
                    if(p->weight<65){
                        flowerInfo[0]++;
                    }
                    else if(p->weight<105){
                        flowerInfo[1]++;
                    }
                    else if(p->weight<147){
                        flowerInfo[2]++;
                    }
                    else{
                        flowerInfo[3]++;
                    }
                    //饲养时间统计
                    if(p->growDay<112){
                        flowerInfo[4]++;
                    }
                    else if(p->growDay<225){
                        flowerInfo[5]++;
                    }
                    else if(p->growDay<337){
                        flowerInfo[6]++;
                    }
                    else{
                        flowerInfo[7]++;
                    }
                }
            }
        }//收集完成
        //显示黑猪统计信息
        ui->blackwbar0->resize(400*blackInfo[0]/(blackInfo[0]+blackInfo[1]+blackInfo[2]+blackInfo[3]),31);
        ui->blackwbar1->resize(400*blackInfo[1]/(blackInfo[0]+blackInfo[1]+blackInfo[2]+blackInfo[3]),31);
        ui->blackwbar2->resize(400*blackInfo[2]/(blackInfo[0]+blackInfo[1]+blackInfo[2]+blackInfo[3]),31);
        ui->blackwbar3->resize(400*blackInfo[3]/(blackInfo[0]+blackInfo[1]+blackInfo[2]+blackInfo[3]),31);
        ui->blacktbar0->resize(400*blackInfo[4]/(blackInfo[4]+blackInfo[5]+blackInfo[6]+blackInfo[7]),31);
        ui->blacktbar1->resize(400*blackInfo[5]/(blackInfo[4]+blackInfo[5]+blackInfo[6]+blackInfo[7]),31);
        ui->blacktbar2->resize(400*blackInfo[6]/(blackInfo[4]+blackInfo[5]+blackInfo[6]+blackInfo[7]),31);
        ui->blacktbar3->resize(400*blackInfo[7]/(blackInfo[4]+blackInfo[5]+blackInfo[6]+blackInfo[7]),31);
        ui->label_b0->setText(QString::number(blackInfo[0]));
        ui->label_b1->setText(QString::number(blackInfo[1]));
        ui->label_b2->setText(QString::number(blackInfo[2]));
        ui->label_b3->setText(QString::number(blackInfo[3]));
        ui->label_b4->setText(QString::number(blackInfo[4]));
        ui->label_b5->setText(QString::number(blackInfo[5]));
        ui->label_b6->setText(QString::number(blackInfo[6]));
        ui->label_b7->setText(QString::number(blackInfo[7]));
        //白猪
        ui->whitewbar0->resize(400*whiteInfo[0]/(whiteInfo[0]+whiteInfo[1]+whiteInfo[2]+whiteInfo[3]),31);
        ui->whitewbar1->resize(400*whiteInfo[1]/(whiteInfo[0]+whiteInfo[1]+whiteInfo[2]+whiteInfo[3]),31);
        ui->whitewbar2->resize(400*whiteInfo[2]/(whiteInfo[0]+whiteInfo[1]+whiteInfo[2]+whiteInfo[3]),31);
        ui->whitewbar3->resize(400*whiteInfo[3]/(whiteInfo[0]+whiteInfo[1]+whiteInfo[2]+whiteInfo[3]),31);
        ui->whitetbar0->resize(400*whiteInfo[4]/(whiteInfo[4]+whiteInfo[5]+whiteInfo[6]+whiteInfo[7]),31);
        ui->whitetbar1->resize(400*whiteInfo[5]/(whiteInfo[4]+whiteInfo[5]+whiteInfo[6]+whiteInfo[7]),31);
        ui->whitetbar2->resize(400*whiteInfo[6]/(whiteInfo[4]+whiteInfo[5]+whiteInfo[6]+whiteInfo[7]),31);
        ui->whitetbar3->resize(400*whiteInfo[7]/(whiteInfo[4]+whiteInfo[5]+whiteInfo[6]+whiteInfo[7]),31);
        ui->label_w0->setText(QString::number(whiteInfo[0]));
        ui->label_w1->setText(QString::number(whiteInfo[1]));
        ui->label_w2->setText(QString::number(whiteInfo[2]));
        ui->label_w3->setText(QString::number(whiteInfo[3]));
        ui->label_w4->setText(QString::number(whiteInfo[4]));
        ui->label_w5->setText(QString::number(whiteInfo[5]));
        ui->label_w6->setText(QString::number(whiteInfo[6]));
        ui->label_w7->setText(QString::number(whiteInfo[7]));
        //花猪
        ui->flowerwbar0->resize(400*flowerInfo[0]/(flowerInfo[0]+flowerInfo[1]+flowerInfo[2]+flowerInfo[3]),31);
        ui->flowerwbar1->resize(400*flowerInfo[1]/(flowerInfo[0]+flowerInfo[1]+flowerInfo[2]+flowerInfo[3]),31);
        ui->flowerwbar2->resize(400*flowerInfo[2]/(flowerInfo[0]+flowerInfo[1]+flowerInfo[2]+flowerInfo[3]),31);
        ui->flowerwbar3->resize(400*flowerInfo[3]/(flowerInfo[0]+flowerInfo[1]+flowerInfo[2]+flowerInfo[3]),31);
        ui->flowertbar0->resize(400*flowerInfo[4]/(flowerInfo[4]+flowerInfo[5]+flowerInfo[6]+flowerInfo[7]),31);
        ui->flowertbar1->resize(400*flowerInfo[5]/(flowerInfo[4]+flowerInfo[5]+flowerInfo[6]+flowerInfo[7]),31);
        ui->flowertbar2->resize(400*flowerInfo[6]/(flowerInfo[4]+flowerInfo[5]+flowerInfo[6]+flowerInfo[7]),31);
        ui->flowertbar3->resize(400*flowerInfo[7]/(flowerInfo[4]+flowerInfo[5]+flowerInfo[6]+flowerInfo[7]),31);
        ui->label_f0->setText(QString::number(flowerInfo[0]));
        ui->label_f1->setText(QString::number(flowerInfo[1]));
        ui->label_f2->setText(QString::number(flowerInfo[2]));
        ui->label_f3->setText(QString::number(flowerInfo[3]));
        ui->label_f4->setText(QString::number(flowerInfo[4]));
        ui->label_f5->setText(QString::number(flowerInfo[5]));
        ui->label_f6->setText(QString::number(flowerInfo[6]));
        ui->label_f7->setText(QString::number(flowerInfo[7]));
        /////////////////////////////
        ui->stackedWidget->setCurrentIndex(2);
    });
    //链接返回按钮
    connect(ui->backButton, &QToolButton::clicked, [=](){
        ui->stackedWidget->setCurrentIndex(1);
    });
    //ui->moneyValue->setText(QString("%1").arg(this->myFarm->farmMoney));
    ui->moneyValue->setText(QString::number(this->myFarm->farmMoney, 'f', 2));
    ui->stackedWidget->setCurrentIndex(0);
    //自动读取猪场信息
    autoRead();

}

MainWindow::~MainWindow()
{
    autoSave();
    delete ui;
}

void MainWindow::setStyButtons(){
    for(int i=0;i<25;i++){
        styButtons[i] = new QToolButton(ui->tab0024);
        styButtons[i]->move((i%5)*80, (i/5)*80);
        styButtons[i]->setFixedSize(81,81);
        styButtons[i]->setIcon(QIcon(":/img1/mossy_cobblestone.png"));
        styButtons[i]->setIconSize(QSize(81, 81));
    }
    for(int i=25;i<50;i++){
        styButtons[i] = new QToolButton(ui->tab2549);
        styButtons[i]->move(((i-25)%5)*80, ((i-25)/5)*80);
        styButtons[i]->setFixedSize(81,81);
        styButtons[i]->setIcon(QIcon(":/img1/mossy_cobblestone.png"));
        styButtons[i]->setIconSize(QSize(81, 81));
    }
    for(int i=50;i<75;i++){
        styButtons[i] = new QToolButton(ui->tab5074);
        styButtons[i]->move(((i-50)%5)*80, ((i-50)/5)*80);
        styButtons[i]->setFixedSize(81,81);
        styButtons[i]->setIcon(QIcon(":/img1/mossy_cobblestone.png"));
        styButtons[i]->setIconSize(QSize(81, 81));
    }
    for(int i=75;i<100;i++){
        styButtons[i] = new QToolButton(ui->tab7599);
        styButtons[i]->move(((i-75)%5)*80, ((i-75)/5)*80);
        styButtons[i]->setFixedSize(81,81);
        styButtons[i]->setIcon(QIcon(":/img1/mossy_cobblestone.png"));
        styButtons[i]->setIconSize(QSize(81, 81));
    }
}

void MainWindow::connectStyButtons(){
    for(int i=0;i<100;i++){
        connect(styButtons[i], &QToolButton::clicked, [=](){
            this->timerDay->stop();
            ui->timeControl->setText("继续");

            QDialog dia(this);
            dia.setFixedSize(256,256);
            dia.setWindowTitle(QString("%1号猪圈:共%2个猪").arg(i).arg(this->myFarm->pigStys[i].styTotalPigNum));
            QPlainTextEdit text(&dia);
            text.move(0, 0);
            text.resize(256,256);
            text.appendPlainText(this->myFarm->pigStys[i].styPrint());
            text.setReadOnly(1);
            dia.exec();


        });
    }
}

void MainWindow::connectStyButtonsInfected(){
    for(int i=0;i<100;i++){
        connect(&(this->myFarm->pigStys[i]), &PigSty::styIsInfected, [=](){
            this->styButtons[i]->setIcon(QIcon(":/img1/image2.png"));
        });
    }
}

void MainWindow::connectStyButtonsHealthy(){
    for(int i=0;i<100;i++){
        connect(&(this->myFarm->pigStys[i]), &PigSty::styHaveHealthPig, [=](){
            this->styButtons[i]->setIcon(QIcon(":/img1/image1.png"));
        });
    }
}

void MainWindow::connectStyButtonsNoPig(){
    for(int i=0;i<100;i++){
        connect(&(this->myFarm->pigStys[i]), &PigSty::styIsCleared, [=](){
            this->styButtons[i]->setIcon(QIcon(":/img1/mossy_cobblestone.png"));
        });
    }
}

void MainWindow::autoRead(){//实际上就是读取按钮里的函数黏贴过来的

    //销售记录
    QFile file("saleRecord.txt");
    //农场信息
    QFile file1("pigData.txt");
    //读取销售记录
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream in(&file);
        logInfo=in.readAll();
        QDialog dia(this);
        dia.setWindowTitle("提示:");
        dia.setFixedSize(230, 70);
        QLabel la(&dia);
        la.move(30,20);
        la.setText("读取销售记录成功!");
        file.close();
        dia.exec();
    }else{
        QDialog dia(this);
        dia.setWindowTitle("提示:");
        dia.setFixedSize(230, 70);
        QLabel la(&dia);
        la.move(30,20);
        la.setText("读取销售记录失败!");
        dia.exec();
    }
    //读取农场信息
    if(file1.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream in1(&file1);
        in1>>this->myFarm->dayNum;
        in1>>this->myFarm->farmMoney;
        int pigNum[100];
        for(int i=0;i<100;i++){
            in1>>pigNum[i];
        }
        for(int i=0;i<100;i++){
            while(pigNum[i]--){
                int pigTypee;
                in1>>pigTypee;
                Pig* p=new Pig(pigTypee);
                in1>>p->weight;
                in1>>p->growDay;
                in1>>p->infectedDay;
                in1>>p->infected;
                this->myFarm->pigStys[i].inPig(p);
            }
        }
        this->myFarm->farmSetPigNum();
        QDialog dia(this);
        dia.setWindowTitle("提示:");
        dia.setFixedSize(230, 70);
        QLabel la(&dia);
        la.move(30,20);
        la.setText("读取猪场信息成功!");
        file1.close();
        dia.exec();
    }else{
        QDialog dia(this);
        dia.setWindowTitle("提示:");
        dia.setFixedSize(230, 70);
        QLabel la(&dia);
        la.move(30,20);
        la.setText("读取猪场信息失败!");
        dia.exec();
    }
    this->myFarm->farmSetPigNum();
    ui->moneyValue->setText(QString::number(this->myFarm->farmMoney, 'f', 2));
    ui->dayValue->setText(QString::number(this->myFarm->dayNum));
}

void MainWindow::autoSave(){//实际上也是保存按钮里的函数黏贴过来的
    this->timerDay->stop();
    ui->timeControl->setText("继续");
    //销售记录
    QFile file("saleRecord.txt");
    //农场信息
    QFile file1("pigData.txt");
    //保存销售记录
    if(file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream out(&file);
        out<<this->logInfo;
        QDialog dia(this);
        dia.setWindowTitle("提示:");
        dia.setFixedSize(230, 70);
        QLabel la(&dia);
        la.move(30,20);
        la.setText("保存销售记录成功!");
        file.close();
        dia.exec();
    }
    else{
        QDialog dia(this);
        dia.setWindowTitle("提示:");
        dia.setFixedSize(230, 70);
        QLabel la(&dia);
        la.move(30,20);
        la.setText("保存销售记录失败!");
        dia.exec();
    }
    //保存农场信息
    if(file1.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream out1(&file1);
        out1<<this->myFarm->dayNum<<'\n';
        out1<<this->myFarm->farmMoney<<'\n';
        //int pigNum[100];
        for(int i=0;i<100;i++){
            //pigNum[i]=this->myFarm->pigStys[i].styTotalPigNum;
            out1<<this->myFarm->pigStys[i].styTotalPigNum<<'\n';
        }

        for(int i=0;i<100;i++){
            for(Pig*p=this->myFarm->pigStys[i].styFirstPig();p!=this->myFarm->pigStys[i].ghostEndPig;p=p->nextPig){
                out1<<p->pigType<<'\n';
                out1<<p->weight<<'\n';
                out1<<p->growDay<<'\n';
                out1<<p->infectedDay<<'\n';
                out1<<p->infected<<'\n';
            }
        }
        QDialog dia(this);
        dia.setWindowTitle("提示:");
        dia.setFixedSize(230, 70);
        QLabel la(&dia);
        la.move(30,20);
        la.setText("保存猪场信息成功!");
        dia.exec();
        file1.close();
        this->timerDay->start(1000);
    }
    else{
        QDialog dia(this);
        dia.setWindowTitle("提示:");
        dia.setFixedSize(230, 70);
        QLabel la(&dia);
        la.move(30,20);
        la.setText("保存猪场信息失败!");
        dia.exec();
    }

}






