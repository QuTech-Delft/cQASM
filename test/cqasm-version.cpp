#include "cqasm-version.hpp"
#include "cqasm-error.hpp"

#include <cstdio>
#include <gmock/gmock.h>
#include <memory>
#include <string>

using namespace ::testing;
using namespace cqasm::error;
using namespace cqasm::version;


TEST(parse_file, filename_does_not_exist) {
    EXPECT_THAT([&]() { parse_file("res/cqasm_version/does_not_exist.cq"); },
        ThrowsMessage<ParseError>(HasSubstr(
            "Error: parse_file failed to open input file 'res/cqasm_version/does_not_exist.cq': "
            "No such file or directory.")));
}
TEST(parse_file, filename_empty) {
    EXPECT_THAT([&]() { parse_file("res/cqasm_version/empty.cq"); },
        Throws<ParseError>(Property(&ParseError::what,
            HasSubstr("Error at res/cqasm_version/empty.cq:1:1: syntax error"))));
}
TEST(parse_file, filename_version_no_number) {
    EXPECT_THAT([&]() { parse_file("res/cqasm_version/version_no_number.cq"); },
        Throws<ParseError>(Property(&ParseError::what,
            HasSubstr("Error at res/cqasm_version/version_no_number.cq:2:1: syntax error"))));
}
TEST(parse_file, filename_version_abc) {
    EXPECT_THAT([&]() { parse_file("res/cqasm_version/version_abc.cq"); },
        Throws<ParseError>(Property(&ParseError::what,
            HasSubstr("Error at res/cqasm_version/version_abc.cq:1:9..10: syntax error"))));
}
TEST(parse_file, filename_version_1_abc) {
    EXPECT_THAT([&]() { parse_file("res/cqasm_version/version_1_abc.cq"); },
        Throws<ParseError>(Property(&ParseError::what,
            HasSubstr("Error at res/cqasm_version/version_1_abc.cq:1:11..12: syntax error"))));
}
TEST(parse_file, filename_version_1_1_abc) {
    EXPECT_THAT([&]() { parse_file("res/cqasm_version/version_1_1_abc.cq"); },
        Throws<ParseError>(Property(&ParseError::what,
            HasSubstr("Error at res/cqasm_version/version_1_1_abc.cq:1:13..14: syntax error"))));
}
TEST(parse_file, filename_version_1_0) {
    EXPECT_EQ(parse_file("res/cqasm_version/version_1_0.cq"), Version{ "1.0" });
}
TEST(parse_file, filename_version_1_1) {
    EXPECT_EQ(parse_file("res/cqasm_version/version_1_1.cq"), Version{ "1.1" });
}
TEST(parse_file, filename_version_1_1_1) {
    EXPECT_EQ(parse_file("res/cqasm_version/version_1_1_1.cq"), Version{ "1.1.1" });
}
TEST(parse_file, filename_version_1_2) {
    EXPECT_EQ(parse_file("res/cqasm_version/version_1_2.cq"), Version{ "1.2" });
}
TEST(parse_file, filename_version_1_3) {
    EXPECT_EQ(parse_file("res/cqasm_version/version_1_3.cq"), Version{ "1.3" });
}
TEST(parse_file, filename_version_2_0) {
    EXPECT_EQ(parse_file("res/cqasm_version/version_2_0.cq"), Version{ "2.0" });
}
TEST(parse_file, filename_version_3_0) {
    EXPECT_EQ(parse_file("res/cqasm_version/version_3_0.cq"), Version{ "3.0" });
}
TEST(parse_file, filename_version_4_0) {
    EXPECT_EQ(parse_file("res/cqasm_version/version_4_0.cq"), Version{ "4.0" });
}
TEST(parse_file, instruction_called_version) {
    EXPECT_THAT([&]() { parse_file("res/cqasm_version/instruction_called_version.cq"); },
        Throws<ParseError>(Property(&ParseError::what,
            HasSubstr("Error at res/cqasm_version/instruction_called_version.cq:1:9..10: syntax error"))));
}
TEST(parse_file, no_version_and_instruction) {
    EXPECT_THAT([&]() { parse_file("res/cqasm_version/no_version_and_instruction.cq"); },
        Throws<ParseError>(Property(&ParseError::what,
            HasSubstr("Error at res/cqasm_version/no_version_and_instruction.cq:1:1..2: syntax error"))));
}
TEST(parse_file, version_1_0_and_instruction) {
    EXPECT_EQ(parse_file("res/cqasm_version/version_1_0_and_instruction.cq"), Version{ "1.0" });
}
TEST(parse_file, instruction_and_version_1_0) {
    EXPECT_THAT([&]() { parse_file("res/cqasm_version/instruction_and_version_1_0.cq"); },
        Throws<ParseError>(Property(&ParseError::what,
            HasSubstr("Error at res/cqasm_version/instruction_and_version_1_0.cq:1:1..2: syntax error"))));
}


TEST(parse_file, fp_does_not_exist) {
    const char *filename{ "res/cqasm_version/does_not_exist.cq" };
    FILE *fp{ fopen(filename, "r") };
    EXPECT_THAT([&]() { parse_file(fp, filename); },
        ThrowsMessage<ParseError>(HasSubstr("Error: ScannerFlexBisonFile couldn't access file.")));
}
TEST(parse_file, fp_empty) {
    const char *filename{ "res/cqasm_version/empty.cq" };
    FILE *fp{ fopen(filename, "r") };
    EXPECT_THAT([&]() { parse_file(fp, filename); },
        Throws<ParseError>(Property(&ParseError::what,
            HasSubstr("Error at res/cqasm_version/empty.cq:1:1: syntax error"))));
}
TEST(parse_file, fp_version_no_number) {
    const char *filename{ "res/cqasm_version/version_no_number.cq" };
    FILE *fp{ fopen(filename, "r") };
    EXPECT_THAT([&]() { parse_file(fp, filename); },
        Throws<ParseError>(Property(&ParseError::what,
            HasSubstr("Error at res/cqasm_version/version_no_number.cq:2:1: syntax error"))));
}
TEST(parse_file, fp_version_abc) {
    const char *filename{ "res/cqasm_version/version_abc.cq" };
    FILE *fp{ fopen(filename, "r") };
    EXPECT_THAT([&]() { parse_file(fp, filename); },
        Throws<ParseError>(Property(&ParseError::what,
            HasSubstr("Error at res/cqasm_version/version_abc.cq:1:9..10: syntax error"))));
}
TEST(parse_file, fp_version_1_abc) {
    const char *filename{ "res/cqasm_version/version_1_abc.cq" };
    FILE *fp{ fopen(filename, "r") };
    EXPECT_THAT([&]() { parse_file(fp, filename); },
        Throws<ParseError>(Property(&ParseError::what,
            HasSubstr("Error at res/cqasm_version/version_1_abc.cq:1:11..12: syntax error"))));
}
TEST(parse_file, fp_version_1_1_abc) {
    const char *filename{ "res/cqasm_version/version_1_1_abc.cq" };
    FILE *fp{ fopen(filename, "r") };
    EXPECT_THAT([&]() { parse_file(fp, filename); },
        Throws<ParseError>(Property(&ParseError::what,
            HasSubstr("Error at res/cqasm_version/version_1_1_abc.cq:1:13..14: syntax error"))));
}
TEST(parse_file, fp_version_1_0) {
    const char *filename{ "res/cqasm_version/version_1_0.cq" };
    FILE *fp{ fopen(filename, "r") };
    EXPECT_EQ(parse_file(fp, filename), Version{ "1.0" });
}
TEST(parse_file, fp_version_1_1) {
    const char *filename{ "res/cqasm_version/version_1_1.cq" };
    FILE *fp{ fopen(filename, "r") };
    EXPECT_EQ(parse_file(fp, filename), Version{ "1.1" });
}
TEST(parse_file, fp_version_1_1_1) {
    const char *filename{ "res/cqasm_version/version_1_1_1.cq" };
    FILE *fp{ fopen(filename, "r") };
    EXPECT_EQ(parse_file(fp, filename), Version{ "1.1.1" });
}
TEST(parse_file, fp_version_1_2) {
    const char *filename{ "res/cqasm_version/version_1_2.cq" };
    FILE *fp{ fopen(filename, "r") };
    EXPECT_EQ(parse_file(fp, filename), Version{ "1.2" });
}
TEST(parse_file, fp_version_1_3) {
    const char *filename{ "res/cqasm_version/version_1_3.cq" };
    FILE *fp{ fopen(filename, "r") };
    EXPECT_EQ(parse_file(fp, filename), Version{ "1.3" });
}
TEST(parse_file, fp_version_2_0) {
    const char *filename{ "res/cqasm_version/version_2_0.cq" };
    FILE *fp{ fopen(filename, "r") };
    EXPECT_EQ(parse_file(fp, filename), Version{ "2.0" });
}
TEST(parse_file, fp_version_3_0) {
    const char *filename{ "res/cqasm_version/version_3_0.cq" };
    FILE *fp{ fopen(filename, "r") };
    EXPECT_EQ(parse_file(fp, filename), Version{ "3.0" });
}
TEST(parse_file, fp_version_4_0) {
    const char *filename{ "res/cqasm_version/version_4_0.cq" };
    FILE *fp{ fopen(filename, "r") };
    EXPECT_EQ(parse_file(fp, filename), Version{ "4.0" });
}
TEST(parse_file, fp_instruction_called_version) {
    const char *filename{ "res/cqasm_version/instruction_called_version.cq" };
    FILE *fp{ fopen(filename, "r") };
    EXPECT_THAT([&]() { parse_file(fp, filename); },
        Throws<ParseError>(Property(&ParseError::what,
            HasSubstr("Error at res/cqasm_version/instruction_called_version.cq:1:9..10: syntax error"))));
}
TEST(parse_file, fp_no_version_and_instruction) {
    const char *filename{ "res/cqasm_version/no_version_and_instruction.cq" };
    FILE *fp{ fopen(filename, "r") };
    EXPECT_THAT([&]() { parse_file(fp, filename); },
        Throws<ParseError>(Property(&ParseError::what,
            HasSubstr("Error at res/cqasm_version/no_version_and_instruction.cq:1:1..2: syntax error"))));
}
TEST(parse_file, fp_version_1_0_and_instruction) {
    const char *filename{ "res/cqasm_version/version_1_0_and_instruction.cq" };
    FILE *fp{ fopen(filename, "r") };
    EXPECT_EQ(parse_file(fp, filename), Version{ "1.0" });
}
TEST(parse_file, fp_instruction_and_version_1_0) {
    const char *filename{ "res/cqasm_version/instruction_and_version_1_0.cq" };
    FILE *fp{ fopen(filename, "r") };
    EXPECT_THAT([&]() { parse_file(fp, filename); },
        Throws<ParseError>(Property(&ParseError::what,
            HasSubstr("Error at res/cqasm_version/instruction_and_version_1_0.cq:1:1..2: syntax error"))));
}
TEST(parse_file, fp_version_1_0_and_no_filename_argument) {
    FILE *fp{ fopen("res/cqasm_version/version_1_0.cq", "r") };
    EXPECT_EQ(parse_file(fp), Version{ "1.0" });
}
TEST(parse_file, fp_empty_and_no_filename_argument) {
    FILE *fp{ fopen("res/cqasm_version/empty.cq", "r") };
    EXPECT_THAT([&]() { parse_file(fp); },
        Throws<ParseError>(Property(&ParseError::what,
            HasSubstr("Error at <unknown>:1:1: syntax error"))));
}


TEST(parse_string, string_empty) {
    const std::string filename{ "res/cqasm_version/empty.cq" };
    EXPECT_THAT([&]() { parse_string("", filename); },
        ThrowsMessage<ParseError>(HasSubstr("Error at res/cqasm_version/empty.cq:1:1: syntax error")));
}
TEST(parse_string, string_version_no_number) {
    const std::string filename{ "res/cqasm_version/version_no_number.cq" };
    EXPECT_THAT([&]() { parse_string("version", filename); },
        ThrowsMessage<ParseError>(HasSubstr("Error at res/cqasm_version/version_no_number.cq:1:8: syntax error")));
}
TEST(parse_string, string_version_abc) {
    const std::string filename{ "res/cqasm_version/version_abc.cq" };
    EXPECT_THAT([&]() { parse_string("version abc", filename); },
        ThrowsMessage<ParseError>(HasSubstr("Error at res/cqasm_version/version_abc.cq:1:9..10: syntax error")));
}
TEST(parse_string, string_version_1_abc) {
    const std::string filename{ "res/cqasm_version/version_1_abc.cq" };
    EXPECT_THAT([&]() { parse_string("version 1.abc", filename); },
        ThrowsMessage<ParseError>(HasSubstr("Error at res/cqasm_version/version_1_abc.cq:1:11..12: syntax error")));
}
TEST(parse_string, string_version_1_1_abc) {
    const std::string filename{ "res/cqasm_version/version_1_1_abc.cq" };
    EXPECT_THAT([&]() { parse_string("version 1.1.abc", filename); },
        ThrowsMessage<ParseError>(HasSubstr("Error at res/cqasm_version/version_1_1_abc.cq:1:13..14: syntax error")));
}
TEST(parse_string, string_version_1_0) {
    const std::string filename{ "res/cqasm_version/version_1_0.cq" };
    EXPECT_EQ(parse_string("version 1.0", filename), Version{ "1.0" });
}
TEST(parse_string, string_version_1_1) {
    const std::string filename{ "res/cqasm_version/version_1_1.cq" };
    EXPECT_EQ(parse_string("version 1.1", filename), Version{ "1.1" });
}
TEST(parse_string, string_version_1_1_1) {
    const std::string filename{ "res/cqasm_version/version_1_1_1.cq" };
    EXPECT_EQ(parse_string("version 1.1.1", filename), Version{ "1.1.1" });
}
TEST(parse_string, string_version_1_2) {
    const std::string filename{ "res/cqasm_version/version_1_2.cq" };
    EXPECT_EQ(parse_string("version 1.2", filename), Version{ "1.2" });
}
TEST(parse_string, string_version_1_3) {
    const std::string filename{ "res/cqasm_version/version_1_3.cq" };
    EXPECT_EQ(parse_string("version 1.3", filename), Version{ "1.3" });
}
TEST(parse_string, string_version_2_0) {
    const std::string filename{ "res/cqasm_version/version_2_0.cq" };
    EXPECT_EQ(parse_string("version 2.0", filename), Version{ "2.0" });
}
TEST(parse_string, string_version_3_0) {
    const std::string filename{ "res/cqasm_version/version_3_0.cq" };
    EXPECT_EQ(parse_string("version 3.0", filename), Version{ "3.0" });
}
TEST(parse_string, string_version_4_0) {
    const std::string filename{ "res/cqasm_version/version_4_0.cq" };
    EXPECT_EQ(parse_string("version 4.0", filename), Version{ "4.0" });
}
TEST(parse_string, string_instruction_called_version) {
    const std::string filename{ "res/cqasm_version/instruction_called_version.cq" };
    EXPECT_THAT([&]() { parse_string("version q[1:6], 3.14159\nmeasure_all", filename); },
        ThrowsMessage<ParseError>(HasSubstr(
            "Error at res/cqasm_version/instruction_called_version.cq:1:9..10: syntax error")));
}
TEST(parse_string, string_no_version_and_instruction) {
    const std::string filename{ "res/cqasm_version/no_version_and_instruction.cq" };
    EXPECT_THAT([&]() { parse_string("qubits 3\n\nx q[0]", filename); },
        ThrowsMessage<ParseError>(HasSubstr(
            "Error at res/cqasm_version/no_version_and_instruction.cq:1:1..2: syntax error")));
}
TEST(parse_string, string_version_1_0_and_instruction) {
    const std::string filename{ "res/cqasm_version/version_1_0_and_instruction.cq" };
    EXPECT_EQ(parse_string("version 1.0\n\nqubits 3\n\nx q[0]", filename), Version{ "1.0" });
}
TEST(parse_string, string_instruction_and_version_1_0) {
    const std::string filename{ "res/cqasm_version/instruction_and_version_1_0.cq" };
    EXPECT_THAT([&]() { parse_string("qubits 3\n\nx q[0]\n\nversion 1.0", filename); },
        ThrowsMessage<ParseError>(HasSubstr(
            "Error at res/cqasm_version/instruction_and_version_1_0.cq:1:1..2: syntax error")));
}
TEST(parse_string, string_version_1_0_and_no_filename_argument) {
    EXPECT_EQ(parse_string("version 1.0"), Version{ "1.0" });
}
TEST(parse_string, string_empty_and_no_filename_argument) {
    EXPECT_THAT([&]() { parse_string(""); },
        ThrowsMessage<ParseError>(HasSubstr("Error at <unknown>:1:1: syntax error")));
}


struct ScannerMock : public ScannerAdaptor {
    MOCK_METHOD((void), parse, (const std::string &, Version &), (const, override));
};

ACTION_P(ThrowOutOfMemoryParseError, filename) {
    throw ParseError(std::string("ScannerFlexBison::parse_: out of memory while parsing '") + filename + "'.");
}

ACTION_P(ThrowFailedToParseParseError, filename) {
    throw ParseError(std::string("ScannerFlexBison::parse_: failed to parse '") + filename + "'.");
}

TEST(ParseHelper_parse, scanner_throws_out_of_memory) {
    auto scanner_up = std::make_unique<ScannerMock>();
    const auto &scanner = *(dynamic_cast<ScannerMock*>(scanner_up.get()));
    const std::string filename = "huge_file.cq";
    EXPECT_CALL(scanner, parse(filename, _)).WillOnce(ThrowOutOfMemoryParseError(filename));
    EXPECT_THROW(ParseHelper(std::move(scanner_up), filename).parse(), ParseError);
}
TEST(ParseHelper_parse, scanner_throws_failed_to_parse) {
    auto scanner_up = std::make_unique<ScannerMock>();
    const auto &scanner = *(dynamic_cast<ScannerMock*>(scanner_up.get()));
    const std::string filename = "wrong_file.cq";
    EXPECT_CALL(scanner, parse(filename, _)).WillOnce(ThrowFailedToParseParseError(filename));
    EXPECT_THROW(ParseHelper(std::move(scanner_up), filename).parse(), ParseError);
}
TEST(ParseHelper_parse, scanner_returns_empty_version) {
    auto scanner_up = std::make_unique<ScannerMock>();
    const auto &scanner = *(dynamic_cast<ScannerMock*>(scanner_up.get()));
    const std::string filename = "file_no_version.cq";
    Version ret;
    EXPECT_CALL(scanner, parse(filename, _)).WillOnce(SetArgReferee<1>(ret));
    EXPECT_THAT([&]() { ParseHelper(std::move(scanner_up), filename).parse(); },
        ThrowsMessage<ParseError>(HasSubstr("no version info nor error info was returned by version parser.")));
}
TEST(ParseHelper_parse, scanner_returns_correct_version) {
    auto scanner_up = std::make_unique<ScannerMock>();
    const auto &scanner = *(dynamic_cast<ScannerMock*>(scanner_up.get()));
    const std::string filename = "file_version_3_0.cq";
    Version ret{ "3.0" };
    EXPECT_CALL(scanner, parse(filename, _)).WillOnce(SetArgReferee<1>(ret));
    EXPECT_EQ(ParseHelper(std::move(scanner_up), filename).parse(), Version{ "3.0" });
}
TEST(ParseHelper_parse, scanner_throws_and_no_filename_argument) {
    auto scanner_up = std::make_unique<ScannerFlexBisonString>("");
    EXPECT_THAT([&]() { ParseHelper(std::move(scanner_up)).parse(); },
        ThrowsMessage<ParseError>(HasSubstr("Error at <unknown>:1:1: syntax error")));
}
