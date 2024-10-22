#ifndef %s
#define %s

#include <PSQLAbstractORM.h>
#include <PSQLAbstractQueryIterator.h>
#include <PSQLAbstractORMIterator.h>

namespace %s {
	%s
}

%s

namespace %s {
	class %s : public PSQLAbstractORM
	{
		private:
	%s
		protected:
	%s
		public:
	%s
	};


	class %s : public PSQLAbstractORMIterator <%s>
	{
		private:

		public:
			%s(string _data_source_name, int _partition_number=-1, string _test_data_folder="" ): PSQLAbstractORMIterator <%s> (_data_source_name,"%s", _partition_number, _test_data_folder)
			{

			}

			virtual ~%s(){}

	};
}



#endif