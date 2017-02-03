#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
using namespace std;
int countWords = 0, wWithPrefix = 0;
int i = 0;
struct Node
{
    int diff;
    string text;
    Node *son[2];
    Node(string t)
    {
        diff = 0;
        text = t;
        son[0] = NULL;
        son[1] = NULL;
    }
};
struct POQ
{
    Node *n;
    POQ *next;
    POQ(Node *nod)
    {
        n = nod;
        next = NULL;
    }
};
void printTree(Node *currentNode, int level)
{
    if (currentNode == NULL || level == 6)
    {
        return;
    }

    for (int i=0; i < level; i++)
    {
        cout << '\t';
    }
    cout << currentNode->text << "(" << currentNode->diff << ") " << '\n';
    if(currentNode->son[1] != NULL)
    {
        printTree(currentNode->son[1], level+1);
    }
    if(currentNode->son[0] != NULL)
    {
        printTree(currentNode->son[0], level+1);
    }
}

Node *findNode(Node *n, string txt)
{
    if (n == NULL) return NULL;
    if (n->text == txt) return n;
    else
    {
        if (n->text < txt)
            return findNode(n->son[1], txt);
        else
            return findNode(n->son[0], txt);
    }
}

Node *singleRotation(Node *A, int dir)
{
    Node *B = A->son[!dir];
    A->son[!dir] = B->son[dir];
    B->son[dir] = A;
    return B;
}

Node *doubleRotation(Node *A, int dir)
{
    Node *B = A->son[!dir];
    Node *C = B->son[dir];
    B->son[dir] = C->son[!dir];
    C->son[!dir] = B;
    A->son[!dir] = C->son[dir];
    C->son[dir] = A;
    return C;
}

void setNewDiffs(Node *A, int dir, int diff)
{
    Node *B = A->son[dir];
    Node *C = B->son[!dir];
    if (C == NULL)
    {
        B->diff = 0;
        A->diff = 0;
        return;
    }
    if (C->diff == 0)
    {
        B->diff = 0;
        A->diff = 0;
    }
    else if (C->diff == diff)
    {
        A->diff = -diff;
        B->diff = 0;
        C->diff = 0;
    }
    else
    {
        A->diff = 0;
        B->diff = diff;
        C->diff = 0;
    }
}
Node *balanceAdd(Node *A, int dir)
{
    Node *B = A->son[dir];
    int difference;
    if (dir == 0) difference = 1;
    else difference = -1;
    if (B->diff == difference)
    {
        A->diff = 0;
        B->diff = 0;
        A = singleRotation(A, !dir);
    }
    else if (B->diff != 0)
    {
        setNewDiffs(A, dir, difference);
        A = doubleRotation(A, !dir);
    }
    return A;
}
Node *addNodeR(Node *n, string txt, bool *added)
{
    if (n == NULL)
    {
        n = new Node(txt);
    }
    else
    {
        int direction = n->text < txt;
        n->son[direction] = addNodeR(n->son[direction], txt, added);
        if (*added == false)
        {
            if (direction == 0)
                n->diff += 1;
            else
                n->diff += -1;
            if (n->diff == 0)
                *added = true;
            else if (n->diff > 1 || n->diff < -1)
            {
                n = balanceAdd(n, direction);
                *added = true;
            }
        }
    }
    return n;
}
Node *addNode(Node *head, string txt)
{
    if (findNode(head, txt) != NULL) return head;
    countWords++;
    bool added = false;
    head = addNodeR(head, txt, &added);
    return head;
}

Node *balanceRM(Node *A, int dir, bool *done)
{
    Node *n = A->son[!dir];
    if (n == NULL) return NULL;
    int difference;
    if (dir == 0) difference = 1;
    else difference = -1;
    if (n->diff == -difference)
    {
        A->diff = 0;
        n->diff = 0;
        A = singleRotation(A, dir);
    }
    else if (n->diff == difference)
    {
        setNewDiffs(A, !dir, -difference);
        A = doubleRotation(A, dir);
    }
    else
    {
        A->diff = -difference;
        n->diff = difference;
        A = singleRotation(A, dir);
        *done = true;
    }
    return A;
}
Node *removeNodeR(Node *A, string txt, bool *done)
{
    if (A != NULL)
    {
        int direction;
        if (A->text == txt)
        {
            if (A->son[0] == NULL || A->son[1] == NULL)
            {
                Node *hold;
                if (A->son[0] == NULL) direction = 1;
                else direction = 0;
                hold = A->son[direction];
                delete(A);
                return hold;
            }
            else
            {
                Node *replacementNode = A->son[1];
                while (replacementNode->son[0] != NULL)
                    replacementNode = replacementNode->son[0];
                A->text = replacementNode->text;
                txt = replacementNode->text;
            }
        }
        direction = A->text < txt;
        A->son[direction] = removeNodeR(A->son[direction], txt, done);
        if (*done == false)
        {
            int difference;
            if (direction == 0)
                difference = -1;
            else
                difference = 1;
            A->diff += difference;

            if (A->diff == 1 || A->diff == -1)
            {
                *done = true;
            }
            else if (A->diff > 1 || A->diff < -1)
            {
                A = balanceRM(A, direction, done);
            }
        }
    }
  return A;
}
Node *removeNode(Node *head, string txt)
{
    if (findNode(head, txt) == NULL)
        return head;
    countWords--;
    bool done = false;
    head = removeNodeR(head, txt, &done);
    return head;
}
void pushEl(POQ *start, Node *n)
{
    POQ *tmp = start;
    while (tmp->next != NULL)
    {
        tmp = tmp->next;
    }
    if (n == NULL)
        tmp->next = NULL;
    else
        tmp->next = new POQ(n);
}
void saveToFile(Node *head, ofstream &file)
{
    POQ *start = new POQ(head);
    while (start != NULL)
    {
        if (start->n->son[0] != NULL )
            pushEl(start, start->n->son[0]);
        if (start->n->son[1] != NULL)
            pushEl(start, start->n->son[1]);
        if (start->n->son[0]== NULL && start->n->son[1] == NULL )
        {
            pushEl(start, NULL);

        }
        file << start->n->text << endl;
        POQ *tmp = start->next;
        delete start;
        start = tmp;
    }
}

Node *readFromFile(Node *head)
{
    ifstream inFile;
    inFile.open("inTree.txt");
    int j;
    inFile >> j;
    for (int i=0; i<j; i++)
    {
        string text;
        inFile >> text;
        head = addNode(head, text);
    }
    return head;
}
Node *findNodeWithPrefix(Node *n, string prefix)
{
    if (n == NULL) return NULL;
    if (n->text.substr(0, prefix.length()) == prefix) return n;
    else
    {
        if (n->text < prefix)
            return findNodeWithPrefix(n->son[1], prefix);
        else
            return findNodeWithPrefix(n->son[0], prefix);
    }
}
void findWordsWithPrefix(Node *N, string prefix)
{
    if (N == NULL) return;
    if (N->text.substr(0, prefix.length()) == prefix)
    {
        wWithPrefix++;
    }
    findWordsWithPrefix(N->son[0], prefix);
    findWordsWithPrefix(N->son[1], prefix);
}


void findNext(Node *head, string txt, Node *next)
{
    if (head == NULL || head->text > next->text) return;
    if (head->text > txt && head->text < next->text)
        next->text = head->text;
    findNext(head->son[0], txt, next);
    findNext(head->son[1], txt, next);
}
void printSort(Node *head)
{
    if (head == NULL) return;
    printSort(head->son[0]);
    cout << head->text << endl;
    printSort(head->son[1]);
}
void printMenu()
{
    cout << "1 aby dodac slowo" << endl;
    cout << "2 aby wypisac drzewo" << endl;
    cout << "3 aby usunac slowo" << endl;
    cout << "4 aby zapisac slownik do pliku" << endl;
    cout << "5 aby wczytac slownik z pliu" << endl;
    cout << "6 aby znalezc liczbe slow o prefiksie" << endl;
    cout << "7 aby znalezc nastepne wieksze slowo" << endl;
    cout << "8 aby wyszukac slowo w slowniku" << endl;
    cout << "9 aby wczytac skrypt" << endl;
    cout << "0 aby wyjsc" << endl;
}
int main()
{
    Node *head = NULL;
    string command, word;
    ifstream in;
    ofstream out;
    ofstream outTree;
    in.open("duzy3.txt");
    out.open("out.txt");
    outTree.open("outTree.txt");
    int numOfOps = 0;
    in >> numOfOps;
    while(1)
    {
    printMenu();
    int cmd;
    cin >> cmd;
    switch(cmd)
    {
    case 1:
        {
        system("cls");
        cout << "Podaj slowo do dodania: " << endl;
        cin >> word;
        head = addNode(head, word);
        break;
        }
    case 2:
        {
        system("cls");
        printTree(head, 0);
        break;
        }
    case 3:
        {
        system("cls");
        cout << "Podaj slowo do usuniecia" << endl;
        cin >> word;
        head = removeNode(head, word);
        break;
        }
    case 4:
        {
        system("cls");
        saveToFile(head, outTree);
        break;
        }
    case 5:
        {
        system("cls");
        if (head != NULL)
        {
            cout << "Istnieje juz jakies drzewo, nie mozna wczytac!" << endl;
        }
        else
            readFromFile(head);
        break;
        }
    case 6:
        {
        system("cls");
        cout << "Podaj prefiks " << endl;
        cin >> word;
        wWithPrefix = 0;
            findWordsWithPrefix(head, word);
            cout << "istnieje " << wWithPrefix << " slow o zadanym prefiksie" << endl;
        break;
        }
    case 7:
        {
        system("cls");
        cout << "podaj slowo: " << endl;
        cin >> word;
        if (head->text < word && head->son[1] == NULL) cout << "nie ma takiego slowa" << endl;
        else if (head->text > word && head->son[1] == NULL) cout << head->text << endl;
        else if (head->son[1]->son[0] == NULL) cout << head->son[1]->text << endl;
        else
        {
            Node *accumulator = new Node(head->son[1]->son[0]->text);
            findNext(head, word, accumulator);
            cout << accumulator->text << endl;
        }
        break;
        }
    case 8:
        {
        system("cls");
        cout << "podaj slowo: " << endl;
        cin >> word;
        Node *tmp = findNode(head, word);
        if (tmp == NULL) cout << "Nie istnieje takie slowo" << endl;
        else cout << "istnieje takie slowo " << endl;
        break;
        }
    case 9:
        {
        for (i=0; i<numOfOps; i++)
        {
            in >> command;
            if (command == "ADD")
            {
                string txt;
                in >> txt;
                head = addNode(head, txt);

            }
            else if(command == "DEL")
            {
                string txt;
                in >> txt;
                head = removeNode(head, txt);
            }
            else if(command == "OP")
            {
                string prefix;
                wWithPrefix = 0;
                in >> prefix;
                Node *startSearch = findNodeWithPrefix(head, prefix);
                if (startSearch != NULL)
                {
                    findWordsWithPrefix(startSearch, prefix);
                }
                out << wWithPrefix << endl;
            }
            else if(command == "FIN")
            {
                string txt;
                in >> txt;
                Node *searchN = findNode(head, txt);
                if (searchN !=NULL) out << "TAK" << endl;
                else out <<"NIE" << endl;
            }
        }
        break;
        }
    case 0:
        {
            exit(0);
        }
    }
    system("PAUSE");
    system("cls");
    }
    return 0;
}
