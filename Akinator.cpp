#include <string.h>
#include <assert.h>
#include <math.h>
#include <unistd.h>

#include <kms/Tree.h>
#include <kms/Logs.h>
#include <kms/UTF8Help.h>
#include <kms/GraphDump.h>
#include <kms/List.h>

#include "Akinator.h"

#define DEFINE_TO_STRING_(a) #a
#define DEFINE_TO_STRING(a) DEFINE_TO_STRING_(a)

char* readString (FILE* file)
{
    assert (file);

    char* answ = (char*) calloc (MAX_STR_BUFFER + 1, sizeof (char));
    size_t len = 0;

    ungetc ('\n', file);
    fscanf (file, "%*[ \n]%"DEFINE_TO_STRING(MAX_STR_BUFFER)"[^\n{}]%n", answ, &len);
    answ = (char*) realloc (answ, len + 1);

    return answ;
}

Akinator::Akinator (const char* fileName) :
    fileName_ (fileName),
    tree_ (new Tree::Node (new Tree::Root (0)))
{
    assert (fileName);

    FILE* file = fopen (fileName, "r");
    if (file == nullptr)
    {
        printf ("Я ничего не знаю о вашем мире.\n"
                "Назовите мне какой-нибудь объект\n");

        tree_ -> setData (readString (stdin));

        printf ("Теперь ");
    }
    else
    {
        tree_ -> read (file, Tree::PRINT_MODE::PRE_ORDER, strReader_);

        fclose (file);
    }
}

Akinator::~Akinator ()
{
    assert (fileName_);
    assert (tree_);

    FILE* file = fopen (fileName_, "w");
    if (file == nullptr)
        printf ("File \"%s\" cant be opened for writing", fileName_);
    else
    {
        tree_ -> print (file, Tree::PRINT_MODE::PRE_ORDER, false, strWriter_);

        fclose (file);
    }

    destructTree (tree_);

    tree_ = nullptr;
    fileName_ = nullptr;
}


bool _checkAnswer ()
{
    char ans[MAX_STR_BUFFER + 1] = {};

    scanf ("%"DEFINE_TO_STRING(MAX_STR_BUFFER)"s", ans);

    StrToLower (ans);

    if (strcmp (ans, "да") == 0)
        return true;

    if (strcmp (ans, "yes") == 0)
        return true;

    return false;
}

void Akinator::guess()
{
    Tree::Node* node = tree_;

    printf ("Загадайте кого(что)-нибудь\n");
    sleep (3);
    
    while (node -> getLeftNode() != nullptr || node -> getRightNode() != nullptr)
    {
        printf ("Это %s?\n", (char*) node -> getData());

        if (_checkAnswer())
            node = node -> getRightNode();
        else
            node = node -> getLeftNode();
    }

    printf ("Это %s!\n", (char*) node -> getData());

    if (_checkAnswer() == false)
    {
        printf ("Как? А кто(что) это?\n");

        char* object = readString(stdin);

        node -> setRightNode (new Tree::Node(node -> getRoot(), object));
        node -> setLeftNode (new Tree::Node (node -> getRoot(), node -> getData()));

        printf ("Чем %s отличается от %s?\n%s ", object, node -> getData(), object);

        node -> setData (readString(stdin));
    }
}

bool Akinator::getDefinition ()
{
    //Stack nodeStack = {};
    //StackCtor (&nodeStack, tree_ -> getRoot() -> getSize(), Tree::Node*);
    List* path = ListCtr (tree_ -> getRoot() -> getSize(), sizeof (Tree::Root*));

    printf ("Введите имя объекта: ");
    char* objName = readString(stdin);

    Tree::Node* node = tree_;

    int cmpRes = 0;
    while (node != nullptr && (
          (node -> getLeftNode() != nullptr && node -> getRightNode() != nullptr) || 
           strcmp (objName, (const char*) node -> getData()) != 0))
    {
        if (node -> getRightNode() != nullptr && node -> getLeftNode() != nullptr)
        {
            ListAdd (path, &node);

            node = node -> getRightNode();
        }
        else
        {
            while (path -> size != 0 &&
                   node == ( * (Tree::Node**) ListGet (path, path -> prev[0]) ) -> getLeftNode())
            {
                node = * (Tree::Node**) ListGet (path, path -> prev[0]);

                ListDelete (path, path -> prev[0]);

                if (path -> size == 0)
                {
                    node = nullptr;
                    break;
                }
            }

            if (path -> size != 0)
                node = ( * (Tree::Node**) ListGet (path, path -> prev[0]) ) -> getLeftNode();
        }
    }

    if (path -> size == 0 || node == nullptr)
    {
        printf ("Не знаю о таком объекте\n");

        free (objName);
        ListDtr (path);

        return false;
    }

    printf ("%s: ", objName);

    ListAdd (path, &node);

    while (path -> size > 1)
    {
        if (( * (Tree::Node**) ListGet (path, path -> next[0])) -> getLeftNode() ==
            ( * (Tree::Node**) ListGet (path, path -> next[path -> next[0]])))
        printf ("не ");

        printf ("%s, ", ( * (Tree::Node**) ListGet (path, path -> next[0])) -> getData());

        ListDelete (path, path -> next[0]);
    }

    printf ("\b\b.\n");

    free (objName);
    ListDtr (path);

    return true;
}

void* strReader_ (FILE* file, void* data)
{
    return readString (file);
}

void strWriter_ (FILE* file, const void* data)
{
    fprintf (file, "%s", (const char*) data);
}

void Akinator::destructTree (Tree::Node* tree)
{
    free (tree -> getData ());

    if (tree -> getLeftNode () != nullptr)
    {
        destructTree (tree -> getLeftNode ()); tree -> setLeftNode (nullptr);
    }

    if (tree -> getRightNode () != nullptr)
    {
        destructTree (tree -> getRightNode ());
        tree -> setRightNode (nullptr);
    }

    delete tree;
}

void Akinator::getGraphOfTree (Tree::Node* node)
{
    if (node == nullptr) node = tree_;

    static Graph* graph;
    static GraphNode gnode;
    static GraphEdge edge;
    if (tree_ -> getRoot() -> getRootNode() == node)
    {
        graph = GraphOpen();

        gnode.shape = RECTANGLE_SHAPE;
        gnode.rounded = true;
        gnode.color = 0xffffff;

    }

    gnode.id = node;

    GraphAddNode (graph, &gnode, "%s", node -> getData());

    if (node -> getLeftNode() != nullptr || node -> getRightNode() != nullptr)
        fputc ('?', graph -> file);

    if (node -> getLeftNode())
    {
        getGraphOfTree (node -> getLeftNode());
        GraphAddEdge (graph, &edge, node, "", node -> getLeftNode(), "", 1, "нет");
    }

    if (node -> getRightNode())
    {
        getGraphOfTree (node -> getRightNode());
        GraphAddEdge (graph, &edge, node, "", node -> getRightNode(), "", 1, "да");
    }

    if (tree_ -> getRoot() -> getRootNode() == node)
    {
        GraphDraw (graph, "Akinator tree.svg", "svg");
    }
}


#undef DEFINE_TO_STRING_
#undef DEFINE_TO_STRING(a) DEFINE_TO_STRING_
