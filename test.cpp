#include <iostream>
#include "src/json_lite.h"

using namespace std;
using namespace json_lite;

void test(string);
string get_filename(string);
void test_locate_label();
void test_get_by_label();

int main(int argc, char** argv)
{
    //get_child_by_label
    test_get_by_label();
    system("pause");
    return 0;


    //locate_element_by_label
    test_locate_label();
    cout << endl;

    // supposed to be passed 
    try
    {
        test("tests\\pass1.json");
        test("tests\\pass2.json");
        test("tests\\pass3.json");
    }
    catch (const char* errmsg)
    {
        cout << errmsg << endl;
    }
    
    // supposed to be failed 
    char temp[20];
    for (int i = 1; i <= 33; i++)
    {
        memset(temp, 0, 20);
        sprintf(temp, "tests\\fail%d.json", i);
        try
        {
            test(temp);
        }
        catch (const char* errmsg)
        {
            cout << errmsg << endl;
        }
    }

    system("pause");
    return 0;
}

void test(string input)
{
    cout << "parsing " << input << " :" << endl;
    string output = "output\\";
    json_parser parser(input);
    json_value *doc = parser.run();
    if (doc)
    {
        //doc->output(output + get_filename(input));
        ofstream fout((output + get_filename(input)).c_str());
        fout << *doc;
        fout.close();
        delete doc;
        doc = NULL;
        cout << "successful~\n" << endl;
        return;
    }
    cout << endl;
}

string get_filename(string path)
{
    string::size_type pos = path.rfind('\\');
    return string(path, pos + 1, string::npos);
}


void test_locate_label()
{
    json_parser parser("tests\\pass1.json");
    parser.locate_element_by_label("integer");

    cout << "The next 10 characters: ";
    int i = 10;
    while (i--)
        cout << parser.get_char();
    cout << endl;
}


void test_get_by_label()
{
    json_parser parser("tests\\pass1.json");
    json_value *doc = parser.run();
    if (doc)
    {
        json_value* elem = doc->get_child_by_label("object with 1 member");
        if (elem != NULL)
            elem->output();
        delete doc;
    }
    cout << endl;
}