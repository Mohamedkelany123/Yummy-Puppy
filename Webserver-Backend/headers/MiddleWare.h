#ifndef _MIDDLEWARE_H_
#define _MIDDLEWARE_H_


class  MiddleWare
{
    private:
    public:
        MiddleWare();
        virtual bool pre() = 0;
        virtual bool post()=0;
        virtual ~MiddleWare();


};


#endif
