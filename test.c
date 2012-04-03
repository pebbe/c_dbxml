#include <stdio.h>

#include "c_dbxml.h"

int main (int argc, char *argv [])
{
    c_dbxml
	db;
    char const
	*s;

    db = c_dbxml_open("test.dact");
    if (c_dbxml_error(db)) {
	printf ("%s\n", c_dbxml_errstring(db));
	c_dbxml_delete(db);
	return 1;
    }

    s = c_dbxml_get(db, "999.xml");
    if (c_dbxml_error(db))
	printf ("%s\n", c_dbxml_errstring(db));
    else
	printf ("%s\n", s);

    c_dbxml_delete(db);
    return 0;
}
