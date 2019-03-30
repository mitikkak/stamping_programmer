#include <gtest/gtest.h>
#include "Parser.h"

using namespace stamping;

class TestParser : public ::testing::Test {
public:

    TestParser()
    : actuators{},
      a1{1},
      a2{2},
      a3{3},
      a4{4}
    {

    }
    void SetUp(){
        actuators.add(a1);
        actuators.add(a2);
        actuators.add(a3);
        actuators.add(a4);
    }

    void TearDown() {

    }
protected:
    Actuators actuators;
    Actuator a1;
    Actuator a2;
    Actuator a3;
    Actuator a4;
};

TEST_F(TestParser, notOkInitially)
{
    Parser p;
    EXPECT_EQ(false, p.ok());
}
TEST_F(TestParser, simpleOkProgram)
{
    Parser p;
    Program prg(actuators);
    std::string const prgStr = std::string(header) + std::string("[1:1]") + std::string(tail);
    p.parse(prgStr, prg);
    EXPECT_EQ(true, p.ok());
    EXPECT_EQ(1, prg.length());
}
TEST_F(TestParser, complexOkProgram)
{
    Parser p;
    Program prg(actuators);
    std::string const prgStr = std::string(header) + std::string("[1:1][2:4534][3:5555][1:4343][2:3333][2:435344][1:435344][2:435344][0:435344][3:435344]") + std::string(tail);
    p.parse(prgStr, prg);
    EXPECT_EQ(true, p.ok());
    EXPECT_EQ(10, prg.length());
}

