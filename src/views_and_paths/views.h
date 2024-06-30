#ifndef INDEX_H
#define INDEX_H

#include "../request_type/request_type.h"
#include "path.h"

view_t *index_view(struct request *req);

view_t *contact_view(struct request *req);

view_t *index_post_view(struct request *req);

#endif