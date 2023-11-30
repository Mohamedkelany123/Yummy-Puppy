import psycopg2


class SQLUtilsService(object):
    @staticmethod
    def dict_fetch_all(cursor):
        """Returns all rows from a cursor as a dict"""
        desc = cursor.description
        return [
            dict(zip([col[0] for col in desc], row))
            for row in cursor.fetchall()
        ]

    @staticmethod
    def connect_to_database(DATABASE_URL):
        try:
            connection = psycopg2.connect(DATABASE_URL)
            return connection
        except psycopg2.Error as e:
            raise AssertionError("Failed to connect to the database")

    @staticmethod
    def execute_query(connection, sql_query):
        try:
            cursor = connection.cursor()
            cursor.execute(sql_query)

            data = SQLUtilsService.dict_fetch_all(cursor)
            return data
        finally:
            if cursor:
                cursor.close()