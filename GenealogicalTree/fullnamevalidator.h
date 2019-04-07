#ifndef FULLNAMEVALIDATOR_H
#define FULLNAMEVALIDATOR_H

#include <QValidator>
#include <QDebug>
#include <QStringList>

#define MAX_FULLNAME_LENGTH 45				// Допустимая длина строки.


class FullNameValidator : public QValidator
{
    Q_OBJECT

public:
    FullNameValidator(QObject *parent);
    ~FullNameValidator();
    QValidator::State validate(QString & input, int & pos ) const;
private:
    /*! Проверка на недопустимые символы
     *\param[in] - QString - вводимая строка.
     *\return - true - содержит недопустимые символы, false - не содержит.
     */
    bool checkOnWrongChars(const QString & inputString) const;

    /*! Проверка: каждое слово начинается с большой буквы
     *\param[in] - QStringList - Спсиок введённых слов.
     *\return - true - все слова начинаются с большой буквы, false - хотя бы одно слово начинается не с большой буквы.
     */
    bool checkOnFirstCharsOfWordsUpperCase(const QStringList & strList) const;
};

#endif // FULLNAMEVALIDATOR_H
