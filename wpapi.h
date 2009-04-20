#ifndef WPFS_H
#define WPFS_H

struct wp_post {
    int post_id;
    char *slug_path;
};

struct wp_post *get_posts(void);
void free_posts(struct wp_post *posts);
int postslen(struct wp_post *posts);

#endif /* WPFS_H */
