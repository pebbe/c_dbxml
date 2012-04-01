#ifndef C_DBXML_H
#define C_DBXML_H

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct c_dbxml_t *c_dbxml;

    c_dbxml c_dbxml_open(char const *filename, int overwrite);
    void c_dbxml_delete(c_dbxml db);

    int c_dbxml_error(c_dbxml db);
    char const * c_dbxml_errstring(c_dbxml db);

    char const * c_dbxml_get(c_dbxml db, char const * key);

#ifdef __cplusplus
}
#endif

#endif /* C_DBXML_H */
