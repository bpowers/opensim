// Copyright 2009 Bobby Powers
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject
// to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

#include "lex-tests.h"

class NumbersTest: public LexTest {
protected:
  void validateNumToken(Token *tok, double val,
			uint32_t line, uint32_t pos, uint32_t len) {

    ASSERT_NE((void *)NULL, tok);

    EXPECT_EQ(tok->kind, TokType::Number);
    EXPECT_EQ(tok->tag, Tag::Num);

    EXPECT_DOUBLE_EQ(val, tok->value);

    EXPECT_EQ(line, tok->start.line);
    EXPECT_EQ(tok->start.line, tok->end.line);

    EXPECT_EQ(pos, tok->start.pos);
    EXPECT_EQ(pos + len, tok->end.pos);
  }
};

// XXX: We don't test negative numbers, because that gets lexed as
//      two tokens, a '-' and a number.

TEST_F(NumbersTest, Zero) {

  Token *tok = firstToken("0");

  validateNumToken(tok, 0, 1, 1, 1);

  delete tok;
}


TEST_F(NumbersTest, ZeroPointZero) {

  Token *tok = firstToken("0.0");

  validateNumToken(tok, 0, 1, 1, 3);

  delete tok;
}


TEST_F(NumbersTest, PointZero) {

  Token *tok = firstToken(".0");

  validateNumToken(tok, 0, 1, 1, 2);

  delete tok;
}


TEST_F(NumbersTest, Decimal) {

  Token *tok = firstToken("123.456");

  validateNumToken(tok, 123.456, 1, 1, 7);

  delete tok;
}


TEST_F(NumbersTest, Whole) {

  Token *tok = firstToken("5.0");

  validateNumToken(tok, 5.0, 1, 1, 3);

  delete tok;
}


TEST_F(NumbersTest, LeadingDecimal) {

  Token *tok = firstToken(".123456");

  validateNumToken(tok, 0.123456, 1, 1, 7);

  delete tok;
}
