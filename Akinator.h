#ifndef AKINATOR_HEADER
#define AKINATOR_HEADER

#include <stdio.h>

#include <kms/Logs.h>
#include <kms/Tree.h>
#include <kms/GraphDump.h>

class Akinator
{
    private:

        Tree::Node* tree_;
        const char* fileName_;

    public:

        explicit    Akinator        (const char* fileName);
                   ~Akinator        ();

        void    guess               ();

        Tree::Node* getTree         ();
        void        getGraphOfTree  (Tree::Node* node = nullptr);


    private:

        void    destructTree(Tree::Node* tree);
};

#define MAX_STR_BUFFER 1000

void*   strReader_          (FILE* file, void* data);
void    strWriter_          (FILE* file, const void* data);


#endif
