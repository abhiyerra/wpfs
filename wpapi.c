/*
# Load posts

# url, isAdmin, blogid, xmlrpc, blogName
self.blog_info = my_blogs[0]

# Find the blog we are looking for based on blog_url if there is more than one blog.
if len(my_blogs) > 1:
for blog in my_blogs:
if self.blog_url in blog['url']:
self.blog_info = blog

self.posts = self.client.metaWeblog.getRecentPosts(self.blog_info['blogid'], username, password, '0')

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libconfig.h>

#include <xmlrpc-c/base.h>
#include <xmlrpc-c/client.h>

#include "wpapi.h"

#define NAME "WPFS XML-RPC Client"
#define VERSION "1.0"

static char *username, *password, *blogurl;

static int post_count = 10;

static xmlrpc_env rpc_env;
static xmlrpc_client *rpc_client;

static int read_config(void);
static struct wp_post *get_posts(void);
static void free_posts(struct wp_post *posts);

int wp_init(struct wp_post *posts)
{
    if(read_config() < 0) {
        exit(-1);
    }

    /* Load up the xmlrpc environment */
    xmlrpc_env_init(&rpc_env);
    xmlrpc_client_setup_global_const(&rpc_env);
    xmlrpc_client_create(&rpc_env, XMLRPC_CLIENT_NO_FLAGS, NAME, VERSION, NULL, 0, &rpc_client);
    /* die_if_fault_occurred(&rpc_env); */

    get_posts();
    return 0;
}

int wp_destroy(struct wp_post *posts)
{
    free_posts(posts);

    free(username);
    free(password);
    free(blogurl);

    /* Clean up the xmlrpc */
    xmlrpc_env_clean(&rpc_env);
    xmlrpc_client_destroy(rpc_client);
    xmlrpc_client_teardown_global_const();

    return 0;
}


static int read_config(void)
{
    int ret = 0;

    struct config_t cfg;
    config_setting_t *setting = NULL;
    char *tmp;

    /* Load up the config */
    config_init(&cfg);
    config_read_file(&cfg, WPFS_CFG);

    /* Load username */
    setting = config_lookup(&cfg, "username");
    if(!setting) {
       fprintf(stderr, "username not defined in config\n"); 
       ret = -1;
    } else {
        tmp = config_setting_get_string(setting);
        username = malloc((strlen(tmp) + 1) * sizeof(char));
        strcpy(username, tmp);
    }
    
    /* Load password */
    setting = config_lookup(&cfg, "password");
    if(!setting) {
       fprintf(stderr, "password not defined in config\n"); 
       ret = -1;
    } else {
        tmp = config_setting_get_string(setting);
        password = malloc((strlen(tmp) + 1) * sizeof(char));
        strcpy(password, tmp);
    }

    /* Load blogurl */
    setting = config_lookup(&cfg, "blogurl");
    if(!setting) {
       fprintf(stderr, "blogurl not defined in config\n"); 
       ret = -1;
    } else {
        tmp = config_setting_get_string(setting);
        blogurl = malloc((strlen(tmp) + 1) * sizeof(char));
        strcpy(blogurl, tmp);
    }

    config_destroy(&cfg);

    return ret;
}

static struct wp_post *get_posts(void)
{
    int i;
    struct wp_post *posts = malloc(post_count * sizeof(struct wp_post));
    struct wp_post *post = posts;

    xmlrpc_value *result;

    /* wp.getUsersBlogs(username, password) */
    char *const method_name = "wp.getUsersBlogs";

    xmlrpc_client_call2f(&rpc_env, rpc_client, blogurl, method_name, &result, "ss", 
                         username, password);

    printf("%d\n", xmlrpc_value_type(result));
    /*
    // Get our state name and print it out. 
    xmlrpc_parse_value(&env, resultP, "i", &sum);
    printf("The sum  is %d\n", sum);

    */

    // Dispose of our result value.
    //xmlrpc_DECREF(result);

    printf("test\n");
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

static void free_posts(struct wp_post *posts)
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

