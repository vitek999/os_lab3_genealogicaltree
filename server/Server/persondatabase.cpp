#include "persondatabase.h"
#include <QDebug>
#include <QFile>
#include <QDataStream>
#include <windows.h>
#include <psapi.h>

PersonDataBase::PersonDataBase()
{
    maxId = 0;
    isModifedDataBase = false;

}

PersonDataBase::~PersonDataBase()
{

}

int PersonDataBase::count() const
{
    return db.size(); // Возвращаем кол-во записей в контейнере
}

unsigned int PersonDataBase::append(Person record)
{
    //Person person;
    //person = record;
    record.setUId(++maxId);     // Устанавливаем id пользователю.
    //record = person;
    int pos = isnsertToDB(record); // Добавляем пользователя в БД.
    return record.getUId();
}

void PersonDataBase::remove(unsigned int id)
{
    bool removed = false; // Считем, что пользователь не удалён.
    QLinkedList<Person>::iterator i;
    for(i = db.begin(); i != db.end() && !removed; ++i)
    { // Для каждого пользователя базы данных и пока пользователь не удалён...
        if(i->getUId() == id)
        {// ...Если id текущего соврадает с id удаляемым...
            removed = true; // Считаем, что пользователь удалён успешно...
            i=db.erase(i);    // Удаляем пользователя с бд.
        }
    }

    isModifedDataBase = true; // Cчитаем, что база данных изменилась.
}

int PersonDataBase::update(const Person record)
{
    // Удаляем пользователя с id оновляемого пользователя
    remove(record.getUId());
    Person tmp = record;

    // Cчитаем, что база данных изменилась.
    isModifedDataBase = true;

    // Добавляем изменнёного пользователя в БД и возвращаем новый индекс пользователя.
    return isnsertToDB(tmp);
}

Person PersonDataBase::record(unsigned int id) const
{
    bool isFind = false; // Считаем, что пользователь не найден.
    QLinkedList<Person>::const_iterator i;
    for(i = db.constBegin(); i != db.constEnd() && !isFind; ++i)
    {// Для каждого пользователя БД  и пока пользователь не найден...
        if(i->getUId() == id)
        {// ...Если id текущего пользователя совпадает с id искомого пользователя...
            isFind = true;  // ...Считем, что пользовател найден.
            return *i;    // ...Считаем найденую запись искомой.
        }
    }

    return Person();
}

QVector<PersonDataBase::tableStruct> PersonDataBase::records() const
{
    QVector<PersonDataBase::tableStruct> vector;
    QLinkedList<Person>::const_iterator i;
    for(i = db.begin(); i != db.end(); ++i)
    { // Для каждого пользователя БД.
        QString parents; // Результрующая строка с родителями.
        if(i->getMother() == UINT_MAX && i->getFather() == UINT_MAX)
        { // "не задано", если пользователь не имеет родителей.
            parents = "не задано";
        }

        if(i->getFather() != UINT_MAX)
        { // Добавляем в результирующую строку отца, если он задан у пользовтаеля.
            Person father = this->record(i->getFather());
            //this->record(i->getFather(),father);
            parents += father.getFullName();
        }

        if(i->getMother() != UINT_MAX)
        { // Добавляем в результирующую строку мать, если она задана у пользовтаеля.
            if(i->getFather() != UINT_MAX)
            { // Если у пользователя ещё задан отец, то добавляем разделитель.
                parents += ", ";
            }

            Person mother = this->record(i->getMother());
            //this->record(i->getMother(), mother);
            parents += mother.getFullName();
        }

        // Формируем структуру
        PersonDataBase::tableStruct ts = {
            i->getUId(),        // Индекс пользователя.
            i->getFullName(),   // ФИО пользователя.
            i->getDateOfBirth().toString(QString("dd MMMM yyyy")),  //Даа рождения пользователя.
            i->isAlive()?(i->getSex()?"жива":"жив"):i->getDateOfDeath().toString(QString("dd MMMM yyyy")), // Жив ли пользователь.
            parents // Роители.
        };

        // Добавляем сформированную структуру в конец вектора.
        vector.push_back(ts);
    }
    return vector;
}

bool PersonDataBase::save(QString filename)
{

    filename = QString("database.db");
    //bool opened = false; // Считаем, что файл не был открыт.
    if(!filename.isEmpty())
    {// Если имя файла не пустое...
        // ... Открываем файл только для записи
        //QFile file(filename);
        //opened = file.open(QIODevice::WriteOnly);
        //isModifedDataBase = false;
        //QDataStream outputData(&file);
        // Записываем в файл БД и максимальный ID.
        //outputData << db << maxId;
        // Закрываем файл.
        //file.close();
        LPTSTR str = (LPTSTR)filename.utf16();
        HANDLE myFile = CreateFile(str, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

        if ( myFile!=INVALID_HANDLE_VALUE)//Если открыт успешно
        {
            DWORD bytesRead;
            int count = this->count();
            WriteFile(myFile, & count ,sizeof(int), & bytesRead, NULL);
            WriteFile(myFile, & maxId ,sizeof(unsigned int), & bytesRead, NULL);

            QLinkedList<Person>::iterator i;
            for(i = db.begin(); i != db.end(); ++i){
                Person tmp = *i;

                WriteFile(myFile, (void*) & tmp.fUId, sizeof (unsigned int), & bytesRead, NULL);

                int ttt = tmp.fFullName.size() + 1;
                WriteFile(myFile, (void*) & ttt, sizeof (int), & bytesRead, NULL);
                WriteFile(myFile, (void*) tmp.fFullName.data(), sizeof (QChar)*ttt, & bytesRead, NULL);

                WriteFile(myFile, (void*) & tmp.fFather, sizeof (unsigned int), & bytesRead, NULL);
                WriteFile(myFile, (void*) & tmp.fMother, sizeof (unsigned int), & bytesRead, NULL);

                WriteFile(myFile, (void*) & tmp.fDateOfBirth, sizeof (QDate), & bytesRead, NULL);
                WriteFile(myFile, (void*) & tmp.fDateOfDeath, sizeof (QDate), & bytesRead, NULL);

                WriteFile(myFile, (void*) & tmp.fIsAlive, sizeof (bool), & bytesRead, NULL);
                WriteFile(myFile, (void*) & tmp.fIsSolider, sizeof (bool), & bytesRead, NULL);

                WriteFile(myFile, (void*) & tmp.fSex, sizeof (int), & bytesRead, NULL);
                WriteFile(myFile, (void*) & tmp.fCitizenship, sizeof (int), & bytesRead, NULL);
            }

            CloseHandle(myFile);

            isModifedDataBase = false;

            return true;
        }else{
            return false;
        }
    }else{
        return false;
    }


    //return opened;
}

bool PersonDataBase::load(QString filename)
{
    filename = QString("database.db");
    if(!filename.isEmpty()){
        LPTSTR str = (LPTSTR) filename.utf16();

        HANDLE myFile = CreateFile(str, GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

        if(myFile != INVALID_HANDLE_VALUE){
            DWORD bytesRead;
            int count = 0;

            ReadFile(myFile, & count ,sizeof(int), & bytesRead, NULL);
            ReadFile(myFile, & maxId ,sizeof(unsigned int), & bytesRead, NULL);

            QLinkedList<Person>::iterator i;
            for(int i = 0; i < count; ++i){
                Person tmp;

                ReadFile(myFile, (void*) & tmp.fUId, sizeof (unsigned int), & bytesRead, NULL);

                int ttt;
                ReadFile(myFile, (void*) & ttt, sizeof (int), & bytesRead, NULL);
                QChar * tmp2 =  new QChar[ttt];
                ReadFile(myFile, (void*) tmp2, sizeof (QChar)*ttt, & bytesRead, NULL);
                tmp.fFullName = QString(tmp2);
                delete [] tmp2;

                ReadFile(myFile, (void*) & tmp.fFather, sizeof (unsigned int), & bytesRead, NULL);
                ReadFile(myFile, (void*) & tmp.fMother, sizeof (unsigned int), & bytesRead, NULL);

                ReadFile(myFile, (void*) & tmp.fDateOfBirth, sizeof (QDate), & bytesRead, NULL);
                ReadFile(myFile, (void*) & tmp.fDateOfDeath, sizeof (QDate), & bytesRead, NULL);

                ReadFile(myFile, (void*) & tmp.fIsAlive, sizeof (bool), & bytesRead, NULL);
                ReadFile(myFile, (void*) & tmp.fIsSolider, sizeof (bool), & bytesRead, NULL);

                ReadFile(myFile, (void*) & tmp.fSex, sizeof (int), & bytesRead, NULL);
                ReadFile(myFile, (void*) & tmp.fCitizenship, sizeof (int), & bytesRead, NULL);

                db.append(tmp);
            }

            CloseHandle(myFile);

            isModifedDataBase = true;

            return true;

        } else{
            return false;
        }

    } else{
        return false;
    }
}

void PersonDataBase::clear()
{
    maxId = 0; // Обнуляем максимальный ID.
    db.clear(); // Отчищаем контейнер с пользователями.
}

int PersonDataBase::isnsertToDB(Person & person)
{
    QLinkedList<Person>::iterator i;
    // Ищем позицию для вставки
    int pos = 0;
    for(i = db.begin(); i!= db.end() && person > *i; ++i)
    {
        ++pos;
    }

    // Вставляем запись в контейнер.
    db.insert(i, person);

    return pos; // Возвращаем позицию вставки.
}


bool PersonDataBase::isModifed() const
{
    return isModifedDataBase;
}


QStringList PersonDataBase::getListOfPossibleFathers(unsigned int userId) const
{
    QStringList fatherList;

    QLinkedList<Person>::const_iterator i;
    for(i = db.constBegin(); i != db.constEnd(); ++i)
    {// Для каждого пользователя БД.
        if(i->getUId() != userId && i->getSex() == 0)
        {// Если id текущего пользователя и id для которого ведётся поиск не совпадает и пол текущей записи мужской...
            fatherList << i->getFullName(); // ...добавляем ФИО в список отцов.
        }
    }

    // Сортируем спсиок отцов.
    fatherList.sort();
    return fatherList;
}

QStringList PersonDataBase::getListOfPossibleMothers(unsigned int userId) const

{
    QStringList motherList;

    QLinkedList<Person>::const_iterator i;
    for(i = db.constBegin(); i != db.constEnd(); ++i)
    {// Для каждого пользователя БД.
        if(i->getUId() != userId && i->getSex() == 1)
        {// Если id текущего пользователя и id для которого ведётся поиск не совпадает и пол текущей записи женский...
            motherList << i->getFullName(); // ...добавляем ФИО в список матерей.
        }
    }

    // Сортируем спсиок матерей.
    motherList.sort();
    return motherList;
}

unsigned int PersonDataBase::getUIdByFullName(QString fullName) const
{
    bool isFind = false;
    unsigned int id = UINT_MAX;
    QLinkedList<Person>::const_iterator i;
    for(i = db.constBegin(); i != db.constEnd() && !isFind; ++i)
    {
        if(i->getFullName() == fullName)
        {
            isFind = true;
            id = i->getUId();
        }
    }

    return id;
}

QList<unsigned int> PersonDataBase::removeFromParents(unsigned userId)
{
    QList<unsigned int> uIdList; // список пользователей, родитель которых был удалён.

    QLinkedList<Person>::iterator i;

    for(i = db.begin(); i != db.end(); ++i)
    {// Для каждого пользователя БД.
        if(i->getFather() == userId || i->getMother() == userId)
        {// Если искомый id равен id отца или матери текущего пользователя...
            // ...Удаляем отца у текущего пользователя, если id совпадает c id отца.
            if(i->getFather() == userId) { i->setFather(UINT_MAX); }
            // ...Удаляем мать у текущего пользователя, если id совпадает c id матери.
            if(i->getMother() == userId) { i->setMother(UINT_MAX); }
            // Доабвляем id текущего пользователя в список пользователей, родитель которых был удалён.
            uIdList.push_back(i->getUId());
        }
    }

    // Возвращаем список пользователей, родитель которых был удалён.
    return uIdList;
}

bool PersonDataBase::isUniqueFullName(unsigned int uId, QString fullName)
{
    bool isUnique = true;   // Счтиаем, что имя уникально..
    QLinkedList<Person>::const_iterator i;
    for(i = db.constBegin(); i != db.constEnd() && isUnique; ++i)
    {// Для каждого пользователя БД и пока имя уникально...
        if(i->getUId() != uId && i->getFullName() == fullName)
        {// Если id текущего пользователя не совпадает с id искомого, а имена совпадают, то...
            isUnique = false; // ...Считать, что имя неуникально.
        }
    }

    // Возвращаем признак уникальности имени.
    return isUnique;
}

QList<unsigned int> PersonDataBase::getListOfPersonChild(unsigned int uId)
{
    QList<unsigned int> listOfPersonChild;// спсиок детей пользователя.
    QLinkedList<Person>::const_iterator i;
    for(i = db.constBegin(); i != db.constEnd(); ++i)
    {// Для каждого пользователя БД...
        if(i->getFather() == uId || i->getMother() == uId)
        {// Если id отца или матери текущего пользователя совпадает с искомым id...
            listOfPersonChild.push_back(i->getUId()); // ...добавляем id в спсиок детей пользователя.
        }
    }

    // Возвращаем спсиок детей пользователя.
    return listOfPersonChild;
}

bool PersonDataBase::canBeMother(Person & person, unsigned int * childWrongAgeId)
{
    bool result = true; // Считаем, что пользователь может быть матерью.
    QLinkedList<Person>::const_iterator i;
    QDate personDateOfBirth = person.getDateOfBirth();
    QDate personDateOfDeath = person.getDateOfDeath();

    for(i = db.constBegin(); i != db.constEnd() && result; ++i)
    {// Для каждого пользователя БД и пока пользователь может быть матерью...
        int tmp;
        QDate childDateOfBirth = i->getDateOfBirth();
        if(person.getUId() == i->getMother() &&
                !Person::checkOnParentAge(personDateOfBirth, personDateOfDeath, person.isAlive(), childDateOfBirth, 1, &tmp))
        {// Если id родителя совпадает с id маетри текущего пользователя и возраст не является корректным.
            *childWrongAgeId = i->getUId();
            result = false; // Считать, что пользователь не моежт быть матерью.
        }
    }

    return result;
}

bool PersonDataBase::canBeFather(Person & person, unsigned int * childWrongAgeId)
{
    bool result = true; // Считаем, что пользователь может быть отцом.
    QLinkedList<Person>::const_iterator i;
    QDate personDateOfBirth = person.getDateOfBirth();
    QDate personDateOfDeath = person.getDateOfDeath();

    for(i = db.constBegin(); i != db.constEnd() && result; ++i)
    {// Для каждого пользователя БД и пока пользователь может быть отцом...
        int tmp;
        QDate childDateOfBirth = i->getDateOfBirth();
        if(person.getUId() == i->getFather() &&
                !Person::checkOnParentAge(personDateOfBirth, personDateOfDeath, person.isAlive(), childDateOfBirth, 0, &tmp))
        {// Если id родителя совпадает с id отца текущего пользователя и возраст не является корректным.
            *childWrongAgeId = i->getUId();
            result = false; // Считать, что пользователь не моежт быть отцом.
        }
    }

    return result;
}
