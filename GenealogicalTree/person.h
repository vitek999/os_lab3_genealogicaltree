#ifndef PERSON_H
#define PERSON_H

#include <QString>
#include <QDate>

class Person
{
private:


public:
    unsigned int fUId;			// Уникальный идентификатор пользователя
    QString fFullName;	// Имя пользователя.
    unsigned int fFather;		// Отец пользователя.
    unsigned int fMother;		// Мать пользователя.
    QDate fDateOfBirth;	// Дата рождения пользователя.
    QDate fDateOfDeath;	// Дата смерти пользователя.
    bool fIsAlive;		// Статус - жив ли пользователь.
    bool fIsSolider;	// Статус - состоял ли пользователь на военной службе.
    int fSex;			// Пол пользователя.
    int fCitizenship;	// Гражданство пользователя.

    Person();
    Person(QString fullName);
    Person(const Person & person);
    Person(unsigned int uId, QString fullName, unsigned int father, unsigned int mother, QDate dateOfBirth,
           QDate dateOfDeath, bool isAlive, bool isSolider, int sex, int citizenship);

    /*! Установить уникальный id пользователю.
     * \param uId уникальный идентификатор
     */
    void setUId(unsigned int uId);

    /*! Получить уникальный идентификатор текущего пользователя.
     * \return уникальный идентификатор пользователя.
     */
    unsigned int getUId() const;

    /*! Проверка на возраст родителя и ребёнка.
     *\param[in] pb - Дата рождения родителя.
     *\param[in] pd - Дата смерти родителя.
     *\param[in] isAllive - жив ли родитель.
     *\param[in] childDateofBirth - дата рождения ребёнка.
     *\param[in] parentSex - пол родителя.
     *\param[in|out] status - конкретная причина несоотвествия возраста.
     *\return - true - возраст не соответсвует, false - соответствует.
     */
    static bool checkOnParentAge(QDate & pb, QDate & pd, bool isAllive, QDate & childDateofBirth, int parentSex, int * status);

    /*! Оператор проверка равенства для Объектов Person
     * \param[in] right правый операнд
     * \return true если объекты совпадают, иначе false
     */
    bool operator==(const Person & right);

    /*! Оператор проверка неравенства для Объектов Person
     * \param[in] right правый операнд
     * \return false если объекты совпадают, иначе true
     */
    bool operator!=(const Person & right);

    /*! Оператор больше для Объектов Person
     * \param[in] right правый операнд
     * \return true, если левый операнд больше правого, иначе false
     */
    bool operator>(const Person & right);

    /*! Оператор больше для Объектов Person
     * \param[in] right правый операнд
     * \return true, если левый операнд больше либо равен правому, иначе false
     */
    bool operator>=(const Person & right);

    /*! Оператор меньше для Объектов Person
     * \param[in] right правый операнд
     * \return true, если левый операнд меньше правого, иначе false
     */
    bool operator<(const Person & right);

    /*! Оператор меньше для Объектов Person
     * \param[in] right правый операнд
     * \return true, если левый операнд меньше либо равен правому, иначе false
     */
    bool operator<=(const Person & right);

    /*! Оператор присваивания для Объектов Person
     * \param[in] right правый операнд
     * \return на объект со значениями правого операнда.
     */
    Person & operator=(const Person & right);

    /*! Устанавилвает имя пользователя записи
     *\param[in] - QString (имя пользователя)
     */
    void setFullName(QString & fullName);

    /*! Возвращает имя пользователя записи
     *\return - QString имя пользователя
     */
    QString getFullName() const;

    /*! Устанавилвает отца для записи
     *\param[in] - индекс отца в массиве
     */
    void setFather(unsigned int index);

    /*! Возвращает индекс отца заданной для записи
     *\return - индекс отца, -1 если отец не задан
     */
    unsigned int getFather() const;

    /*! Устанавилвает мать для записи
     *\param[in] - индекс матери в массиве
     */
    void setMother(unsigned int index);

    /*! Возвращает индекс матери заданной для записи
     *\return - индекс матери, -1 если мать не задан
     */
    unsigned int getMother() const;

    /*! Устанавилвает дату рождения для пользователя записи
     *\param[in] - QDate (дата рождения)
     */
    void setDateOfBirth(QDate date);

    /*! Возвращает дату рожденя пользователя записи
     *\return - Дата рождения
     */
    QDate getDateOfBirth() const;

    /*! Устанавилвает дату смерти для пользователя записи
     *\param[in] - QDate (дата смерти)
     */
    void setDateOfDeath(QDate date);

    /*! Возвращает дату смерти пользователя записи
     *\return - Дата смерти
     */
    QDate getDateOfDeath() const;

    /*! Устанавливает жив ли пользователь
     *\param[in] - булевое выражение, жив ли пользователь.
     */
    void setAlive(bool isAlive);

    /*! Возвращает жив ли пользователь
     *\return - true - пользователь жив, false - мёртв
     */
    bool isAlive() const;

    /*! Устанавливает состоял ли пользователь на службе
     *\param[in] - булевое выражение, состоял ли пользователь на службе.
     */
    void setSolider(bool isSolder);

    /*! Возвращает состоял ли пользователь на службе
     *\return - true - пользователь состоял на службе, flase - не состоял
     */
    bool isSolider() const;

    /*! Устанавливает пол пользователя
     *\param[in] - 0 - мужской, 1 - женский
     */
    void setSex(int sex);

    /*! Возвращает пол пользователя
     *\return - 0 - мужской, 1 - женский
     */
    int getSex() const;

    /*! Устанавливает гражданство пользователя
     *\param[in] - 0 - Россия, 1 - Украина, 2 - Польша, 3 - Германия
     */
    void setCitizenship(int citizenship);

    /*! Возвращает гражданство пользователя
     *\return - 0 - Россия, 1 - Украина, 2 - Польша, 3 - Германия
     */
    int getCitizenship() const;
};

/*! Оператор вывода в поток
 * \param out поток
 * \param data данные для вывода
 * \return поток
 */
QDataStream & operator<<(QDataStream & out, const Person & data);

/*! Оператор ввода из потока
 * \param out поток
 * \param data данные для ввода
 * \return поток
 */
QDataStream & operator>>(QDataStream & out, Person & data);

#endif // PERSON_H
