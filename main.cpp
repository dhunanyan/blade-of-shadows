// author: Davit Hunanyan
// Laboratorium 3 -  Dziedziczenie w C++
#include <iostream>
#include "engine.h"

#define ERR_FIRSTNAME_EXISTS      "You must input your first name!"
#define ERR_SECONDNAME_EXISTS     "You must input your surname!"
#define ERR_BOOKID_EXISTS         "You must input your student book id!"
#define ERR_BOOKID_ONLY_DIGITS    "You student book id should consist only digits, shouldn't it?!"
#define ERR_MAIL_EXISTS           "You must input your mail (preferred AGH mail)!"
#define ERR_MAIL_VALIDATION_AT    "Invalid mail! It should contain '@'!"
#define ERR_MAIL_VALIDATION_DOT   "Invalid mail! It should contain '.'!"

constexpr const char* const FIRSTNAME    = "Davit";
constexpr const char* const SURNAME      = "Hunanyan";
constexpr const char* const MAIL         = "hunanyan@student.agh.edu.pl";
constexpr const char* const BOOK_ID      = "414179";
constexpr const char* const TEACHER_MAIL = "bazior@agh.edu.pl";
void validateStudentsInfo();

int main()
{
  std::cout << "Hello " << FIRSTNAME << " " << SURNAME << "!" << '\n'
            << "make Your teacher: " << TEACHER_MAIL << " proud of you!" << std::endl;
            
  validateStudentsInfo();
}


constexpr inline size_t compileTimeStrlen(const char* text) noexcept
{
  return (*text ? 1+compileTimeStrlen(text+1) : 0);
}

constexpr inline size_t compileTimeCountFirstDigits(const char* text) noexcept
{
  return (*text && ('0'<=*text && *text<='9') ? 1+compileTimeCountFirstDigits(text+1) : 0);
}

constexpr inline bool compileTimeIsDigit(const char* text) noexcept
{
  return compileTimeStrlen(text) == compileTimeCountFirstDigits(text);
}

constexpr inline bool compileTimeContains(const char* text, char letter) noexcept
{
  if ('\n' == *text)
      return false;

  return *text == letter ? 1 : compileTimeContains(text+1, letter);
}

void validateStudentsInfo()
{
  static_assert(compileTimeStrlen(FIRSTNAME) > 2, ERR_FIRSTNAME_EXISTS);
  static_assert(compileTimeStrlen(SURNAME) > 2, ERR_SECONDNAME_EXISTS);
  static_assert(compileTimeStrlen(BOOK_ID) > 1, ERR_BOOKID_EXISTS);
  static_assert(compileTimeIsDigit(BOOK_ID), ERR_BOOKID_ONLY_DIGITS);
  static_assert(compileTimeStrlen(MAIL) > 3, ERR_MAIL_EXISTS);
  static_assert(compileTimeContains(MAIL, '@'), ERR_MAIL_VALIDATION_AT);
  static_assert(compileTimeContains(MAIL, '.'), ERR_MAIL_VALIDATION_DOT);
}

