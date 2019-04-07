#include "fullnamevalidator.h"

FullNameValidator::FullNameValidator(QObject *parent)
    : QValidator(parent)
{

}

FullNameValidator::~FullNameValidator()
{

}


QValidator::State FullNameValidator::validate( QString & input, int & pos ) const
{
    //Invalid
    bool isLong = input.length() > MAX_FULLNAME_LENGTH;					// Длина строки больше допустимой.
    bool isContainWrongChars = checkOnWrongChars(input);				// Налчие недопустимых символов в строке.
    bool isMoreTwoSpcae = input.count(" ") > 2;							// Кол-во пробелов превышает допустимое.
    bool isInvalid = isLong || isContainWrongChars || isMoreTwoSpcae;	// Строка не удовлетворяет требованиям.

    QStringList wordsList = input.split(" ");							// Список слов.

    //Acceptable
    bool isThreeWords = wordsList.size() == 3;												// Кол-во слов равно трём.
    bool isFirstCharsOfWordsUpperCase = checkOnFirstCharsOfWordsUpperCase(wordsList);		// Все слова начинаются с большой буквы.
    bool isAcceptable = isThreeWords && isFirstCharsOfWordsUpperCase;						// Строка удовлетворяет требованиям

    if(isInvalid)
    {
        return QValidator::Invalid;
    }

    if(isAcceptable)
    {
        return QValidator::Acceptable;
    }

    return QValidator::Intermediate;
}

bool FullNameValidator::checkOnWrongChars(const QString & inputString) const
{
    bool isCorrect = false;	// Считаем, что строка корректна.
    for(int i = 0; i < inputString.size() && !isCorrect; ++i) // Для каждого символа и пока строка корректна.
    {
        if(!inputString[i].isLetter() && !inputString[i].isSpace()) // Если текущий символ не символ и не пробел.
        {
            isCorrect = true; // Считаем, что строка некоректна.
        }
    }

    return isCorrect; // Вовзращаем статус проверки строки.
}

bool FullNameValidator::checkOnFirstCharsOfWordsUpperCase(const QStringList & strList) const
{
    bool isCorrect = true; // Считаем, что все слова корректны.
    for(int i = 0; i < strList.size() && isCorrect; ++i) // Для каждого слова и пока слова корректны.
    {
        if(strList[i].length() > 0 && !strList[i][0].isUpper()) // Если первый символ строки не является большим
        {
            isCorrect = false; // Считаем, что слова некорректны.
        }
    }

    return isCorrect; // Вовзращаем статус проверки слов.
}
