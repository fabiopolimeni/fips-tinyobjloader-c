#define TEST_NO_MAIN

#include "tinyobj_regression_tests.h"
#include "tinyobj_loader_c.h"
#include "acutest.h"

#include <math.h>
#include <float.h>

static int float_equals(float x, float y)
{
  if (fabs(x - y) < FLT_EPSILON) {
    return 1;
  }

  return 0;
}

static size_t loadFile(const char * filename, char ** buffer)
{
    *buffer = NULL;
    long string_size = 0, read_size = 0;
    FILE * handler = fopen(filename, "r");

    if (handler) {
        fseek(handler, 0, SEEK_END);
        string_size = ftell(handler);
        rewind(handler);
        *buffer = (char *) malloc(sizeof(char) * (string_size + 1));
        read_size = fread(*buffer, sizeof(char), (size_t) string_size, handler);
        (*buffer)[string_size] = '\0';
        if (string_size != read_size) {
            free(buffer);
            *buffer = NULL;
        }
        fclose(handler);
    }

    return (size_t) read_size;
}

void test_tinyobj_crlf_string(void)
{
    {
        const char * filename = "fixtures/texname-crlf.mtl";

        tinyobj_material_t * material;
        size_t num_materials;

        TEST_CHECK(tinyobj_parse_mtl_file(&material, &num_materials, filename) == TINYOBJ_SUCCESS);

        TEST_CHECK(num_materials == 1);
        TEST_CHECK(strcmp(material->name, "CubeMaterial") == 0);

        TEST_CHECK(strcmp(material->diffuse_texname, "input.jpg") == 0);
    }
}

void test_tinyobj_negative_exponent(void)
{
    {
        const char * filename = "fixtures/negative-exponent.obj";

        tinyobj_shape_t * shape = NULL;
        tinyobj_material_t * material = NULL;
        tinyobj_attrib_t attrib;

        unsigned long num_shapes;
        unsigned long num_materials;

        tinyobj_attrib_init(&attrib);

        char * obj_contents;
        size_t file_size = loadFile(filename, &obj_contents);

        int result = tinyobj_parse_obj(&attrib, &shape, &num_shapes, &material, &num_materials, obj_contents, file_size, TINYOBJ_FLAG_TRIANGULATE);

        TEST_CHECK(result == TINYOBJ_SUCCESS);

        // TODO: should these two values be swapped?
        TEST_CHECK(attrib.num_faces == 36);
        TEST_CHECK(attrib.num_face_num_verts == 12);

        TEST_CHECK(float_equals(attrib.vertices[0], 2.0e+5f));
        TEST_CHECK(float_equals(attrib.vertices[1], 2.0e-5f));
        TEST_CHECK(float_equals(attrib.vertices[2], 2.0e-0f));
    }
}
