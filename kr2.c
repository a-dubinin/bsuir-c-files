/**
 * Программа обработки результатов экзамена студентов.
 * Разработчик - Дубинин А. В (http://dubinin.net).
 * 26.11.2016
 */

// Подключение файлов из стандартной библиотеки
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Объявление символических констант
#define MAX_ROW_NUMBER   32
#define MAX_STR_LENGTH   80
#define MARK_COMPARE     5
#define COLUMN_SEPARATOR ";"
#define ROW_LIST_PATTERN "%s%s%d\n"
#define FILE_LIST_MAIN   "list_main.csv"
#define FILE_LIST_MED    "list_mediocre.csv"

#define MSG_FILE_ERROR     "Ошибка! Невозможно открыть файл \"%s\"\n"
#define MSG_FILE_EMPTY     "Файл \"%s\" пуст\n"
#define MSG_ALL_SUCCESSFUL "У всех студентов оценка более %d баллов\n"
#define MSG_ALL_MEDIOCRE   "У всех студентов оценка менее %d баллов\n"
#define MSG_LIST_MAIN      "Список студентов с оценкой более %d баллов: %s\n"
#define MSG_LIST_MED       "Список студентов с оценкой менее %d баллов: %s\n"
#define MSG_APP_EXIT       "Для выхода из программы нажмите любую клавишу"

// Объявление новой структуры и имени типа - record
typedef struct {
    char student[MAX_STR_LENGTH - 3]; // фамилия студента
    int mark;                         // оценка
} record;

/*
 * Прототипы функций displayExitDialog, writeRecord
 */
void displayExitDialog();
void writeRecord(FILE *, record);

void main()
{
    setlocale(LC_ALL, "");            // установка локализации
    int i, listLength, mediocreCount; // объявление переменных типа int
    char rowStr[MAX_STR_LENGTH];      // объявление переменных для строк
    char *buffer;
    FILE *listMain, *listMed;         // объявление файловых переменных

    // Объявление переменной list - массива структур типа record
    record list[MAX_ROW_NUMBER];

    /*
     * Открытие файла с результатами экзамена студентов
     * с проверкой на существование файла
     */
    if ((listMain = fopen(FILE_LIST_MAIN, "rt")) == NULL) {
        printf(MSG_FILE_ERROR, FILE_LIST_MAIN);
        displayExitDialog();
        return;
    }

    // Инициализация счетчиков кол-ва записей в файле и 'троечников'
    i = 0;
    mediocreCount = 0;

    /*
     * Построчное чтение файла
     */
    while (fgets(rowStr, MAX_STR_LENGTH, listMain)) {
        /*
         * Разбиение строки из файла разделителем ";" с записью
         * данных в поля структуры record массива list
         */
        buffer = strtok(rowStr, COLUMN_SEPARATOR);
        strcpy(list[i].student, buffer); // фамилия в поле student
        buffer = strtok(NULL, COLUMN_SEPARATOR);
        list[i].mark = atoi(buffer);     // оценка в поле mark

        // Увеличение счетчиков
        if (list[i].mark < MARK_COMPARE) {
            mediocreCount++;
        }
        i++;
    }
    listLength = i;   // количество записей в файле
    fclose(listMain); // закрытие файлового потока

    /*
     * Анализ кол-ва записей в файле и кол-ва 'троечников':
     *
     * 1. Если записей нет, вывод соответствующего сообщения.
     * 2. Если в файле нет 'троечников', вывод соответствующего сообщения.
     * 3. Если результаты экзамена всех студентов ниже желаемого,
     *    вывод соответствующего сообщения. При этом исходный файл
     *    переименовывается в файл результатов 'троечников' и создается
     *    новый исходный файл, не содержащий записей.
     */
    if (listLength == 0) {
        printf(MSG_FILE_EMPTY, FILE_LIST_MAIN);
        displayExitDialog();
        return;
    } else if (mediocreCount == 0) {
        printf(MSG_ALL_SUCCESSFUL, MARK_COMPARE);
        displayExitDialog();
        return;
    } else if (mediocreCount == listLength) {
        rename(FILE_LIST_MAIN, FILE_LIST_MED);
        listMain = fopen(FILE_LIST_MAIN, "w+t");
        fclose(listMain);
        printf(MSG_ALL_MEDIOCRE, MARK_COMPARE);
        displayExitDialog();
        return;
    }

    // Открытие файлов для записи результатов
    listMain = fopen(FILE_LIST_MAIN, "wt");
    listMed = fopen(FILE_LIST_MED, "wt");

    /*
     * Проход по массиву list с занесением записей о результатах
     * экзамена студентов в соответствующие файлы.
     */
    for (i = 0; i < listLength; i++) {
        if (list[i].mark < MARK_COMPARE) {
            writeRecord(listMed, list[i]);
        } else {
            writeRecord(listMain, list[i]);
        }
    }

    // Закрытие файлов
    fclose(listMain);
    fclose(listMed);

    // Вывод на экран сообщения с результирующими файлами
    printf(MSG_LIST_MAIN, MARK_COMPARE, FILE_LIST_MAIN);
    printf(MSG_LIST_MED, MARK_COMPARE, FILE_LIST_MED);

    displayExitDialog();
    return;
}

/*
 * Функция вывода на экран диалога выхода из программы.
 */
void displayExitDialog()
{
    printf(MSG_APP_EXIT);
    getchar();
    return;
}

/*
 * Функция записи в файл строки ('фамилия;оценка\n').
 * list - параметр файлового типа
 * listRow - параметр типа record
 */
void writeRecord(FILE *list, record listRow)
{
    fprintf(
        list,
        ROW_LIST_PATTERN,
        listRow.student,
        COLUMN_SEPARATOR,
        listRow.mark
    );
    return;
}
