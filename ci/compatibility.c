#include <assert.h>
#include <expat.h>
#include <string.h>

static void XMLCALL start_element(void *data, const XML_Char *name,
                                  const XML_Char **attributes) {
    int *seen = data;

    assert(strcmp(name, "record") == 0);
    assert(attributes[0] && strcmp(attributes[0], "id") == 0);
    assert(attributes[1] && strcmp(attributes[1], "123") == 0);
    assert(attributes[2] == NULL);
    ++*seen;
}

int main(void) {
    const char first[] = "<record id=\"1";
    const char last[] = "23\"/>";
    int seen = 0;
    XML_Parser parser = XML_ParserCreate(NULL);

    assert(parser);
    XML_SetUserData(parser, &seen);
    XML_SetStartElementHandler(parser, start_element);
    assert(XML_Parse(parser, first, sizeof(first) - 1, XML_FALSE) == XML_STATUS_OK);
    assert(seen == 0);
    assert(XML_Parse(parser, last, sizeof(last) - 1, XML_TRUE) == XML_STATUS_OK);
    assert(seen == 1);
    XML_ParserFree(parser);
    return 0;
}
