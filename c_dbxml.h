#ifndef C_DBXML_H
#define C_DBXML_H

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct c_dbxml_t *c_dbxml;

    c_dbxml c_dbxml_open(char const *filename);
    void c_dbxml_delete(c_dbxml db);

    int c_dbxml_error(c_dbxml db);
    char const * c_dbxml_errstring(c_dbxml db);

    /**** WRITE ****/

    /* replace if replace != 0
     * return 0 bij fout
     */
    int c_dbxml_put_file(c_dbxml db, char const *filename, int replace);

    /* replace if replace != 0
     * return 0 bij fout
     */
    int c_dbxml_put_xml(c_dbxml db, char const *name, char const *data, int replace);

    /* replace if replace != 0
     * return 0 bij fout
     */
    int c_dbxml_merge(c_dbxml db, char const *dbxmlfile, int replace);

    /* return 0 bij fout */
    int c_dbxml_remove(c_dbxml db, char const *name);

    /**** READ ****/

    char const * c_dbxml_get(c_dbxml db, char const * key);




#ifdef __cplusplus
}
#endif

#endif /* C_DBXML_H */
