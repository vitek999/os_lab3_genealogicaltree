#include "genealogicaltree.h"
#include "ui_genealogicaltree.h"
#include "person.h"

#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>

GenealogicalTree::GenealogicalTree(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GenealogicalTree)
{
    ui->setupUi(this);
    qDebug() << "Start client";
    qDebug() << "Starting server ...";
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si,sizeof(si));
    si.cb=sizeof(si);

    QString str("\\\\.\\PIPE\\MYPIPE");
    LPCWSTR str2 = (LPTSTR)str.utf16();
    qDebug() << "Connecting to pipe ...";

    hPipeIn = CreateFile(str2,
           GENERIC_READ|GENERIC_WRITE,
           0,
           NULL,
           OPEN_EXISTING,
           0,
           NULL);
    if(hPipeIn == INVALID_HANDLE_VALUE){
          qDebug() << "\nPipe not exists";
    }
    Sleep(5000);
    QString str3("\\\\.\\PIPE\\MYPIPEOUT");
    LPCWSTR str4 = (LPTSTR)str3.utf16();
   qDebug() << "Connecting to pipe ...";

    hPipeOut = CreateFile(str4,
           GENERIC_READ|GENERIC_WRITE,
           0,
           NULL,
           OPEN_EXISTING,
           0,
           NULL);
    if(hPipeOut == INVALID_HANDLE_VALUE){
           qDebug() << "\nPipe not exists";
    }

    // Подключаем валидатор.
    QValidator * fullNameValidator = new FullNameValidator(this);
    ui->fullNameLineEdit->setValidator(fullNameValidator);


    menuLabel = new QLabel();   // Создаём Label.
    menuLabel->setText("Текущий файл:");
    statusBar()->addWidget(menuLabel); // Помещаем Label в статус бар.

    // Настраиваем таблцу.
    ui->personsTableWidget->setColumnHidden(0, true);
    ui->personsTableWidget->setColumnWidth(1, 190);
    ui->personsTableWidget->setColumnWidth(2, 120);
    ui->personsTableWidget->setColumnWidth(4, 420);

    // Настраиваем список полов.
    QStringList sexList;
    sexList << "муж." << "жен";
    ui->sexComboBox->addItems(sexList);

    // Устанавливаем максимальную дату смерти текущей.
    ui->dateOfDeathDateEdit->setMaximumDate(QDate::currentDate());

    // Настраиваем список возможных гражданств.
    QStringList citizenshipList;
    citizenshipList << "Россия" << "Украина" << "Польша" << "Германия";
    ui->citizenshipComboBox->addItems(citizenshipList);

    // Соединение сигналов и слотов
    connect(ui->savePushButton, SIGNAL(clicked()), this, SLOT(save()));
    connect(ui->fillPushButton, SIGNAL(clicked()), this, SLOT(fillTable()));
    connect(ui->createPushButton, SIGNAL(clicked()), this, SLOT(addRecord()));
    connect(ui->deletePushButton, SIGNAL(clicked()), this, SLOT(deleteRecord()));
    connect(ui->isAliveCheckBox, SIGNAL(clicked()), this, SLOT(changeDeathDateEnableStatus()));
    connect(ui->dateOfBirthDateEdit, SIGNAL(dateChanged(const QDate &)), this, SLOT(setCorrectBirthdayDate()));
    connect(ui->dateOfDeathDateEdit, SIGNAL(dateChanged(const QDate &)), this, SLOT(setCorrectBirthdayDate()));
    connect(ui->personsTableWidget, SIGNAL(currentCellChanged(int, int, int, int)), this, SLOT(showPersonOnTableChange(int)));
    connect(ui->createFile, SIGNAL(triggered()), this, SLOT(createFile()));
    connect(ui->openFile, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(ui->saveFile, SIGNAL(triggered()), this, SLOT(saveFile()));
    connect(ui->saveAsFile, SIGNAL(triggered()), this, SLOT(saveAsFile()));
    db.load("we", hPipeIn, hPipeOut);
    updateUI();
    emit changeDeathDateEnableStatus();
    emit setCorrectBirthdayDate();
    canChangeAllWidgets();
}

GenealogicalTree::~GenealogicalTree()
{
    delete ui;
}

// Слоты
void GenealogicalTree::save()
{
    // TODO: Дописать этот метод!!!!!!!!
   if(currentRecordCanBeSave())
   {
        //Получаем текущую запись
        Person tmp;
        //db.record(vect.at(current).uId, tmp);
        tmp = db.record(vect.at(current).uId);

        // Получаем данные с виджетов
        QString name = ui->fullNameLineEdit->text();
        QDate dateOfBirthday = ui->dateOfBirthDateEdit->date();
        QDate dateOfDeath = ui->dateOfDeathDateEdit->date();
        bool isAlive = ui->isAliveCheckBox->isChecked();
        QString father = ui->fatherComboBox->currentText();
        QString mother = ui->motherComboBox->currentText();
        int sex = ui->sexComboBox->currentIndex();
        int citizenship = ui->citizenshipComboBox->currentIndex();
        bool isSolider = ui->isSoliderCheckBox->isChecked();

        // Обнволяем данные записи
        tmp.setFullName(name);
        tmp.setDateOfBirth(dateOfBirthday);
        tmp.setDateOfDeath(dateOfDeath);
        tmp.setSex(sex);
        if(ui->fatherComboBox->currentIndex() != 0)
        {
            tmp.setFather(db.getUIdByFullName(father));
        }
        else
        {
            tmp.setFather(UINT_MAX);
        }
        if(ui->motherComboBox->currentIndex() != 0)
        {
            tmp.setMother(db.getUIdByFullName(mother));
        }
        else
        {
            tmp.setMother(UINT_MAX);
        }
        tmp.setAlive(isAlive);
        tmp.setCitizenship(citizenship);
        tmp.setSolider(isSolider);

        // Формируем структуру для таблицы
        PersonDataBase::tableStruct ts = formStructForTable(tmp);

        // Обновляем запись в базе данных
        int pos = db.update(tmp);
        vect.removeAt(current);
        vect.insert(pos, ts);

        // Обновляем таблицу
        delete ui->personsTableWidget->takeItem(current,0);
        delete ui->personsTableWidget->takeItem(current,1);
        delete ui->personsTableWidget->takeItem(current,2);
        delete ui->personsTableWidget->takeItem(current,3);
        delete ui->personsTableWidget->takeItem(current,4);
        ui->personsTableWidget->removeRow(current);

        current = pos;
        ui->personsTableWidget->insertRow(current);
        QTableWidgetItem * item;
        ui->personsTableWidget->setRowHeight(current,20);
        item = new QTableWidgetItem(QString::number(ts.uId));
        ui->personsTableWidget->setItem(current, 0, item);
        item = new QTableWidgetItem(ts.fio);
        ui->personsTableWidget->setItem(current, 1, item);
        item = new QTableWidgetItem(ts.DateOfBirthDay);
        ui->personsTableWidget->setItem(current, 2, item);
        item = new QTableWidgetItem(ts.isAlive);
        ui->personsTableWidget->setItem(current, 3, item);
        item = new QTableWidgetItem(ts.Parents);
        ui->personsTableWidget->setItem(current, 4, item);

        QList<unsigned int> childs = db.getListOfPersonChild(tmp.getUId());
        updateTableRows(childs);

        canChangeAllWidgets();
        showPerson();
        ui->personsTableWidget->setCurrentCell(current, 0);
    }
}

void GenealogicalTree::changeDeathDateEnableStatus()
{
    ui->dateOfDeathDateEdit->setEnabled(!ui->isAliveCheckBox->isChecked()); // Устанавливаем активность виджета даты смерти, если пользователь мёртв
    if(ui->isAliveCheckBox->isChecked())
    {// Если пользователь жив, то максимальной датой смерти задаём текущую.
            ui->dateOfBirthDateEdit->setMaximumDate(QDate::currentDate());
    }
    else
    {
        if(ui->dateOfBirthDateEdit->date() > ui->dateOfDeathDateEdit->date())
        {// Если дата рождения больше даты смерти, то датой рождения ставим дату смерти.
            ui->dateOfBirthDateEdit->setDate(ui->dateOfDeathDateEdit->date());
        }
    }
}

void GenealogicalTree::showPerson()
{
        Person tmp;
        tmp = db.record(vect[current].uId);
       // db.record(vect[current].uId, tmp);
        ui->fullNameLineEdit->setText(tmp.getFullName());				// Установить ФИО.
        setMotherList();												// Установить список возможных матерей.
        setFatherList();												// Установить список возможных отцов.
        ui->dateOfBirthDateEdit->setDate(tmp.getDateOfBirth());			// Установить дату рождения.
        ui->isAliveCheckBox->setChecked(tmp.isAlive());					// Установить флажок, если пользователь жив.
        ui->dateOfDeathDateEdit->setEnabled(!ui->isAliveCheckBox->isChecked());	// Установить активность виджета даты смерти.
        ui->dateOfDeathDateEdit->setDate(tmp.getDateOfDeath());			// Установить дату смерти.
        ui->sexComboBox->setCurrentIndex(tmp.getSex());					// Установить пол.
        ui->citizenshipComboBox->setCurrentIndex(tmp.getCitizenship());	// Установить гражданство.
        ui->isSoliderCheckBox->setChecked(tmp.isSolider());				// Установить состояние на сулжбе.
        qDebug() << tmp.getUId();
}

void GenealogicalTree::setCorrectBirthdayDate()
{
    if(!ui->isAliveCheckBox->isChecked())
    {// Если флаг того, что человек жив, не установлен
        ui->dateOfBirthDateEdit->setMaximumDate(ui->dateOfDeathDateEdit->date()); //Устанаваливаем максиммальную дату рождения равной датой смерти.
    }
    else
    {// Иначе
        ui->dateOfBirthDateEdit->setMaximumDate(QDate::currentDate());  //Устанаваливаем максиммальную дату рождения равной текущей датой.
    }
    ui->dateOfDeathDateEdit->setMinimumDate(ui->dateOfBirthDateEdit->date()); //Устанаваливаем минимальную дату рождения равной датой рождения.
}

void GenealogicalTree::showPersonOnTableChange(int currentRow)
{
    if(current != currentRow)
    {// Если текущий иденкс не равен иднекс строки.
        current = currentRow; // Делаем текущйи индекс равынм индексу строки в таблице.
        if(current >= 0)
        {// Если текущий индекс не отрицательный.
            showPerson();   // Показать текущего пользователя.
        }
    }
}

void GenealogicalTree::fillTable()
{
        // Заполняем бд и таблицу 10 записями.
        db.clear();

        Person person;
        for(int i = 0; i < 10; ++i)
            db.append(person);

        Person person1 = Person(1, "Иванов Иван Иванович", UINT_MAX, UINT_MAX, QDate(1999,8,27), QDate(2000,1,1),
                                true, false, 0, 3);
        db.update(person1);
        Person person2 = Person(2, "Сергеев Сергей Сергеевич", UINT_MAX, UINT_MAX, QDate(1963,2,7), QDate(2000,1,1),
                                true, false, 0, 3);
        db.update(person2);
        Person person3 = Person(3, "Иванова Ангелина Андреевна", 5, UINT_MAX, QDate(1945,3,12), QDate(2002,5,18),
                                false, true, 1, 2);
        db.update(person3);
        Person person4 = Person(4, "Иванова Маргарита Андреевна", 5, 7, QDate(1946,2,12), QDate(2002,5,18),
                                true, true, 1, 2);
        db.update(person4);
        Person person5 = Person(5, "Иванов Андрей Владимирович", UINT_MAX, UINT_MAX, QDate(1921,2,12), QDate(2002,5,18),
                                false, true, 0, 2);
        db.update(person5);
        Person person6 = Person(6, "Дмитриев Сергей Андреевич", UINT_MAX, UINT_MAX, QDate(2001,2,12), QDate(2002,5,18),
                                true, false, 0, 2);
        db.update(person6);
        Person person7 = Person(7, "Иванова Светлана Дмитриевна", UINT_MAX, UINT_MAX, QDate(1921,3,12), QDate(1994,2,21),
                                false, true, 1, 2);
        db.update(person7);
        Person person8 = Person(8, "Васильев Иван Андреевич", UINT_MAX, UINT_MAX, QDate(1937,3,4), QDate(2003,2,10),
                                false, false, 0, 1);
        db.update(person8);
        Person person9 = Person(9, "Дмитриев Андрей Олегович", UINT_MAX, UINT_MAX, QDate(1964,3,4), QDate(2001,1,1),
                                true, false, 0, 0);
        db.update(person9);
        Person person10 = Person(10, "Маисеев Антон Викторович", UINT_MAX, UINT_MAX, QDate(1987,1,2), QDate(2001,1,1),
                                true, false, 0, 2);
        db.update(person10);

        updateUI();
}

void GenealogicalTree::addRecord()
{
    current = db.count(); // Текущий индекс ставим последним в бд.
    // Добавляем строку в таблицу.
    ui->personsTableWidget->insertRow(current);
    QTableWidgetItem * item;
    ui->personsTableWidget->setRowHeight(current,20);
    Person tmp;
    // Добавляем запись в бд.
    int id = db.append(tmp);
    Person added = db.record(id);
    // Заполняем строку в таблице.
    PersonDataBase::tableStruct ts = formStructForTable(added);
    item = new QTableWidgetItem(QString::number(ts.uId));
    ui->personsTableWidget->setItem(current, 0, item);
    item = new QTableWidgetItem(ts.fio);
    ui->personsTableWidget->setItem(current, 1, item);
    item = new QTableWidgetItem(ts.DateOfBirthDay);
    ui->personsTableWidget->setItem(current, 2, item);
    item = new QTableWidgetItem(ts.isAlive);
    ui->personsTableWidget->setItem(current, 3, item);
    item = new QTableWidgetItem(ts.Parents);
    ui->personsTableWidget->setItem(current, 4, item);

    // Добавляем структуру пользователя в вектор.
    vect.push_back(ts);
    // Проверка возможной активности виджетов.
    canChangeAllWidgets();
    // Показать текущую запись.
    showPerson();
    // Установить текущую строку в таблице равной индексцу текущей записи.
    ui->personsTableWidget->setCurrentCell(current, 0);
}

void GenealogicalTree::deleteRecord()
{
    // Запоминаем удаляемый индекс.
    int removeRecordIndex = current;

    // Удаляем запись с таблицы.
    delete ui->personsTableWidget->takeItem(current,0);
    delete ui->personsTableWidget->takeItem(current,1);
    delete ui->personsTableWidget->takeItem(current,2);
    delete ui->personsTableWidget->takeItem(current,3);
    delete ui->personsTableWidget->takeItem(current,4);
    ui->personsTableWidget->removeRow(current);
    //Удалить человека как родителя у других пользователей.
    QList<unsigned int> uIdList = db.removeFromParents(vect[removeRecordIndex].uId);

    // Удаляем запись с модели
    db.remove(vect[removeRecordIndex].uId);
    // Удаляем запись с вектора
    vect.removeAt(removeRecordIndex);

    updateTableRows(uIdList);

    if(current == -1)
        current = 0;

    // Удаляем строку.
    current = ui->personsTableWidget->currentRow();

    if(current == -1)
        current = 0;

    if(removeRecordIndex == vect.size())
        current = vect.size() - 1;

    // Проверка возможной активности виджетов.
    canChangeAllWidgets();

    // Если индекс существует...
    if(current > -1)
        showPerson();   //... Показать текущую запись.
}

void GenealogicalTree::canChangeAllWidgets()
{
    int dbSize = db.count();
    // Отключить активность виджетов, кроме виджетов добавлений записей, при нулевом кол-ве записей В базе данных.
    ui->fullNameLineEdit->setEnabled(dbSize);
    ui->parentsGroupBox->setEnabled(dbSize);
    ui->dateOfBirthDateEdit->setEnabled(dbSize);
    ui->isAliveCheckBox->setEnabled(dbSize);
    ui->dateOfDeathDateEdit->setEnabled(dbSize);
    ui->sexComboBox->setEnabled(dbSize);
    ui->citizenshipComboBox->setEnabled(dbSize);
    ui->isSoliderCheckBox->setEnabled(dbSize);
    ui->savePushButton->setEnabled(dbSize);
    ui->fillPushButton->setEnabled(!dbSize);
    ui->deletePushButton->setEnabled(dbSize);
}

void GenealogicalTree::setFatherList()
{
    // Получаем список возможных отцов для текущего пользователя от модели
    QStringList fatherList = db.getListOfPossibleFathers(vect.at(current).uId);

    // Добавляем вариант "не задан"
    fatherList.push_front("Не задан");

    // Обнуляем список виджета
    ui->fatherComboBox->clear();

    // Добавляем список отуцов в виджет.
    ui->fatherComboBox->addItems(fatherList);

    // Получаем текущего пмользователя с БД.
    Person currentPerson;
    currentPerson = db.record(vect.at(current).uId);
    //db.record(vect.at(current).uId, currentPerson);


    if(currentPerson.getFather() == UINT_MAX)
    {// Если отец у текущего пользователя не задан...
        ui->fatherComboBox->setCurrentText(fatherList[0]); //... Устанавливаем "Не задано" как текущий элемент виджета.
    }
    else
    {// Иначе...
        // ...Получаем отца с БД.
        Person father;
        father = db.record(currentPerson.getFather());
        //db.record(currentPerson.getFather(), father);
        // ...Устанавливаем отца как текущий элемент виджета.
        ui->fatherComboBox->setCurrentText(father.getFullName());
    }
}

void GenealogicalTree::setMotherList()
{
    // Получаем список возможных отцов для текущего пользователя от модели
    QStringList motherLsit = db.getListOfPossibleMothers(vect.at(current).uId);

    // Добавляем вариант "не задан"
    motherLsit.push_front("Не задан");

    // Обнуляем список виджета
    ui->motherComboBox->clear();

    // Добавляем список отуцов в виджет.
    ui->motherComboBox->addItems(motherLsit);

    // Получаем текущего пмользователя с БД.
    Person currentPerson;
    currentPerson = db.record(vect.at(current).uId);
    //db.record(vect.at(current).uId, currentPerson);

    if(currentPerson.getMother() == UINT_MAX)
    {// Если мать у текущего пользователя не задана...
        ui->motherComboBox->setCurrentText(motherLsit[0]);
    }
    else
    {// Иначе...
        // ...Получаем мать с БД.
        Person mother;
        mother = db.record(currentPerson.getMother());
        //db.record(currentPerson.getMother(), mother);
        // ...Устанавливаем мать как текущий элемент виджета.
        ui->motherComboBox->setCurrentText(mother.getFullName());
    }
}

void GenealogicalTree::updateTableRow(int rowIndex)
{
    // Получить текущую запись с БД.
    Person person;
    person = db.record(vect.at(rowIndex).uId);
    //db.record(vect.at(rowIndex).uId, person);
    // Заполнить структуру для полученной записи.
    PersonDataBase::tableStruct ts = formStructForTable(person);
    // Обновить данные таблицы по сформированной структуре.
    QTableWidgetItem * item;
    item = new QTableWidgetItem(QString::number(ts.uId));
    ui->personsTableWidget->setItem(rowIndex, 0, item);
    item = new QTableWidgetItem(ts.fio);
    ui->personsTableWidget->setItem(rowIndex, 1, item);
    item = new QTableWidgetItem(ts.DateOfBirthDay);
    ui->personsTableWidget->setItem(rowIndex, 2, item);
    item = new QTableWidgetItem(ts.isAlive);
    ui->personsTableWidget->setItem(rowIndex, 3, item);
    item = new QTableWidgetItem(ts.Parents);
    ui->personsTableWidget->setItem(rowIndex, 4, item);
}

void GenealogicalTree::updateTableRows(QList<unsigned int> & uIdList)
{
    QList<unsigned int>::const_iterator i;
    for(i = uIdList.constBegin(); i != uIdList.constEnd(); ++i)
    {// Для каждого уникального идентификатора списка
        bool isFind = false; // Cчитать, что пользователь не найден
        QVector<PersonDataBase::tableStruct>::const_iterator j;
        for(j = vect.constBegin(); j != vect.constEnd() && !isFind; ++j)
        {// Для каждого пользователя базы данных
            if(*i == j->uId)
            {// Если уникальный айди совпадает с id пользователя...
                isFind = true; //.. считать, что пользователь найден.
                updateTableRow(j - vect.constBegin()); //Обновить строку в таблице найденного пользователя
            }
        }
    }
}

PersonDataBase::tableStruct GenealogicalTree::formStructForTable(Person person)
{
    QString parents; // Результрующая строка с родителями.
    if(person.getMother() == UINT_MAX && person.getFather() == UINT_MAX)
    { // "не задано", если пользователь не имеет родителей.
        parents = "не задано";
    }

    if(person.getFather() != UINT_MAX)
    { // Добавляем в результирующую строку отца, если он задан у пользовтаеля.
        // Получаем отца с БД.
        Person father;
        father = db.record(person.getFather());
        //db.record(person.getFather(),father);
        // Добавляем отца к строке родителей
        parents += father.getFullName();
    }

    if(person.getMother() != UINT_MAX)
    { // Добавляем в результирующую строку мать, если она задана у пользовтаеля.
        if(person.getFather() != UINT_MAX)
        { // Если у пользователя ещё задан отец, то добавляем разделитель.
            parents += ", ";
        }

        // Получаем мать с БД.
        Person mother;
        mother = db.record(person.getMother());
        //db.record(person.getMother(), mother);
        // Добавляем мать к строке родителей
        parents += mother.getFullName();
    }

    // Формируем структуру
    PersonDataBase::tableStruct ts = {
        person.getUId(),        // Индекс пользователя.
        person.getFullName(),   // ФИО пользователя.
        person.getDateOfBirth().toString(QString("dd MMMM yyyy")),  //Даа рождения пользователя.
        person.isAlive()?(person.getSex()?"жива":"жив"):person.getDateOfDeath().toString(QString("dd MMMM yyyy")), // Жив ли пользователь.
        parents // Роители.
    };

    return ts;
}

bool GenealogicalTree::currentRecordCanBeSave()
{
    bool result = true; // Считать, что текущая запись может быть сохранена.
    QString fullName = ui->fullNameLineEdit->text(); // Получаем ФИО текущей записи.
    bool isUniqueFullName = db.isUniqueFullName(vect[current].uId, fullName); // Проверка на уникальность ФИО.
    bool isCorrectedName = ui->fullNameLineEdit->hasAcceptableInput();  // Проверка на корректность ФИО.
    bool isCorrectAge = true; // Cчитаем, что возраст корректный.
    unsigned int childWrongAgeId = UINT_MAX; // Индекс ребёнка с некоректными параметрами.
    int saveInformationStatus = -1; // Статус ошибки.
    bool canBeParent = true;   // Считаем, что текущий пользователь может быть родителем.
    bool isSexNormal = true;   // Считаем, что текущий пользователь не изменял пол.

    unsigned int motherIndex = db.getUIdByFullName(ui->motherComboBox->currentText());  // Получаем индекс матери.
    unsigned int fatherIndex = db.getUIdByFullName(ui->fatherComboBox->currentText());  // Получаем индекс отца.

    if(!ui->isAliveCheckBox->isChecked() && ui->dateOfDeathDateEdit->date() < ui->dateOfBirthDateEdit->date())
    { // Если пользователь мёртв и дата смерти меньше даты рождения, считаем, что возраст некоректен.
        isCorrectAge = false;
        saveInformationStatus = 3;
    }

    //Проверка возраста когда задана только мать.
    if(motherIndex != UINT_MAX && fatherIndex == UINT_MAX && isCorrectAge)
    {
        int codeOfAgeStatus;
        Person mother;
        mother = db.record(motherIndex);
        //db.record(motherIndex, mother);
        QDate motherDateOfBirth = mother.getDateOfBirth();
        QDate motherDateOfDeath = mother.getDateOfDeath();
        QDate currentPersonDateOfBirth = ui->dateOfBirthDateEdit->date();
        isCorrectAge = Person::checkOnParentAge(motherDateOfBirth, motherDateOfDeath, mother.isAlive(),
                                               currentPersonDateOfBirth, 1, &codeOfAgeStatus);
        if(!isCorrectAge)
            saveInformationStatus = codeOfAgeStatus;
    }

    //Проверка возраста когда задан только отец.
    if(motherIndex == UINT_MAX && fatherIndex != UINT_MAX && isCorrectAge)
    {
        int codeOfAgeStatus;
        Person father;
        father = db.record(fatherIndex);
        QDate fatherDateOfBirth = father.getDateOfBirth();
        QDate fatherDateOfDeath = father.getDateOfDeath();
        QDate currentPersonDateOfBirth = ui->dateOfBirthDateEdit->date();
        isCorrectAge = Person::checkOnParentAge(fatherDateOfBirth, fatherDateOfDeath, father.isAlive(),
                                               currentPersonDateOfBirth, 0, &codeOfAgeStatus);
        if(!isCorrectAge)
            saveInformationStatus = codeOfAgeStatus;
    }

    //Проверка возраста когда заданы отец и мать.
    if(motherIndex != UINT_MAX && fatherIndex != UINT_MAX && isCorrectAge){
        int codeOfAgeStatus;
        Person mother;
        mother = db.record(motherIndex);
        //db.record(motherIndex, mother);
        Person father;
        father = db.record(fatherIndex);
        QDate motherDateOfBirth = mother.getDateOfBirth();
        QDate motherDateOfDeath = mother.getDateOfDeath();
        QDate fatherDateOfBirth = father.getDateOfBirth();
        QDate fatherDateOfDeath = father.getDateOfDeath();
        QDate currentPersonDateOfBirth = ui->dateOfBirthDateEdit->date();


        isCorrectAge =(Person::checkOnParentAge(motherDateOfBirth, motherDateOfDeath, mother.isAlive(),
                                               currentPersonDateOfBirth, 1, &codeOfAgeStatus) &&
                       Person::checkOnParentAge(fatherDateOfBirth, fatherDateOfDeath, father.isAlive(),
                                               currentPersonDateOfBirth, 0, &codeOfAgeStatus));
        if(!isCorrectAge)
            saveInformationStatus = codeOfAgeStatus;
    }

    // Задаём параметры объекту класса пользователя.
    Person currentPerson;
    currentPerson.setUId(vect[current].uId);
    currentPerson.setDateOfBirth(ui->dateOfBirthDateEdit->date());
    currentPerson.setDateOfDeath(ui->dateOfDeathDateEdit->date());
    currentPerson.setAlive(ui->isAliveCheckBox->isChecked());

    // Проверяем, может ли он быть родителем.
    canBeParent = true; //db.canBeFather(currentPerson, &childWrongAgeId) && db.canBeMother(currentPerson, &childWrongAgeId);

    // Получаем список детей пользователя.
    QList<unsigned int> listOfChild = db.getListOfPersonChild(vect[current].uId);

    // Получаем текущего пользователя с БД.
    Person tmp;
    tmp = db.record(vect[current].uId);
    // Проверяем изменил ли он пол и есть ли у него дети
    if(tmp.getSex() != ui->sexComboBox->currentIndex() && listOfChild.size() > 0)
    {
        if(isSexChangeAccept())
        {// Если пользователь согласился на изменения пола...
            isSexNormal = true;
            // ...Удаляем у остальных пользователей текущего как родителя.
            QList<unsigned int> personIdList = db.removeFromParents(vect[current].uId);
            // ...Обновляем информацию других пользователей.
            updateTableRows(personIdList);
        }
        else
        {   // Иначе
            isSexNormal = false;
            // Возвращаем пол, который был.
            ui->sexComboBox->setCurrentIndex(tmp.getSex());
        }
    }

    // Проверяем корректность всех аспектов.
    result = isCorrectedName && isUniqueFullName && isCorrectAge && canBeParent && isSexNormal;

    //Вывод ошибок сохранения.
    if(!isCorrectedName)
    {// Вывод ошибки, если имя введено некорректно.
        QMessageBox::warning(this,"Ошибка сохранения", "Проверьте правильность Ф.И.О.");
    }
    else if(!isUniqueFullName)
    {// Вывод ошибки, если имя уже сущетсвует.
        QMessageBox::warning(this,"Ошибка сохранения", "Пользователь с таким именем уже существует");
    }
    else if(!isCorrectAge)
    {// Вывод причины некорректности возратса.
        QString msg = "dd";
        switch(saveInformationStatus)
        {
            case 0:
                msg = "Родителям должно быть не меньше 15 лет до рождения ребёнка";
                break;
            case 1:
                msg = "Мать умерла до рождения ребёнка";
                break;
            case 2:
                msg = "Отец умер до зачатия ребёнка";
                break;
            case 3:
                msg = "Дата смерти меньше даты рождения";
                break;
        }
        QMessageBox::warning(this,"Ошибка сохранения", "Неверная дата рождения: " + msg);
    }
    else if(!canBeParent)
    {// Вывод ошибки, если пользователь не может быть родителем.
        QMessageBox::warning(this,"Ошибка сохранения", "Данный человек указан как родитель"
                                                           "\nИзмените данные текущего пользователя или ребёнка");
    }

    // Возвращаем результат порверки.
    return result;
}

bool GenealogicalTree::isSexChangeAccept()
{
    // Запрашиваем согласие пользователя на изменения пола.
    bool result;
    QMessageBox msgBox;
            msgBox.setText("Пользователь изменён");
            msgBox.setInformativeText("Вы изменили пол человека данной записи, но он является родителем "
                                      "\nВы действительно хотите изменить пол? Данный человек будет сброшен "
                                      "как родитель в другой записи");
            msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
            int ret = msgBox.exec();

            switch(ret)
            {
                case QMessageBox::Save:
                    result = true;
                    break;
                case QMessageBox::Discard:
                    result = false;
                    break;
                default:
                    result = false;
            }

    // Возвращаем результат согласия.
    return result;
}

void GenealogicalTree::createFile()
{
    if(isCurrentFileModifed())
    {// Если текущий файл был модифицирован...
        if(isAcceptSaveFile())
        { // Если пользователь согласился на сохранения файла...
            saveFile(); // ...сохранить файл
        }
    }

    currentFileName = ""; // Текущее имя файла
    db.clear(); // Чистим БД.
    updateUI(); // Обновляем UI.
    menuLabel->setText("Текущий файл: "+ currentFileName); // Устанавливаем в статус бар имя текущего файла.

}

void GenealogicalTree::openFile()
{
    if(isCurrentFileModifed())
    {// Если текущий файл был модифицирован...
        if(isAcceptSaveFile())
        { // Если пользователь согласился на сохранения файла...
            saveFile(); // ...сохранить файл
        }
    }

    // Получаем имя открываемого файла.
    QString newFileName = QFileDialog::getOpenFileName(this, "Открыть файл", "/", "База данных пользователей (*.userdb)");

    if(!newFileName.isEmpty())
    {// Если новое имя файла не пустое...
        currentFileName = newFileName; //... текущему имени файла присваиваем новое.
        db.clear(); // Чистим БД.
        db.load(currentFileName, hPipeIn, hPipeOut); // Загружаем файл в БД.
        updateUI(); // Обновляем UI.
        menuLabel->setText("Текущий файл: "+ currentFileName); // Устанавливаем в статус бар имя текущего файла.
    }
}

void GenealogicalTree::saveFile()
{
    if(!currentFileName.isEmpty())
    {// Если имя текущего файла не пустое...
        db.save(currentFileName); // ...сохраняем БД в текущий файл.
    }
    else
    {// Иначе..
        saveAsFile(); // ...Выполняем сохранение с запросом имени файла.
    }

    menuLabel->setText("Текущий файл: "+ currentFileName); // Устанавливаем в статус бар имя текущего файла.
}

void GenealogicalTree::saveAsFile()
{
    // Получаем имя сохраняемого файла.
    QString newFileName = QFileDialog::getSaveFileName(this, "Сохранить как", "/", "База данных пользователей (*.userdb)");
    if(!newFileName.isEmpty())
    {// Если новое имя файла не пустое...
        currentFileName = newFileName;  //... текущему имени файла присваиваем новое.
        db.save(currentFileName);  // ...сохраняем БД в текущий файл.
        menuLabel->setText("Текущий файл: "+ currentFileName);  // Устанавливаем в статус бар имя текущего файла.
    }
}

bool GenealogicalTree::isCurrentFileModifed()
{
    // Если файл не модифицирован и не является только что созданным и пустым.
    return db.isModifed() && !(currentFileName.isEmpty() && db.count() == 0);
}

bool GenealogicalTree::isAcceptSaveFile()
{
    // Запрос на согласие сохранения файла...
    bool result;
    QMessageBox msgBox;
            msgBox.setText("Файл не сохранён");
            msgBox.setInformativeText("Данный файл был изменён, хотите ли вы сохранить файл? "
                                      "\nВ противном случае все изменения будут потеряны.");
            msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
            int ret = msgBox.exec();

            switch(ret)
            {
                case QMessageBox::Save:
                    result = true;
                    break;
                case QMessageBox::Discard:
                    result = false;
                    break;
                default:
                    result = false;
            }

    // Возвращаем результат ответа пользователя
    return result;
}

void GenealogicalTree::updateUI()
{
    ui->personsTableWidget->clear(); // Чистим таблицу.
    // Устанавливаем заголовки для таблицы.
    QStringList tableHeader;
    tableHeader << "Id" << "ФИО" << "Дата рождения" << "Дата смерти" << "Родители";
    ui->personsTableWidget->setHorizontalHeaderLabels(tableHeader);
    ui->personsTableWidget->setRowCount(0);
    // Чистим вектор записей.
    vect.clear();

    // Получаем список записей с БД.
    vect = db.records();
    // Добавляем каждую запись в таблицу
    QVector<PersonDataBase::tableStruct>::iterator i;
    for(i = vect.begin(); i != vect.end(); ++i)
    {
        int index = i - vect.begin();
        ui->personsTableWidget->insertRow(index);
        QTableWidgetItem * item;
        ui->personsTableWidget->setRowHeight(index,20);
        item = new QTableWidgetItem(QString::number(i->uId));
        ui->personsTableWidget->setItem(index, 0, item);
        item = new QTableWidgetItem(i->fio);
        ui->personsTableWidget->setItem(index, 1, item);
        item = new QTableWidgetItem(i->DateOfBirthDay);
        ui->personsTableWidget->setItem(index, 2, item);
        item = new QTableWidgetItem(i->isAlive);
        ui->personsTableWidget->setItem(index, 3, item);
        item = new QTableWidgetItem(i->Parents);
        ui->personsTableWidget->setItem(index, 4, item);
    }

    // Проверяем могут ли изменяться виджеты.
    canChangeAllWidgets();

    if(vect.size() > 0)
    {// Если записи существуют...
        current = 0;    // ... Делаем первую запись текущей.
        ui->personsTableWidget->setCurrentCell(current, 0); // ...Выбираем первую строку таблицы.
        showPerson();   // ...Показываем текущую запись.
    }
}

void GenealogicalTree::closeEvent(QCloseEvent *event)
{
    //if(isCurrentFileModifed())
    //{  // Если текущий файл был модифицирован...
    //    if(isAcceptSaveFile())
    //    {   // Если пользователь согласился на сохранения файла...
    //        saveFile(); // ...сохранить файл
    //    }
   // }
    // Закрыть окно.

    db.save("we");
    db.disconnect();
    QMainWindow::closeEvent(event);
}
