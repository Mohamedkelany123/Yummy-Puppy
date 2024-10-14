# Coding Conventions:


# Directory Structure
- **Common**
- **Factory**
    - **ORMS**
- **Microservice apps**
    - **Sources**
    - **Headers**
    - **Serializers**
    - **Exceptions**
    - **Tests**
    - **TestGenerators**
    - **TestData**
    - **Readme** For each Directory


# Naming Conventions
- **Files**
    - Any **header File** containing implementation should be saved as **.hpp**
    - Any **header File** that doesnt contain any implementation should be saved as **.h**
- **Variables**
    - **snake_case** should be used for any **variable's** name, ex: customer_name
    - Start with an **_**  for **parameters** in any method, ex: _customer_name 
    - Start with a **m_**  for **Class Data members**, ex: m_customer_name 
- **Functions**
    - **Constructors** should take the exact format of the class name, ex: BlnkCustomer::BlnkCustomer(){}
    - All functions-other than the constructor-should be **camelCase**, ex: getCustomerLoans(){}
    - Any function that gets something from the DB should start with get+what it gets+ how it gets it, ex: getCustomerByPhoneNumber(string _phone_number){}
- **Classes & Objects**
    - **PascalCase** should be used for any **class** name, ex: class BlnkCustomer{}
    - **camelCase** should be used for any **object** name, ex: BlnkCustomer blnkCustomer();
- **Enums**
    - **PascalCase** shoud be used for enum names, ex: enum ClosureStatus{...}
    - **CAPITAL_SNAKE_CASE** should be used for enum values, ex: enum ClosureStatus{ UNDUE_TO_DUE }
- **HTTP Responses**
    - status_code (should never be 200 in case of an error)
    - payload (a JSON that holds all the data of the response, do not send independent variables in the response)
    - message (a message specifying the status of the request or describing the problem that occurred in case of an error, Human-Readable)
- All **Macros** are capital
- Each **Application** needs to have a **namespace**

# General Guidelines:
- Break down complex functions to smaller modular and reusable functions.
- Always search for previously created functions that do the same as the function you're going to create to avoid redundancy.
    - If a function exists with similar functionality but has extra functionalities related to it, break the reusable part down and use it across both functions. 
- Never throw an exception in an endpoint and not catch it, as this can lead to security hazards and inconsistent responses.
- Use custom exceptions if needed and try to make the response status code always correspond with the error that has happened.
- Messages sent in the response should be user friendly and understandable and shouldn't contain any code related info, these are not error logs.
- Use proper and consistent indentation of one TAB to keep the code clean and readable.

