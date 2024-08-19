#ifndef __BROWSER_H__
#define __BROWSER_H__

#ifdef __cplusplus
extern "C" {
#endif

#define browser_handler void *
#define browser_item void *

browser_handler img_browser_create();
void            img_browser_free(browser_handler bh);

void            img_append(browser_handler bh, const char *name);
const char      *img_browser_get_name(browser_handler bh, browser_item i);

browser_item    img_iterator_begin(browser_handler bh);
void            img_iterator_free(browser_item i);
browser_item    img_iterator_next(browser_item i);
int             img_iterator_is_end(browser_handler bh, browser_item i);


#ifdef __cplusplus
}
#endif 

#endif /* __BROWSER_H__ */

