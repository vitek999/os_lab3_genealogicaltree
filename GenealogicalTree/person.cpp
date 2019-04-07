#include "person.h"
#include <QDataStream>

Person::Person()
{
    fUId = 0;
    fFullName = QString("");
    fFather = UINT_MAX;
    fMother = UINT_MAX;
    fDateOfBirth = QDate(2000,1,1);
    fDateOfDeath = QDate(2000,1,1);
    fIsAlive = true;
    fIsSolider = false;
    fSex = 0;
    fCitizenship = 0;
}

Person::Person(QString fullName)
{
    fFullName = fullName;
}

Person::Person(const Person & person){
    fUId = person.fUId;
    fFullName = person.fFullName;
    fFather = person.fFather;
    fMother = person.fMother;
    fDateOfBirth = person.fDateOfBirth;
    fDateOfDeath = person.fDateOfDeath;
    fIsAlive = person.fIsAlive;
    fIsSolider = person.fIsSolider;
    fSex = person.fSex;
    fCitizenship = person.fCitizenship;
}

Person::Person(unsigned int uId, QString fullName, unsigned int father, unsigned int mother, QDate dateOfBirth,
               QDate dateOfDeath, bool isAlive, bool isSolider, int sex, int citizenship)
{
    fUId = uId;
    fFullName = fullName;
    fFather = father;
    fMother = mother;
    fDateOfDeath = dateOfDeath;
    fDateOfBirth = dateOfBirth;
    fIsAlive = isAlive;
    fIsSolider = isSolider;
    fSex = sex;
    fCitizenship = citizenship;
}

unsigned int Person::getUId() const
{
    return fUId;
}

void Person::setUId(unsigned int id)
{
    fUId = id;
}

void Person::setFullName(QString & fullName)
{
    fFullName = fullName;
}

QString Person::getFullName() const
{
    return fFullName;
}

void Person::setFather(unsigned int index)
{
    fFather = index;
}

unsigned int Person::getFather() const
{
    return fFather;
}

void Person::setMother(unsigned int index)
{
    fMother = index;
}

unsigned int Person::getMother() const
{
    return fMother;
}

void Person::setDateOfBirth(QDate date)
{
    fDateOfBirth = date;
}

QDate Person::getDateOfBirth() const
{
    return fDateOfBirth;
}

void Person::setDateOfDeath(QDate date)
{
    fDateOfDeath = date;
}

QDate Person::getDateOfDeath() const
{
    return fDateOfDeath;
}

void Person::setAlive(bool isAlive)
{
    fIsAlive = isAlive;
}

bool Person::isAlive() const
{
    return fIsAlive;
}

void Person::setSolider(bool isSolder)
{
    fIsSolider = isSolder;
}

bool Person::isSolider() const
{
    return fIsSolider;
}

void Person::setSex(int sex)
{
    fSex = sex;
}

int Person::getSex() const
{
    return fSex;
}

void Person::setCitizenship(int citizenship)
{
    fCitizenship = citizenship;
}

int Person::getCitizenship() const
{
    return fCitizenship;
}

bool Person::operator==(const Person & right)
{
    return fFullName == right.fFullName && fFather == right.fFather &&
        fMother == right.fMother && fDateOfBirth == right.fDateOfBirth && fDateOfDeath == right.fDateOfDeath &&
        fIsAlive == right.fIsAlive && fIsSolider == right.fIsSolider && fSex == right.fSex && fCitizenship == right.fCitizenship;
}

bool Person::operator!=(const Person & right)
{
    return !(*this == right);
}

bool Person::operator<(const Person & right)
{
    if(fDateOfBirth < right.fDateOfBirth)
        return true;
    if(fDateOfBirth == right.fDateOfBirth && fFullName < right.fFullName)
        return true;

    return false;
}

bool Person::operator<=(const Person & right)
{
        return *this < right || (fDateOfBirth == right.fDateOfBirth && fFullName == right.fFullName);
}

bool Person::operator>(const Person & right)
{
        return !(*this < right) && (fDateOfBirth != right.fDateOfBirth || fFullName != right.fFullName);
}

bool Person::operator>=(const Person & right)
{
    return *this > right || (fDateOfBirth == right.fDateOfBirth && fFullName == right.fFullName);
}

Person & Person::operator=(const Person & right)
{
    fUId = right.fUId;
    fFullName = right.fFullName;
    fFather = right.fFather;
    fMother = right.fMother;
    fDateOfBirth = right.fDateOfBirth;
    fDateOfDeath = right.fDateOfDeath;
    fIsAlive = right.fIsAlive;
    fIsSolider = right.fIsSolider;
    fSex = right.fSex;
    fCitizenship = right.fCitizenship;

    return *this;
}

QDataStream & operator<<(QDataStream & out, const Person & data)
{
    out << data.getUId() << data.getFullName() << data.getFather() <<
           data.getMother() << data.getDateOfBirth() << data.getDateOfDeath() <<
           data.isAlive() << data.isSolider() << data.getSex() << data.getCitizenship();

    return out;
}


QDataStream & operator>>(QDataStream & out, Person & data)
{
    unsigned int dUId;			// Уникальный идентификатор пользователя
    QString dFullName;	// Имя пользователя.
    unsigned int dFather;		// Отец пользователя.
    unsigned int dMother;		// Мать пользователя.
    QDate dDateOfBirth;	// Дата рождения пользователя.
    QDate dDateOfDeath;	// Дата смерти пользователя.
    bool dIsAlive;		// Статус - жив ли пользователь.
    bool dIsSolider;	// Статус - состоял ли пользователь на военной службе.
    int dSex;			// Пол пользователя.
    int dCitizenship;	// Гражданство пользователя.

    out >> dUId >> dFullName >> dFather >> dMother >> dDateOfBirth >> dDateOfDeath >>
            dIsAlive >> dIsSolider >> dSex >> dCitizenship;

    data.setUId(dUId);
    data.setFullName(dFullName);
    data.setFather(dFather);
    data.setMother(dMother);
    data.setDateOfBirth(dDateOfBirth);
    data.setDateOfDeath(dDateOfDeath);
    data.setAlive(dIsAlive);
    data.setSolider(dIsSolider);
    data.setSex(dSex);
    data.setCitizenship(dCitizenship);

    return  out;
}

bool Person::checkOnParentAge(QDate & pb, QDate & pd, bool isAllive, QDate & childDateofBirth, int parentSex, int * status)
{
    bool result = false; // Считаем, что возраст некоректен.
        *status = -1; // Считаем, что причины некорректности возраста нет.
        QDate parentBitrh = pb.addYears(15); //Добавим минимально допустимую разницу в возрасте.

        if(parentBitrh <= childDateofBirth)
        {// Если дата рождения родителя с учётом допустимой меньше даты рождения ребёнка.
            //qDebug()<<"check" << parentBitrh.toString() << " " << childDateofBirth.toString();
            if(isAllive)
            {// Если жив, то считве, что возраст корректен.
                result = true;
            }
            else
            {
                if(parentSex == 1)
                {// Если родитель мать
                    if(pd >= childDateofBirth)
                    {//Если Дата смерти больше даты рождения, то возраст счиает корретным
                        result = true;
                    }
                    else
                    {
                        *status = 1;
                    }
                }
                else
                {// Если родитель отец.
                    QDate minDateDeath = childDateofBirth.addMonths(9); // Добавляем 9 месяцев к дате рождения ребёнка.
                    if(pd >= minDateDeath)
                    {// Если дата смерти не меньше чем дата рождения + 9 месяцев, то возраст корректен.
                        result = true;
                    }
                    else
                    {
                        *status = 2;
                    }
                }
            }
        }
        else
        {
            *status = 0;
        }

        return result;
}
