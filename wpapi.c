#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wpapi.h"

static int post_count = 10;

struct wp_post *get_posts(void)
{
    int i;
    struct wp_post *posts = malloc(post_count * sizeof(struct wp_post));
    struct wp_post *post = posts;

    if(posts == NULL)
        return NULL;

    for(i = 0; i < post_count; i++) {
        post->post_id = i;

        char tmp[100];
        sprintf(tmp, "/slug-%d", i);
        post->slug_path = malloc((strlen(tmp) + 1) * sizeof(char));
        strcpy(post->slug_path, tmp);

        post++;
    }

    return posts;
}

void free_posts(struct wp_post *posts)
{
    int i;
    struct wp_post *post = posts;

    for(i = 0; i < post_count; i++) {
        free(post->slug_path);
        post++;
    }

    free(posts);
}

int postslen(struct wp_post *posts)
{
    return post_count;
}

