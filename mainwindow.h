#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QDataWidgetMapper>
#include <QPixmap>
#include <QFileDialog>
#include <QMessageBox>
#include "qwcomboboxdelegate.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_OpenAction_triggered();

    void on_AboutAction_triggered();

    void on_VersionAction_triggered();

    void on_SaveAction_triggered();

    void on_ExitAction_triggered();

    void on_CancleAction_triggered();
    //theSelection当前项变化时触发currentChanged信号
    void on_currentChanged(const QModelIndex &current, const QModelIndex &previous);
    // QTableView的SelectionModel的行发生了变化，进行处理
    void on_currentRowChanged(const QModelIndex &current, const QModelIndex &previous);

    void on_AddAction_triggered();

    void on_Insertaction_triggered();

    void on_DeleteAction_triggered();

    void on_AscendBtn_clicked();

    void on_ComboFile_currentIndexChanged(int index);

    void on_DeacendBtn_clicked();

    void on_ManBtn_clicked();

    void on_WomenBtn_clicked();

    void on_radioButton_3_clicked();

private:
    QSqlDatabase DB;//数据库连接
    QSqlTableModel *tabModel;//数据库模型
    QItemSelectionModel *theSelection;//选择模型
    QDataWidgetMapper *dataMapper;//数据映射
    QWComboBoxDelegate delegateSex;//自定义数据代理;性别
    QWComboBoxDelegate delegateDepart;//自定义数据代理;部门
    void openTable();//打开数据表
    void getFieldNames();//获取字段名称,填充排序字段

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
