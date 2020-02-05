#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon("icon.png"));
    setWindowTitle(QStringLiteral("单位人事管理系统"));
    //设置图标……UI界面设置失灵了mmp
    ui->mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);//设置文字在图标下
    ui->OpenAction->setIcon(QIcon("open.png"));
    ui->CancleAction->setIcon(QIcon("cancle.png"));
    ui->AddAction->setIcon(QIcon("add.png"));
    ui->ExitAction->setIcon(QIcon("exit.png"));
    ui->SaveAction->setIcon(QIcon("save.png"));
    ui->Insertaction->setIcon(QIcon("file.png"));
    ui->SetPhotoAction->setIcon(QIcon("view.png"));
    ui->DeleteAction->setIcon(QIcon("delete.png"));
    ui->AboutAction->setIcon(QIcon("about.png"));
    ui->VersionAction->setIcon(QIcon("version.png"));
//    QPixmap photo("dali3.png");
    ui->PhotoLabel->setPixmap(QPixmap("dali3.png"));

}

MainWindow::~MainWindow()
{
    delete ui;
}
/**********************************窗口工具栏按键******************************************/
//"打开"按键打开数据库
void MainWindow::on_OpenAction_triggered()
{
    QString aFile=QFileDialog::getOpenFileName(this,QStringLiteral("选择数据库文件"),"",
                                               QStringLiteral("SQLite数据库(*.db *.db3)"));
    if(aFile.isEmpty()){//选择失败
        QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("未能正确选择数据库！"));
        return;
    }else{
        DB = QSqlDatabase::addDatabase("QSQLITE");
        DB.setDatabaseName(aFile);//设置数据库名称
        if(!DB.isOpen()){//数据库打开失败
            QMessageBox::warning(this,QStringLiteral("错误"),QStringLiteral("打开数据库失败！"));
            return;
        }else{
            openTable();//打开数据库
        }
    }
}

//"关于"键
void MainWindow::on_AboutAction_triggered()
{
    QMessageBox::information(this,QStringLiteral("联系作者"),QStringLiteral("作者：西电_沉迷单车的追风少年\n邮箱：15385433870@163.com"));
}

//"版本信息"键
void MainWindow::on_VersionAction_triggered()
{
    QMessageBox::information(this,QStringLiteral("版本信息"),QStringLiteral("Version：1.1\n开发时间：2020.02.05"));
}

//"保存键"
void MainWindow::on_SaveAction_triggered()
{
    //调用进行缓存
    if(!tabModel->submitAll()){//保存失败
        QMessageBox::warning(this,QStringLiteral("错误"),QStringLiteral("保存失败！"));
    }else{//保存成功
        ui->SaveAction->setEnabled(false);//保存功能锁定
        ui->CancleAction->setEnabled(false);//取消功能锁定
    }
}

//"退出键"
void MainWindow::on_ExitAction_triggered()
{
    close();//关闭程序
}

//"取消键"
void MainWindow::on_CancleAction_triggered()
{
    tabModel->revertAll();
    ui->SaveAction->setEnabled(false);//保存功能锁定
    ui->CancleAction->setEnabled(false);//取消功能锁定
}

//“添加键”
void MainWindow::on_AddAction_triggered()
{
    tabModel->insertRow(tabModel->rowCount(),QModelIndex()); //在末尾添加一个记录

    QModelIndex curIndex=tabModel->index(tabModel->rowCount()-1,1);//创建最后一行的ModelIndex
    theSelection->clearSelection();//清空选择项
    theSelection->setCurrentIndex(curIndex,QItemSelectionModel::Select);//设置刚插入的行为当前选择行

    int currow=curIndex.row(); //获得当前行
    tabModel->setData(tabModel->index(currow,0),2000+tabModel->rowCount()); //自动生成编号
    tabModel->setData(tabModel->index(currow,2),"男");
    // 插入行时设置缺省值，需要在primeInsert()信号里去处理
}

//“插入键”
void MainWindow::on_Insertaction_triggered()
{
    QModelIndex curIndex=ui->tableView->currentIndex();

    tabModel->insertRow(curIndex.row(),QModelIndex());

    theSelection->clearSelection();//清除已有选择
    theSelection->setCurrentIndex(curIndex,QItemSelectionModel::Select);
}

//“删除键”
void MainWindow::on_DeleteAction_triggered()
{
    QModelIndex curIndex = theSelection->currentIndex();//获取当前模型索引
    tabModel->removeRow(curIndex.row());//删除一行
}
/**********************************排序选择******************************************/
//升序
void MainWindow::on_AscendBtn_clicked()
{
    tabModel->setSort(ui->ComboFile->currentIndex(),Qt::AscendingOrder);
    tabModel->select();
}

//降序
void MainWindow::on_DeacendBtn_clicked()
{
    tabModel->setSort(ui->ComboFile->currentIndex(),Qt::DescendingOrder);
    tabModel->select();
}
/**********************************数据过滤******************************************/
//男按键
void MainWindow::on_ManBtn_clicked()
{
    tabModel->setFilter(" Gender='男' ");
}

//女按键
void MainWindow::on_WomenBtn_clicked()
{
    tabModel->setFilter(" Gender='女' ");
}
//全部显示
void MainWindow::on_radioButton_3_clicked()
{
    tabModel->setFilter("");
}
/********************************************************************************/
//选择字段组合框
void MainWindow::on_ComboFile_currentIndexChanged(int index)
{
    if(ui->AscendBtn->isChecked()){//如果升序键按下
        tabModel->setSort(index,Qt::AscendingOrder);
    }else{//降序
        tabModel->setSort(index,Qt::DescendingOrder);
    }
    tabModel->select();
}

//打开数据库
void MainWindow::openTable(){
    tabModel = new QSqlTableModel(this,DB);//数据表
    tabModel->setTable("employee");//设置数据表
    //选择列号进行排序
    tabModel->setSort(tabModel->fieldIndex("EmpNo"),Qt::AscendingOrder);//设置为升序排序
    //设置编辑策略
    tabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);//设置为所有修改暂时缓存，手动调用时才缓存
    if(!(tabModel->select())){//查询数据
        //显示错误信息
        QMessageBox::critical(this,QStringLiteral("错误"),QStringLiteral("打开数据表出现错误！\n")
                              +tabModel->lastError().text(),QMessageBox::Ok,QMessageBox::NoButton);
        return;
    }
    //字段显示名,设置表头数据;每段字符显示设置相应的中文标题
    tabModel->setHeaderData(tabModel->fieldIndex("empNo"),Qt::Horizontal,"工号");
    tabModel->setHeaderData(tabModel->fieldIndex("Name"),Qt::Horizontal,"姓名");
    tabModel->setHeaderData(tabModel->fieldIndex("Gender"),Qt::Horizontal,"性别");
    tabModel->setHeaderData(tabModel->fieldIndex("Height"),Qt::Horizontal,"身高");
    tabModel->setHeaderData(tabModel->fieldIndex("Birthday"),Qt::Horizontal,"出生日期");
    tabModel->setHeaderData(tabModel->fieldIndex("Mobile"),Qt::Horizontal,"手机");
    tabModel->setHeaderData(tabModel->fieldIndex("Province"),Qt::Horizontal,"省份");
    tabModel->setHeaderData(tabModel->fieldIndex("City"),Qt::Horizontal,"城市");
    tabModel->setHeaderData(tabModel->fieldIndex("Department"),Qt::Horizontal,"部门");
    tabModel->setHeaderData(tabModel->fieldIndex("Education"),Qt::Horizontal,"学历");
    tabModel->setHeaderData(tabModel->fieldIndex("Salary"),Qt::Horizontal,"工资");
    //这两个字段不再tableView中显示
    tabModel->setHeaderData(tabModel->fieldIndex("Memo"),Qt::Horizontal,"备注");
    tabModel->setHeaderData(tabModel->fieldIndex("Photo"),Qt::Horizontal,"照片");

    //选择模型
    theSelection=new QItemSelectionModel(tabModel);//关联选择模型
    //theSelection当前项变化时触发currentChanged信号
    connect(theSelection,SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this,SLOT(on_currentChanged(QModelIndex,QModelIndex)));
    //选择行变化时
    connect(theSelection,SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this,SLOT(on_currentRowChanged(QModelIndex,QModelIndex)));

    ui->tableView->setModel(tabModel);//设置数据模型
    ui->tableView->setSelectionModel(theSelection); //设置选择模型
    ui->tableView->setColumnHidden(tabModel->fieldIndex("Memo"),true);//隐藏列
    ui->tableView->setColumnHidden(tabModel->fieldIndex("Photo"),true);//隐藏列

    //tableView上为“性别”和“部门”两个字段设置自定义代理组件
    QStringList strList;
    strList<<"男"<<"女";
    bool isEditable=false;
    delegateSex.setItems(strList,isEditable);
    ui->tableView->setItemDelegateForColumn(
       tabModel->fieldIndex("Gender"),&delegateSex); //Combbox选择型

    strList.clear();
    strList<<"销售部"<<"技术部"<<"生产部"<<"行政部";
    isEditable=true;
    delegateDepart.setItems(strList,isEditable);
    ui->tableView->setItemDelegateForColumn(tabModel->fieldIndex("Department"),&delegateDepart); //Combbox选择型

    //创建界面组件与数据模型的字段之间的数据映射
    dataMapper= new QDataWidgetMapper();
    dataMapper->setModel(tabModel);//设置数据模型
    dataMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);

    //界面组件与tabModel的具体字段之间的联系
    dataMapper->addMapping(ui->JobNumEdit,tabModel->fieldIndex("empNo"));
    dataMapper->addMapping(ui->NameEdit,tabModel->fieldIndex("Name"));
    dataMapper->addMapping(ui->SexEdit,tabModel->fieldIndex("Gender"));
    dataMapper->addMapping(ui->HeightEdit,tabModel->fieldIndex("Height"));
    dataMapper->addMapping(ui->dateEdit,tabModel->fieldIndex("Birthday"));
    dataMapper->addMapping(ui->WechatEdit,tabModel->fieldIndex("Mobile"));
    dataMapper->addMapping(ui->ProEdit,tabModel->fieldIndex("Province"));
    dataMapper->addMapping(ui->CityEdit,tabModel->fieldIndex("City"));
    dataMapper->addMapping(ui->DepEdit,tabModel->fieldIndex("Department"));
    dataMapper->addMapping(ui->EduEdit,tabModel->fieldIndex("Education"));
    dataMapper->addMapping(ui->WageEdit,tabModel->fieldIndex("Salary"));
    dataMapper->addMapping(ui->NoteEdit,tabModel->fieldIndex("Memo"));

    //移动到首记录
    dataMapper->toFirst();
    //获取字段名称列表，填充ui->groupBoxSort组件
    getFieldNames();

    //更新actions和界面组件的使能状态
    ui->AddAction->setEnabled(false);
    ui->OpenAction->setEnabled(true);
    ui->ExitAction->setEnabled(true);
    ui->SaveAction->setEnabled(true);
    ui->Insertaction->setEnabled(true);
    ui->SetPhotoAction->setEnabled(true);
    ui->AboutAction->setEnabled(true);
    ui->VersionAction->setEnabled(true);
    ui->groupBoxSort->setEnabled(true);
    ui->groupBoxFillter->setEnabled(true);
}

//theSelection当前项变化时触发currentChanged信号
void MainWindow::on_currentChanged(const QModelIndex &current, const QModelIndex &previous){
    //定义不在函数体中使用过的参数,可以消除编译器产生的警告
    Q_UNUSED(current);
    Q_UNUSED(previous);
    //更新状态,将保存键和取消键设置为可用状态
    ui->SaveAction->setEnabled(true);
    ui->CancleAction->setEnabled(true);
}

// QTableView的SelectionModel的行发生了变化，进行处理
void MainWindow::on_currentRowChanged(const QModelIndex &current, const QModelIndex &previous){
    Q_UNUSED(previous);
    //行切换时的状态控制
    ui->DeleteAction->setEnabled(current.isValid());
    ui->SetPhotoAction->setEnabled(current.isValid());
    //更新映射行号
    dataMapper->setCurrentIndex(current.row());
    int currentRow = current.row();//获取行号
    QSqlRecord currRec = tabModel->record(currentRow);//获取当前记录
    if(currRec.isNull("Photo")){//如果当前图片字段为空
        qDebug()<<QStringLiteral("当前图片字段为空")<<endl;
    }else{
        //图片映射部分要转QPixmap。。懒得写了
        qDebug()<<QStringLiteral("设置图片")<<endl;
    }
}

//获取字段名称列表，填充ui->groupBoxSort组件
void MainWindow::getFieldNames()
{ //获取所有字段名称
    QSqlRecord  emptyRec=tabModel->record();//获取空记录，只有字段名
    for (int i=0;i<emptyRec.count();i++){
        ui->ComboFile->addItem(emptyRec.fieldName(i));
    }
}



