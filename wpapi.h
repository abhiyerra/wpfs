#ifndef WPAPI_H
#define WPAPI_H

struct wp_post {
    int post_id;
    char *slug_path;
    char *content;
};


int wp_init(struct wp_post *posts);
int wp_destroy(struct wp_post *posts);
int postslen(struct wp_post *posts);

#endif /* WPAPI_H */
