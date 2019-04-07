#include "persondatabase.h"
#include <QDebug>
#include <QFile>
#include <QDataStream>

enum comands
{
    LOAD = 0,
    ISMOD = 1,
    CLEAR = 2,
    SAVE = 3,
    COUNT = 4,
    RECORDS = 5,
    RECORD = 6,
    APPEND = 7,
    REMOVE = 8,
    UPDATE = 9,
    GET_LIST_OF_POSSIBLE_FATHERS = 10,
    GET_LIST_OF_POSSIBLE_MOTHERS = 11,
    GET_UID_BY_FULLNAME = 12,
    REMOVE_FROM_PARENTS = 13,
    IS_UNIQUE_FULL_NAME = 14,
    GET_LIST_OF_PERSON_CHILD = 15,
    END = 16
};

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
    comands c = COUNT;
    WriteFile(hPipeOut, (void *) &c,  sizeof(comands), NULL, NULL);

    DWORD bytes;
    int size;
    ReadFile(hPipeIn, (void*) &size, sizeof (int), &bytes, NULL);

    return size; // Возвращаем кол-во записей в контейнере
}

unsigned int PersonDataBase::append(Person record)
{
    comands c = APPEND;
    WriteFile(hPipeOut, (void *) &c,  sizeof(comands), NULL, NULL);

    Person person = record;
    DWORD bytes;

    WriteFile(hPipeOut, (void*) &person.fUId, sizeof (unsigned int), & bytes, NULL);

    int ttt = person.fFullName.size() + 1;
    WriteFile(hPipeOut, (void*) &ttt, sizeof (int), & bytes, NULL);
    WriteFile(hPipeOut, (void*) person.fFullName.data(), sizeof (QChar)*ttt, & bytes, NULL);

    WriteFile(hPipeOut, (void*) &person.fFather, sizeof (unsigned int), & bytes, NULL);
    WriteFile(hPipeOut, (void*) &person.fMother, sizeof (unsigned int), & bytes, NULL);

    WriteFile(hPipeOut, (void*) &person.fDateOfBirth, sizeof (QDate), & bytes, NULL);
    WriteFile(hPipeOut, (void*) &person.fDateOfDeath, sizeof (QDate), & bytes, NULL);

    WriteFile(hPipeOut, (void*) &person.fIsAlive, sizeof (bool), & bytes, NULL);
    WriteFile(hPipeOut, (void*) &person.fIsSolider, sizeof (bool), & bytes, NULL);

    WriteFile(hPipeOut, (void*) &person.fSex, sizeof (int), & bytes, NULL);
    WriteFile(hPipeOut, (void*) &person.fCitizenship, sizeof (int), & bytes, NULL);

    unsigned int ID;
    ReadFile(hPipeIn, (void*) &ID, sizeof(unsigned int), &bytes, NULL);

    return ID;
}

void PersonDataBase::remove(unsigned int id)
{
    comands c = REMOVE;
    WriteFile(hPipeOut, (void *) &c,  sizeof(comands), NULL, NULL);

    DWORD bytes;
    unsigned int ID = id;
    WriteFile(hPipeOut, (void *) &ID, sizeof(unsigned int), &bytes, NULL);
}

int PersonDataBase::update(const Person record)
{
    comands c = UPDATE;
    WriteFile(hPipeOut, (void *) &c,  sizeof(comands), NULL, NULL);

    Person person = record;
    DWORD bytes;

    WriteFile(hPipeOut, (void*) &person.fUId, sizeof (unsigned int), & bytes, NULL);

    int ttt = person.fFullName.size() + 1;
    WriteFile(hPipeOut, (void*) &ttt, sizeof (int), & bytes, NULL);
    WriteFile(hPipeOut, (void*) person.fFullName.data(), sizeof (QChar)*ttt, & bytes, NULL);

    WriteFile(hPipeOut, (void*) &person.fFather, sizeof (unsigned int), & bytes, NULL);
    WriteFile(hPipeOut, (void*) &person.fMother, sizeof (unsigned int), & bytes, NULL);

    WriteFile(hPipeOut, (void*) &person.fDateOfBirth, sizeof (QDate), & bytes, NULL);
    WriteFile(hPipeOut, (void*) &person.fDateOfDeath, sizeof (QDate), & bytes, NULL);

    WriteFile(hPipeOut, (void*) &person.fIsAlive, sizeof (bool), & bytes, NULL);
    WriteFile(hPipeOut, (void*) &person.fIsSolider, sizeof (bool), & bytes, NULL);

    WriteFile(hPipeOut, (void*) &person.fSex, sizeof (int), & bytes, NULL);
    WriteFile(hPipeOut, (void*) &person.fCitizenship, sizeof (int), & bytes, NULL);

    int pos;
    ReadFile(hPipeIn, (void*) &pos, sizeof(int), &bytes, NULL);
    return pos;
}

Person PersonDataBase::record(unsigned int id) const
{
    comands c = RECORD;
    WriteFile(hPipeOut, (void *) &c,  sizeof(comands), NULL, NULL);


    DWORD bytes;
    Person tmp;
    unsigned int ID = id;
    WriteFile(hPipeOut, (void *) &ID, sizeof(unsigned int), &bytes, NULL);

    ReadFile(hPipeIn, (void*) &tmp.fUId, sizeof (unsigned int), & bytes, NULL);

    int ttt;
    ReadFile(hPipeIn, (void*) &ttt, sizeof (int), & bytes, NULL);
    QChar * tmp2 =  new QChar[ttt];
    ReadFile(hPipeIn, (void*) tmp2, sizeof (QChar)*ttt, & bytes, NULL);
    tmp.fFullName = QString(tmp2);
    delete [] tmp2;

    ReadFile(hPipeIn, (void*) &tmp.fFather, sizeof (unsigned int), & bytes, NULL);
    ReadFile(hPipeIn, (void*) &tmp.fMother, sizeof (unsigned int), & bytes, NULL);

    ReadFile(hPipeIn, (void*) &tmp.fDateOfBirth, sizeof (QDate), & bytes, NULL);
    ReadFile(hPipeIn, (void*) &tmp.fDateOfDeath, sizeof (QDate), & bytes, NULL);

    ReadFile(hPipeIn, (void*) &tmp.fIsAlive, sizeof (bool), & bytes, NULL);
    ReadFile(hPipeIn, (void*) &tmp.fIsSolider, sizeof (bool), & bytes, NULL);

    ReadFile(hPipeIn, (void*) &tmp.fSex, sizeof (int), & bytes, NULL);
    ReadFile(hPipeIn, (void*) &tmp.fCitizenship, sizeof (int), & bytes, NULL);
    return tmp;
}

QVector<PersonDataBase::tableStruct> PersonDataBase::records() const
{
    DWORD bytesRead;
    comands c = RECORDS;
    QVector<PersonDataBase::tableStruct> vect;
    WriteFile(hPipeOut, (void *) &c,  sizeof(comands), NULL, NULL);

    DWORD written;
    int count;
    ReadFile(hPipeIn, (void *) &count, sizeof(int), &written, NULL);
    qDebug() << count;
    for(int i = 0; i < count; ++i)
    {
        PersonDataBase::tableStruct it;
        ReadFile(hPipeIn, (void*) &it.uId, sizeof(unsigned int), &written, NULL);


        int ttt;
        ReadFile(hPipeIn, (void*) &ttt, sizeof (int), & written, NULL);
        QChar * tmp2 =  new QChar[ttt];
        ReadFile(hPipeIn, (void*) tmp2, sizeof (QChar)*ttt, & written, NULL);
        it.fio = QString(tmp2);
        delete [] tmp2;

        ReadFile(hPipeIn, (void*) &ttt, sizeof (int), & written, NULL);
        QChar * tmp3 =  new QChar[ttt];
        ReadFile(hPipeIn, (void*) tmp3, sizeof (QChar)*ttt, & written, NULL);
        it.DateOfBirthDay = QString(tmp3);
        delete [] tmp3;

        ReadFile(hPipeIn, (void*) &ttt, sizeof (int), & written, NULL);
        QChar * tmp4 =  new QChar[ttt];
        ReadFile(hPipeIn, (void*) tmp4, sizeof (QChar)*ttt, & written, NULL);
        it.isAlive = QString(tmp4);
        delete [] tmp4;

        ReadFile(hPipeIn, (void*) &ttt, sizeof (int), & written, NULL);
        QChar * tmp5 =  new QChar[ttt];
        ReadFile(hPipeIn, (void*) tmp5, sizeof (QChar)*ttt, & written, NULL);
        it.Parents = QString(tmp5);
        delete [] tmp5;

        vect.push_back(it);
    }

    return vect;
}

bool PersonDataBase::save(QString filename)
{
    comands c = SAVE;
    WriteFile(hPipeOut, (void *) &c,  sizeof(comands), NULL, NULL);
    DWORD bytes;
    bool res = true;
    ReadFile(hPipeIn, (void*) &res, sizeof(bool), &bytes, NULL);
    return res;
}

bool PersonDataBase::load(QString filename, HANDLE hIn, HANDLE hOut)
{
    hPipeIn = hOut;
    hPipeOut = hIn;
    comands c = LOAD;
    WriteFile(hPipeOut, (void*) &c,sizeof(comands), NULL, NULL);

    bool a = false;
    DWORD bytes;
    ReadFile(hPipeIn, (void*) &a, sizeof(bool), &bytes, NULL);

    qDebug() << a;
    return true;
}

void PersonDataBase::clear()
{
    comands c = CLEAR;
    WriteFile(hPipeOut, (void *) &c,  sizeof(comands), NULL, NULL);
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
    comands c = ISMOD;
    WriteFile(hPipeOut, (void *) &c,  sizeof(comands), NULL, NULL);

    bool tmp = false;
    DWORD written;
    ReadFile(hPipeIn, (void*) &tmp, sizeof(bool), &written, NULL);
    return written;
}

// TODO: Others

QStringList PersonDataBase::getListOfPossibleFathers(unsigned int userId) const
{
     qDebug() << "check1";
    QStringList fatherList;

    comands c = GET_LIST_OF_POSSIBLE_FATHERS;
    WriteFile(hPipeOut, (void *) &c,  sizeof(comands), NULL, NULL);

    unsigned int ID = userId;
    DWORD bytes;
    WriteFile(hPipeOut, (void *) &ID, sizeof(unsigned int), &bytes, NULL);

    int sizeOfList;
    ReadFile(hPipeIn, (void *) &sizeOfList, sizeof(int), &bytes, NULL);
    qDebug() << "check2";
    for(int i = 0; i < sizeOfList; ++i)
    {
        int ttt;
        ReadFile(hPipeIn, (void*) &ttt, sizeof (int), &bytes, NULL);
        QChar * tmp2 =  new QChar[ttt];
        ReadFile(hPipeIn, (void*) tmp2, sizeof (QChar)*ttt, &bytes, NULL);
        fatherList << QString(tmp2);
        delete [] tmp2;
    }

    qDebug() << "check3";
    return fatherList;
}

QStringList PersonDataBase::getListOfPossibleMothers(unsigned int userId) const
{

    QStringList motherList;

    comands c = GET_LIST_OF_POSSIBLE_MOTHERS;
    WriteFile(hPipeOut, (void *) &c,  sizeof(comands), NULL, NULL);

    unsigned int ID = userId;
    DWORD bytes;
    WriteFile(hPipeOut, (void *) &ID, sizeof(unsigned int), &bytes, NULL);

    int sizeOfList;
    ReadFile(hPipeIn, (void *) &sizeOfList, sizeof(int), &bytes, NULL);
    for(int i = 0; i < sizeOfList; ++i)
    {
        int ttt;
        ReadFile(hPipeIn, (void*) &ttt, sizeof (int), &bytes, NULL);
        QChar * tmp2 =  new QChar[ttt];
        ReadFile(hPipeIn, (void*) tmp2, sizeof (QChar)*ttt, &bytes, NULL);
        motherList << QString(tmp2);
        delete [] tmp2;
    }

    return motherList;
}

unsigned int PersonDataBase::getUIdByFullName(QString fullName) const
{
    unsigned int Id;
    comands c = GET_UID_BY_FULLNAME;
    WriteFile(hPipeOut, (void *) &c,  sizeof(comands), NULL, NULL);

    QString fn = fullName;
    DWORD bytes;
    int tmp = fullName.size() + 1;
    WriteFile(hPipeOut, (void *) &tmp, sizeof(int), &bytes, NULL);
    WriteFile(hPipeOut, (void *) fn.data(), sizeof(QChar)*tmp, &bytes, NULL);

    ReadFile(hPipeIn, (void *) &Id, sizeof(unsigned int), &bytes, NULL);

    return Id;
}

QList<unsigned int> PersonDataBase::removeFromParents(unsigned userId)
{
    QList<unsigned int> uIdList; // список пользователей, родитель которых был удалён.

    unsigned Id = userId;
    comands c = REMOVE_FROM_PARENTS;
    WriteFile(hPipeOut, (void *) &c,  sizeof(comands), NULL, NULL);

    DWORD bytes;

    WriteFile(hPipeOut, (void*) &Id, sizeof(unsigned), &bytes, NULL);

    int size;
    ReadFile(hPipeIn, (void *) &size, sizeof(int), &bytes, NULL);

    for(int i = 0; i < size; ++i)
    {
        unsigned int tmp;
        ReadFile(hPipeIn, (void *) &tmp, sizeof(unsigned int), &bytes, NULL);
        uIdList.push_back(tmp);
    }

    return uIdList;
}

bool PersonDataBase::isUniqueFullName(unsigned int uId, QString fullName)
{
    bool isUnique = true;   // Счтиаем, что имя уникально..

    unsigned int Id = uId;
    QString fn = fullName;
    comands c = IS_UNIQUE_FULL_NAME;
    WriteFile(hPipeOut, (void *) &c,  sizeof(comands), NULL, NULL);

    DWORD bytes;
    WriteFile(hPipeOut, (void*) &Id, sizeof(unsigned int), &bytes, NULL);

    int size = fn.size() + 1;
    WriteFile(hPipeOut, (void*) &size, sizeof(int), &bytes, NULL);
    WriteFile(hPipeOut, (void*) fn.data(), sizeof(QChar)*size, &bytes, NULL);

    ReadFile(hPipeIn, (void *) &isUnique, sizeof(bool), &bytes, NULL);
    // Возвращаем признак уникальности имени.
    return isUnique;
}

QList<unsigned int> PersonDataBase::getListOfPersonChild(unsigned int uId)
{
    QList<unsigned int> uIdList; // список пользователей, родитель которых был удалён.

    unsigned Id = uId;
    comands c = REMOVE_FROM_PARENTS;
    WriteFile(hPipeOut, (void *) &c,  sizeof(comands), NULL, NULL);

    DWORD bytes;

    WriteFile(hPipeOut, (void*) &Id, sizeof(unsigned), &bytes, NULL);

    int size;
    ReadFile(hPipeIn, (void *) &size, sizeof(int), &bytes, NULL);

    for(int i = 0; i < size; ++i)
    {
        unsigned int tmp;
        ReadFile(hPipeIn, (void *) &tmp, sizeof(unsigned int), &bytes, NULL);
        uIdList.push_back(tmp);
    }

    return uIdList;
}

void PersonDataBase::disconnect()
{
    comands c = END;
    WriteFile(hPipeOut, (void *) &c,  sizeof(comands), NULL, NULL);
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
