#ifndef GENEALOGICALTREE_H
#define GENEALOGICALTREE_H

#include <QMainWindow>
#include "fullnamevalidator.h"
#include "persondatabase.h"
#include <windows.h>
#include <psapi.h>

#include <QLabel>
namespace Ui {
class GenealogicalTree;
}

class GenealogicalTree : public QMainWindow
{
    Q_OBJECT

public:
    explicit GenealogicalTree(QWidget *parent = nullptr);
    ~GenealogicalTree();

private:
    Ui::GenealogicalTree *ui;
    PersonDataBase db;      // База данных пользователей.
    int current;            // Индекс текущего пользователея в таблице.
    QString currentFileName;    // Имя открытого файла.
    QVector<PersonDataBase::tableStruct> vect;  // Вектор структур для представления в таблице.
    QLabel * menuLabel; // QLabel для QStatusBar.

    HANDLE hPipeIn;
    HANDLE hPipeOut;

    //! Изменить состояние видимости виджетов в зависимости от кол-ва текущих записей.
    void canChangeAllWidgets();

    //! Заполнить виджет возможными матерями
    void setMotherList();

    //! Заполнить виджет возможными отцами
    void setFatherList();

    void closeEvent(QCloseEvent *event);

    //! Сформировать структуру для таблицы
    PersonDataBase::tableStruct formStructForTable(Person person);

    //! Подтверждено ли изменения пола текущего пользователя.
    bool isSexChangeAccept();


    /*! Обновить строку в табице.
     * \param rowIndex индекс обновляемой строки.
     */
    void updateTableRow(int rowIndex);

    /*! Обновить несколько строк в таблице.
     * \param uIdList список обновляемых строк.
     */
    void updateTableRows(QList<unsigned int> & uIdList);

    /*! Изменён ли текущий файл.
     * \return true если изменён, иначе false.
     */
    bool isCurrentFileModifed();

    /*! Запрос на согласие сохранения файла
     * \return true если пользлователь согласился, иначе false
     */
    bool isAcceptSaveFile();

    // Обновить интерфейс.
    void updateUI();
private slots:
    //! Сохранить запись.
    void save();

    /*! Может ли текущая запись быть сохранена
     * \return true - может, fale - нет.
     */
    bool currentRecordCanBeSave();

    //! Изменить видимость виджета даты смерти.
    void changeDeathDateEnableStatus();

    //! Показать запись.
    void showPerson();

    //! Установить корректные параметры для виджета даты рождения.
    void setCorrectBirthdayDate();

    /*! Показать пользователя при смене ячейки в таблице.
    * \param[in] currentRow текущая строка
    */
    void showPersonOnTableChange(int currentRow);

    //! Заполнить таблицу.
    void fillTable();

    //! Добавить запись.
    void addRecord();

    //! Удалить запись.
    void deleteRecord();

    // Создать новый файл.
    void createFile();

    // Открыть файл.
    void openFile();

    // Сохранить файл.
    void saveFile();

    // Сохранить текущий файл с запросом имени сохраняемого файла.
    void saveAsFile();
};



#endif // GENEALOGICALTREE_H
