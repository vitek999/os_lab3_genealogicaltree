#include <QCoreApplication>
#include "persondatabase.h"
#include <windows.h>
#include <psapi.h>

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

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    puts("##### Start server #####");
    QString str("\\\\.\\PIPE\\MYPIPE");
    LPCWSTR str2 = (LPTSTR)str.utf16();
    HANDLE hPipeIn = CreateNamedPipe(str2,
               PIPE_ACCESS_DUPLEX,
               PIPE_TYPE_MESSAGE|PIPE_READMODE_MESSAGE|PIPE_WAIT,
               254,
               1024,
               1024,
               5000,
               NULL);
    if(hPipeIn == INVALID_HANDLE_VALUE)
               return 0;
    puts("Input pipe created. Wailting for clients ...");
    ConnectNamedPipe(hPipeIn,NULL);
    puts("Client output connected");

    QString str3("\\\\.\\PIPE\\MYPIPEOUT");
    LPCWSTR str4 = (LPTSTR)str3.utf16();
    HANDLE hPipeOut = CreateNamedPipe(str4,
           PIPE_ACCESS_DUPLEX,
           PIPE_TYPE_MESSAGE|PIPE_READMODE_MESSAGE|PIPE_WAIT,
           254,
           1024,
           1024,
           5000,
           NULL);
    if(hPipeOut == INVALID_HANDLE_VALUE)
           return 0;
    ConnectNamedPipe(hPipeOut,NULL);
    puts("Client input connected");

    PersonDataBase db;
    while(1)
    {
        comands c;
        DWORD readed;
        DWORD retVal = ReadFile(hPipeIn,(void*)&c,sizeof(comands),&readed,NULL);
        if(retVal && readed!=sizeof(comands))
            break;
        if(c == END)
            break;

        switch(c)
        {
            case CLEAR:
                puts("CLEAR");
                db.clear();
            break;
            case LOAD:
            {
                puts("LOAD");
                db.load("asd");
                bool a = true;
                DWORD bytes;
                WriteFile(hPipeOut, (void*) &a, sizeof(bool), &bytes, NULL);
            }
            break;
            case ISMOD:
            {
                puts("ISMODIFIED");
                bool tmp = db.isModifed();
                DWORD written;
                WriteFile(hPipeOut, (void*) &tmp, sizeof(bool), &written, NULL);
            }
            break;
            case SAVE:
            {
                puts("SAVE");
                bool res = db.save("asd");
                DWORD bytes;
                WriteFile(hPipeOut, (void*) &res, sizeof(bool), &bytes, NULL);
            }
            break;
            case COUNT:
            {
                puts("COUNT");
                int tmp = db.count();
                DWORD written;
                WriteFile(hPipeOut, (void *) &tmp, sizeof(int), &written, NULL);
            }
            break;
            case RECORDS:
            {
                puts("RECORDS");
                DWORD written;
                int count = db.count();
                WriteFile(hPipeOut, (void *) &count, sizeof(int), &written, NULL);
                QVector<PersonDataBase::tableStruct> vect = db.records();
                puts(qPrintable(QString("Records send: ") + QString::number(count)));
                for(int i = 0; i < count; ++i)
                {
                    PersonDataBase::tableStruct it = vect[i];
                    WriteFile(hPipeOut, (void*) &it.uId, sizeof(unsigned int), &written, NULL);

                    int tmp = it.fio.size() + 1;
                    WriteFile(hPipeOut, (void*) &tmp, sizeof(int), &written, NULL);
                    WriteFile(hPipeOut, (void*) it.fio.data(), sizeof(QChar)*tmp, &written, NULL);

                    tmp = it.DateOfBirthDay.size() + 1;
                    WriteFile(hPipeOut, (void*) &tmp, sizeof(int), &written, NULL);
                    WriteFile(hPipeOut, (void*) it.DateOfBirthDay.data(), sizeof(QChar)*tmp, &written, NULL);

                    tmp = it.isAlive.size() + 1;
                    WriteFile(hPipeOut, (void*) &tmp, sizeof(int), &written, NULL);
                    WriteFile(hPipeOut, (void*) it.isAlive.data(), sizeof(QChar)*tmp, &written, NULL);

                    tmp = it.Parents.size() + 1;
                    WriteFile(hPipeOut, (void*) &tmp, sizeof(int), &written, NULL);
                    WriteFile(hPipeOut, (void*) it.Parents.data(), sizeof(QChar)*tmp, &written, NULL);
                }
            }
            break;
            case RECORD:
            {
                puts("RECORD");
                DWORD bytes;
                int ID;
                ReadFile(hPipeIn, (void *) &ID, sizeof(unsigned int), &bytes, NULL);
                Person person = db.record(ID);

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
            }
            break;
            case APPEND:
            {
                puts("APPEND");
                DWORD bytes;
                Person tmp;

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

                unsigned int i = db.append(tmp);
                WriteFile(hPipeOut, (void *) &i, sizeof(unsigned int), &bytes, NULL);
            }
            break;
            case REMOVE:
            {
                puts("REMOVE");
                DWORD bytes;
                unsigned int ID;
                ReadFile(hPipeIn, (void*) &ID, sizeof(unsigned int), &bytes, NULL);

                db.remove(ID);
            }
            break;
            case UPDATE:
            {
                puts("UPDATE");

                DWORD bytes;
                Person tmp;

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

                int pos = db.update(tmp);
                WriteFile(hPipeOut, (void *) &pos, sizeof(int), &bytes, NULL);
            }
            break;
            case GET_LIST_OF_POSSIBLE_FATHERS:
            {
                puts("GET_LIST_OF_POSSIBLE_FATHERS");

                DWORD bytes;
                unsigned int ID;
                ReadFile(hPipeIn, (void *) &ID, sizeof(unsigned int), &bytes, NULL);

                QStringList list = db.getListOfPossibleFathers(ID);
                int sizeOfList = list.size();
                WriteFile(hPipeOut, (void *) &sizeOfList, sizeof(sizeOfList), &bytes, NULL);

                for(QString str : list){
                    int tmp = str.size() + 1;
                    WriteFile(hPipeOut, (void*) &tmp, sizeof(int), &bytes, NULL);
                    WriteFile(hPipeOut, (void *) str.data(), sizeof(QChar)*tmp, &bytes, NULL);
                }

            }
            break;
            case GET_LIST_OF_POSSIBLE_MOTHERS:
            {
                puts("GET_LIST_OF_POSSIBLE_MOTHERS");

                DWORD bytes;
                unsigned int ID;
                ReadFile(hPipeIn, (void *) &ID, sizeof(unsigned int), &bytes, NULL);

                QStringList list = db.getListOfPossibleMothers(ID);
                int sizeOfList = list.size();
                WriteFile(hPipeOut, (void *) &sizeOfList, sizeof(sizeOfList), &bytes, NULL);

                for(QString str : list){
                    int tmp = str.size() + 1;
                    WriteFile(hPipeOut, (void*) &tmp, sizeof(int), &bytes, NULL);
                    WriteFile(hPipeOut, (void *) str.data(), sizeof(QChar)*tmp, &bytes, NULL);
                }
            }
            break;
            case GET_UID_BY_FULLNAME:
            {
                puts("GET_UID_BY_FULLNAME");
                DWORD bytes;
                int ttt;
                ReadFile(hPipeIn, (void*) & ttt, sizeof (int), & bytes, NULL);
                QChar * tmp2 =  new QChar[ttt];
                ReadFile(hPipeIn, (void*) tmp2, sizeof (QChar)*ttt, & bytes, NULL);
                QString str = QString(tmp2);
                delete [] tmp2;

                unsigned int uid = db.getUIdByFullName(str);
                WriteFile(hPipeOut, (void*) &uid, sizeof(unsigned int), &bytes, NULL);
            }
            break;
            case REMOVE_FROM_PARENTS:
            {
                puts("REMOVE_FROM_PARENTS");
                DWORD bytes;
                unsigned id;
                ReadFile(hPipeIn, (void *) &id, sizeof(unsigned), &bytes, NULL);

                QList<unsigned int> ids = db.removeFromParents(id);
                int size = ids.size();
                WriteFile(hPipeOut, (void *) &size, sizeof(int), &bytes, NULL);

                for(unsigned int i: ids)
                {
                    WriteFile(hPipeOut, (void *) &i, sizeof(unsigned int), &bytes, NULL);
                }
            }
            break;
            case IS_UNIQUE_FULL_NAME:
            {
                puts("IS_UNIQUE_FULL_NAME");

                DWORD bytes;
                unsigned int id;
                ReadFile(hPipeIn, (void *) &id, sizeof(unsigned int), &bytes, NULL);

                int ttt;
                ReadFile(hPipeIn, (void*) & ttt, sizeof (int), & bytes, NULL);
                QChar * tmp2 =  new QChar[ttt];
                ReadFile(hPipeIn, (void*) tmp2, sizeof (QChar)*ttt, & bytes, NULL);
                QString str = QString(tmp2);
                delete [] tmp2;

                bool res = db.isUniqueFullName(id, str);
                WriteFile(hPipeOut, (void*) &res, sizeof(bool), &bytes, NULL);
            }
            break;
            case GET_LIST_OF_PERSON_CHILD:
            {
                puts("GET_LIST_OF_PERSON_CHILD");

                DWORD bytes;
                unsigned int id;
                ReadFile(hPipeIn, (void *) &id, sizeof(unsigned int), &bytes, NULL);

                QList<unsigned int> ids = db.getListOfPersonChild(id);
                int size = ids.size();
                WriteFile(hPipeOut, (void *) &size, sizeof(int), &bytes, NULL);

                for(unsigned int i: ids)
                {
                    WriteFile(hPipeOut, (void *) &i, sizeof(unsigned int), &bytes, NULL);
                }
            }
            break;
        }

    }
    puts("##### Server finish ######");
    DisconnectNamedPipe(hPipeIn);
    CloseHandle(hPipeIn);
    return a.exec();
}
