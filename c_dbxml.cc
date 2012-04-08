#include "c_dbxml.h"
#include <dbxml/DbXml.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/framework/Wrapper4InputSource.hpp>
#include <xqilla/xqilla-dom3.hpp>
#include <list>
#include <string>

namespace xerces = XERCES_CPP_NAMESPACE;

#define ALIAS "c_dbxml"

extern "C" {

    struct c_dbxml_t {
	DbXml::XmlManager manager;
	DbXml::XmlUpdateContext context;
	DbXml::XmlContainer container;
	bool error;
	std::string errstring;
	std::string result;
    };

    struct c_dbxml_docs_t {
	DbXml::XmlDocument doc;
	DbXml::XmlResults it;
	DbXml::XmlQueryContext context;
	bool more;
	std::string name;
	std::string content;
    };

    struct c_dbxml_markers_t {
	int n;
	std::vector<std::string> queries;
	std::vector<std::string> attrs;
	std::vector<std::string> values;
    };

    bool c_dbxml_global_error_d;
    std::string c_dbxml_global_errstring_d;

    int c_dbxml_global_error()
    {
	return c_dbxml_global_error_d ? 1 : 0;
    }

    char const * c_dbxml_global_errstring()
    {
	return c_dbxml_global_errstring_d.c_str();
    }

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
	    if (!db->container.addAlias(ALIAS)) {
		db->errstring = "Unable to add alias \"" ALIAS "\"";
		db->error = true;
	    }
	} catch (DbXml::XmlException &xe) {
	    db->errstring = xe.what();
	    db->error = true;
	}

	return db;
    }

    void c_dbxml_free(c_dbxml db)
    {
	db->container.removeAlias(ALIAS);
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


    char const * c_dbxml_get(c_dbxml db, char const * name)
    {
	db->errstring = "";
	db->error = false;

	try {
	    DbXml::XmlDocument doc = db->container.getDocument(name);
	    doc.getContent(db->result);
	    return db->result.c_str();
	} catch (DbXml::XmlException &xe) {
	    db->errstring = xe.what();
	    db->error = true;
	    return db->errstring.c_str();
	}
    }

    unsigned long long c_dbxml_size(c_dbxml db)
    {
	return (unsigned long long) db->container.getNumDocuments();
    }

    c_dbxml_docs c_dbxml_get_all(c_dbxml db)
    {
	db->errstring = "";
	db->error = false;

	c_dbxml_docs docs;
	docs = new c_dbxml_docs_t;
	docs->it = db->container.getAllDocuments(DbXml::DBXML_LAZY_DOCS);
	docs->more = true;
	return docs;
    }

    c_dbxml_docs c_dbxml_get_query(c_dbxml db, char const *query)
    {
	db->errstring = "";
	db->error = false;

	c_dbxml_docs docs;
	docs = new c_dbxml_docs_t;
	docs->more = true;
	try {

	    docs->context = db->manager.createQueryContext(DbXml::XmlQueryContext::LiveValues, DbXml::XmlQueryContext::Lazy);
	    docs->context.setDefaultCollection(ALIAS);
	    docs->it = db->manager.query(std::string("collection('" ALIAS "')") + query,
					 docs->context,
					 DbXml::DBXML_LAZY_DOCS | DbXml::DBXML_WELL_FORMED_ONLY
					 );

	} catch (DbXml::XmlException const &xe) {
	    docs->more = false;
	    db->error = true;
	    db->errstring = xe.what();
	}

	return docs;
    }

    int c_dbxml_docs_next(c_dbxml_docs docs)
    {
	if (docs->more) {
	    docs->more = docs->it.next(docs->doc);
	    docs->name.clear();
	    docs->content.clear();
	}
	return docs->more ? 1 : 0;
    }

    char const * c_dbxml_docs_name(c_dbxml_docs docs)
    {
	if (docs->more && ! docs->name.size())
	    docs->name = docs->doc.getName();

	return docs->name.c_str();
    }

    char const * c_dbxml_docs_content(c_dbxml_docs docs)
    {
	if (docs->more && ! docs->content.size())
	    docs->doc.getContent(docs->content);

	return docs->content.c_str();
    }

    void c_dbxml_docs_free(c_dbxml_docs docs)
    {
	delete docs;
    }

    c_dbxml_markers c_dbxml_markers_new()
    {
	c_dbxml_markers m = new c_dbxml_markers_t;
	m->n = 0;
	m->queries.clear();
	m->attrs.clear();
	m->values.clear();
	return m;
    }

    void c_dbxml_markers_add(c_dbxml_markers markers, char const *query, char const *attr, char const *value)
    {
	markers->n += 1;
	markers->queries.push_back(query);
	markers->attrs.push_back(attr);
	markers->values.push_back(value);
    }

    void c_dbxml_markers_free(c_dbxml_markers markers)
    {
	delete markers;
    }

    char const * c_dbxml_mark_entry(char const *entry, c_dbxml_markers markers)
    {
	if (markers->n < 1)
	    return entry;

	static std::string result;
	std::string content(entry);

	result = "";
	c_dbxml_global_error_d = false;
	c_dbxml_global_errstring_d = "";


	// Prepare the DOM parser.
	xerces::DOMImplementation *xqillaImplementation =
	    xerces::DOMImplementationRegistry::getDOMImplementation(X("XPath2 3.0"));
	AutoRelease<xerces::DOMLSParser> parser(xqillaImplementation->createLSParser(xerces::DOMImplementationLS::MODE_SYNCHRONOUS, 0));

	// Parse the document.
	xerces::MemBufInputSource xmlInput(reinterpret_cast<XMLByte const *>(content.c_str()),
					   content.size(), "input");

	xerces::Wrapper4InputSource domInput(&xmlInput, false);

	xerces::DOMDocument *document;
	try {
	    document = parser->parse(&domInput);
	} catch (xerces::DOMException const &e) {
	    c_dbxml_global_errstring_d = std::string("Could not parse XML data: ") + UTF8(e.getMessage());
	    c_dbxml_global_error_d = true;
	    return "";
	}

	// No exceptions according to the documentation...
	AutoRelease<xerces::DOMXPathNSResolver> resolver(document->createNSResolver(document->getDocumentElement()));
	resolver->addNamespaceBinding(X("fn"),
				      X("http://www.w3.org/2005/xpath-functions"));


	for (int i = 0; i < markers->n; i++) {

	    AutoRelease<xerces::DOMXPathExpression> expression(0);
	    try {
		expression.set(document->createExpression(X(markers->queries[i].c_str()), resolver));
	    } catch (xerces::DOMXPathException const &) {
		c_dbxml_global_errstring_d = "Could not parse expression: " + markers->queries[i];
		c_dbxml_global_error_d = true;
		return "";
	    } catch (xerces::DOMException const &) {
		c_dbxml_global_errstring_d = "Could not resolve namespace prefixes.";
		c_dbxml_global_error_d = true;
		return "";
	    }

	    AutoRelease<xerces::DOMXPathResult> domresult(0);
	    try {
		domresult.set(expression->evaluate(document,
						   xerces::DOMXPathResult::ITERATOR_RESULT_TYPE, 0));
	    } catch (xerces::DOMXPathException const &e) {
		c_dbxml_global_errstring_d = "Could not retrieve an iterator over evaluation results.";
		c_dbxml_global_error_d = true;
		return "";
	    } catch (xerces::DOMException &e) {
		c_dbxml_global_errstring_d = "Could not evaluate the expression on the given document.";
		c_dbxml_global_error_d = true;
		return "";
	    }

	    std::list<xerces::DOMNode *> markNodes;

	    while (domresult->iterateNext())
		{
		    xerces::DOMNode *node;
		    try {
			node = domresult->getNodeValue();
		    } catch (xerces::DOMXPathException &e) {
			c_dbxml_global_errstring_d = "Matching node value invalid while marking nodes.";
			c_dbxml_global_error_d = true;
			return "";
		    }

		    // Skip non-element nodes
		    if (node->getNodeType() != xerces::DOMNode::ELEMENT_NODE)
			continue;

		    markNodes.push_back(node);
		}

	    for (std::list<xerces::DOMNode *>::iterator nodeIter = markNodes.begin();
		 nodeIter != markNodes.end(); ++nodeIter)
		{
		    xerces::DOMNode *node = *nodeIter;

		    xerces::DOMNamedNodeMap *map = node->getAttributes();
		    if (map == 0)
			continue;

		    // Create new attribute node.
		    xerces::DOMAttr *attr;
		    try {
			attr = document->createAttribute(X(markers->attrs[i].c_str()));
		    } catch (xerces::DOMException const &e) {
			c_dbxml_global_errstring_d = "Attribute name contains invalid character.";
			c_dbxml_global_error_d = true;
			return "";
		    }
		    attr->setNodeValue(X(markers->values[i].c_str()));
		    map->setNamedItem(attr);
		}
	}

	// Serialize DOM tree
	AutoRelease<xerces::DOMLSSerializer> serializer(xqillaImplementation->createLSSerializer());
	AutoRelease<xerces::DOMLSOutput> output(xqillaImplementation->createLSOutput());
	xerces::MemBufFormatTarget target;
	output->setByteStream(&target);
	serializer->write(document, output.get());

	result = reinterpret_cast<char const *>(target.getRawBuffer()), target.getLen();

	return result.c_str();
    }
}
