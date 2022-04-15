#include <stdio.h>
#include <string.h>

#include <kms/UTF8Help.h>

#include "Akinator.h"


int main ()
{
    Akinator Akinator ("AkinatorData.txt");


    printf ("Привет. Я сверхсовременный искуственный интелект.\n"
            "Я могу:\n"
            "Угадать объект (введите \"угадать\")\n"
            "Дать определение объекту (введите \"определение\")\n");

    while (true)
    { 
        char* str = readString (stdin);
        StrToLower (str);

        if (strcmp (str, "угадать") == 0 ||
            strcmp (str, "guess")   == 0)
                Akinator.guess();

        else if (strcmp (str, "опред")          == 0 ||
                 strcmp (str, "def")            == 0 ||
                 strcmp (str, "определение")    == 0 ||
                 strcmp (str, "definition")     == 0)
            Akinator.getDefinition();

        else if (strcmp (str, "выход")          == 0 ||
                 strcmp (str, "exit")           == 0)
        {
            free (str);
            Akinator.getGraphOfTree();
            return 0;
        }

        else
            printf ("Я не понимаю что вы хотите, попробуйте еще раз\n");

        fputc ('\n', stdout);

        free (str);
    }



    return 0;
}
