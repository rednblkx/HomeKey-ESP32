#include <assert.h>
#include <string.h>
#include "json_parser.h"
#include "unity.h"

#define json_test_str   "{\n\"str_val\" :    \"JSON Parser\",\n" \
            "\t\"float_val\" : 2.0,\n" \
            "\"int_val\" : 2017,\n" \
            "\"bool_val\" : false,\n" \
            "\"supported_el\" :\t [\"bool\",\"int\","\
            "\"float\",\"str\"" \
            ",\"object\",\"array\"],\n" \
            "\"features\" : { \"objects\":true, "\
            "\"arrays\":\"yes\"},\n"\
            "\"int_64\":109174583252}"

TEST_CASE("json_parser basic tests", "[json_parser]")
{
    jparse_ctx_t jctx;
    int ret = json_parse_start(&jctx, json_test_str, strlen(json_test_str));
    TEST_ASSERT_EQUAL(OS_SUCCESS, ret);

    char str_val[64];
    int int_val, num_elem;
    int64_t int64_val;
    bool bool_val;
    float float_val;

    TEST_ASSERT_EQUAL(OS_SUCCESS, json_obj_get_string(&jctx, "str_val", str_val, sizeof(str_val)));
    TEST_ASSERT_EQUAL_STRING("JSON Parser", str_val);

    TEST_ASSERT_EQUAL(OS_SUCCESS, json_obj_get_float(&jctx, "float_val", &float_val));
    TEST_ASSERT(fabs(float_val - 2.0f) < 0.0001f)

    TEST_ASSERT_EQUAL(OS_SUCCESS, json_obj_get_int(&jctx, "int_val", &int_val));
    TEST_ASSERT_EQUAL_INT(2017, int_val);

    TEST_ASSERT_EQUAL(OS_SUCCESS, json_obj_get_bool(&jctx, "bool_val", &bool_val));
    TEST_ASSERT_EQUAL(false, bool_val);

    TEST_ASSERT_EQUAL(OS_SUCCESS, json_obj_get_array(&jctx, "supported_el", &num_elem));
    const char *expected_values[] = {"bool", "int", "float", "str", "object", "array"};
    TEST_ASSERT_EQUAL(sizeof(expected_values) / sizeof(expected_values[0]), num_elem);
    for (int i = 0; i < num_elem; ++i) {
        TEST_ASSERT_EQUAL(OS_SUCCESS, json_arr_get_string(&jctx, i, str_val, sizeof(str_val)));
        TEST_ASSERT_EQUAL_STRING(expected_values[i], str_val);
    }
    json_obj_leave_array(&jctx);

    TEST_ASSERT_EQUAL(OS_SUCCESS, json_obj_get_object(&jctx, "features"));
    TEST_ASSERT_EQUAL(OS_SUCCESS, json_obj_get_bool(&jctx, "objects", &bool_val));
    TEST_ASSERT_EQUAL(true, bool_val);
    TEST_ASSERT_EQUAL(OS_SUCCESS, json_obj_get_string(&jctx, "arrays", str_val, sizeof(str_val)));
    TEST_ASSERT_EQUAL_STRING("yes", str_val);
    json_obj_leave_object(&jctx);

    TEST_ASSERT_EQUAL(OS_SUCCESS, json_obj_get_int64(&jctx, "int_64", &int64_val));
    TEST_ASSERT(int64_val == 109174583252);

    json_parse_end(&jctx);
}