#include <stdio.h>

#include "c_dbxml.h"

int main (int argc, char *argv [])
{
    c_dbxml
	db;
    c_dbxml_docs
	docs;
    char const
	*s;

    db = c_dbxml_open("test.dact");
    if (c_dbxml_error(db)) {
	printf ("%s\n", c_dbxml_errstring(db));
	c_dbxml_free(db);
	return 1;
    }

    s = c_dbxml_get(db, "999.xml");
    if (c_dbxml_error(db))
	printf ("%s\n", c_dbxml_errstring(db));
    else
	printf ("%s\n", s);

    if (! c_dbxml_put_file(db, "../corpora/test-xml/7153.xml", 1))
	printf ("%s\n", c_dbxml_errstring(db));

    if (! c_dbxml_remove(db, "../corpora/test-xml/7153.xml"))
	printf ("%s\n", c_dbxml_errstring(db));

    if (! c_dbxml_remove(db, "7153.xml"))
	printf ("%s\n", c_dbxml_errstring(db));

    /*
    if (! c_dbxml_merge(db, "../corpora/geloof.dact", 0))
	printf ("%s\n", c_dbxml_errstring(db));
    */

    if (! c_dbxml_put_xml(db, "test.xml", s, 0))
	printf ("%s\n", c_dbxml_errstring(db));



    docs = c_dbxml_get_all(db);
    while (c_dbxml_docs_next(docs)) {
	printf ("Name: %s\n", c_dbxml_docs_name(docs));
	/* printf ("Data: %s\n", c_dbxml_docs_content(docs)); */
    }
    c_dbxml_docs_free(docs);

    printf ("%llu\n", c_dbxml_size(db));

    c_dbxml_free(db);


    return 0;
}
