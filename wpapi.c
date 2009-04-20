#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <xmlrpc-c/base.h>
#include <xmlrpc-c/client.h>

#include "wpapi.h"

#define NAME "WPFS XML-RPC Client"
#define VERSION "1.0"

static int post_count = 10;

static xmlrpc_env rpc_env;
static xmlrpc_client *rpc_client;

static struct wp_post *get_posts(void);
static void free_posts(struct wp_post *posts);

int wp_init(struct wp_post *posts)
{
    /* Load up the xmlrpc environment */
    xmlrpc_env_init(&rpc_env);
    xmlrpc_client_setup_global_const(&rpc_env);
    xmlrpc_client_create(&rpc_env, XMLRPC_CLIENT_NO_FLAGS, NAME, VERSION, NULL, 0, &rpc_client);
    /* die_if_fault_occurred(&rpc_env); */

/*
     xmlrpc_value * resultP;
     int sum;
     char * const url = "http://localhost:8080/RPC2";
     char * const methodName = "sample.add";
 

    // Make the remote procedure call
       xmlrpc_client_call2f(&env, clientP, url, methodName, &resultP,
       "(ii)", (xmlrpc_int32) 5, (xmlrpc_int32) 7);
       die_if_fault_occurred(&env);

     // Get our state name and print it out. 
     xmlrpc_parse_value(&env, resultP, "i", &sum);
     die_if_fault_occurred(&env);
     printf("The sum  is %d\n", sum);

     // Dispose of our result value.
     xmlrpc_DECREF(resultP);

*/
    get_posts();
    return 0;
}

int wp_destroy(struct wp_post *posts)
{
    free_posts(posts);

    /* Clean up the xmlrpc */
    xmlrpc_env_clean(&rpc_env);
    xmlrpc_client_destroy(rpc_client);
    xmlrpc_client_teardown_global_const();

    return 0;
}

static struct wp_post *get_posts(void)
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

