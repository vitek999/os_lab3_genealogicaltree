#ifndef PERSONDATABASE_H
#define PERSONDATABASE_H

#include <QLinkedList>
#include <QVector>
#include "person.h"

class PersonDataBase
{
public:
    PersonDataBase();
    ~PersonDataBase();

    struct tableStruct
    {
        unsigned int uId;       // Уникальный идентификтор пользователя.
        QString fio;            // ФИО пользователя.
        QString DateOfBirthDay; // Датарождения пользователя.
        QString isAlive;        // Жив ли пользователь.
        QString Parents;        // Строка содержащая родителей пользователя.
    };

    /*! Получить кол-во записей
     * \return количество записей
     */
    int count() const;

    /*! Добавить запись в базу данных
     *\param[in] добавляемая запись
     *\return позиция записи в соответствии с порядком сортировки
     */
    unsigned int append(Person record);

    /*! Удалить из базы данных записи с заданным идентификатором
     * \param[in] id идентификатор удаляемой записи.
     */
    void remove(unsigned int id);

    /*! Изменить запись в базе данных
     * \param[in] record - изменённая запись
     * \return новая позиция записи в соответствии с порядком сортировки
     */
    int update(const Person record);

    /*! Возвращает запись (только для чтения)
     * \param[in] id Уникальный идентификатор.
     * \param[in|out] record запись
     */
    Person record(unsigned int id) const;

    /*! Возвратить вектор записей с учётом сортировки.
     * \return вектор записей.
     */
    QVector<tableStruct> records() const;

    /*! Сохранить данные в заданный файл
     * \param filename имя файла
     * \return false если сохранить не удлалось, иначе true
     */
    bool save(QString filename);


    /*! Загрузить данные из заданного файла
     * \param filename имя файла
     * \return true если удалось сохранмить, иначе false
     */

    bool load(QString filename);

    //! Уничтожает все данные
    void clear();

    /*! Показывает, имеются ли изменения в бд после её загрузки/сохранения.
     * \return true если меняется, иначе false
     */
    bool isModifed() const;

    /*! Вернуть спсиок возможных отцов
     * \param userId индентификатор пользователя, для которого формируется список
     * \return список отцов
     */
    QStringList getListOfPossibleFathers(unsigned userId) const;

    /*! Вернуть спсиок возможных матерей
     * \param userId индентификатор пользователя, для которого формируется список
     * \return список матерей
     */
    QStringList getListOfPossibleMothers(unsigned userId) const;

    /*! Получить id записи по имени.
     * \param fullName имя пользователя
     * \return уникальный id пользователя
     */
    unsigned int getUIdByFullName(QString fullName) const;

    /*! Удалить пользователя со списка родителей
     * \param userId уникальный id пользователя
     * \return список id у которых пользователь был удалён
     */
    QList<unsigned int> removeFromParents(unsigned userId);

    /*! Имя пользователя Уникально.
     * \param uId уникальный id Пользователя.
     * \param fullName полное имя пользователя.
     * \return true - уникально, false - Если пользователь с таким ФИО уже существует.
     */
    bool isUniqueFullName(unsigned int uId ,QString fullName);

    /*! Получить список детей пользователя.
     * \param uId уникальный id пользователя.
     * \return Список id детей пользователя.
     */
    QList<unsigned int> getListOfPersonChild(unsigned int uId);

    /*! Может ли пользователь быть матерью своих детей.
     * \param person текущий пользователь
     * \param[in|out] childWrongAgeIndex индекс человека, матерью которого не может быть текущий пользователь.
     * \return true если может быть родитлем, иначе false.
     */
    bool canBeMother(Person & person, unsigned int * childWrongAgeIndex);

    /*! Может ли пользователь быть отцом своих детей.
     * \param person текущий пользователь
     * \param[in|out] childWrongAgeIndex индекс человека, отцом которого не может быть текущий пользователь.
     * \return true если может быть родитлем, иначе false.
     */
    bool canBeFather(Person & person, unsigned int * childWrongAgeIndex);
private:
    unsigned int maxId; // Максиммальный уникальный идентификтор последнего пользователя в БД.
    QLinkedList<Person> db; // Контейнер с пользователями
    bool isModifedDataBase; // Была ли изменена база данных.

    /*! Вставить запис в контейнер
     * \param person вставляемая запись
     * \return индекс втсавки с учётом сортировки
     */
    int isnsertToDB(Person & person);
};

#endif // PERSONDATABASE_H
