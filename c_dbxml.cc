#include "c_dbxml.h"
#include <dbxml/DbXml.hpp>
#include <cstdlib>
#include <cstring>

extern "C" {

    struct c_dbxml_t {
	DbXml::XmlManager manager;
	DbXml::XmlUpdateContext context;
	DbXml::XmlContainer container;
	bool overwrite;
	bool error;
	std::string errstring;
    };

    std::string result;

    void c_dbxml_errclear(c_dbxml db)
    {
	db->errstring = "";
	db->error = false;
    }
    
    c_dbxml c_dbxml_open(char const *filename, int overwrite)
    {
	c_dbxml db;

	db = new c_dbxml_t;
	db->error = false;
	db->errstring = "";
	db->overwrite = overwrite ? true : false;

	try {
	    db->context = db->manager.createUpdateContext();
	    db->container = db->manager.existsContainer(filename) ?
		db->manager.openContainer(filename) :
		db->manager.createContainer(filename);
	} catch (DbXml::XmlException &xe) {
	    db->errstring = strdup(xe.what());
	    db->error = true;
	}

	return db; 
    }

    void c_dbxml_delete(c_dbxml db)
    {
	c_dbxml_errclear(db);
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

    char const * c_dbxml_get(c_dbxml db, char const * key)
    {
	c_dbxml_errclear(db);

	try {
	    DbXml::XmlDocument doc = db->container.getDocument(key);
	    doc.getContent(result);
	    return result.c_str();
	} catch (DbXml::XmlException &xe) {
	    db->errstring = strdup(xe.what());
	    db->error = true;
	    return db->errstring.c_str();
	}
    }
}
