#include "c_dbxml.h"
#include <dbxml/DbXml.hpp>

extern "C" {

    struct c_dbxml_t {
	DbXml::XmlManager manager;
	DbXml::XmlUpdateContext context;
	DbXml::XmlContainer container;
	bool error;
	std::string errstring;
	std::string result;
    };

    c_dbxml c_dbxml_open(char const *filename)
    {
	c_dbxml db;

	db = new c_dbxml_t;

	db->error = false;
	db->errstring = "";

	try {
	    db->context = db->manager.createUpdateContext();
	    db->container = db->manager.existsContainer(filename) ?
		db->manager.openContainer(filename) :
		db->manager.createContainer(filename);
	} catch (DbXml::XmlException &xe) {
	    db->errstring = xe.what();
	    db->error = true;
	}

	return db;
    }

    void c_dbxml_delete(c_dbxml db)
    {
	delete db;
    }

    int c_dbxml_error(c_dbxml db)
    {
	return db->error ? 1 : 0;
    }

    char const *c_dbxml_errstring(c_dbxml db)
    {
	return db->errstring.c_str();
    }

    int c_dbxml_put_file(c_dbxml db, char const * filename, int replace)
    {
	db->errstring = "";
	db->error = false;

	if (replace) {
	    try {
		db->container.deleteDocument(filename, db->context);
	    } catch (DbXml::XmlException &xe) {
		;
	    }
	}
        try {
            DbXml::XmlInputStream *is = db->manager.createLocalFileInputStream(filename);
            db->container.putDocument(filename, is, db->context);
        } catch (DbXml::XmlException &xe) {
	    db->error = true;
	    db->errstring = xe.what();
	    return 0;
        }

	return 1;
    }

    /* replace if replace != 0
     * return 0 bij fout
     */
    int c_dbxml_put_xml(c_dbxml db, char const *name, char const *data, int replace)
    {
	db->errstring = "";
	db->error = false;

	if (replace) {
	    try {
		db->container.deleteDocument(name, db->context);
	    } catch (DbXml::XmlException &xe) {
		;
	    }
	}

        try {
            db->container.putDocument(name, data, db->context);
        } catch (DbXml::XmlException &xe) {
	    db->error = true;
	    db->errstring = xe.what();
	    return 0;
        }
	return 1;
    }

    /* replace if replace != 0
     * return 0 bij fout
     */
    int c_dbxml_merge(c_dbxml db, char const * dbxmlfile, int replace) {
	db->errstring = "";
	db->error = false;

	DbXml::XmlContainer input = db->manager.openContainer(dbxmlfile);
	DbXml::XmlDocument doc;
	DbXml::XmlResults it = input.getAllDocuments(DbXml::DBXML_LAZY_DOCS);
	while (it.next(doc)) {
	    if (replace) {
		try {
		    db->container.deleteDocument(doc.getName(), db->context);
		} catch (DbXml::XmlException &xe) {
		    ;
		}
	    }
	    try {
		db->container.putDocument(doc, db->context);
	    } catch (DbXml::XmlException &xe) {
		db->error = true;
		db->errstring = xe.what();
		return 0;
	    }
	}
	return 1;
    }

    int c_dbxml_remove(c_dbxml db, char const * filename)
    {
	db->errstring = "";
	db->error = false;

	try {
	    db->container.deleteDocument(filename, db->context);
        } catch (DbXml::XmlException &xe) {
	    db->error = true;
	    db->errstring = xe.what();
	    return 0;
	}
	return 1;
    }


    char const * c_dbxml_get(c_dbxml db, char const * key)
    {
	db->errstring = "";
	db->error = false;

	try {
	    DbXml::XmlDocument doc = db->container.getDocument(key);
	    doc.getContent(db->result);
	    return db->result.c_str();
	} catch (DbXml::XmlException &xe) {
	    db->errstring = xe.what();
	    db->error = true;
	    return db->errstring.c_str();
	}
    }
}
