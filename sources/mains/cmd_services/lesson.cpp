#include <common.h>


class Shape 
{
    protected:
    public:
        Shape (){}
        Shape (float x){}
        virtual float area () = 0;
        virtual ~Shape ()
        {
                cout << "This is shape destructor !!!" << endl;
        }
};

class Circle: public Shape
{
    private:
        int radius;
    public:
        Circle(int r) : Shape (){
            radius = r;
        }
        float area(){
            cout << "This is circle area !!!" << endl;
            return (float) (radius * radius * (22.0/7));
        }
        ~Circle (){
            cout << "This is circle destructor !!!" << endl;

        }

};

class Square: public Shape
{
    private:
        int side;
    public:
        Square(int s):Shape(){ side=s;}
        float area() {
            cout << "This is square area !!!" << endl;
            return (float) (side*side);}
        ~Square ()
        {
            cout << "This is square destructor !!!" << endl;
        }
};


int main (int argc, char ** argv)
{

    if (argc != 3)
    {
        printf ("./lesson 1<square>|2<circle> <int>");
        exit(1);
    }
    float area = 0;
    Shape * s ;

    if (atoi(argv[1]) == 1)
        s = new Circle (atoi (argv[2]));
    else if (atoi(argv[1]) == 2)
        s = new Square (atoi (argv[2]));
    else 
    {
        printf ("./lesson 1<square>|2<circle> <int>");
        exit(1);
    }
    area= s->area();
    cout << "Area: " << area  << endl;
    delete (s);
    return 0;
}